    /*
 Controlling large 7-segment displays
 By: Nathan Seidle
 SparkFun Electronics
 Date: February 25th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This code demonstrates how to post two numbers to a 2-digit display usings two large digit driver boards.

 Here's how to hook up the Arduino pins to the Large Digit Driver IN

 Arduino pin 6 -> CLK (Green on the 6-pin cable)
 5 -> LAT (Blue)
 7 -> SER on the IN side (Yellow)
 5V -> 5V (Orange)
 Power Arduino with 12V and connect to Vin -> 12V (Red)
 GND -> GND (Black)

 There are two connectors on the Large Digit Driver. 'IN' is the input side that should be connected to
 your microcontroller (the Arduino). 'OUT' is the output side that should be connected to the 'IN' of addtional
 digits.

 Each display will use about 150mA with all segments and decimal point on.

MODIFICATIONS BY Andres Leon
 - new function that blanks out display. it will do so when non integers are sent to the serial
 - no leading zeroes for single digits

*/

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClockPIN = 6;
byte segmentLatch = 5;
byte segmentDataPIN = 7;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
String inputString = "";         // a string to hold incoming data
String prevInputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  Serial.begin(115200);
  inputString.reserve(2);  //reserve 2 bytes for number to be shown on display
  Serial.println("Large Digit Driver Example");

  pinMode(segmentClockPIN, OUTPUT);
  pinMode(segmentDataPIN, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  digitalWrite(segmentClockPIN, LOW);
  digitalWrite(segmentDataPIN, LOW);
  digitalWrite(segmentLatch, LOW);
  
  BlankDisplay();
}


int number = 0;

void loop()
{
  //showCount(300);
  // print the string when a newline arrives:
  if (stringComplete) {
    inputString.trim();
    if (prevInputString != "" && prevInputString != inputString)
    {
      Serial.print("Print ");
      Serial.println(inputString);
    }    
    
    if (inputString.toInt())
    {
      //Serial.print(inputString.toInt());
      //Serial.println(" - num between 0 and 99");
      char carray[inputString.length() + 1];
      inputString.toCharArray(carray, sizeof(carray));
      float inputNum = atof(carray);
      
      //Serial.print("--- ");
      //Serial.println(inputNum);
      
      //Serial.print("inputNum: ");
      //Serial.println(inputNum);
      showNumber(inputNum);
    }
    else
    {
      //Serial.print("not a number '");
      //Serial.print(inputString);
      //Serial.println("'");
      //Serial.println("setting display blank");
      BlankDisplay();
    }
    
    prevInputString = inputString;
  
    // clear the string:
    inputString = "";
    stringComplete = false;   
    //Serial.println("--------------");
  }
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;
    //Serial.print("x pos: "); Serial.print(x); Serial.print(" - remainder: "); Serial.println(remainder);
    if (x == 1 && remainder == 0)  //if the second number is a zero then don't show it
      BlankDisplay();
    else
      postNumber(remainder, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

  #define a  1<<0
  #define b  1<<6
  #define c  1<<5
  #define d  1<<4
  #define e  1<<3
  #define f  1<<1
  #define g  1<<2
  #define dp 1<<7

  byte segments;
  
  //Serial.print(number);
  //Serial.println(" number between 0 and 9");  
  switch (number)
  {
      case 1 : segments = b | c; break;
      case 2 : segments = a | b | d | e | g; break;
      case 3 : segments = a | b | c | d | g; break;
      case 4 : segments = f | g | b | c; break;
      case 5 : segments = a | f | g | c | d; break;
      case 6 : segments = a | f | g | e | c | d; break;
      case 7 : segments = a | b | c; break;
      case 8 : segments = a | b | c | d | e | f | g; break;
      case 9 : segments = a | b | c | d | f | g; break;
      case 0 : segments = a | b | c | d | e | f; break;
  }
  
  if (decimal) segments |= dp;
  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClockPIN, LOW);
    digitalWrite(segmentDataPIN, segments & 1 << (7 - x));
    digitalWrite(segmentClockPIN, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
  
}

void BlankDisplay()
{
  //Serial.println("blaking display");
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClockPIN, LOW);
    digitalWrite(segmentDataPIN, 0);
    digitalWrite(segmentClockPIN, HIGH); //Data transfers to the register on the rising edge of SRCK    
  }
  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
  
  //Serial.println("display is now blank");  
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void showCount(int timeDelay)
{  //counts to 99 and then starts over
  showNumber(number); //Test pattern
  number++;
  number %= 100; //Reset x after 99

  Serial.println(number); //For debugging

  delay(timeDelay);
}
