#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define WIFI_SSID "POCO M6 Pro 5G"
#define WIFI_PASSWORD "12344321"

const int GPS_RX_PIN = 4;
const int GPS_TX_PIN = 2;

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
const char* serverName = "https://script.google.com/macros/s/AKfycbzPmt_Aq2FajHi89F9kKvVcyokQTqoLzUeCyo_uN7H2zR2cvNUBjB9KPZU-jechysLPCA/exec";

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println("GPS module is ready...");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      float altitude = gps.altitude.meters();
      float speed = gps.speed.kmph();
      int satellites = gps.satellites.value();
      String date = String(gps.date.month()) + "/" + String(gps.date.day()) + "/" + String(gps.date.year());
      String time = String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());

      Serial.print("Latitude: ");
      Serial.println(latitude, 6);
      Serial.print("Longitude: ");
      Serial.println(longitude, 6);
      Serial.print("Altitude: ");
      Serial.println(altitude);
      Serial.print("Speed: ");
      Serial.println(speed);
      Serial.print("Satellites: ");
      Serial.println(satellites);
      Serial.print("Date: ");
      Serial.println(date);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.println();

      Serial.print("Connecting to server: ");
      Serial.println(serverName);
      http.begin(serverName); 
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      String httpRequestData = "latitude=" + String(latitude, 6) + 
                               "&longitude=" + String(longitude, 6) + 
                               "&altitude=" + String(altitude) + 
                               "&speed=" + String(speed) + 
                               "&satellites=" + String(satellites) + 
                               "&date=" + date + 
                               "&time=" + time;
      
      int httpResponseCode = http.POST(httpRequestData);
      
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.print("Server response: ");
        Serial.println(response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
    }
  } else {
    Serial.println("Error in WiFi connection");
  }
  
  delay(1000);
}
