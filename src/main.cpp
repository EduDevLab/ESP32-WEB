#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <SPIFFS.h>

const char *ssid = "tuwifijosefierro";
const char *password = "tucontrasenajosefierro";

DHT dht(4, DHT11);
WebServer server(80);

void handleDatos()
{
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (isnan(temp) || isnan(hum))
  {
    server.send(500, "application/json", "{\"error\":\"sensor fallo\"}");
    return;
  }

  String json = "{";
  json += "\"temperatura\":" + String(temp) + ",";
  json += "\"humedad\":" + String(hum);
  json += "}";

  server.send(200, "application/json", json);
}

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  if (!SPIFFS.begin(true))
  {
    Serial.println("Error montando SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/datos", handleDatos);
  server.begin();
}

void loop()
{
  server.handleClient();
}