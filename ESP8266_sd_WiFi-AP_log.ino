// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "buon_giorno";
const char *password = "12345678";

ESP8266WebServer server(80);
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

 

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double tmp_double;
int16_t AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
int16_t AcX3,AcY3,AcZ3,Tmp3,GyX3,GyY3,GyZ3;
int maxNumEntries=3600;
File data;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);

  Serial.print("Initializing SD card...");
  if (!SD.begin(D8)) {//D8
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  // Check to see if the file exists:
  if (SD.exists("data.txt")) {
    Serial.println("data.txt exists.");
    data = SD.open("data.txt", FILE_WRITE);
    data.println("x,y,z,t,r,p,w;");
  }

  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  
  
  
  server.on("/dati", handleDati);


}
void handleDati(){
  int val = analogRead(0);
  char testo[200];
  sprintf(testo,"%d,%d,%d,%d,%d,%d", AcX,AcY,AcZ,GyX,GyY,GyZ);
  
  server.send(200, "text/html", testo);
  
}
void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,

  "<html>\
    <head>\
      <meta http-equiv='refresh' content='5'/>\
      <title>ESP8266 Demo</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>Hello from ESP8266!</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <img src=\"/test.svg\" />\
    </body>\
  </html>",

    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
}

void loop(){
  server.handleClient();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  
  Serial.print("AcX(x) = "); Serial.print(AcX);
  Serial.print(" | AcY(y) = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  tmp_double= (Tmp/340.00)+36.53;
  Serial.print(" | Tmp(t) = "); Serial.print(tmp_double);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX(r) = "); Serial.print(GyX);
  Serial.print(" | GyY(p) = "); Serial.print(GyY);
  Serial.print(" | GyZ(w) = "); Serial.println(GyZ);
  if(AcX<14000){
    Serial.print("vibra");
  }
  data = SD.open("data.txt", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (data) {
    Serial.print("Writing to data.txt...");
    data.print(AcX);data.print(",");
    data.print(AcY);data.print(",");
    data.print(AcZ);data.print(",");
    data.print(tmp_double);data.print(",");
    data.print(GyX);data.print(",");
    data.print(GyY);data.print(",");
    data.print(GyZ);data.println(";");
    // close the file:
    data.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening sensorData.txt");
  }
  delay(1000);
}
