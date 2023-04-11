# ESP8266ProbeCounter

## Installation

**1. Schritt:**   
ESP8266 in der Arduino-IDE installieren  

**2. Schritt:**   
Board auswählen  

**3. Schritt:**   
Code kompilieren und hochladen  

**4. Schritt:**  
Seriellen Monitor öffnen (auf 115200 Baud) und das Board resetten (Neustarten). Dort steht die DeviceID.  

**5. Schritt:**  
Nach einem Netzwerk mit dem Namen WifiProberSetup-XXXXXX suchen, wobei XXXXXX für die GeräteID steht.  

**6. Schritt:**  
Mit dem Netzwerk verbinden und die Webseite [http://192.168.4.1](http://192.168.4.1) öffnen.  

**7. Schritt:**  
Die Einstellungen Konfigurieren:  
SSID - Der Name von dem Steuernetzwerk. Hierbei muss bedacht werden, dass das Gerät nur 2,4 GHZ unterstützt.  
Passwort - Das Passwort für das Steuernetzwerk.  
IP - Die IP von dem Steuerserver. Diese IP muss vom Steuernetzwerk erreichbar sein.  
Port - Der Port vom Steuerserver. Dieser muss vom Steuernetzwerk erreichbar sein.  
Raum - Der Raumname, in dem der ESP8266 steht.  
Checkzeit - Die Zeit, in die der ESP8266 die Daten zum Server schickt in Minuten. Standard: 10  
Max RSSI - Der Maximale RSSID-Wert (Ohne Minus). Dieser muss bestimmt werden, in dem Man einen hohen Wert einträgt und in den Seriellen Monitor schaut.  

**8. Schritt**  
Auf `Anwenden und Neustarten` klicken, um die Änderungen anzuwenden.  

## Zurücksetzen / Einstellungen ändern

Um das Gerät zurückzusetzen und das Web-Panel zu starten reicht es, den FLASH-Button an D0 zu betätigen und für 10 Sekunden zu halten.

## Server  
[ESP8266ProbeCounterServer](https://github.com/schemil053/ESP8266ProbeCounterServer)

## TODO:

Multi-ESP-Rooming. Dafür werden 3-8 ESP8266-Chips in einem Raum verteilt. Jeder ESP8266 sendet MAC und RSSI an den Server, dieser kann errechnen, wo genau sich die Geräte aufhalten.  
