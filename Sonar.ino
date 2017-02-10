
#include <Stepper.h>
#include <Arduino.h>


const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins D5...D8:
Stepper myStepper(stepsPerRevolution, D5, D7, D6, D8);
const int triggerPort = D2;
const int echoPort = D3;


void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
  Serial.begin(115200);
  pinMode(triggerPort, OUTPUT);
  pinMode(echoPort, INPUT);

  //delay so that the motor won't start as the nodemcu is plugged
  delay(5000);

  // do 200 samples, move 1/200 of revolution each time
  for(int i=0;i<204;i++){
    digitalWrite( triggerPort, LOW );digitalWrite( triggerPort, HIGH );delayMicroseconds( 10 );digitalWrite( triggerPort, LOW );
    long interval = pulseIn( echoPort, HIGH );
    long distance = 0.034 * interval / 2;

    //dopo 38ms è fuori dalla portata del sensore
    if( interval > 38000 ){
    Serial.println("10000");
    }
    else{
    Serial.print(distance);
    Serial.println("");
    }

      myStepper.step(10);
      delay(100);



  }

}

void loop() {



}
