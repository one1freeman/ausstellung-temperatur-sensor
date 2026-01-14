#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
// with http client
#include <HTTPClient.h>
#include "credentials.h"

#define OW_TEMP 27

OneWire tempWire(OW_TEMP);
DallasTemperature sensors(&tempWire);

HTTPClient client;

bool apConnect() {
    Serial.print("Attempting to connect to: ");
    Serial.println(SSID);

    WiFi.begin(SSID, AP_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        if (WiFi.status() == WL_CONNECT_FAILED)
        {
            Serial.println("Failed to connnect!");
            return false;
        }
        Serial.print(".");
        delay(500);
    }
    Serial.println("Connected to the AP");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI:");
    Serial.println(WiFi.RSSI());
    return true;
}

void setup()
{
    Serial.begin(9600);
    delay(200);

    sensors.begin();
    delay(200);

    WiFi.mode(WIFI_STA);
    Serial.println("Setup finished!");
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (client.begin("http://192.168.4.1/temp"))
        {
            float temp = sensors.getTempCByIndex(0);
            Serial.println(temp);
            int httpCode = client.POST(String(temp));
            if (httpCode > 0) {
                // file found at server
                if (httpCode == HTTP_CODE_OK) {
                    Serial.println("OK!");
                }
            } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", client.errorToString(httpCode).c_str());
            }

            client.end();
            Serial.println("disconnected!");
            delay(3000);
        } else
        {
            Serial.println("Connected to AP but not to Server!");
        }
    } else
    {
        Serial.println("Disconnected from WiFi!");
        if (!apConnect())
        {
            Serial.println("New attempt in 5 seconds.");   
        }
    }
    delay(5000);
}