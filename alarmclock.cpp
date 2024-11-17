// alarmclock.cpp
// Authors: Grant Hillman

#include <LiquidCrystal.h>

struct ClockTime {
  // Clock time
  int centiseconds;
  int seconds;
  int minutes;
  int hours;
};

struct PushButton {
  // Button states
  int previousState;
  int currentState;
};

// Create normal and alarm times
ClockTime normal, alarm;

// Create Push Buttons
PushButton setTime, setAlarm, incrementHours, incrementMinutes, reset;

// Create LCD object
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// state = 0: time, state = 1: set time, state = 2: set alarm
int state;

// Measure how long loop takes to complete
unsigned long loopStartTime;


void setup() {
  // LCD display
  lcd.begin(16, 2);   // Set up the number of columns and rows on the LCD

  // Initialize alarm state (Time state default)
  state = 0;

  // Initialize normal time
  normal.centiseconds = 00;
  normal.seconds = 00;
  normal.minutes = 00;
  normal.hours = 00;

  // Initialize alarm time (seconds/milliseconds not used)
  alarm.minutes = 00;
  alarm.hours = 00;

  // Button inputs
  pinMode(13, INPUT); // Set time
  pinMode(10, INPUT); // Set alarm
  pinMode(9, INPUT);  // Increment Hours
  pinMode(8, INPUT);  // Increment Minutes
  pinMode(7, INPUT);  // Reset

  // Initialize current button states
  setTime.currentState = digitalRead(13);
  setAlarm.currentState = digitalRead(10);
  incrementHours.currentState = digitalRead(9);
  incrementMinutes.currentState = digitalRead(8);
  reset.currentState = digitalRead(7);

  // Initialize previous button states
  setTime.previousState = 0;
  setAlarm.previousState = 0;
  incrementHours.previousState = 0;
  incrementMinutes.previousState = 0;
  reset.previousState = 0;

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

void incrementTime(ClockTime &time) {
    // Increment seconds
    time.centiseconds++;
    if (time.centiseconds >= 100) {
      time.centiseconds = 0;
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


void loop() {

loopStartTime = millis(); // Record the start time of the loop

// Update button states
setTime.currentState = digitalRead(13);
setAlarm.currentState = digitalRead(10);
incrementHours.currentState = digitalRead(9);
incrementMinutes.currentState = digitalRead(8);
reset.currentState = digitalRead(7);
  
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

  incrementTime(normal);
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
  // Run button checks, only allow to go back to time state (no set time transition allowed)
  if (fallingEdge(setAlarm)) // Check signal edge of setAlarm button
  { 
    state = 0;               // Switch back to time state
  }

  // Run button time incremenation
  buttonIncrementTime(incrementHours, incrementMinutes, alarm);
}

  if (state != 2) {
  displayTime(normal);
  }
  else {
  displayTime(alarm);
  }

  // Calculate elapsed time and adjust delay
  unsigned long elapsedTime = millis() - loopStartTime;
  if (elapsedTime < 1000) {
    delay(10 - elapsedTime); // Wait for the remainder of 1 second
  }

  // Update previous button states
  setTime.previousState = setTime.currentState;
  setAlarm.previousState = setAlarm.currentState;
  incrementHours.previousState = incrementHours.currentState;
  incrementMinutes.previousState = incrementMinutes.currentState;
}