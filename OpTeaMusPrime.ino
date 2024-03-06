// External includes
#include <Servo.h> // Arduino default servo motor library
#include <AccelStepper.h> // Arduino non-default stepper motor library with acceleration and deceleration methods
#include <LiquidCrystal.h> // Arduino default LCD display library
#include <Bounce2.h> // Button debouncing library
#include <NewPing.h> // Ultrasonic sensor library
#include <EncoderStepCounter.h> // Rotary encoder library

#define ENCODER_PIN1 3
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 2
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)

// Define Digital IO pin numbers - Skip Pin 13 if possible

	// Servo pins
const int pivotServoPin = 4; // PWM Pin
const int grabberServoPin = 5; // PWM Pin

	// User Input
const int loadButtonPin = 22; // Pulled up and debounced in setup
const int nextButtonPin = 23; // Pulled up and debounced in setup
  
  // Create "Bounce2" button objects for debouncing buttons
Bounce2::Button loadButton = Bounce2::Button();
Bounce2::Button nextButton = Bounce2::Button();
const int debounceInterval = 10; // Button debounce interval in milliseconds

	// Bool Inputs & Sensors
const int ultrasonicTrig = 45; // Ultrasonic sensor trigger pin
const int ultrasonicEcho = 46; // Ultrasonic sensor echo pin

	// Bool Outputs
const int heatingCoil = 47;
const int waterPump = 48;
const int airPump = 49;

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7) - See appendix below for LCD Pin wiring and assignments
LiquidCrystal lcd(30, 31, 32, 33, 34, 35);

// Define Analog IO Pins
const int temperatureSensor = A0; // NTC Thermistor on voltage divider
const int waterReservoir = A1;  // Water level probes inside cold water reservoir tank (outside tube 2 wires)
const int waterFill = A2;  // Water level probe inside boiler for regular size
const int waterFillMax = A3; // Water level probe inside boiler for max size

// NTC Thermistor red to 5v, black to junction of 5k resistor, junction to A0, 5k resistor to ground
const float Rref = 5000.0;  // Reference resistance
const float nominal_temeprature = 25.0;  // Nominal temperature in Celsius
const float nominal_resistance = 50000.0;  // Nominal resistance at nominal temperature (ohms)
const float beta = 3950.0;  // Beta value of the NTC thermistor

// Calculate temp C from NTC Thermistor
float calculateTemperature(int temperatureSensor) {
  float R = Rref * (1023.0 / (float)temperatureSensor - 1.0); // Calculate NTC resistance
  float T = 1.0 / (1.0 / (nominal_temeprature + 273.15) + log(R / nominal_resistance) / beta);
  T -= 273.15; // convert absolute temp to C
  return T;
}

// Define other constants
const int dispenseDuration = 10000;  // Air pump avtivation time to dispense all hot water from boiler to cup (in milliseconds)
const int generalDelay = 15; // Update this to adjust the general delay time for all functions
const int servoDelay = 30; // Update this to adjust the slow movement of servos
const int steepTimeAdjustInterval = 30000; // Adjust steep time by this increment in +/- milliseconds
const int shortWait = 100; // Some delay variables
const int medWait = 1000; // Some delay variables
const int longWait = 5000; // Some delay variables

// Create Servo and Stepper objects
Servo pivotServo; // Create pivotServo object using Servo library class
Servo grabberServo; // Create grabberServo object using Servo library class
AccelStepper elevatorRack(AccelStepper::FULL4WIRE, 6, 7, 8, 9); // Create AccelStepper object called "elevatorRack"
NewPing sonar(ultrasonicTrig, ultrasonicEcho); // Create a NewPing object for ultrasonic sensor

const int elevatorRackLimitSwitch = 10; // Limit switch pulled high in setup
const float ballscrewPitch = 8.0; // ballscrewPitch in millimeters
const float stepsPerRevolution = 200.0; // Number of steps per full revolution

// Calculate steps required for given distance in centimeters (hence why x10)
int calculateSteps(float distanceCm) {
  return static_cast<int>((distanceCm * 10 / ballscrewPitch) * stepsPerRevolution);
}

// Word substitutions for pivotServo positions.
const byte pivotOffset = 0; // pivotServo gets out of alignment. Plus value is servo more towards wood plank (opposite for grabber angle)
const byte NORTH = 8+pivotOffset; // For pivotServo pointing straight up
const byte EAST  = 45+pivotOffset; // For pivotServo pointing to the right
const byte SOUTH = 87+pivotOffset; // For pivotServo pointing straight down - This is the expected initial position and index pivot arm to be pointing down
const byte SEAST = 65+pivotOffset; // For pivotServo down and right
const byte NEAST = 23+pivotOffset; // For pivotServo up and right

// Word substitutions for grabberServo positions
const byte CLOSE = 90; // Grabber servo closed position (90 deg)
const byte OPEN = 0; // Grabber servo open position (0 deg)


// Define Tea Types
struct teaRecipe {
  const char* name;
  unsigned long time;
  int temp;
};

teaRecipe teaParams[] = {
  {"Green Tea", 120000, 78},   // Index 0
  {"White Tea", 150000, 78},   // Index 1
  {"Oolong Tea", 180000, 90},  // Index 2
  {"Black Tea", 240000, 98},   // Index 3
  {"Herbal Tea", 300000, 99},  // Index 4
  // Future option:   {"Hot Water", 0, 99},  // Index 5
};


unsigned long finalSteepTime = 0;  // Variable to store new steeping time from progAdjust function
const int numTeas = sizeof(teaParams) / sizeof(teaParams[0]);  // Calculate size of teaRecipe array index
unsigned int selectedTeaTime = 0;
unsigned int selectedTeaTemp = 0;
char selectedTeaName[15];
int currentRecipeIndex = 0;
// Create instance for one full step encoder
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2, HALF_STEP);

byte selectedCupSize = 0; // Variable to store whcih size the user selects (0 is small, 1 is large)




//					MAIN CODE STARTS HERE
//			MAIN CODE STARTS HERE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MAIN CODE STARTS HERE      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//			MAIN CODE STARTS HERE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//					MAIN CODE STARTS HERE




void setup() {
  Serial.begin(9600);
  Serial.println("Void setup is running");
  Serial.println("Setting default servo position NOW");
  pivotServo.write(SOUTH);
  grabberServo.write(OPEN);
  delay(servoDelay);
  Serial.println("Servo default position DONE");
  delay(servoDelay);
  pinMode(elevatorRackLimitSwitch, INPUT_PULLUP);
  pinMode(heatingCoil, OUTPUT);
  pinMode(airPump, OUTPUT);
  pinMode(waterPump, OUTPUT);
  lcd.begin(16, 2);
  delay(generalDelay);
  lcd.clear();
  delay(generalDelay);
  lcd.print("OpTeaMus Prime");
  delay(generalDelay);

  grabberServo.attach(grabberServoPin);
  pivotServo.attach(pivotServoPin, 500, 2500); // Min pulse is 500uSec and max is 2500uSec for AGFRC B53DHS Servo
  elevatorRack.setMaxSpeed(800);
  elevatorRack.setAcceleration(500);

  // Initialize encoder
  encoder.begin();
  // Initialize interrupts
  attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
  attachInterrupt(ENCODER_INT2, interrupt, CHANGE);
  
  // Button setup and debouncing
  loadButton.attach(loadButtonPin, INPUT_PULLUP);
  loadButton.interval(debounceInterval);
  loadButton.setPressedState(LOW);

  nextButton.attach(nextButtonPin, INPUT_PULLUP);
  nextButton.interval(debounceInterval);
  nextButton.setPressedState(LOW);
  
  // Ultrasonic configs
  pinMode(ultrasonicTrig, OUTPUT);
  pinMode(ultrasonicEcho, INPUT);

  // Ensure these devices are turned off on initial setup
  digitalWrite(heatingCoil, LOW); // SSR set to low until needed.
  digitalWrite(airPump, HIGH);  // For some reason relay module has NO closed when low. So set to HIGH at setup. LOW to activate.
  digitalWrite(waterPump, HIGH); // For some reason relay module has NO closed when low. So set to HIGH at setup. LOW to activate.
  

}

void loop() {
 nextButton.update();  // First refresh of buttons states required here
 loadButton.update();  // First refresh of buttons states required here
   // Main loop calls functions declared below
    Serial.println("The main loop function is starting");
  //pivotServoCalibration();
  //debugStartup();
    Serial.println("The main loop debugStartup is finished, moving to startupInit");
    delay(generalDelay);
  startupInit();
    Serial.println("The main loop startupInit is finished, moving to loadGrabber");
    delay(generalDelay);
  loadGrabber();
    Serial.println("The main loop loadGrabber function is finished, moving to teaSelection");
    delay(generalDelay);
  teaSelection();
    Serial.println("The main loop teaSelection function is finished, moving to progAdjust");
    delay(generalDelay);
  progAdjust();
    Serial.println("The main loop progAdjust function is finished, moving to whereAmI");
    delay(generalDelay);
  selectCupSize();
  //whereAmI();
  //Serial.println("The main loop whereAmI function is finished, moving to the beginning of the loop.");
  //delay(generalDelay);
// preFlight();
// pumpColdWater();
// heatWater();
// pumpHotWater();
// steepFunction();
 shutDown();
}

void pivotServoCalibration() {
  //pivotServo rotation arm degree calibration
  while(true) {
  delay(longWait);
  pivotServo.write(SOUTH);  // South grabber facing down
  Serial.println("We're pointing South.");
  delay(longWait);
  pivotServo.write(SEAST);  // SouthEast
  Serial.println("We're pointing SouthEast.");
  delay(longWait);
  pivotServo.write(EAST);  // East
  Serial.println("We're pointing East.");
  delay(longWait);
  pivotServo.write(NEAST);  // NorthEast
  Serial.println("We're pointing NorthEast.");
  delay(longWait);
  pivotServo.write(NORTH);  // North
  Serial.println("We're pointing North.");
  }
}


void rotateServoSlowly(Servo servo, int targetPosition) {
  // Function to rotate the servo slowly to the specified position
  int currentPosition = servo.read();
  int step = (targetPosition > currentPosition) ? 1 : -1;

  while (currentPosition != targetPosition) {
    currentPosition += step;
    servo.write(currentPosition);
    Serial.println("Inside rotateServoSlowly WHILE Loop");
    delay(servoDelay);  // Adjust the delay for the desired rotation speed
  }
}

void whereAmI() {
  Serial.println("Code has moved to the next phase correctly");
  lcd.clear();
  lcd.print("WhereAmI");
  Serial.println("Current variables are: ");
  Serial.print("Tea Index: ");
  Serial.println(currentRecipeIndex);
  Serial.print("Tea Name: ");
  Serial.println(teaParams[currentRecipeIndex].name);
  Serial.print("Tea Time: ");
  Serial.println(teaParams[currentRecipeIndex].time);
  Serial.print("Tea Temp: ");
  Serial.println(teaParams[currentRecipeIndex].temp);
  delay(5000);
}


void debugStartup() {
  nextButton.update();
  Serial.println("debugStartup is running!");
  // Clear the LCD display
  lcd.clear();
  lcd.print("Press start");
  while (true) {
    nextButton.update();
    if (nextButton.fell()) {
      // Next button pressed, exit the function
      Serial.println("Next button pressed, exiting debugStartup function");
      delay(generalDelay);
      return;
    }
    // Delay to avoid rapid changes due to noise
    delay(generalDelay);
  }
  Serial.print("End of total function:  ");
  Serial.println(nextButton.read());
}

void interrupt() {  // Used for encoder library to track interrupts
  encoder.tick();
}

void startupInit() {
  Serial.println("startupInit function is running");

  // Print messages to the LCD
  lcd.clear();
  lcd.print("OpTeaMus Prime");
  lcd.setCursor(0, 1);
  lcd.print("is getting ready");
  delay(generalDelay);
  int targetPosition = SOUTH;                     // Set the target position (degrees)
  rotateServoSlowly(pivotServo, targetPosition);  // Call the function to rotate the servo
  delay(1000);                                    // Delay for 1 second (1000 milliseconds)

  // Perform homing procedure
  Serial.println("Homing procedure started...");
  delay(shortWait);

  lcd.clear();
  lcd.print("Rack Indexing...");
  delay(generalDelay);

  while (digitalRead(elevatorRackLimitSwitch) == HIGH) {
    elevatorRack.moveTo(-12000);  // Adjust the distance as needed  //-12000 steps is up (negative) 12cm
    elevatorRack.run();
  }

  elevatorRack.stop();
  elevatorRack.setCurrentPosition(0);
  lcd.clear();
  lcd.print("Indexing Done.");

  delay(shortWait);

  // Move away from the limit switch, 2cm
  elevatorRack.moveTo(calculateSteps(2));  // Move down 2cm
  while (elevatorRack.distanceToGo() != 0) {
    elevatorRack.run();
  }
  elevatorRack.stop();

  Serial.println("Homing procedure completed.");
  delay(medWait);
  rotateServoSlowly(pivotServo, targetPosition);
}


void loadGrabber() {
  Serial.println("loadGrabber function is running");
  // Print instructions to the LCD
  lcd.clear();
  lcd.print("Hold load btn");
  lcd.setCursor(0, 1);
  lcd.print("Then next");
  grabberServo.write(CLOSE);
  while (true) {
    nextButton.update();
    loadButton.update();
    if (nextButton.fell()) {
      // Next button pressed, exit the function
      Serial.println("Next button pressed, exiting loadGrabber function");
      return;
    }
    if (loadButton.read() == LOW) {
      // Load button pressed
      grabberServo.write(OPEN);
    } else {
      // Load button released
      grabberServo.write(CLOSE);
    }
    delay(generalDelay);
  }
}


void teaSelection() {
  Serial.println("teaSelection function is running");
  lcd.clear();
  lcd.print("Select Tea");
  // Print initial value of currentRecipeIndex
  lcd.setCursor(0, 1);
  lcd.print(teaParams[currentRecipeIndex].name);

  while (true) {
    nextButton.update();
    signed char pos = encoder.getPosition();
    if (pos != 0) {
      if (pos > 0) {
        currentRecipeIndex++;  // Clockwise rotation
        if (currentRecipeIndex >= numTeas) {
          currentRecipeIndex = 0;  // Loop back to the beginning
        }
      } else {
        currentRecipeIndex--;  // Counterclockwise rotation
        if (currentRecipeIndex < 0) {
          currentRecipeIndex = numTeas - 1;  // Loop to the end
        }
      }
      encoder.reset();
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(teaParams[currentRecipeIndex].name);
    }
    // Check if the button is pressed
    if (nextButton.fell()) {
      delay(generalDelay);
      return;
    }
    delay(generalDelay);
  }
}


void progAdjust() {
  Serial.println("progAdjust function is running");
  long adjustedTime = 0;  // Variable to store adjusted time delta
  lcd.clear();
  lcd.print("Steep Time:");
  lcd.setCursor(0, 1);
  lcd.print("+0s");
  Serial.print("BeforeWhileStatement: ");
  Serial.println(adjustedTime);

  // While the nextButton is not pressed, allow the user to adjust the steep time
  while (true) {
    nextButton.update();
    signed char pos = encoder.getPosition();
    
    if (pos != 0) {
      if (pos > 0) {
        adjustedTime += steepTimeAdjustInterval;  // Clockwise rotation
        Serial.print("Adding time. New value is: ");
        Serial.println(adjustedTime);
      } else {
        adjustedTime -= steepTimeAdjustInterval;  // Counter-clockwise rotation
        Serial.print("Subtracting time. New value is: ");
        Serial.println(adjustedTime);
      }
      
      // Check if adjusted time falls below one minute
      if (teaParams[currentRecipeIndex].time + adjustedTime < 0) {
        lcd.clear();
        lcd.print("Error");
        lcd.setCursor(0, 1);
        lcd.print("Time too low");
        delay(3000); // Delay for 3 seconds to display the error message
        
        // Display "Resetting" message
        lcd.clear();
        lcd.print("Resetting");
        lcd.setCursor(0, 1);
        lcd.print("Adj to zero");
        delay(2000); // Delay for 2 seconds to display the resetting message
        
        lcd.clear();
        lcd.print("Steep Time:");
        lcd.setCursor(0, 1);
        lcd.print("+0s");
        
        adjustedTime = 0;  // Reset adjusted time to zero
      }
      
      encoder.reset();
      Serial.print("InsideWhileStatement: ");
      Serial.println(adjustedTime);
      
      // Calculate adjusted steep time
      finalSteepTime = teaParams[currentRecipeIndex].time + adjustedTime;

      // Convert total steep time to minutes and seconds
      int minutes = finalSteepTime / 60000;
      int seconds = (finalSteepTime % 60000) / 1000;

      // Display the adjusted steep time on the LCD
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Total: ");
      lcd.print(minutes);
      lcd.print("m");
      lcd.print(seconds);
      lcd.print("s");
    }

    // Exit the function if the nextButton is pressed
    if (nextButton.fell()) {
      Serial.print("The value of finalSteepTime is: ");
      Serial.println(finalSteepTime);
      delay(generalDelay);
      return;
    }
    delay(generalDelay);
  }
}


void selectCupSize() {
  Serial.println("selectCupSize function is running");
  lcd.clear();
  lcd.print("Select Cup Size");
  lcd.setCursor(0, 1);
  lcd.print(">Small   Large");

  while (true) {
    nextButton.update();
    signed char pos = encoder.getPosition();
   
    if (pos != 0) {
      // Toggle between small and large cup sizes
      selectedCupSize = (selectedCupSize + 1) % 2;

      // Update the display with the selected cup size
      lcd.setCursor(0, 1);
      if (selectedCupSize == 0) {
        lcd.print(">Small     Large");
      } else {
        lcd.print(" Small    >Large");
      }
     
      encoder.reset();
    }

    // Check if the nextButton is pressed to confirm the selection
    if (nextButton.fell()) {
      Serial.print("Selected cup size: ");
      Serial.println(selectedCupSize == 0 ? "Small" : "Large");
      delay(generalDelay);
      return;
    }
    delay(generalDelay);
  }
}

/*

void preFlight() {
  Serial.println("preFlight function is running");

  // Check ultrasonic sensor to ensure a cup is placed for dispensing hot water into it
  int distance = sonar.ping_cm();

  if (distance > 0 && distance <= 10) {
    // Cup is present within 10cm
    lcd.clear();
    lcd.print("Cup Detected");
    delay(medWait); // Display the message for 1 second
  } else {
    // Cup is not present or out of range
    lcd.clear();
    lcd.print("Place a cup");
    while (sonar.ping_cm() > 10 || sonar.ping_cm() == 0); // Wait here until a cup is placed
  }

  // Check boolean sensor to ensure the water reservoir has enough water to perform water heating/brewing/steeping
  if (digitalRead(waterReservoir) == LOW) {
    // Water reservoir is empty
    lcd.clear();
    lcd.print("Add water");
    while (digitalRead(waterReservoir) == LOW); // Wait here until water is refilled
  }

  // Wait for the next button to be pressed
  lcd.clear();
  lcd.print("Press Next");
  lcd.setCursor(0, 1); // Move cursor to the beginning of the second row
  lcd.print("to Start");

  // Update the debouncer before checking the button state
  nextButtonDebouncer.update();

  while (nextButtonDebouncer.read() == HIGH) {
    delay(generalDelay);
  }
}


void pumpColdWater() {  // Function to pump water from the cold reservoir to the boiler
  Serial.println("pumpColdWater fucnction is running");
  digitalWrite(waterPump, LOW); // Activate the water pump

  // Check which water fill probe to use based on cup size selection
  int waterFillProbe;

  if (selectedCupSize == 0) {
    waterFillProbe = waterFill;
  } else {
    waterFillProbe = waterFillMax;
  }

  // Wait until the water reaches the selected water fill probe level
  while (digitalRead(waterFillProbe) == LOW) {
    // Delay to avoid rapid checking
    delay(generalDelay);
  }

  digitalWrite(waterPump, HIGH); // Stop the water pump
}


void heatWater() { // Function to heat water in the boiler
  Serial.println("heatWater fucnction is running");
  digitalWrite(heatingCoil, LOW); // Activate the heating coil

  // Wait for the heating coil to warm up (adjust as needed)
  delay(medWait);

  lcd.clear();
  lcd.print("Heating Water");

  while (true) {
    int temperatureReading = analogRead(temperatureSensor);
    float temperatureCelsius = calculateTemperature(temperatureReading);

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperatureCelsius);
    lcd.print(" C   ");

    if (temperatureCelsius >= selectedTeaTemp) {  // Continuously check if the current temperature is below the target temperature, otherwise, turn off heatingCoil
      digitalWrite(heatingCoil, HIGH);  // Turn off the heatingCoil because selectedTeaTemp has been reached
      break;
    }
    delay(generalDelay);  // Delay to avoid rapid checking
  }
}


void pumpHotWater() {
  Serial.println("pumpHotWater fucnction is running");
  unsigned long startTime = millis();

  while (millis() - startTime < dispenseDuration) {
    digitalWrite(airPump, HIGH);
  }

  digitalWrite(airPump, LOW);
}


void steepFunction() {
  Serial.println("steepFunction is running");

  // Lower the Stepper "elevatorRack" 150mm down into the cup
  int lowerDistance = 150;  // Distance to move down in mm
  int lowerSteps = lowerDistance * pitchToDistance;  // Convert distance to steps
  elevatorRack.move(lowerSteps);
  elevatorRack.runToPosition();  // Wait for the move to complete

  // Show a countdown on the LCD display as the tea is being steeped
  lcd.clear();
  lcd.print("Steeping...");

  // Variables for periodic movement
  int dunkDistance = 20;  // Distance to move up and down during dunking in mm
  int dunkSteps = dunkDistance * pitchToDistance;  // Convert distance to steps
  unsigned long startTime = millis();
  int dunkInterval = 20000;  // Dunking interval in milliseconds (20 seconds)

  int secondsRemaining = selectedTeaTime / 1000;

  while (secondsRemaining >= 0) {
    lcd.setCursor(0, 1);
    lcd.print("Time left: ");
    lcd.print(secondsRemaining);
    lcd.print("s  ");

    // Check if it's time to perform a dunk
    if (millis() - startTime >= dunkInterval) {
      // Move up for dunking
      elevatorRack.move(dunkSteps);
      elevatorRack.runToPosition();  // Wait for the move to complete

      // Move down for dunking
      elevatorRack.move(-dunkSteps);
      elevatorRack.runToPosition();  // Wait for the move to complete

      startTime = millis();  // Reset the timer for the next dunk
    }

    // Update time remaining every second
    if (millis() % 1000 == 0) {
      secondsRemaining--;
    }

    // Add a small delay to avoid rapid checking
    delay(generalDelay);
  }

  // Raise the elevatorRack back up to its starting position
  elevatorRack.move(referenceOffset);  // Return to home position (named referenceOffset)
  elevatorRack.runToPosition();  // Wait for the move to complete
}


void disposeBag() {
  // Print to LCD first row "please remove". Print to LCD second line "cup".
  lcd.clear();
  lcd.print("Please remove");
  lcd.setCursor(0, 1);
  lcd.print("cup");

  // Wait 1 second (non-blocking delay)
  delay(medWait);

  // Check ultrasonic sensor that the cup has indeed been removed and there is no object detected within 3 cm of sensor.
  while (sonar.ping_cm() <= 3) {
    // Wait here until the cup is removed
    delay(generalDelay);
  }

  // Clear LCD and print "Resetting"
  lcd.clear();
  lcd.print("Resetting");

  // Move elevatorRack down to 200mm position (down 200mm away from "referenceOffset" position).
  elevatorRack.move(200 * pitchToDistance);
  elevatorRack.runToPosition();

  // Rotate grabber to SEAST position while the elevatorRack is moving down.
  pivotServo.write(SEAST);

  // Once elevatorRack and grabberArm reach their final positions, open the grabberServo (OPEN).
  while (elevatorRack.isRunning()) {
    delay(generalDelay);
  }
  grabberServo.write(OPEN);
  delay(medWait); // Wait for a second

  // Move the elevatorRack back to "referenceOffset" position
  elevatorRack.move(referenceOffset);
  elevatorRack.runToPosition();

  // Reset grabberArm to SOUTH.
  pivotServo.write(SOUTH);
  
  lcd.clear();
  lcd.print("Please");
  lcd.setCursor(0, 1);
  lcd.print("Power off");

// Wave goodbye?

}

*/

void shutDown() {
  lcd.clear();
  lcd.print("Please");
  lcd.setCursor(0, 1);
  lcd.print("Power off");
  delay(500000); // Long delay for now so it doesnt loop the functions
  // Code for shutting down the unit
  // Activate latching circuit
}


/*

APPENDIX:

	LCD display

		LCD Pins               Arduino Pins
		---------------------------------------
		1.  LCD VCC ---------------> 5V
		2.  LCD GND ---------------> GND
		3.  LCD Vo  ---------------> Connect to 10k Potentiometer center (for contrast control)
							    ---> 5V  (Potentiometer)
							    ---> GND (Potentiometer)
		4.  LCD RS  ---------------> D30
		5.  LCD RW  ---------------> GND
		6.  LCD EN  ---------------> D31
		11. LCD D4  ---------------> D32
		12. LCD D5  ---------------> D33
		13. LCD D6  ---------------> D34
		14. LCD D7  ---------------> D35
		15. LCD Backlight Anode ---> 5V (with Resistor (220 ohm))
		16. LCD Backlight Cathode-> GND

		LCD power consumption is 1.25mA
	
	
*/



/* 
// Test code for measuring water temp sensor
// Note for water sensor voltage divider
const int waterProbePin = A0;  // Analog input pin for water probe

void setup() {
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  // Read the analog value from the water probe
  int waterValue = analogRead(waterProbePin);

  // Print the analog reading to the Serial Monitor
  //Serial.print("Analog Reading: ");
  Serial.println(waterValue);

  // Add a delay to control the rate of readings (adjust as needed)
  delay(1000);
}
*/