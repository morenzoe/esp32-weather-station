
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
#define AIO_KEY      "aio_ctxH90ZUchd17uYoN1tNRib4TbE6"

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
Adafruit_MQTT_Publish TemperatureC = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TemperatureC");
Adafruit_MQTT_Publish TemperatureF = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TemperatureF");
Adafruit_MQTT_Publish Pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Pressure");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish HeatIdx = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HeatIdx");
Adafruit_MQTT_Publish Power = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Power");
Adafruit_MQTT_Publish DateTime = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/DateTime");
/*************************** Sketch Code ************************************/

// BME280 sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

// Heat Index
#define hi_coeff1 -42.379
#define hi_coeff2   2.04901523
#define hi_coeff3  10.14333127
#define hi_coeff4  -0.22475541
#define hi_coeff5  -0.00683783
#define hi_coeff6  -0.05481717
#define hi_coeff7   0.00122874
#define hi_coeff8   0.00085282
#define hi_coeff9  -0.00000199

// Actuator
int led = 2;
int buzz = 4;
int ledh = 12;

// Deep Sleep
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */

void setup() {
  Serial.begin(9600);
  delay(10);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

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

  unsigned status;
  
  // default settings
  status = bme.begin(0x76);  

    MQTT_connect();

    // Now we can publish stuff!

    // Power (On)
    Serial.print(("\nTurning on..."));
    if (! Power.publish(1)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("Power On!"));
    }

    delay(2000);

    // Temperature (C)
    Serial.print(F("\nSending val "));
    float tempC = bme.readTemperature();
    Serial.print(tempC);
    Serial.print(F(" to TemperatureC feed..."));
    if (! TemperatureC.publish(tempC)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("OK!"));
    }

    delay(2000);

    // Temperature (F)
    Serial.print(F("\nSending val "));
    float tempF = tempC*9/5+32;
    Serial.print(tempF);
    Serial.print(F(" to TemperatureF feed..."));
    if (! TemperatureF.publish(tempF)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("OK!"));
    }

    delay(2000);

    // Pressure (mbar)
    Serial.print(F("\nSending val "));
    float pressure = bme.readPressure() / 100.0F;
    Serial.print(pressure);
    Serial.print(F(" to Pressure feed..."));
    if (! Pressure.publish(pressure)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("OK!"));    
    }

    delay(2000);

    // Humidity (% RH)
    Serial.print(F("\nSending val "));
    float humid = bme.readHumidity();
    Serial.print(humid);
    Serial.print(F(" to Humidity feed..."));
    if (! Humidity.publish(humid)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("OK!"));    
    }

    delay(2000);

    // Heat Index
    Serial.print(F("\nSending val "));
    float heatIdx = HeatIndex(tempF, humid);
    Serial.print(heatIdx);
    Serial.print(F(" to HeatIdx feed..."));
    if (! HeatIdx.publish(heatIdx)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("OK!"));    
    }

    delay(2000);
  
    readSensor();
    if (heatIdx >= 103.00){
      digitalWrite(buzz, HIGH);
      danger();
      }
    else {
      safe();
      }

    // Power (Off)
    Serial.print(("\nTurning off..."));
    if (! Power.publish(0)) {
      Serial.print(F("Failed"));
    } else {
      Serial.print(F("Power Off!"));
    }

    Serial.println("\nGoing to sleep now");
    Serial.flush(); 
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void danger(){
for (int count = 0;count < 10; count++) {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
}
}

void safe(){
  digitalWrite(buzz, LOW);
for (int n = 0;n < 10; n++) {
    digitalWrite(ledh, HIGH);
    delay(100);
    digitalWrite(ledh, LOW);
    delay(100);
}
}

void readSensor(){
  float h = bme.readHumidity();
  float c = bme.readTemperature();
  float f = bme.readTemperature();
  float p = bme.readPressure() / 100.0F;
  if (isnan(h) || isnan(c) || isnan(p) || isnan(f)) {
    Serial.println(F("Failed to read from BME280 sensor!"));
    return;
  }
}

float HeatIndex
(
  float temperature,
  float humidity
)
{
  float heatIndex(NAN);

  if ( isnan(temperature) || isnan(humidity) ) 
  {
    return heatIndex;
  }

  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  if (temperature <= 40) 
  {
    heatIndex = temperature;  //first red block
  }
  else
  {
    heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (humidity * 0.094)); //calculate A -- from the official site, not the flow graph

    if (heatIndex >= 79) 
    {
      /*
      * calculate B  
      * the following calculation is optimized. Simply spoken, reduzed cpu-operations to minimize used ram and runtime. 
      * Check the correctness with the following link:
      * http://www.wolframalpha.com/input/?source=nav&i=b%3D+x1+%2B+x2*T+%2B+x3*H+%2B+x4*T*H+%2B+x5*T*T+%2B+x6*H*H+%2B+x7*T*T*H+%2B+x8*T*H*H+%2B+x9*T*T*H*H
      */
      heatIndex = hi_coeff1
      + (hi_coeff2 + hi_coeff4 * humidity + temperature * (hi_coeff5 + hi_coeff7 * humidity)) * temperature
      + (hi_coeff3 + humidity * (hi_coeff6 + temperature * (hi_coeff8 + hi_coeff9 * temperature))) * humidity;
      //third red block
      if ((humidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      {
        heatIndex -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);
      } //fourth red block
      else if ((humidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      {
        heatIndex += (0.02 * (humidity - 85.0) * (87.0 - temperature));
      }
    }
  }
  
  return heatIndex; //fifth red block
  
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  
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
