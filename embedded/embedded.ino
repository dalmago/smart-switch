#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define DEBUG 1

#define RELAY_PIN 12
#define LED_PIN 2

const char* ssid = "MY SSID";
const char* password = "MY PASSWORD";

static char relay_state;
unsigned long t;

WiFiServer server(8080);
WiFiClient clientServer;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, 0);
  relay_state = 0;

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);

#if DEBUG
  Serial.begin(115200);
  Serial.println("Booting");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
#if DEBUG
    Serial.println("Connection Failed! Rebooting...");
#endif
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  ArduinoOTA.setHostname("OTA hostname");
  ArduinoOTA.setPassword("OTA PASSWD");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
#if DEBUG
    Serial.println("Start updating " + type);
#endif
  });
  ArduinoOTA.onEnd([]() {
#if DEBUG
    Serial.println("\nEnd");
#endif
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#if DEBUG
    Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
#endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
#if DEBUG
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
#endif
  });
  ArduinoOTA.begin();
  server.begin();
  digitalWrite(LED_PIN, 0);

#if DEBUG
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
  t = millis();
}

static int i = 0;

void loop() {
  ArduinoOTA.handle();

  if (clientServer = server.available()) {
    String req = clientServer.readStringUntil('\r');
    delay(200);
    clientServer.flush();

    if (req == "toggle") {
      toggleSwitch();
    }
    clientServer.stop();
  }

  if (millis() - t > 3000) {
    t = millis();
    toggleLed();
  }
}

void toggleLed() {
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}

void toggleSwitch() {
  relay_state ^= 1;
  digitalWrite(RELAY_PIN, relay_state);
}

