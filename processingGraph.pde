import processing.serial.*;


Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port
int x;
int y;
int z;
int x1;
int y1;
int z1;

double[] buffer;

double angle_x;
double angle_y;
double angle_z;
void setup()
{
  // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  buffer=new double[7];

  for(int i =0; i< Serial.list().length;i++){
    if(Serial.list()[i]=="/dev/ttyUSB0"){
      
      
    }
   println(Serial.list()[i]); 
   println(i);
  }
  String portName = Serial.list()[32]; 
  myPort = new Serial(this, portName, 115200);
  size(600, 400,P3D);  // size always goes first!
  surface.setResizable(true);
}

void draw()
{
  background(205,205,205); 
  if ( myPort.available() > 0) 
  {  // If data is available,
  val = myPort.readStringUntil('\n');         // read it and store it in val
  } 
  println(val); //print it out in the console
  String[] list = split(val, ',');
  if(list!=null){
    for(int i =0;i<list.length;i++){
       println(list[i]);
       if(list[i].equals("Writing to data.txt...done.")){
       }
       else if(list[i].equals("")){
       }
       else{
        double number = int(list[i]);
        //line(0,200+10*i,0+number/100,200+10*i);
        buffer[i]=number;
       }
    }
  }
  for(int i =0;i<18;i++){
     line(i*10+300-90,0, i*10+300-90,10); 
    
  }
  for(int i =0;i<6;i++){
    line(300,40+10*i,300+(float)buffer[i]/163,40+10*i);
    
  }
  translate(300, 200, 0); 
        rotateX(atan((float)buffer[3]/16384));
        rotateY(atan((float)buffer[1]/16384));
        rotateZ(atan((float)buffer[2]/16384));
        noFill();
        box(120, 80, 12);
        /*
         translate(0, 80, 0); 
        rotateX(atan((float)buffer[3]/16384));
        rotateY(atan((float)buffer[4]/16384));
        rotateZ(atan((float)buffer[5]/16384));
        noFill();
        box(120, 80, 12);
  */
  delay(100);
}
