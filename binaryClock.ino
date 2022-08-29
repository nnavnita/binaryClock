#include <TimeLib.h>

#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define TIME_REQUEST 7 // ASCII bell character requests a time sync message

int led_hr[] = {2, 3, 4, 5};
int led_mnt[] = {6, 7, 8, 9, 10, 11};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int i;
  for (i = 0; i < 6; i = i + 1) {
    if (i < 4) {
      pinMode(led_hr[i], OUTPUT);
    }
    pinMode(led_mnt[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() == timeNotSet)
    Serial.println("waiting for sync message");
  else
    binaryClock();
  delay(1000);
}

void binaryClock() {
  int hr = hour();
  int mnt = minute();
  int i;
  for (i = 3; i > -1; i = i - 1) {
    int bin_hr = (int) (0.5 + pow(2, i));
    if (hr < bin_hr) {
      continue;
    }
    digitalWrite(led_hr[i], HIGH);
    hr = hr - bin_hr;
  }
  for (i = 5; i > -1; i = i - 1) {
    int bin_mnt = (int) (0.5 + pow(2, i));
    if (mnt < bin_mnt) {
      continue;
    }
    digitalWrite(led_mnt[i], HIGH);
    mnt = mnt - bin_mnt;
  }
}

void processSyncMessage() {
 // if time sync available from serial port, update time and return true
 while (Serial.available() >= TIME_MSG_LEN ) { // time message consists of header & 10 ASCII digits
   char c = Serial.read() ;
   Serial.print(c);
   if ( c == TIME_HEADER ) {
     time_t pctime = 0;
     for (int i = 0; i < TIME_MSG_LEN - 1; i++) {
       c = Serial.read();
       if ( c >= '0' && c <= '9') {
        pctime = (10 * pctime) + (c - '0') ; // convert digits to a number
       }
     }
     setTime(pctime); // Sync Arduino clock to the time received on the serial port
   }
 }
}
