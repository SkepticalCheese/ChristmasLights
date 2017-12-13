/* 
 * TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
 T1357041600  
 *
 * A Processing example sketch to automatically send the messages is included in the download
 * On Linux, you can use the commands below:
 
stty -F /dev/ttyUSB0 9600
date +T%s\n > /dev/ttyUSB0

 * This can also be done interactively
screen /dev/ttyUSB0 9600
 
 * 
 * for testing: 
 * 
 *  00:28 = T1479626883n
 *  06:28 = T1479648483n
 *  07:28 = T1479652083n
 *  14:28 = T1479677283n
 *  20:28 = T1479698883n
 */ 
 
#include <TimeLib.h>

#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message
 
const unsigned long TIME_ZONE_SEC = -25200L; // UTC 7 hours ahead of MT

const unsigned long TIME_ON  = 180000L;
const unsigned long TIME_OFF = 70000L;

#define PIN_ON 6
#define PIN_OFF 7

//#define TEST

#ifdef TEST
  const unsigned long interval = 2000L; // 2 secs
#else
  const unsigned long interval = 600000L; // 10 min
#endif

int lightsOn = 0;
long nowInt = 0;

void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  digitalWrite(LED_BUILTIN, LOW); // LED off if synced
  
  pinMode(PIN_ON,  OUTPUT);
  pinMode(PIN_OFF, OUTPUT);
    
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
  Serial.println(TIME_ON);
  Serial.println(TIME_OFF);
}

void loop(){    
  if (Serial.available()) {
    processSyncMessage();
  }

  if (timeStatus()== timeNotSet) {
    digitalWrite(LED_BUILTIN, HIGH); // LED on needs refresh
    delay(1000);
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // LED off if synced
    nowInt = getTimeInt();

#ifdef TEST
    digitalClockDisplay();
    Serial.println(nowInt);
#endif

    if (lightsOn) {
       if ((nowInt > TIME_OFF) && (nowInt < TIME_ON)) {
        pressButton (PIN_OFF);
        lightsOn = 0;
        Serial.println("OFF");
       }
    } else {
       if ((nowInt > TIME_ON) || (nowInt < TIME_OFF)) {
        pressButton (PIN_ON);
        lightsOn = 1;
        Serial.println("ON");
       }      
    }
    delay(interval);
  }

}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt() + TIME_ZONE_SEC;
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
       Serial.println("Sync message received");
       
       digitalClockDisplay();
     }
  }
}

void pressButton (int pin) {

  digitalWrite(pin, HIGH);   // presses the button
  delay(200);                // waits for 0.2 seconds
  digitalWrite(pin, LOW);    // releases the button
  delay(1000);               // waits for a second

}

unsigned long int getTimeInt() {
  return (10000L* hour() + 100L* minute() + 1L*second() );
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
