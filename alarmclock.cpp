// alarmclock.cpp
// Authors: Grant Hillman, Jonas Dickens, Madhurishitha Boddu, Maxwell Evans

#include <LiquidCrystal.h>

struct ClockTime {
  // Clock time
  int deciseconds;
  int seconds;
  int minutes;
  int closeBlindsHour;
  int hours;
  int bell;
  int active;
};

struct PushButton {
  // Button states
  int previousState;
  int currentState;
};

// Create normal and alarm times
ClockTime normal, alarm;

// Create Push Buttons
PushButton setTime, setAlarm, incrementHours, incrementMinutes, snooze;

// Create LCD object
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// state = 0: time, state = 1: set time, state = 2: set alarm
int state;

// State of blinds (0 closed, 1 open)
int open;

// Measure how long loop takes to complete
unsigned long loopStartTime;


void setup() {
  open = 0;
  // LCD display
  lcd.begin(16, 2);   // Set up the number of columns and rows on the LCD

  // Initialize alarm state (Time state default)
  state = 0;

  // Initialize normal time
  normal.deciseconds = 00;
  normal.seconds = 00;
  normal.minutes = 00;
  normal.hours = 00;

  // Initialize alarm time (seconds/milliseconds not used)
  alarm.seconds = 00;
  alarm.minutes = 00;
  alarm.hours = 00;
  alarm.active = 0;

  // Intiailize alarm bell
  normal.bell = 0;

  // Button inputs
  pinMode(13, INPUT); // Set time
  pinMode(10, INPUT); // Set alarm
  pinMode(9, INPUT);  // Increment Hours
  pinMode(8, INPUT);  // Increment Minutes
  pinMode(7, INPUT);  // Snooze
  pinMode(6, OUTPUT); // Motor
  pinMode(1, OUTPUT); // Buzzer
  pinMode(0, OUTPUT); // DC Motor

  // Initialize current button states
  setTime.currentState = digitalRead(13);
  setAlarm.currentState = digitalRead(10);
  incrementHours.currentState = digitalRead(9);
  incrementMinutes.currentState = digitalRead(8);
  snooze.currentState = digitalRead(7);

  // Initialize previous button states
  setTime.previousState = 0;
  setAlarm.previousState = 0;
  incrementHours.previousState = 0;
  incrementMinutes.previousState = 0;
  snooze.previousState = 0;

  // Set the cursor position for each alarm clock value
  lcd.setCursor(6, 1);    
  lcd.print("00");        // Print the seconds value

  lcd.setCursor(3, 1);       
  lcd.print("00");        // Print the minutes value
  lcd.setCursor(5, 1); 
  lcd.print(":");

  lcd.setCursor(0, 1);       
  lcd.print("00");          // Print the hours value
  lcd.setCursor(2, 1); 
  lcd.print(":"); 
  
}

void displayTime(ClockTime time) {
  // Set the cursor position for each alarm clock value
  lcd.setCursor(6, 1);
  if (time.seconds < 10) {
    lcd.print("0");               // Add leading zero 
  }   
  lcd.print(time.seconds);        // Print the seconds value

  lcd.setCursor(3, 1);   
  if (time.minutes < 10) {
    lcd.print("0");               // Add leading zero
  }    
  lcd.print(time.minutes);        // Print the minutes value
  lcd.setCursor(5, 1); 
  lcd.print(":");

  lcd.setCursor(0, 1);
  if (time.hours < 10) {
    lcd.print("0");               // Add leading zero  
  }     
  lcd.print(time.hours);          // Print the hours value
  lcd.setCursor(2, 1); 
  lcd.print(":");   

}

unsigned long flashLED(int pin, int lightState) {
  unsigned long startTime = millis(); // Records the start time
  
  // while (count < flashes) {
  //   digitalWrite(pin, HIGH);          // This will trun on LED
  //   delay(interval / 2);              // Time interval
  //   digitalWrite(pin, LOW);           // Led will trn off
  //   delay(interval / 2);            
  //   count++;                          
  //   }
  digitalWrite(pin, lightState);          // This will toggle LED
  // lightState = !lightState;
  return startTime;

  }

void incrementTime(ClockTime &time) {
    // Increment seconds
    time.deciseconds++;
    if (time.deciseconds >= 10) {
      time.deciseconds = 0;
      time.seconds++;
      if (time.seconds >= 60) {
          time.seconds = 00;
            lcd.setCursor(6, 1);    
            lcd.print("00"); 
          time.minutes++;
          if (time.minutes >= 60) {
              time.minutes = 00;
              lcd.setCursor(3, 1);       
              lcd.print("00");
              time.hours++;
              if (time.hours >= 24) {
                  time.hours = 00;
                  lcd.setCursor(0, 1);       
                  lcd.print("00");
              }
          }
      }
    }
}

bool fallingEdge(PushButton button) {
  if ((button.currentState == 0) && (button.previousState == 1)) {
    return true;
  }
  return false;
}

void buttonIncrementTime(PushButton hoursButton, PushButton minutesButton, ClockTime &time) {
  // Run button time incremenation
  if (fallingEdge(hoursButton)) 
  {
    if (time.hours == 24) 
    {
      time.hours = 0;
    }
    else {
      time.hours++;
    }
  }

  if (fallingEdge(minutesButton)) 
  {
    if (time.minutes == 60) 
    {
      time.minutes = 0;
    }
    else {
      time.minutes++;
    }
  }
}

void resetLCD() {
  lcd.clear();
  lcd.begin(16, 2); // Reinitialize with the same configuration
}


void openBlinds (int pin) {
  lcd.setCursor(0, 1);
  lcd.print("Opening Blinds...");

  analogWrite(pin, 150);
  delay(1000);
  analogWrite(pin, 0);
  delay(1000);
  resetLCD();
  normal.seconds = normal.seconds + 2;
}

void closeBlinds(int pin) {
  lcd.setCursor(0, 1);
  lcd.print("Closing Blinds...");

  analogWrite(pin, 150);
  delay(1000);
  analogWrite(pin, 0);
  delay(1000);
  resetLCD();
  normal.seconds = normal.seconds + 2;
}


void loop() {

unsigned long currentMillis = millis();
static unsigned long lastUpdate = 0;

// Update button states
setTime.currentState = digitalRead(13);
setAlarm.currentState = digitalRead(10);
incrementHours.currentState = digitalRead(9);
incrementMinutes.currentState = digitalRead(8);
snooze.currentState = digitalRead(7);

if (state == 0) // Time state
{ 
  // Run button checks
  if (fallingEdge(setTime)) // Check signal edge of setTime button
  { 
    state = 1;              // Switch to set time state
  }
  else if (fallingEdge(setAlarm)) // Check signal edge of setAlarm button
  {
    state = 2;                    // Switch to set alarm state
  }

  // Check to see if the alarm time matches the normal time
  if (alarm.active && alarm.hours == normal.hours && alarm.minutes == normal.minutes && alarm.seconds == normal.seconds) {
    normal.bell = 1;
  }

  else if (fallingEdge(snooze)) {
    normal.bell = 0;
    digitalWrite(6, LOW);
    digitalWrite(1, LOW);
  }

  else if (normal.bell == 1) {
    digitalWrite(6, HIGH);
    digitalWrite(1, HIGH);
    if (open == 0) {
      openBlinds(0);
      open = 1;
      normal.closeBlindsHour = normal.hours + 12;

    }
  }

  else if (open == 1 && normal.closeBlindsHour == normal.hours) {
    closeBlinds(0);
    open = 0;
  }

  else {
    digitalWrite(6, LOW);
    digitalWrite(1, LOW);   
  }

incrementTime(normal); // Update the normal clock
delay(1);

} 

else if (state == 1) // Set time state
{
  // Run button checks, only allow to go back to time state (no set alarm transition allowed)
  if (fallingEdge(setTime)) // Check signal edge of setTime button
  { 
    state = 0;              // Switch back to time state
  }

  // Run button time incremenation
  buttonIncrementTime(incrementHours, incrementMinutes, normal);
}

else if (state == 2) 
{

  if (fallingEdge(setAlarm)) // Check signal edge of setAlarm button
  { 
    state = 0;               // Switch back to time state
  }

  // Run button time incremenation
  buttonIncrementTime(incrementHours, incrementMinutes, alarm);
  alarm.active = 1;
}

  if (state != 2) {
  displayTime(normal);
  }
  else {
  displayTime(alarm);
  }

  // Update previous button states
  setTime.previousState = setTime.currentState;
  setAlarm.previousState = setAlarm.currentState;
  incrementHours.previousState = incrementHours.currentState;
  incrementMinutes.previousState = incrementMinutes.currentState;
  snooze.previousState = snooze.currentState;
}