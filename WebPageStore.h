#ifndef WEBPAGESTORE_H
#define WEBPAGESTORE_H


const char setuppage[] PROGMEM =  R"(<!DOCTYPE HTML>
<html lang="de">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      button{width:11em;height:2.5em}
      body{background: #87cefa; text-align: center;}
      input:invalid {background-color: ivory; border: none; outline: 2px solid red; border-radius: 5px; }
    </style>
    <title>SchemilWIFIRoomCounter</title>
  </head>
  <body>
    <h2>Zugangsdaten</h2>
    <form>
      <p>
        <label>SSID:<br>
          <input name="ssid" placeholder="SSID" required>
        </label>
      </p>
      <p>
        <label>Passwort:<br>
          <input name="password" type="password" pattern="[!-~]{8,64}" placeholder="KEY" required>
        </label>
      </p>
      <p>
        <label>IP:<br>
          <input name="ip" pattern="^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$" placeholder="192.168.XX.XX" required>
        </label>
      </p>
      <p>
        <label>Port:<br>
          <input name="port" placeholder="29000" pattern="^\d+$" required>
        </label>
      </p>
      <p>
        <label>Raum:<br>
          <input name="room" placeholder="Room" required>
        </label>
      </p>
      <p>
        <label>Checkzeit:<br>
          <input name="checktime" placeholder="10 = Alle 10 Minuten" pattern="^\d+$" required>
        </label>
      </p>
      <p>
        <label>Max RSSI (Ohne Minus):<br>
          <input name="rssi" placeholder="80" pattern="^\d+$" required>
        </label>
      </p>
    </form>
    <button>
      Anwenden und Neustarten
    </button>
    <script>
      document.querySelector('button').addEventListener('click', async () =>{
        let elem = document.querySelector('form');
        if(elem.checkValidity() && document.querySelector('[pattern]').checkValidity()){
          let resp = await fetch('/wifisave', {method: 'post', body: new FormData(elem)});
          let json = await resp.json();
          document.body.innerHTML = json;
        }
      });
    </script>
  </body>
</html>)";

const char successfullywritten[] PROGMEM = R"("<h3>Die Zugangsdaten wurden übertragen. Der SchemilWifiProbe-ESP startet in 3 Sekunden neu.</h3>")";

#endif
