#include <MicroView.h>


int val;
int x[64];
int dispVal;


void setup() {

  uView.begin();				// start MicroView
  uView.clear(PAGE);			// clear page
  uView.print("ExtraDigit");	// display HelloWorld
  uView.display();

  // read analog input pin
  val = analogRead(A0);
  // map the value to y value of the graph. first 1-10 pixel is taken for number display
  dispVal = map(val, 0, 1023, 48, 10);
   for (int i = 0; i < 63; i++) {
    x[i] = dispVal;
  }

  delay(1000);

  // start serial communication for debugging
  Serial.begin(9600);
}

void loop () {

  // clear the display
  uView.clear(PAGE);
  
  // read analog input pin
  val = analogRead(A0);
  // map the value to y value of the graph. first 1-10 pixel is taken for number display
  dispVal = map(val, 0, 1023, 48, 10);

  // sent the number display position to 0,0
  uView.setCursor(0, 0);
  uView.print(val);  // display val

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

  // print to Serial with Plotter so we can see the sensor value
  serialPlotterView(val);

  delay(20);
}

void serialPlotterView(int num) {
  Serial.print(1023);
  Serial.print(" , ");
  Serial.print(0);
  Serial.print(" , ");
  Serial.println(num);
}

