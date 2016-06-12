
/* Author Tran Hai Dang
*/
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h>
#include <TimerOne.h>           // Avaiable from http://www.arduino.cc/playground/Code/Timer1
#define ONE_WIRE_BUS 9 // Data wire is plugged into pin 13 on the Arduino
LiquidCrystal_I2C lcd(0x27,16,2); 
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices 
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.
#define butonup  4                 // first button at pin 4
#define butondown  5                // second button at pin 5
#define butonmode  6                  //third buton ar pin 6                                                                                                                                                                                                                                                                                                                                                       // third button at pin 6
int AC_pin = 3;                    // Output to Opto Triac
int p_m, m;                       //prrvious millis and millis (mills is the total time of program from beginning)
volatile boolean zero_cross=0;  // Boolean to store a "switch" to tell us if we have crossed zero
volatile int i=0;               // Variable to use as a counter
int dim = 128;  //128                // Dimming level (0-128)  0 = ON, 128 = OFF
int pas = 6;                    // step for count in controlPower
int inc= 1;                     //increase/decrease step in controlTemp
int freqStep = 75;              // This is the delay-per-brightness step in microseconds for 50Hz
int setTemp=30; // initial setTemp 
int dim2 = 0;
void detectZero_cross();
byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};// create a temperature character

void setup() {
  Serial.begin(9600);
  pinMode(butonup, INPUT);                          // set buton pin as input
  pinMode(butondown, INPUT);                        // set buton pin as input
  pinMode(butonmode,INPUT_PULLUP) ;                 //set button pin as input
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  attachInterrupt(0, detectZero_cross, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(checkDim, freqStep);      // Use the TimerOne Library to attach an interrupt
                                    
 lcd.init();   // Intro LCD 
 lcd.backlight(); 
 lcd.clear();               // clear the screen
 lcd.setCursor(2, 0);               // put cursor at colon 0 and row 0
 lcd.print("Capstone design ");     // print a text
 lcd.setCursor(5, 1);               // put cursor at colon 0 and row 1
 lcd.print("Project");             // print a text
 delay (1000);
 lcd.clear();                     // clear the screen
 lcd.setCursor(0, 0);             // put cursor at colon 1 and row 0
 lcd.print("Infrared heating");      // print a text
 lcd.setCursor(3, 1);             // put cursor at colon 0 and row 1
 lcd.print("Controller");         // print a text
 delay (2000);
 lcd.createChar(1, degree);
 lcd.clear();                     // clear the screen
 sensors.begin();  
}
void loop() {
digitalWrite(butonup, HIGH);
digitalWrite(butondown, HIGH);
int mode = digitalRead(butonmode);
int t=readTemp();
if (mode == HIGH)
{
  controlTemp();
  if (digitalRead(butonup) == LOW){setTemp -= 1;}
  if (digitalRead(butondown) == LOW){setTemp += 1;}
  lcd.setCursor(0, 0);
  lcd.print("setTemp is: ");
  lcd.print(setTemp);
  lcd.write(1);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Temp is: ");
  lcd.print(t);
  lcd.write(1);
  lcd.print("C");
if (digitalRead(butonup)==LOW)  
{while ((digitalRead(butonup))==LOW);
  // waiting little bit... 
} 
if (digitalRead(butondown)==LOW){
while (digitalRead(butondown)==LOW); 
  // waiting little bit...   
	}
}
else if (mode == LOW){
  controlPower();
  //converse dim to Power
  dim2 = 255 - 2 * dim;
  if (dim2 < 0) {dim2 = 0;}
   lcd.setCursor(0, 0); 
  lcd.print("Power is:  %    ");
  lcd.setCursor(0, 0); 		// put cursor at colon 0 and row 0
  lcd.print("Power is: "); // print a text
  lcd.print(100 - 100 * (255 - dim2) / 255);
  lcd.print("%"); 			// print 
  lcd.setCursor(0, 1); 		// put cursor at colon 0 and row 1
  lcd.print("Temp is: "); 	// print a text
  lcd.print(t) ;
  lcd.write(1);
  lcd.print("C"); 	
  if (digitalRead(butonup)==LOW)  
{while ((digitalRead(butonup))==LOW);
  // waiting little bit... 
} 
if (digitalRead(butondown)==LOW){
while (digitalRead(butondown)==LOW); 
  // waiting little bit...   
	}     
}
}
void detectZero_cross() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);
}
// Turn on the TRIAC at the appropriate time
void checkDim() {                   
  if(zero_cross == true) {              
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH);     // turn on light       
        i=0;                         // reset time step counter                         
      zero_cross=false;              // reset zero cross detection
    } 
    else {
      i++;                          // increment time step counter                     
    }                                
  }    
} 
                 
//Read instant temperature function
int readTemp()
{
  sensors.requestTemperatures();
  int a = round(sensors.getTempCByIndex(0));
  return a;
}
void controlTemp() {	
  int t = readTemp();
  int delta_temp = setTemp - t;
  if (delta_temp >= 2)
  {
    if (dim > 5)
    {
      p_m = millis();
      while (1 > 0) {
      m = millis();
      if (m - p_m > 00) { dim = dim - inc; break; }
      }
      if (dim < 0) { dim = 0;}
     }
    }
  else if (delta_temp <= -2)
  {
    if (dim < 127)
    {
      p_m = millis();
      while (1 > 0) {
      m = millis();
      if (m - p_m > 100) { dim = dim + inc; break; }
		  }
      if (dim > 127){ dim = 128;}
	   }
     }
}
int controlPower(){	
        if (digitalRead(butonup) == LOW)
		{
			if (dim < 127){dim = dim + pas;}
			if (dim > 127){ dim = 128;}
		}
	if (digitalRead(butondown) == LOW)
		{
			if (dim > 0){dim = dim - pas;}
			if (dim < 0){dim = 0;}
		}
		    return dim;
                      }



