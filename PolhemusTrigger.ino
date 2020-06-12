/* This code is meant to adapt an ATMega1284p to work as a sketchy 
   serial to TTL DIO interface for LabVIEW. The intended purpose is
   to use serial out from LabVIEW to trigger P&O frame output from
   a Polhemus Viper configured in External Sync Mode. 

   The Polhemus Viper accepts a ground pulse to pins A7 and B7 on the 
   External Sync USB-C connector [this needs to be verified]. My guess 
   is that the Polhemus operates on 3.3V logic, and A7 and B7 are pulled 
   HIGH by internal pullup resistors. 

   This code has the ATMega wait for a predefined serial character (triggerChar). 
   Upon reception, the ATMega brings the output pin (triggerPin) LOW for the 
   duration specified by triggerDuration, followed by returning the pin to
   the HIGH state.

   Since the ATMega1284p is a 5V device, it is reccommended that a small
   signal diode be used in series between the output pin and the External
   Sync pins of the Polhemus Viper to prevent current flow from the ATMega
   to the Polhemus when the output pin is held HIGH.

   This code was written on June 12, 2020 by Keiran Cantilina.

   Last revision 6-12-2020 11:53AM by Keiran Cantilina
*/

// Choose triggering hardware pin
int triggerPin = 5;

// Choose trigger duration (microseconds, minimum of 10)
int triggerDuration = 20;

// Choose triggering serial char
char triggerChar = 'R';

// Declare globals
String inputString = "";
bool serialTrigger = false;

// Setup routine runs before first time loop()
void setup() {
  // Initialize serial connection (baud rate can be increased if necessary)
  Serial.begin(9600);
  
  // Reserve 200 bytes for the inputString (this is overkill but why not)
  inputString.reserve(200);
  
  // Initialize trigger signal pin and set HIGH
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, HIGH);
}

// Runs continuously
void loop() {
  if (serialTrigger) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(triggerDuration);
    digitalWrite(triggerPin, HIGH);
    serialTrigger = false;
  }
}

// Runs after every time loop() runs; does nothing if there's no serial input
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == triggerChar) {
      serialTrigger = true;
    }
  }
}
