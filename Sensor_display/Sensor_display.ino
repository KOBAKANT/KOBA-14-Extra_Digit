#include <MicroView.h>
#include <Servo.h>

int val;
int x[64];
int angle;
int Avg;

Servo myservo;

// initially the below number is given as min/max for the sensor. it gets callibrated in setup
int bendSensor_max = 850;
int bendSensor_min = 750;


//---------------------------------
//adjust the following number
//---------------------------------

int sampleSize = 5; // how many sample one take for avarage. change to bigger number for more smoothing

int calibrationTime=500; // currently 5 seconds. Adjust the length

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
  // initialize the graph value with the reading
  for (int i = 0; i < 63; i++) {
    x[i] = map(val, 0, 1023, 48, 10);
  }

  // set initial avarage and angle
  angle = map(val, 0, 1023, 0, 180);
  myservo.attach(6);

  delay(1000);

// call the calibration
  calibration();

  // start serial communication for debugging
  Serial.begin(9600);
}

void loop () {

  

  // read analog input pin
  val = analogRead(A2);
  // reverse the reading and scale
  val = map (val, 1023, 0, 0, 1023);

  // take avarage of sample size. currently disabled
  Avg = (val + Avg * (sampleSize - 1)) / sampleSize;
  val = Avg;
  


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
  // clear the display
  uView.clear(PAGE);
  
  // display text for sensor input value at y position 0
  uView.setCursor(0, 0);
  uView.print(val);
  // display text for bending angle at y position 10
  uView.setCursor(0, 10);
  uView.print(angle);

  // display sensor input as graph
  MicroViewGraph(val);

  // show on the display
  uView.display();

  //------------------------------

  // print to Serial so we can see the sensor value on serial. this is or debugging
  //serialPlotterView(val);
  serialPrintDebugger();

  delay(10);
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
  Serial.print("\t min; ");
  Serial.print(bendSensor_min);
  Serial.print("\t max; ");
  Serial.print(bendSensor_max);
  Serial.println();
}

void calibration() {
  int tempMax = 0;
  int tempMin = 1023;

  for (int i = 0; i < calibrationTime; i++) {
    // read analog input pin
    val = analogRead(A2);
    // reverse the reading and scale
    val = map (val, 1023, 0, 0, 1023);

    // register the value as max if it is bigger than current max
    if (val > tempMax) {
      tempMax = val;
    }
    // register the value as min if it is smaller than current min
    if (val < tempMin) {
      tempMin = val;
    }

    //----------------
    // display related
    //----------------
    // clear the display
    uView.clear(PAGE);
    // display text for sensor input value at y position 0
    uView.setCursor(0, 0);
    uView.print(val);
    // display theat it is in calibration
    uView.setCursor(0, 10);
    uView.print("calibrating");
    // display sensor input as graph
    MicroViewGraph(val);
    // show on the display
    uView.display();
    //----------------

    delay (10);
  }

  bendSensor_max = tempMax;
  bendSensor_min = tempMin;

  //----------------
  // end of calibration display
  //----------------
  // clear the display
  uView.clear(PAGE);
  
  // display the end messgae with min and max value
  uView.setCursor(0, 0);
  uView.print("calibration end!");
  uView.setCursor(0, 10);
  uView.print("min: ");
  uView.setCursor(6, 10);
  uView.print(bendSensor_min);
  uView.setCursor(0, 20);
  uView.print("max: ");
  uView.setCursor(6, 20);
  uView.print(bendSensor_max);
  
  // display sensor input as graph
  MicroViewGraph(val);
  
  // show on the display
  uView.display();
//----------------
  
  // pause for 1 second
  delay (1000);

}



void MicroViewGraph(int value) {
  // map the value to y value of the graph. first 1-10 pixel is taken for number display
  int dispVal = map(value, 0, 1023, 48, 10);

  // microview display resolution is 64x48
  for (int i = 0; i < 63; i++) {
    // pass the last y value to one position before
    x[i] = x[i + 1];
    uView.pixel(i, x[i]);
  }
  // set the newest value y position to the left most pixel
  x[63] = dispVal;
  uView.pixel(63, x[63]);

}

