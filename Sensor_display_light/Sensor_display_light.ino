#include <MicroView.h>
#include <Servo.h>

int val;
int x[64];
int angle;
int avg;
int sampleSize = 4; // how many sample one take for avarage. change to bigger number for more smoothing

Servo myservo;

// initially the below number is given as min/max for the sensor. it gets callibrated in setup
int bendSensor_max = 850;
int bendSensor_min = 750;

int sensorPin = A2;
int LEDpin = 5;
int servoPin;

int switchPin = 2;

bool switchState;
bool lastSwitchState;
bool lightOn = false;

long switchMarker;
int state;

int lightState;

bool debugger = false;





//---------------------------------
//adjust the following number
//---------------------------------


int motorPos_max = 170;  // streight position. 180 max
int motorPos_min = 20;  // the smaller number the bigger bend angle

//------------------------------------


void setup() {

  uView.begin();				// start MicroView
  uView.clear(PAGE);			// clear page
  uView.print("ExtraDigit");	// display HelloWorld
  uView.display();

  // set the pinModes
  pinMode(LEDpin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  // if (debugger) {
  // start serial communication for debugging
  Serial.begin(9600);
  //}

  // read analog input pin
  val = analogRead(sensorPin);
  avg = val;

  // initialize the graph value with the reading
  for (int i = 0; i < 63; i++) {
    x[i] = map(val, 0, 1023, 48, 10);
  }

  // set initial avarage and angle
  angle = map(val, 0, 1023, 0, 180);

  // setup servo
  myservo.attach(servoPin);

  // wait 1 second
  // delay(1000);

  // call the calibration
  //  calibration(500);


}

void loop () {

  // read sensor
  val = sensorReading();

  // read switch
  readSwitch();


  //------------------------------
  // motor control related----
  //------------------------------
  // map the bend sensor value to the motor position
  angle = map(val, bendSensor_max, bendSensor_min, motorPos_min, motorPos_max);
  angle = constrain(angle,  motorPos_min, motorPos_max);

  // control the servo motor
  myservo.write(angle);

  //------------------------------
  // light related----
  //------------------------------
  if (lightOn) {
    int brightness = map(val, bendSensor_min, bendSensor_max, 0, 255);
    brightness = constrain(brightness, 0, 255);
    analogWrite(LEDpin, brightness);
  }
  if (!lightOn) {
    analogWrite(LEDpin, 255);  // this is off
  }
  // display the sensor reading and angle of the motor
  microViewDisplay(val, angle);

  // print to Serial so we can see the sensor value on serial. this is or debugging
  //serialPlotterView(val);
  if (debugger) serialPrintDebugger();

  delay(1);
}

void microViewDisplay(int v, int a) {

  //------------------------------
  // microview display related----
  //------------------------------
  // clear the display
  uView.clear(PAGE);

  // display text for sensor input value at y position 0
  uView.setCursor(0, 0);
  uView.print(v);
  // display text for bending angle at y position 10
  uView.setCursor(0, 10);
  uView.print(a);

  // display sensor input as graph
  MicroViewGraph(v);

  // show on the display
  uView.display();

  //------------------------------
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

void readSwitch(){
   switchState = digitalRead(switchPin);
  if (switchState != lastSwitchState) {
    if (switchState == 0) {
      switch (state) {
        case 0:
          switchMarker = millis();
          state = 1; // single click
          break;

        case 1:
          state = 2; // double click
          break;
      }
    }
  }
  lastSwitchState = switchState;


  long currentTime = millis();
  // check the time out, 300ms
  if (  currentTime - switchMarker > 300) {
    if (state == 1) {
      lightOn = !lightOn; // switch on/off the light
      state = 0; // time out
    }
  }

  if (state == 2) {
    // double click, call calibration
    calibration(1000);
    state = 0;
  }
}

void calibration(int calibrationTime) {
  int tempMax = 0;
  int tempMin = 1023;

  // wait for 0.5 second
  delay(500);

  for (int i = 0; i < calibrationTime; i++) {

    val = sensorReading();

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

    if (debugger) {
      Serial.print("sensorVal; ");
      Serial.print(val);
      Serial.println();
    }

    delay (1);
  }

  bendSensor_max = tempMax - (tempMax-tempMin)*0.1;
  bendSensor_min = tempMin + (tempMax-tempMin)*0.2;

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

int sensorReading() {
  // read analog input pin
  int v = analogRead(sensorPin);
  // reverse the reading
  v = map (v, 1023, 0, 0, 1023);
  v = (avg * (sampleSize - 1) + v) / sampleSize;
  avg = v;
  return v;
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

