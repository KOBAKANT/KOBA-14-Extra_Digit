#include <MicroView.h>
#include <Servo.h>

int val;
int x[64];
int dispVal;

int angle;
int Avg;
int sampleSize = 5;

Servo myservo;


// adjust the following number--------

int bendSensor_max = 850;
int bendSensor_min = 750;

int motorPos_max = 170;  // streight position. 180 max
int motorPos_min = 20;  // the smaller number the bigger bend angle

//------------------------------------


void setup() {

  uView.begin();				// start MicroView
  uView.clear(PAGE);			// clear page
  uView.print("ExtraDigit");	// display HelloWorld
  uView.display();

  // read analog input pin
  val = analogRead(A2);
  // map the value to y value of the graph. first 1-10 pixel is taken for number display
  dispVal = map(val, 0, 1023, 48, 10);
  for (int i = 0; i < 63; i++) {
    x[i] = dispVal;
  }

  // set initial avarage and angle
  angle = map(val, 0, 1023, 0, 180);
  myservo.attach(6);

  delay(1000);

  // start serial communication for debugging
  Serial.begin(9600);
}

void loop () {

  // clear the display
  uView.clear(PAGE);

  // read analog input pin
  val = analogRead(A2);
  // reverse the reading and scale
  val = map (val, 1023, 0, 0, 1023);

// take avarage of sample size. currently disabled
//  Avg = (val + Avg * (sampleSize - 1)) / sampleSize;
//  val = Avg;
//----------------------------
 

  //------------------------------
  // motor control related----
  //------------------------------
  // map the bend sensor value to the motor position
  angle = map(val, bendSensor_max, bendSensor_min, motorPos_min, motorPos_max);
  angle = constrain(angle,  motorPos_min, motorPos_max);

  // control the servo motor
  myservo.write(angle);

   //------------------------------
  // microview display related----
  //------------------------------
  // map the value to y value of the graph. first 1-10 pixel is taken for number display
  dispVal = map(val, 0, 1023, 48, 10);

  // sent the number display position to 0,0
  uView.setCursor(0, 0);
  uView.print(val);  // display val
  uView.setCursor(0, 10);
  uView.print(angle);  // display val

  // microview display resolution is 64x48
  for (int i = 0; i < 63; i++) {
    // pass the last y value to one position before
    x[i] = x[i + 1];
    uView.pixel(i, x[i]);
  }
  // set the newest value y position to the left most pixel
  x[63] = dispVal;
  uView.pixel(63, x[63]);

  // show on the display
  uView.display();

  //------------------------------

  // print to Serial so we can see the sensor value
  //serialPlotterView(val);
  serialPrintDebugger();

  delay(20);
}

void serialPlotterView(int num) {
  Serial.print(1023);
  Serial.print(" , ");
  Serial.print(0);
  Serial.print(" , ");
  Serial.println(num);
}

void serialPrintDebugger() {
  Serial.print("sensorVal; ");
  Serial.print(val);
  Serial.print("\t motorAngle; ");
  Serial.print(angle);
  Serial.println();
}

