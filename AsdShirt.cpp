
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Mpu6050.h>
#include <Wire.h>

//TODO
/*
-polling NTP// alternative js tells the nodemcu the time
-add button inizia
-add button finisci calibrazione
-read wifi ssid pass from sd card
-vibration with mosfet
-nome sd incrementale, possibly with date time (DATA-2016-11-9-16-45-V1.CSV)
*/


void handleRoot();
void handleDati();
void handleAnalysis();
void handleClient();
void drawGraph();
void sendImage();
void sdcard();
void getInfo();
void timestamp();
void tara();
void analisiTarata();
int sdSetup();
int wifiSetup();
int wireSetup();
void serverSetup();
void readT12sensor();
void readC7sensor();
void readShoulderSensor();
void vibra();

/* Set these to your desired credentials. */
const char *ssid = "asdShirt";
const char *password = "12345678";

ESP8266WebServer server(80);

MDNSResponder mdns;

int sd_true=0;
const int primary_MPU_address=0x68;
const int secondary_MPU_address=0x69;

//Mpu6050 sensore1(D1,D2, primary_MPU_address);

int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double tmp_double;
int AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
double tmp_double1;
int AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
double tmp_double2;
int AcX3,AcY3,AcZ3,Tmp3,GyX3,GyY3,GyZ3;

int AcX_riposo, AcY_riposo, AcZ_riposo;
int AcX1_riposo, AcY1_riposo, AcZ1_riposo;

double cifosi = 0;
double scogliosi = 0;
double rotazione = 0;

double cifosi1 = 0;
double scogliosi1 = 0;
double rotazione1 = 0;

double cifosi_riposo = 0;
double scogliosi_riposo = 0;
double rotazione_riposo = 0;

double cifosi1_riposo = 0;
double scogliosi1_riposo = 0;
double rotazione1_riposo = 0;

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

int serverTimer=0;
/*
red vcc
brown gnd
orange scl
yellow sda
*/

void setup(){

  Serial.begin(115200);
  wifiSetup();
  sdSetup();
  wireSetup();
  serverSetup();

}


void loop(){
  if(serverTimer==100){
  serverTimer=0;

  void readT12sensor();
  Wire.beginTransmission(primary_MPU_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(primary_MPU_address,14,1);  // request a total of 14 registers
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
  Wire.requestFrom(secondary_MPU_address,14,1);  // request a total of 14 registers
  AcX1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  //void readShoulderSensor();
  Wire.beginTransmission(secondary_MPU_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(secondary_MPU_address,14,1);  // request a total of 14 registers
  AcX2=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)

  cifosi = (atan2(AcX, AcZ))*90/1.6384;
  scogliosi = ((atan2(AcY, AcZ))*90/1.6384);
  rotazione = (atan2(AcX, AcY))*90/1.6384;
  cifosi_riposo = (atan2(AcX_riposo, AcZ_riposo))*90/1.6384;
  scogliosi = ((atan2(AcY_riposo, AcZ_riposo))*90/1.6384);
  rotazione = (atan2(AcX_riposo, AcY_riposo))*90/1.6384;
  //Serial.print(","); Serial.print(AcX);
  //Serial.print(","); Serial.print(AcY);
  //Serial.print(","); Serial.print(AcZ);
  tmp_double= (Tmp/340.00)+36.53;

  Serial.print(","); Serial.print(cifosi-cifosi_riposo);
  Serial.print(","); Serial.print(scogliosi-scogliosi_riposo);
  //Serial.print(" | Tmp(t) = "); Serial.print(tmp_double);  //equation for temperature in degrees C from datasheet
  //Serial.print(","); Serial.print(GyX);
  //Serial.print(","); Serial.print(GyY);
  //Serial.print(","); Serial.print(GyZ);

  cifosi1 = (atan2(AcX1, AcZ1))*90/1.6384;
  scogliosi1 = ((atan2(AcY1, AcZ1))*90/1.6384);
  rotazione1 = (atan2(AcX1, AcY1))*90/1.6384;
  cifosi1 = (atan2(AcX1_riposo, AcZ1_riposo))*90/1.6384;
  scogliosi1 = ((atan2(AcY1_riposo, AcZ1_riposo))*90/1.6384);
  rotazione1 = (atan2(AcX1_riposo, AcY1_riposo))*90/1.6384;
  //Serial.print(","); Serial.print(AcX1);
  //Serial.print(","); Serial.print(AcY1);
  //Serial.print(","); Serial.print(AcZ1);
  tmp_double= (Tmp1/340.00)+36.53;
  Serial.print(","); Serial.print(cifosi1-cifosi1_riposo);
  Serial.print(","); Serial.print(scogliosi1-scogliosi1_riposo);
  //Serial.print(" | Tmp1(t) = "); Serial.print(tmp_double);  //equation for temperature in degrees C from datasheet
  //Serial.print(","); Serial.print(GyX1);
  //Serial.print(","); Serial.print(GyY1);
  //Serial.print(","); Serial.println(GyZ1);

  Serial.print(","); Serial.print(AcX2);
  Serial.print(","); Serial.print(AcX3);


  Serial.print(","); Serial.print(cifosi1-cifosi);


  if(AcX<14000){
    //Serial.print("vibra");
  }

 Serial.println();


  data = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (data) {
    //Serial.print("Writing to data.txt...");
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
    //Serial.println("done.");
    sd_true=1;
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening data.txt");
  }

  }else{
    server.handleClient();
    serverTimer+=1;
  }
  delay(20);
}

void serverSetup(){
  server.on("/test.svg",drawGraph);
  server.on("/dati", handleDati);
  server.on("/analisi", handleAnalysis);
  server.on("/info", getInfo);
  server.on("/sdcard", sdcard);
  server.on("/vibra", vibra);
  server.on("/sendimage", sendImage);
  server.on("/timestamp", timestamp);
  server.on("/analisitarata",analisiTarata);
  server.on("/tara", tara);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  //NTP poll to know how to name the file
}

int wireSetup(){

  Wire.begin(D3,D4);
  Wire.beginTransmission(primary_MPU_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

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
    else if(WiFi.SSID(i) == "Edisu Piemonte"){
      if(!connectedToKnown){
        WiFi.begin("Edisu Piemonte", "");

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
void sendImage(){
  File f = SD.open("bridge.png", FILE_READ);
  server.streamFile(f, "image/png");
  f.close();
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
    double difference = cifosi1-cifosi1_riposo-cifosi-cifosi_riposo;
    sprintf(testo,"%d,%d,%d,%d,%d", cifosi-cifosi_riposo, scogliosi-scogliosi_riposo, cifosi1-cifosi1_riposo, scogliosi1_riposo, difference);
  }
  String out = "";
  double gobba= cifosi1-cifosi1_riposo-cifosi-cifosi_riposo;
  String g = String(gobba);
  //out += ""+g;
  out +=String(cifosi-cifosi_riposo)+","+String(scogliosi-scogliosi_riposo)+","+String(cifosi1-cifosi1_riposo)+","+String(scogliosi1-scogliosi1_riposo)+","+g;

  Serial.print("analisi fornita :) ");
  server.send(200, "text/html", out);

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
void sdcard(){
  String out = "TODO get arbitrary file from sd card";
  server.send(200, "text/html", out);
}
void timestamp(){
  String out = "TODO grazie per avermi detto l'ora!";
  server.send(200, "text/html", out);
}
void tara(){
  String out = "il dato attuale viene registrato come tara\n";
  AcX_riposo=AcX;
  AcY_riposo=AcY;
  AcZ_riposo=AcZ;
  AcX1_riposo=AcX1;
  AcY1_riposo=AcY1;
  AcZ1_riposo=AcZ1;
  out += "dati registrati come tara\n";
  out = out +"X "+ AcX_riposo + ",Y "+AcY_riposo+",Z "+AcZ_riposo+"\n";
  out = out +"X1 "+AcX1_riposo+",Y1 "+AcY1_riposo+",Z1 "+AcZ1_riposo+"\n";
  server.send(200, "text/html", out);
}
void analisiTarata(){
  String out = "dati netti\n";
  AcX=AcX-AcX_riposo;
  AcY=AcY-AcY_riposo;
  AcZ=AcZ-AcZ_riposo;
  AcX1=AcX1-AcX1_riposo;
  AcY1=AcY1-AcY1_riposo;
  AcZ1=AcZ1-AcZ1_riposo;
  out += "dati registrati come tara\n";
  out = out +"X "+ AcX + ",Y "+AcY+",Z "+AcZ+"\n";
  out = out +"X1 "+AcX1+",Y1 "+AcY1+",Z1 "+AcZ1+"\n";
  server.send(200, "text/html", out);
}
int timer_vibra=0;
void vibra(){
  digitalWrite(D0, HIGH);
  delay(1000);
  digitalWrite(D0, LOW);

}

void readT12sensor(){

}
void readC7sensor(){

}
void readShoulderSensor(){


}
