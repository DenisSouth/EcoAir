
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char *ssid = "myPont";
const char *password = "12345678";
ESP8266WebServer server(80);

void handleRoot() {
	server.send(200, "text/html", "<h1>Hello</h1>");
}

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println("http://192.168.4.1");

	WiFi.softAP(ssid, password);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("IP  " + IP);
	server.on("/", handleRoot);
	server.begin();
}

void loop() {
	server.handleClient();
}
