
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
//TODO
/*
-polling NTP// alternative js tells the nodmcu the time
-add button inizia
-add button finisci calibrazione
-read wifi ssid pass from sd card
-vibration with mosfet
-nome sd incrementale, possibly with date time (DATA-2016-11-9-16-45-V1.CSV)

*/

/*



*/

void handleRoot();
void handleDati();
void handleAnalysis();
void handleClient();
void drawGraph();
void getInfo();
int sdSetup();
int wifiSetup();
int wireSetup();
void readT12sensor();
void readC7sensor();
void readShoulderSensor();

/* Set these to your desired credentials. */
const char *ssid = "asdShirt";
const char *password = "12345678";

ESP8266WebServer server(80);

MDNSResponder mdns;

int sd_true=0;
const int primary_MPU_address=0x68;
const int secondary_MPU_address=0x69;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double tmp_double;
int16_t AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
double tmp_double1;
int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
double tmp_double2;
int16_t AcX3,AcY3,AcZ3,Tmp3,GyX3,GyY3,GyZ3;

double cifosi = 0;
double scogliosi = 0;
double rotazione = 0;

double cifosi1 = 0;
double scogliosi1 = 0;
double rotazione1 = 0;

int acx_buffer[10];
int index_acx=0;
int acy_buffer[10];
int index_acy=0;
int acz_buffer[10];
int index_acz=0;

int acx_buffer1[10];
int index_acx1=0;
int acy_buffer1[10];
int index_acy1=0;
int acz_buffer1[10];
int index_acz1=0;



int maxNumEntries=3600;
File data;
File network_config;
String filename="data.csv";

/*
red vcc
brown gnd
orange scl
yellow sda
*/

void setup(){

  Serial.begin(115200);
  wifiSetup();

  delay(1000);

  sdSetup();
  wireSetup();


  server.on("/test.svg",drawGraph);

  server.on("/dati", handleDati);
  server.on("/analisi", handleAnalysis);
  server.on("/info", getInfo);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  //NTP poll to know how to name the file

}
int wireSetup(){
  Wire.begin(D1,D2);
  Wire.beginTransmission(primary_MPU_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Wire.beginTransmission(secondary_MPU_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);


}

int wifiSetup(){

  int n = WiFi.scanNetworks();
  Serial.println(n);
  Serial.println("ricerca SSID named wifi");
  boolean connectedToKnown =false;
  for (int i =0; i<n; i++){
    Serial.println(i);
    Serial.println(WiFi.SSID(i));

    if(WiFi.SSID(i) == "Wifi"){

      WiFi.begin("Wifi", "wifidicasa1.");

      while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");

      Serial.println("Server started");

      // Print the IP address
      Serial.println(WiFi.localIP());
      connectedToKnown =true;
    }

    else{

    }
  }
  if(!connectedToKnown){
    Serial.println("init soft AP");
      WiFi.softAP(ssid, password);
      //while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
      delay(5000);
      Serial.println("done");
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);

      Serial.println("Server started");
  }
  if (!mdns.begin("sensore", WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");

  }else{
    Serial.println("mDNS responder started");
  }
  Serial.println(" ");




}


int sdSetup(){
  Serial.print("Initializing SD card...");
  if (!SD.begin(D8)) {//D8
    Serial.println("initialization failed!");
    return 0;
  }
  else{
  Serial.println("initialization done.");
  // Check to see if the file exists:

  }


  Serial.print("sd card done");

}

void getInfo(){
  char testo[2000];
  for(int i =0;i<10;i++){
    sprintf(testo,"Maglietta");
  }
  server.send(200, "text/html", testo);

}

void handleDati(){
  int val = analogRead(0);
  char testo[2000];
  for(int i =0;i<1;i++){
    sprintf(testo,"%d,%d,%d,%d", AcX, AcY, AcX1, AcY1);
  }
  Serial.print("dati forniti :) ");
  server.send(200, "text/html", testo);

}
void handleAnalysis(){
  int val = analogRead(0);
  char testo[2000];
  for(int i =0;i<1;i++){
    sprintf(testo,"%d,%d,%d,%d,%d", cifosi, scogliosi, cifosi1, scogliosi1, cifosi1-cifosi);
  }
  Serial.print("dati forniti :) ");
  server.send(200, "text/html", testo);

}
void handleRoot() {

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char testo[2000];
  for(int i =0;i<1;i++){
    sprintf(testo,"Attivo da %d:%d:%d Stream su sd (1 funziona, 0 non funziona:)%d", hr, min, sec, sd_true);
  }
  Serial.println("diagnostica fornita.");
  server.send(200, "text/html", testo);
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"800\" height=\"900\">\n";
  out += "<rect width=\"800\" height=\"450\" fill=\"rgb(250, 250, 250)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";

  out += "<g stroke=\"blue\">\n";
  int z = acx_buffer[0]*450/18000;
  int prev_y=z;
  for (int i=0; i<9;i++) {
    int current_y = acx_buffer[i]*450/18000;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", i*70, 900/2 - prev_y, (i+1)*70, 900/2 - current_y);
    out += temp;
    prev_y = current_y;
  }
  out += "</g>\n";
  out += "<g stroke=\"black\">\n";
  out += "<line x1=\"0\" y1=\"450\" x2=\"800\" y2=\"450\" stroke-width=\"1\">\n";
  out += "</g>\n";
  out += "</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}

void readT12sensor(){

}
void readC7sensor(){

}
void readShoulderSensor(){


}


int serverTimer=0;
void loop(){
  if(serverTimer==10){
  serverTimer=0;

  void readT12sensor();

  Wire.beginTransmission(primary_MPU_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(primary_MPU_address,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  void readC7sensor();
  Wire.beginTransmission(secondary_MPU_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(secondary_MPU_address,14,true);  // request a total of 14 registers
  AcX1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  //void readShoulderSensor();

  cifosi = (atan2(AcX, AcZ))*90/1.6384;
  scogliosi = ((atan2(AcY, AcZ))*90/1.6384);
  rotazione = (atan2(AcX, AcY))*90/1.6384;
  //Serial.print(","); Serial.print(AcX);
  //Serial.print(","); Serial.print(AcY);
  //Serial.print(","); Serial.print(AcZ);
  tmp_double= (Tmp/340.00)+36.53;

  Serial.print(","); Serial.print(cifosi);
  Serial.print(","); Serial.print(scogliosi);
  //Serial.print(" | Tmp(t) = "); Serial.print(tmp_double);  //equation for temperature in degrees C from datasheet
  //Serial.print(","); Serial.print(GyX);
  //Serial.print(","); Serial.print(GyY);
  //Serial.print(","); Serial.print(GyZ);

  cifosi1 = (atan2(AcX1, AcZ1))*90/1.6384;
  scogliosi1 = ((atan2(AcY1, AcZ1))*90/1.6384);
  rotazione1 = (atan2(AcX1, AcY1))*90/1.6384;
  //Serial.print(","); Serial.print(AcX1);
  //Serial.print(","); Serial.print(AcY1);
  //Serial.print(","); Serial.print(AcZ1);
  tmp_double= (Tmp1/340.00)+36.53;
  Serial.print(","); Serial.print(cifosi1);
  Serial.print(","); Serial.print(scogliosi1);
  //Serial.print(" | Tmp1(t) = "); Serial.print(tmp_double);  //equation for temperature in degrees C from datasheet
  //Serial.print(","); Serial.print(GyX1);
  //Serial.print(","); Serial.print(GyY1);
  //Serial.print(","); Serial.println(GyZ1);



  Serial.print(","); Serial.print(cifosi1-cifosi);


  if(AcX<14000){
    //Serial.print("vibra");
  }

 Serial.println();


  data = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (data) {
    Serial.print("Writing to data.txt...");
    data.print(AcX);data.print(",");
    data.print(AcY);data.print(",");
    data.print(AcZ);data.print(",");
    data.print(tmp_double);data.print(",");
    data.print(GyX);data.print(",");
    data.print(GyY);data.print(",");
    data.print(GyZ);data.print(",");

    data.print(AcX1);data.print(",");
    data.print(AcY1);data.print(",");
    data.print(AcZ1);data.print(",");
    data.print(tmp_double);data.print(",");
    data.print(GyX1);data.print(",");
    data.print(GyY1);data.print(",");
    data.print(GyZ1);data.println(",;");
    // close the file:
    data.close();
    Serial.println("done.");
    sd_true=1;
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening data.txt");
  }

  }else{
    server.handleClient();
    serverTimer+=1;
  }
  delay(200);
}
