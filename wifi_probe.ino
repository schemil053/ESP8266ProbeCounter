#include "ESP8266WiFi.h"

#include "ArrayList.h"
#include "RXSniffer.h"
#include "Util.h"
#include "WebPageStore.h"

#include <FS.h>
#include <SPI.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#define LED_PIN 2
#define RESET_SWITCH 0

#define CHANNEL_HOP_INTERVAL_MS 2000
static os_timer_t channelHop_timer;

#define DISABLE 0
#define ENABLE 1

long rebootmillis = 0;

const byte DNS_PORT = 53;
DNSServer dnsServer;

ESP8266WebServer server(80);

boolean setupMode = false;

#define DATA_LENGTH 112

#define TYPE_MANAGEMENT 0x00
#define TYPE_CONTROL 0x01
#define TYPE_DATA 0x02
#define SUBTYPE_PROBE_REQUEST 0x04

String cdevid = "?????";

String wifiname = "";
String wifipass = "";
String room = "";
IPAddress ip;
int port = 29000;

int minrssi = 80;
int checktime = 10;

long lastdata = 0;

ArrayList<String> devices;





void initSpiffs()
{
  bool initok = false;
  initok = SPIFFS.begin();
  if (!(initok))
  {
    Serial.println("Format SPIFFS");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!(initok))
  {
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!initok) {
    Serial.println("Error while reading SPIFFS! Halting System...");
    while (true) {
      delay(10000);
    }
  } else {
    if (!(SPIFFS.exists ("/devid"))) {
      Serial.print("Setting new DevID...  ");
      File devidfile = SPIFFS.open("/devid", "w");
      if (!devidfile) {
        Serial.println("Error while reading SPIFFS! Halting System...");
        while (true) {
          delay(10000);
        }
      }
      char devid[7];
      getRandomStrF(devid, 7);
      devidfile.println(devid);
      devidfile.close();
      Serial.println(((String) "[OK] DevID is ") + devid);

      cdevid = devid;
    } else {
      Serial.print("Loading DevID...  ");

      File devidfile = SPIFFS.open("/devid", "r");

      String devid = devidfile.readStringUntil('\n');

      devidfile.close();

      Serial.println("[OK] DevID is " + devid);

      cdevid = devid;
    }
  }
}




static void showMetadata(SnifferPacket *snifferPacket) {

  unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];

  uint8_t version = (frameControl & 0b0000000000000011) >> 0;
  uint8_t frameType = (frameControl & 0b0000000000001100) >> 2;
  uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  uint8_t toDS = (frameControl & 0b0000000100000000) >> 8;
  uint8_t fromDS = (frameControl & 0b0000001000000000) >> 9;

  if (frameType != TYPE_MANAGEMENT ||
      frameSubType != SUBTYPE_PROBE_REQUEST) {
    return;
  }

  Serial.print("RSSI: ");
  Serial.print(snifferPacket->rx_ctrl.rssi, DEC);

  Serial.print(" Channel: ");
  Serial.print(wifi_get_channel());

  char addr[] = "00:00:00:00:00:00";
  getMAC(addr, snifferPacket->data, 10);
  Serial.print(" MAC: ");
  Serial.print(addr);

  uint8_t SSID_length = snifferPacket->data[25];
  Serial.print(" SSID: ");
  printDataSpan(26, SSID_length, snifferPacket->data);



  Serial.println();

  int rssi = snifferPacket->rx_ctrl.rssi;
  if (rssi < 0) {
    rssi = -rssi;
  }

  if (rssi >= minrssi) {
    if (!devices.contains((String) addr)) {
      devices.add((String) addr);
    }
  }
}


static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
  struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
  showMetadata(snifferPacket);
}

static void printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
  for (uint16_t i = start; i < DATA_LENGTH && i < start + size; i++) {
    Serial.write(data[i]);
  }
}

static void getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset + 0], data[offset + 1], data[offset + 2], data[offset + 3], data[offset + 4], data[offset + 5]);
}

void channelHop()
{
  uint8 new_channel = wifi_get_channel() + 1;
  if (new_channel > 12) {
    new_channel = 1;
  }
  wifi_set_channel(new_channel);
}

void enableLED() {
  digitalWrite(LED_PIN, ENABLE);
}

void disableLED() {
  digitalWrite(LED_PIN, DISABLE);
}

void setup() {
  Serial.begin(115200);
  Serial.println();


  pinMode(RESET_SWITCH, INPUT);
  pinMode(LED_PIN, OUTPUT);

  disableLED();

  initSpiffs();

  if (checkFirstStart()) {
    setupMode = true;
    return;
  }

  WiFi.mode(WIFI_STA);


  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);


  os_timer_disarm(&channelHop_timer);
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);

  WiFi.hostname("SchemilWifiProbe-" + cdevid);
}

void loop() {
  if (rebootmillis != 0) {
    if (millis() > rebootmillis) {
      ESP.restart();
      return;
    }
  }
  if (setupMode) {
    dnsServer.processNextRequest();
    server.handleClient();
    return;
  }
  if (lastdata == 0) {
    lastdata = millis();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(wifiname, wifipass);
  }
  if ((lastdata + (checktime * 60000)) < millis()) {
    lastdata = millis();
    int devs = devices.size();
    devices.clear();
    WiFi.persistent(false);
    delay(20);
    // WiFi.setAutoReconnect(false);
    delay(20);
    WiFi.mode(WIFI_STA);

    delay(100);

    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    WiFi.disconnect();

    delay(100);
    WiFi.scanDelete();
    delay(50);
    WiFi.scanNetworks();


    wifi_promiscuous_enable(DISABLE);
    os_timer_disarm(&channelHop_timer);

    delay(200);

    Serial.println();
    Serial.print("Connecting to WiFi ...");
    int i = 0;
    WiFi.hostname("SchemilWifiProbe-" + cdevid);
    WiFi.begin(wifiname, wifipass);
    //  while (WiFi.status() != WL_CONNECTED)
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500); //100
      Serial.print(".");
      yield();

      /*WiFi.mode(WIFI_STA);
        delay(100);
        WiFi.disconnect();
        delay(100);
        WiFi.begin(wifiname, wifipass);
        delay(5000);
      */


      i++;

      if (i > 500) {
        Serial.println();
        Serial.println("Connection lost.");
        WiFi.printDiag(Serial);
        Serial.println(WiFi.status());
        Serial.println("0 WiFi is in process of changing between statuses");
        Serial.println("1 SSID cannot be reached");
        Serial.println("2 Scan Completed");
        Serial.println("3 Successful connection is established");
        Serial.println("4 Password is incorrect");
        Serial.println("5 Connection Lost");
        Serial.println("6 Module is disconnected or not configured in station mode");
        recheckWiFi();
        return;
      }
    }
    delay(100);
    Serial.println();
    Serial.println("WiFi connected.");
    Serial.println(WiFi.localIP());
    delay(500);

    WiFiClient client;
    client.connect(ip, port);

    client.println("WiFiProbe V0.1");
    client.println(room);
    client.println(devs + ((String) ""));

    client.stop();

    delay(200);

    WiFi.disconnect();

    delay(500);
    recheckWiFi();
  }
  if (digitalRead(RESET_SWITCH) == 0) {
    int i = 0;
    int maxtime = 100;


    while (digitalRead(RESET_SWITCH) == 0 && i < maxtime) {
      enableLED();
      delay(50);
      disableLED();
      delay(50);
      i++;
      Serial.println(((String) "Resetbutton pressed. ") + i + ((String) "/") + maxtime);
    }

    if (i < maxtime) {
      return;
    }

    Serial.println("Resetting... ");

    SPIFFS.format();
    ESP.eraseConfig();

    File devidfile = SPIFFS.open("/devid", "w");
    if (!devidfile) {
      Serial.println("Error while reading SPIFFS! Halting System...");
      while (true) {
        delay(10000);
      }
    }

    devidfile.println(cdevid);
    devidfile.close();

    ESP.restart();
    while (true) {

    }
  }
}


boolean checkFirstStart() {
  if ((SPIFFS.exists ("/wifidata"))) {
    File wififile = SPIFFS.open("/wifidata", "r");

    wifiname = wififile.readStringUntil('\n');
    wifipass = wififile.readStringUntil('\n');
    ip.fromString(wififile.readStringUntil('\n'));
    port = wififile.readStringUntil('\n').toInt();
    room = wififile.readStringUntil('\n');
    checktime = wififile.readStringUntil('\n').toInt();
    minrssi = wififile.readStringUntil('\n').toInt();

    Serial.println(wifiname);
    Serial.println(wifipass);

    wififile.close();
    return false;
  }


  WiFi.mode(WIFI_AP);
  WiFi.persistent(false);

  WiFi.softAP("WifiProberSetup-" + cdevid);



  delay(500);

  dnsServer.start(DNS_PORT, "*", WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", setuppage);
  });

  server.on("/wifisave", []() {
    if (!server.hasArg("checktime") || !server.hasArg("ssid") || !server.hasArg("password") || !server.hasArg("room") || !server.hasArg("ip") || !server.hasArg("port") || !server.hasArg("rssi")) {
      server.send(200, "text/plain", "Es fehlen Argumente!");
      return;
    }
    String ssid = server.arg("ssid");
    String checktime = server.arg("checktime");
    String room = server.arg("room");
    String password = server.arg("password");
    String ip = server.arg("ip");
    String port = server.arg("port");
    String rssi = server.arg("rssi");

    File wififile = SPIFFS.open("/wifidata", "w");

    wififile.println(ssid);
    wififile.println(password);
    wififile.println(ip);
    wififile.println(port);
    wififile.println(room);
    wififile.println(checktime);
    wififile.println(rssi);


    wififile.close();

    server.send(200, "text/html", successfullywritten);

    rebootmillis = millis() + 5000;
  });


  server.begin();

  return true;
}

void recheckWiFi() {
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);

  os_timer_disarm(&channelHop_timer);
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);
}
