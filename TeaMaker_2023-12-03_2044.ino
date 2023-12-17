// External includes
#include <Servo.h> // Arduino default servo motor library
#include <Stepper.h> // Arduino default stepper motor library
#include <LiquidCrystal.h> // Arduino default LCD display library

// Define Digital IO pin numbers - Skip Pin 13 if possible

	// Movement with PWM Pins
const int pivotServoPin = 4; // PWM Pin
const int grabberServoPin = 5; // PWM Pin
const int elevatorRackStep = 6; // PWM Pin
const int elevatorRackDir = 7; // PWM Pin
const int elevatorRackLimitSwitch = 8; // PWM Pin

	// User Input
const int loadButton = 22;
const int nextButton = 23;
const int rotaryInput = 24;

	// Bool Inputs & Sensors
const int cupPresence = 44;
const int ultrasonicTrig = 45;
const int ultrasonicEcho = 46;

	// Bool Outputs
const int heatingCoil = 47;
const int waterPump = 48;
const int airPump = 49;

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7) - See appendix below for LCD Pin wiring and assignments
LiquidCrystal lcd(30, 31, 32, 33, 34, 35);

// Define Analog IO Pins
const int temperatureSensor = A0; 
const int waterReservoir = A1;
const int waterFill = A2;  // Water level probe inside boiler for regular size
const int waterFillMax = A3; // Water level probe inside boiler for max size

// Define other constants
const int STEPS_PER_REVOLUTION = 200; // Stepper value
const int dispenseTime = 5000;  // Adjust as needed
const float Rref = 50000.0;  // Reference resistance
const float nominal_temeprature = 25.0;  // Nominal temperature in Celsius
const float nominal_resistance = 50000.0;  // Nominal resistance at nominal temperature
const float beta = 3950.0;  // Beta value of the NTC thermistor
const int startupDelay = 2000; // Update this to adjust startup delay globally
const int generalDelay = 50; // Update this to adjust delays for button inputs
const int servoDelay = 1000; // Update this to adjust delays for servos to arrive to set positions
const int steepTimeAdjustInterval = 30000; // Adjust steep time by this increment in +/- milliseconds

// Create Servo and Stepper objects
Servo pivotServo;
Servo grabberServo;
Stepper elevatorRack(STEPS_PER_REVOLUTION, elevatorRackStep, elevatorRackDir);

// Word substitutions for pivotServo positions

const int NORTH = 180; // For pivotServo pointing straight up
const int EAST = 90; // For pivotServo pointing to the right
const int SOUTH = 0; // For pivotServo pointing straight down
const int SEAST = 45; // For pivotServo down and right
const int NEAST = 135; // For pivotServo up and right

// Word substitutions for grabberServo positions

const int CLOSE = 90; // Grabber servo closed position (90 deg)
const int OPEN = 0; // Grabber servo open position (0 deg)


// Define Tea Types - External Include?
struct teaRecipe {
  const char* name;
  unsigned long time;
  int temp;
};

teaRecipe teaParams[] = {
  {"White Tea", 270000, 79},
  {"Green Tea", 240000, 79},
  {"Black Tea", 210000, 91},
  {"Oolong Tea", 210000, 91},
  {"Herbal Tea", 800000, 99},
};

// Define variables
int currentTeaIndex = 0;
unsigned long selectedTeaTime = teaParams[currentTeaIndex].time;
int selectedTeaTemp = teaParams[currentTeaIndex].temp;
char selectedTeaName[15];
// teaRecipe currentTea = {"White Tea", 270000, 79};  // Default tea type - Commented out for now. Don't think i need this.
// int teaTimeAdjustment = 0; - Unused variable for now


//					MAIN CODE STARTS HERE
//			MAIN CODE STARTS HERE
//	MAIN CODE STARTS HERE
//			MAIN CODE STARTS HERE
//					MAIN CODE STARTS HERE


//Setup - Runs once

void setup() {
  Serial.begin(9600);
  Serial.print("The program has begun");
  pinMode(loadButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);
  pinMode(elevatorRackLimitSwitch, INPUT_PULLUP); // Remember, HIGH means open circuit because of Pullup
  pinMode(heatingCoil, OUTPUT);
  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  lcd.print("OpTeaMus Prime"); // Print a message to the LCD.
  grabberServo.attach(grabberServoPin);
  pivotServo.attach(pivotServoPin);
}

//Main program

void loop() {
// Main loop calls functions declared below
  startupInit();
  loadGrabber();
  teaSelection();
  progAdjust();
  preFlight();
  heatWater();
  steepFunction();
  disposeBag();
  shutDown();
}

// Segment function declarations and operations

void startupInit() {
  // Print messages to the LCD
  lcd.clear();
  lcd.print("OpTeaMus Prime");
  lcd.setCursor(0, 1);
  lcd.print("Getting ready..");

  pivotServo.write(EAST); // Rotate grabber to EAST
  delay(servoDelay); // Wait a second for it to arrive
  
  // Move gantry until the limit switch is triggered
  while (digitalRead(elevatorRackLimitSwitch) == HIGH) {
    elevatorRack.step(1);
  }
  elevatorRack.step(-10);  // Adjust as needed
  pivotServo.write(SOUTH); // Rotate grabber to SOUTH
}

void loadGrabber() {
  // Print instructions to the LCD
  lcd.clear();
  lcd.print("Hold load btn");
  lcd.setCursor(0, 1);
  lcd.print("Then next");

  grabberServo.write(CLOSE);  // Start by closing the grabber

  while (digitalRead(nextButton) == HIGH) {  // While the nextButton is not pressed, loop the following
    if (digitalRead(loadButton) == LOW) {  // If the load button is pushed, this becomes low, grabber opens.
      grabberServo.write(OPEN);
    } else { // Otherwise, the load button is not pressed, and the grabber becomes closed (servo 90 degrees)
      grabberServo.write(CLOSE);
    }
  }
}

void teaSelection() {
  // Clear the LCD display
  lcd.clear();

  // Display the heading on the first row
  lcd.print("Select Tea");

  // Display the current tea name on the second row
  lcd.setCursor(0, 1);
  lcd.print(teaParams[currentTeaIndex].name);

  // Set the selectedTeaName variable
  strcpy(selectedTeaName, teaParams[currentTeaIndex].name); // Copy the tea name to the selectedTeaName variable

  // Rotary encoder variables
  int encoderValue = 0;
  int encoderLastValue = 0;
  int encoderMaxValue = sizeof(teaParams) / sizeof(teaParams[0]) - 1;

  // While the nextButton is not pressed, allow the user to scroll through tea options
  while (digitalRead(nextButton) == HIGH) {
    // Update encoder value based on the rotary input
    encoderValue += (digitalRead(rotaryInput) == HIGH) - (digitalRead(rotaryInput) == LOW);

    // Ensure the encoder value stays within valid bounds
    if (encoderValue < 0) {
      encoderValue = encoderMaxValue;
    } else if (encoderValue > encoderMaxValue) {
      encoderValue = 0;
    }

    // If the encoder value changes, update the selected tea and LCD display
    if (encoderValue != encoderLastValue) {
      currentTeaIndex = encoderValue;
      selectedTeaTime = teaParams[currentTeaIndex].time;
      selectedTeaTemp = teaParams[currentTeaIndex].temp;

      // Copy the tea name to the selectedTeaName variable
      strcpy(selectedTeaName, teaParams[currentTeaIndex].name);

      // Clear the LCD and display the updated information
      lcd.clear();
      lcd.print("Select Tea");
      lcd.setCursor(0, 1);
      lcd.print(selectedTeaName);

      // Update the last encoder value
      encoderLastValue = encoderValue;
    }

    // Delay to avoid rapid changes due to noise
    delay(generalDelay);
  }
}

void progAdjust() {
  // Allows the user to tweak or adjust teaTime variable by using the rotary encoder (rotaryInput) to add or subtract time from teaTime variable in increments of 30 seconds (30000ms)
  int encoderValue = 0;
  int encoderLastValue = 0;
  
  // Display "Adjust steep" on the first line
  lcd.clear();
  lcd.print("Adjust steep");
  
  while (digitalRead(nextButton) == HIGH) {
    encoderValue += (digitalRead(rotaryInput) == HIGH) - (digitalRead(rotaryInput) == LOW);
    
    // Calculate adjusted steep time
    int adjustedTime = selectedTeaTime + encoderValue * steepTimeAdjustInterval;

    // Display plus or minus and the total time dynamically on the second line
    lcd.setCursor(0, 1);
    lcd.print(encoderValue > 0 ? "+" : "-");
    lcd.print(abs(adjustedTime) / 1000);
    lcd.print("s");

    // Update selectedTeaTime if the encoder value changes
    if (encoderValue != encoderLastValue) {
      selectedTeaTime = adjustedTime;
      encoderLastValue = encoderValue;
    }
    delay(generalDelay);
  }
}

void preFlight() {
  // Check ultrasonic sensor to ensure a cup is placed for dispensing hot water into it
  if (digitalRead(cupPresence) == HIGH) {
    // Cup is not present
    lcd.clear();
    lcd.print("Place a cup");
    while(digitalRead(cupPresence) == HIGH); // Wait here until a cup is placed
  }
  
  // Check boolean sensor to ensure the water reservoir has enough water to perform water heating/brewing/steeping
  if (digitalRead(waterReservoir) == LOW) {
    // Water reservoir is empty
    lcd.clear();
    lcd.print("Add water");
    while(digitalRead(waterReservoir) == LOW); // Wait here until water is refilled
  }
  
  // Wait for next button to be pressed
  lcd.clear();
  lcd.print("Press Next");
  lcd.setCursor(0, 1); // Move cursor to the beginning of the second row
  lcd.print("to Start");
  while (digitalRead(nextButton) == HIGH) {
    delay(generalDelay);
  }
}


void heatWater() {
  // Water is pumped from the reservoir to the boiler
  
  digitalWrite(heatingCoil, HIGH);   // Heating coil is energized by activating a pin connected to a relay

  // Wait for the heating coil to warm up (adjust as needed)
  delay(startupDelay);

  while (true) {
    int temperatureReading = analogRead(temperatureSensor);
    float temperatureCelsius = calculateTemperature(temperatureReading);

    Serial.print("Temperature: ");
    Serial.println(temperatureCelsius);

    // Check if the current temperature is below the target temperature
    if (temperatureCelsius < selectedTeaTemp) {
      // Continue heating
      // You may need to adjust the heating process based on your system characteristics
    } else {
      // Stop heating
      digitalWrite(heatingCoil, LOW);
      break;
    }
  }

  // Add additional code here for dispensing water into the cup based on dispenseTime
  // This can include controlling pumps or valves
}

void steepFunction() {
  // Code for steeping the tea bag
  // Lower tea bag, countdown, raise tea bag
  // Move on to next function automatically
}

void disposeBag() {
  // Code for disposing of the tea bag
  // Listen to ultrasonic sensor, move gantry, rotate arm, release grabber, move gantry
  // Wait for next button press
}

void shutDown() {
  // Code for shutting down the unit
  // Activate latching circuit
}

float calculateTemperature(int temperatureSensor) {
  float R = Rref * (1023.0 / (float)temperatureSensor - 1.0); // Calculate NTC resistance
  float T = 1.0 / (1.0 / (nominal_temeprature + 273.15) + log(R / nominal_resistance) / beta);
  T -= 273.15; // convert absolute temp to C
  return T;
}


/*

APPENDIX:

	LCD display

		LCD Pins               Arduino Pins
		---------------------------------------
		LCD VCC ---------------> 5V
		LCD GND ---------------> GND
		LCD RS  ---------------> D30
		LCD RW  ---------------> GND
		LCD EN  ---------------> D31
		LCD D4  ---------------> D32
		LCD D5  ---------------> D33
		LCD D6  ---------------> D34
		LCD D7  ---------------> D35
		LCD Vo  ---------------> Connect to Potentiometer (for contrast control)
							  ---> 5V (Potentiometer)
							  ---> GND (Potentiometer)
		LCD Backlight Anode ---> 5V (with Resistor (220 ohm))
		LCD Backlight Cathode-> GND

		LCD power consumption is 1.25mA
	
	
*/