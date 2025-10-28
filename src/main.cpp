#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include "credentials.h"

#define OW_TEMP 17

OneWire tempWire(OW_TEMP);
DallasTemperature sensors(&tempWire);

WiFiClient client(80);

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
        if (client.connect("192.168.4.1", 80))
        {
            float temp = sensors.getTempCByIndex(0);
            client.print(String(temp).c_str());
            client.println("$POST");
            delay(3000);
            client.stop();
            Serial.println(temp);
            Serial.println("disconnected!");
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