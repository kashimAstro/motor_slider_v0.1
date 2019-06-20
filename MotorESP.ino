#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

//192.168.4.1/motor?dir=HIGH&delay=5&steps=200

const char *ssid     = "miscia-motor-slider";
const char *password = "misciamotorslider1234";
ESP8266WebServer server(80);
IPAddress IP;

const int dirPin    = 16;
const int stepPin   = 14;
const int enablePin = 12;

int steps           = 0;
int stepDelay       = 0;
bool dir            = HIGH;
bool enabled        = false;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin,OUTPUT);
  digitalWrite(enablePin, LOW);
  
  delay(1000);
  Serial.begin(9600);

  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRootPath);
  server.on("/motor", handleInit);
  server.on("/disable", handleDisable);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRootPath() {
  String html = "";
  html += "<html>";
  html += "<head>";
  html += "<style>";
  html += "a { text-decoration: none; display: inline-block; padding: 8px 16px; font-size:225px; text-decoration:none; width:50%; height:50%; }";
  html += "a:hover { background-color: #ddd; color: black; font-size:225px; text-decoration:none;  width:50%; height:50%; }";
  html += ".previous { background-color: #f1f1f1; color: black; }";
  html += ".next { background-color: #4CAF50; color: white; }";
  html += ".round { border-radius: 100%; text-align: center; }";
  html += "</style>";
  html += "<script>\n";

  /**************/
  html += "function left() {\n";
  html += "var step = document.getElementById(\"step\").value;\n";
  html += "var delay = document.getElementById(\"delay\").value;\n";
  html += "var xhttp = new XMLHttpRequest();\n";
  html += "xhttp.onreadystatechange = function() {\n";
  html += "  if (this.readyState == 4 && this.status == 200) {\n";
  html += "    document.getElementById(\"info\").innerHTML = this.responseText;\n";
  html += "  }\n";
  html += "};\n";
  html += "xhttp.open(\"GET\", \"http://192.168.4.1/motor?dir=HIGH&delay=\"+delay+\"&steps=\"+step+\"\", true);\n";
  html += "xhttp.send();\n";
  html += "}\n";
  /**************/
  /**************/
  html += "function right() {\n";
  html += "var step = document.getElementById(\"step\").value;\n";
  html += "var delay = document.getElementById(\"delay\").value;\n";
  html += "var xhttp = new XMLHttpRequest();\n";
  html += "xhttp.onreadystatechange = function() {\n";
  html += "  if (this.readyState == 4 && this.status == 200) {\n";
  html += "    document.getElementById(\"info\").innerHTML = this.responseText;\n";
  html += "  }\n";
  html += "};\n";
  html += "xhttp.open(\"GET\", \"http://192.168.4.1/motor?dir=LOW&delay=\"+delay+\"&steps=\"+step+\"\", true);\n";
  html += "xhttp.send();\n";
  html += "\n";
  html += "}\n";
  /**************/
  /**************/
  html += "function enabledDisable() {\n";
  html += "var xhttp = new XMLHttpRequest();\n";
  html += "xhttp.onreadystatechange = function() {\n";
  html += "  if (this.readyState == 4 && this.status == 200) {\n";
  html += "    document.getElementById(\"info\").innerHTML = this.responseText;\n";
  html += "  }\n";
  html += "};\n";
  html += "xhttp.open(\"GET\", \"http://192.168.4.1/disable\", true);\n";
  html += "xhttp.send();\n";
  html += "\n";
  html += "}\n";
  /**************/

  html += "</script>";
  html += "</head>";
  html += "<body>";

  html += "<table border=\"1\" width=\"100%\">";
  html += "<tr><td>";
  html += "<a href=\"#\" onclick=\"left()\" ><div class=\"previous round\">&#8249;</div></a>";
  html += "</td><td>";
  html += "<a href=\"#\" onclick=\"right()\" ><div class=\"next round\">&#8250;</div></a>";
  html += "</td></tr>";
  html += "<tr><td>";
  html += "<span>step:</span><br>";
  html += "<input type=\"number\" id=\"step\" name=\"step\" min=\"1\" max=\"2500000\" value=\"200\">";
  html += "</td><td>";
  html += "<span>delay:</span><br>";
  html += "<input type=\"number\" id=\"delay\" name=\"delay\" min=\"1\" max=\"2500000\" value=\"5\">";
  html += "</td></tr>";
  html += "</table>";
  html += "<div id=\"info\"></div>";
  String snd = (enabled)?"Enabled":"Disabled";
  html += "<button id=\"btn\" type=\"button\" onclick=\"enabledDisable()\">"+snd+"</button>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void handleDisable() {
  enabled =! enabled;
  digitalWrite(enablePin, (enabled)?HIGH:LOW);
  Serial.print("Enabled: ");
  Serial.println(enabled);
  String out = (enabled)?"Enabled":"Disabled";
  server.send(200, "text/plain", out);
}

void handleInit() {
  steps = 0;
  stepDelay = 0;
  String message = "";

  if (server.hasArg("dir")) {
    digitalWrite(dirPin, server.arg("dir") == "HIGH");
    message += "direction: ";
    message += server.arg("dir");
  }
  if (server.hasArg("delay")) {
    stepDelay = (server.arg("delay")).toInt();
    message += " delay: ";
    message += server.arg("delay");
  }
  if (server.hasArg("steps")) {
    steps = (server.arg("steps")).toInt();
    message += " steps: ";
    message += server.arg("steps");
  }
  server.send(200, "text/plain", message);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delay(stepDelay);
    digitalWrite(stepPin, LOW);
    delay(stepDelay);
  }
}
/* end */
