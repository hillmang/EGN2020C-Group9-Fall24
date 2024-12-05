// alarmclock.cpp
// Basic alarm clock state machine intended for an Arduino UNO.

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

// Arduino setup function
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

// Function for displaying time. Takes time structure (normal or alarm time)
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

// Function for flashing an LED. Not included to decrease delay
unsigned long flashLED(int pin, int lightState) {
  unsigned long startTime = millis(); // Records the start time
  
  while (count < flashes) {
    digitalWrite(pin, HIGH);          // This will trun on LED
    delay(interval / 2);              // Time interval
    digitalWrite(pin, LOW);           // Led will trn off
    delay(interval / 2);            
    count++;                          
    }
  digitalWrite(pin, lightState);          // This will toggle LED
  // lightState = !lightState;
  return startTime;

  }

// Function for incrementing time
// Increments in deciseconds for the alarm clock to be more responsive to button input (less delay in main loop)
void incrementTime(ClockTime &time) {
    time.deciseconds++;           // Increment deciseconds
    if (time.deciseconds >= 10) {
      time.deciseconds = 0;
      time.seconds++;             // Increment seconds
      if (time.seconds >= 60) {
          time.seconds = 00;
            lcd.setCursor(6, 1);    
            lcd.print("00");      // Clear LCD seconds
          time.minutes++;         // Increment minutes
          if (time.minutes >= 60) {
              time.minutes = 00;
              lcd.setCursor(3, 1);       
              lcd.print("00");    // Clear LCD minutes
              time.hours++;       // Increment hours
              if (time.hours >= 24) {
                  time.hours = 00;
                  lcd.setCursor(0, 1);       
                  lcd.print("00"); // Clear LCD hours
              }
          }
      }
    }
}

// Function for detecting the falling edge of button input (1 to 0 transition). Detects button presses
bool fallingEdge(PushButton button) {
  if ((button.currentState == 0) && (button.previousState == 1)) { // Return true if 1 to 0 transition
    return true;
  }
  return false;                                                    // Return false otherwise (no button press)
}

// Function for incrementing hours or minutes with respective buttons. Uses falling edge detetction for buttons
void buttonIncrementTime(PushButton hoursButton, PushButton minutesButton, ClockTime &time) {
  // Run button time incremenation
  if (fallingEdge(hoursButton))   // Hours button press
  {
    if (time.hours == 24)         // Reset hours after reaching 24
    {
      time.hours = 0;
    }
    else {
      time.hours++;               // Increment hours
    }
  }

  if (fallingEdge(minutesButton)) // Minutes button press
  {
    if (time.minutes == 60)       // Reset minutes after reaching 60
    {
      time.minutes = 0;         
    }
    else {
      time.minutes++;             // Increment minutes
    }
  }
}

// Function for resetting LCD
void resetLCD() {
  lcd.clear();      // Clear LCD
  lcd.begin(16, 2); // Reinitialize with the same configuration
}

// Function for opening blinds
void openBlinds (int pin) {
  lcd.setCursor(0, 1);
  lcd.print("Opening Blinds...");       // Display blinds are being open

  analogWrite(pin, 150);                // Turn on motor
  delay(1000);
  analogWrite(pin, 0);                  // Turn off motor
  delay(1000);
  resetLCD();                           // Reset LCD
  normal.seconds = normal.seconds + 2;  // Increment seconds to account for the delay while turning the motor
}

// Function for closing blinds
void closeBlinds(int pin) {
  lcd.setCursor(0, 1);
  lcd.print("Closing Blinds...");       // Display blinds are being closed

  analogWrite(pin, 150);                // Turn on motor
  delay(1000);
  analogWrite(pin, 0);                  // Turn off motor
  delay(1000);
  resetLCD();                           // Reset LCD
  normal.seconds = normal.seconds + 2;  // Increment seconds to account for the delay while turning the motor
}

// Main loop function
void loop() {

// Update button states
setTime.currentState = digitalRead(13);
setAlarm.currentState = digitalRead(10);
incrementHours.currentState = digitalRead(9);
incrementMinutes.currentState = digitalRead(8);
snooze.currentState = digitalRead(7);

// Time state
if (state == 0)
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
    normal.bell = 1; // Set the alarm on
  }

  // Check if snooze button is pressed
  else if (fallingEdge(snooze)) {
    normal.bell = 0;      // Set alarm off
    digitalWrite(6, LOW); // Turn off LED
    digitalWrite(1, LOW); // Turn off buzzer
  }

  // Check if alarm is on
  else if (normal.bell == 1) {
    digitalWrite(6, HIGH); // Turn on LED
    digitalWrite(1, HIGH); // Turn on buzzer
    if (open == 0) {       // Open blinds (if blinds are closed)
      openBlinds(0);
      open = 1;            // Set blinds open
      normal.closeBlindsHour = normal.hours + 12; // Set blinds closing time 12 hours later than current time

    }
  }
  
  // Close blinds if closed and if blinds closing time is reached
  else if (open == 1 && normal.closeBlindsHour == normal.hours) { 
    closeBlinds(0);
    open = 0;             // Set blinds closed
  }

  // Set LED and buzzer off
  else {
    digitalWrite(6, LOW);
    digitalWrite(1, LOW);   
  }

incrementTime(normal); // Update the normal clock
delay(100);            // Delay 100 ms (time of decisecond). Allows seconds to increment in real time

} 

// Set time state
else if (state == 1) 
{
  // Run button checks, only allow to go back to time state (no set alarm transition allowed)
  if (fallingEdge(setTime)) // Check signal edge of setTime button
  { 
    state = 0;              // Switch back to time state
  }

  // Run button time incremenation
  buttonIncrementTime(incrementHours, incrementMinutes, normal);
}

// Set Alarm state
else if (state == 2) 
{

  if (fallingEdge(setAlarm)) // Check signal edge of setAlarm button
  { 
    state = 0;               // Switch back to time state
  }

  // Run button time incremenation
  buttonIncrementTime(incrementHours, incrementMinutes, alarm);
  alarm.active = 1; // Set alarm active
}
  // Display alarm time in alarm state, otherwise display normal time
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