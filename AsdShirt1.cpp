
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
#include <mpu.h>
#include <wifiSetup.cpp>

//tara lavoro cartaceo X 16344,Y 62488,Z 1844 X1 15000,Y1 63604,Z1 58464


//TODO
/*
-polling NTP// alternative js tells the nodemcu the time
-read wifi ssid pass from sd card
-nome sd incrementale, possibly with date time (DATA-2016-11-9-16-45-V1.CSV)
*/
/* Set these to your desired credentials. */
const char *ssid = "asdShirt";
const char *password = "12345678";

ESP8266WebServer server(80);

MDNSResponder mdns;
int pin_vibratore = D1;
int sd_true=0;
long number_lines=0;
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

double cifosi_tarata = 0;

double cifosi1 = 0;
double scogliosi1 = 0;
double rotazione1 = 0;

double cifosi1_tarata = 0;

double cifosi_dorsale = 0;
double cifosi_dorsale_tarata = 0;

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
int giorno;
int mese;
int anno;
int ora;
int minuto;
int secondo;

int loop_milliseconds=20;

int timer_polling=50;//5/sec

int warning_before_vibration=5*60*3;

int vibrazione_abilitata=0;


int serverTimer=0;
/*
red vcc
brown gnd
orange scl
yellow sda
*/

/*
SPI
CS D8
SCK D5
MOSI D7
MISO D6

I2C
D2 SCL
D3 SDA


Vibratorino
D1


*/

void setup(){

  Serial.begin(115200);
  pinMode(pin_vibratore, OUTPUT);
  digitalWrite(pin_vibratore, HIGH);
  delay(100);
  digitalWrite(pin_vibratore, LOW);
  delay(100);
  digitalWrite(pin_vibratore, HIGH);
  delay(300);
  digitalWrite(pin_vibratore, LOW);
  wifiSetup();
  sdSetup();
  wireSetup();
  serverSetup();

  Wire.beginTransmission(secondary_MPU_address);
  Wire.write(0x6A);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(secondary_MPU_address,14,1);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  String out = out + AcX+" "+AcY+" "+AcZ+" "+Tmp+" "+GyX+" "+GyY+" "+GyZ;
  Serial.println(out);



  digitalWrite(pin_vibratore, HIGH);
  delay(300);
  digitalWrite(pin_vibratore, LOW);
  delay(100);
  digitalWrite(pin_vibratore, HIGH);
  delay(100);
  digitalWrite(pin_vibratore, LOW);

}


int millisecondi_readSensor=0;
int millisecondi_calcoli=0;
int millisecondi_prints=0;
int millisecondi_sdCard=0;
int millisecondi_server=0;
int timerAvvisoCifosi=0;
void loop(){
  if(serverTimer==timer_polling){
  serverTimer=0;
  timerAvvisoCifosi-=10;

  millisecondi_readSensor=millis();
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

  if(AcX>32000){AcX=-65336+AcX;}
  if(AcY>32000){AcY=-65336+AcY;}
  if(AcZ>32000){AcZ=-65336+AcZ;}
  if(AcX1>32000){AcX1=-65336+AcX1;}
  if(AcY1>32000){AcY1=-65336+AcY1;}
  if(AcZ1>32000){AcZ1=-65336+AcZ1;}
  //void readShoulderSensor();

  millisecondi_readSensor=millis()-millisecondi_readSensor;

  millisecondi_calcoli=millis();
  //CALCOLI
  cifosi = (atan2(AcX, AcZ))*90/1.6384;
  scogliosi = ((atan2(AcY, AcZ))*90/1.6384);
  rotazione = (atan2(AcX, AcY))*90/1.6384;
  cifosi_riposo = (atan2(AcX_riposo, AcZ_riposo))*90/1.6384;
  scogliosi_riposo = ((atan2(AcY_riposo, AcZ_riposo))*90/1.6384);
  rotazione_riposo = (atan2(AcX_riposo, AcY_riposo))*90/1.6384;
  tmp_double= (Tmp/340.00)+36.53;

  cifosi1 = (atan2(AcX1, AcZ1))*90/1.6384;
  scogliosi1 = ((atan2(AcY1, AcZ1))*90/1.6384);
  rotazione1 = (atan2(AcX1, AcY1))*90/1.6384;
  cifosi1_riposo = (atan2(AcX1_riposo, AcZ1_riposo))*90/1.6384;
  scogliosi1_riposo = ((atan2(AcY1_riposo, AcZ1_riposo))*90/1.6384);
  rotazione1_riposo = (atan2(AcX1_riposo, AcY1_riposo))*90/1.6384;
  tmp_double1= (Tmp1/340.00)+36.53;

  cifosi_dorsale = cifosi1-cifosi;
  cifosi_dorsale_tarata = (cifosi1-cifosi1_riposo) - (cifosi-cifosi_riposo);
  //FINE CALCOLI
  millisecondi_calcoli=millis()-millisecondi_calcoli;

  millisecondi_prints=millis();
  boolean rawData=true;
  if(rawData){
    String printLine="";
    printLine = printLine + ","+AcX+","+AcY+","+AcZ;
    //printLine = printLine + ","+tmp_double;
    //printLine = printLine + ","+GyX+","+GyY+","+GyZ;
    printLine = printLine + ","+AcX1+","+AcY1+","+AcZ1;
    //printLine = printLine + ","+tmp_double1;
    //printLine = printLine + ","+GyX1+","+GyY1+","+GyZ1;
    Serial.println(printLine);
  }
  boolean analisis=true;
  if(analisis){
    String printLine="";
    printLine = printLine + ","+cifosi+","+scogliosi+","+cifosi1+","+scogliosi1+","+cifosi_dorsale;
    Serial.println(printLine);
  }
  boolean taratura=false;
  if(taratura){
    String printLine="";
    printLine = printLine+ ","+cifosi_dorsale_tarata;
    Serial.println(printLine);
  }

  millisecondi_prints=millis()-millisecondi_prints;

  if(vibrazione_abilitata==1){
    if(cifosi_dorsale_tarata>20){
      warning_before_vibration-=1;
      if(warning_before_vibration<0){
        digitalWrite(pin_vibratore, HIGH);
        delay(200);
        digitalWrite(pin_vibratore, LOW);
        Serial.println("vibrato");
      }
      Serial.println("avviso");
    }
    else{
      if(warning_before_vibration<5*60*3){
        warning_before_vibration+=1;
      }
    }
  }



  millisecondi_sdCard = millis();
  data = SD.open(filename, FILE_WRITE);
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
    data.print(GyZ1);data.println(",");
    // close the file:
    data.close();
    //Serial.println("done.");
    sd_true=1;
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening data.txt");
  }
  number_lines=number_lines++;
  millisecondi_sdCard=millis()-millisecondi_sdCard;
  String out = out+"," +millisecondi_readSensor + "," + millisecondi_calcoli+ "," + millisecondi_prints+ "," +millisecondi_sdCard;
  Serial.println(out);

  }else{
    server.handleClient();
    serverTimer+=1;
  }
  delay(loop_milliseconds);
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
  server.on("/showposition", showPosition);
  server.on("/vibrazioneon", vibrazioneOn);
  server.on("/vibrazioneoff", vibrazioneOff);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  //NTP poll to know how to name the file
}

int wireSetup(){

  Wire.begin(D2,D3);
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
    else if(WiFi.SSID(i) == "asdTelecom-29579675"){
      if(!connectedToKnown){
        //rete dello studio di Zignin
        WiFi.begin("asdTelecom-29579675", "twBu6ABH0iXDVtYw6iIhkJpk");

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
  data = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (data) {
    //Serial.print("Writing to data.txt...");
    data.print("AcX");data.print(",");
    data.print("AcY");data.print(",");
    data.print("AcZ");data.print(",");
    data.print("tmp_double");data.print(",");
    data.print("GyX");data.print(",");
    data.print("GyY");data.print(",");
    data.print("GyZ");data.print(",");

    data.print("AcX1");data.print(",");
    data.print("AcY1");data.print(",");
    data.print("AcZ1");data.print(",");
    data.print("tmp_double");data.print(",");
    data.print("GyX1");data.print(",");
    data.print("GyY1");data.print(",");
    data.print("GyZ1");data.println(",");
    // close the file:
    data.close();
    //Serial.println("done.");
    sd_true=1;
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening data.txt");
  }
  number_lines=number_lines++;



  }


  Serial.print("sd card done");

}
void sendImage(){
  File f = SD.open("bridge.png", FILE_READ);
  server.streamFile(f, "image/png");
  f.close();
}
void showPosition(){
  if(cifosi<40){
    File f = SD.open("pos01.png", FILE_READ);
    server.streamFile(f, "image/png");
  }else if(cifosi<60){
    File f = SD.open("pos02.png", FILE_READ);
    server.streamFile(f, "image/png");
  }else{
    File f = SD.open("pos03.png", FILE_READ);
    server.streamFile(f, "image/png");
  }

}
void getInfo(){
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  String out = "";

  out = out +"attivo da "+hr+":"+min+":"+sec+", ";
  if(sd_true==1){
    out +="SD card attiva";
  }
  else{
    out +="errore inizializzazione SD card";
  }
  out +=" numero rilevamenti: "+number_lines;

  Serial.print("info fornite :) ");
  server.send(200, "text/html", out);
}
void vibrazioneOn(){

  Serial.print("vibrazione on");
  vibrazione_abilitata=1;
  server.send(200, "text/html", "vibrazione on");
}
void vibrazioneOff(){

  Serial.print("vibrazione off");
  vibrazione_abilitata=0;
  server.send(200, "text/html", "vibrazione off");
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
  out = out+"server args"+server.args();
  out = out+" giorno "+server.arg(0);
  out = out+" mese "+server.arg(1);
  out = out+" anno "+server.arg(2);
  Serial.println(out);

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
  Serial.println("tara impostata");
  server.send(200, "text/html", out);
}
void analisiTarata(){
  String out = "";

  out = out +""+cifosi_dorsale_tarata;
  server.send(200, "text/html", out);
}
int timer_vibra=0;
void vibra(){
  digitalWrite(pin_vibratore, HIGH);
  delay(500);
  digitalWrite(pin_vibratore, LOW);
  Serial.print(" vibrato ");
 server.send(200, "text/html", "vibrato");
}

void readT12sensor(){

}
void readC7sensor(){

}
void readShoulderSensor(){


}
