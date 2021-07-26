
#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID "Morenzoe"
#define WLAN_PASS "123456789"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"

// Using port 8883 for MQTTS
#define AIO_SERVERPORT  8883

// Adafruit IO Account Configuration
#define AIO_USERNAME "morenzoe"
#define AIO_KEY      "aio_UNYx88SUKE81kr3cXy50xt37rjtd"

/************ Global State (you don't need to change this!) ******************/

// WiFiFlientSecure for SSL/TLS support
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// io.adafruit.com root CA
const char* adafruitio_root_ca = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
    "-----END CERTIFICATE-----\n";

/****************************** Feeds ***************************************/
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish Pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Pressure");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish heatc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/heatc");
/*************************** Sketch Code ************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

int led = 2;
int buzz = 4;
int ledh = 12;

unsigned long delayTime;

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.println(F("Adafruit IO MQTTS (SSL/TLS) Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  delay(1000);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Set Adafruit IO's root CA
  client.setCACert(adafruitio_root_ca);

  pinMode (buzz, OUTPUT);
  pinMode (led, OUTPUT);
  pinMode (ledh, OUTPUT);

  
  //while(!Serial);    // time to get serial running
  Serial.println(F("BME280 test"));

  unsigned status;
  
  // default settings
  status = bme.begin(0x76);  
  /*
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
    }
    */
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();
    
}

void danger(){
for (int count = 0;count < 40; count++) {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
}
}

void aman(){
  digitalWrite(buzz, LOW);
for (int n = 0;n < 40; n++) {
    digitalWrite(ledh, HIGH);
    delay(100);
    digitalWrite(ledh, LOW);
    delay(100);
}
}

void ukur(){
  float h = bme.readHumidity();
  float t = bme.readTemperature();
  float p = bme.readPressure();
  if (isnan(h) || isnan(t) || isnan(p)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // Now we can publish stuff!
  Serial.print(F("\nSending val "));
  Serial.print(bme.readTemperature());
  Serial.print(F(" to test feed..."));
  if (! Temperature.publish(bme.readTemperature())) {
    Serial.print(F("Failed"));
  } else {
    Serial.print(F("OK!"));
  }
    
  Serial.print(F("\nSending val "));
  Serial.print(bme.readPressure());
  Serial.print(F(" to fal feed..."));
  if (! Pressure.publish(bme.readPressure())) {
    Serial.print(F("Failed"));
  } else {
    Serial.print(F("OK!"));    
  }
      
  Serial.print(F("\nSending val "));
  Serial.print(bme.readHumidity());
  Serial.print(F(" to lembap feed..."));
  if (! Humidity.publish(bme.readHumidity())) {
    Serial.print(F("Failed"));
  } else {
    Serial.print(F("OK!"));    
  }
  /*

  Serial.print(F("\nSending val "));
  Serial.print(dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false));
  Serial.print(F(" to heatc feed..."));
  if (! heatc.publish(dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false))) {
    Serial.print(F("Failed"));
  } else {
    Serial.print(F("OK!"));    
  }
  */
  
  ukur();
  if ((bme.readTemperature()) >= 34.00){
    digitalWrite(buzz, HIGH);
    danger();
    }
  else {
    aman();
    }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}
