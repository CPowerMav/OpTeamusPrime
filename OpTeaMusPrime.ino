// External includes
#include <Servo.h> // Arduino default servo motor library
#include <Stepper.h> // Arduino default stepper motor library
#include <LiquidCrystal.h> // Arduino default LCD display library
#include <Bounce2.h> // Button debouncing library
#include <Encoder.h> // Rotary encoder library


// Define Digital IO pin numbers - Skip Pin 13 if possible

	// Movement with PWM Pins
const int pivotServoPin = 4; // PWM Pin
const int grabberServoPin = 5; // PWM Pin
const int elevatorRackStep = 6; // PWM Pin
const int elevatorRackDir = 7; // PWM Pin

	// User Input
const int loadButton = 22; // Pulled up and debounced in setup
const int nextButton = 23; // Pulled up and debounced in setup
const int rotaryButton = 24; // Rotary encoder SW pin
const int rotaryA = 2; // DT Pin - Interrupt capable pin
const int rotaryB = 3; // CLK Pin - Interrupt capable pin
Encoder selectorKnob(rotaryA, rotaryB); // Create Encoder object for rotary encoder "Encoder" class called "selectorKnob".
Bounce loadButtonDebouncer = Bounce();
Bounce nextButtonDebouncer = Bounce();

	// Bool Inputs & Sensors
const int cupPresence = 44;
const int ultrasonicTrig = 45;
const int ultrasonicEcho = 46;
const int elevatorRackLimitSwitch = 8; // Limit switch pulled high in setup

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

// Define other constants
const int STEPS_PER_REVOLUTION = 200; // Stepper value
const int dispenseDuration = 5000;  // Adjust as needed
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
const int debounceInterval = 10; // Button debounce interval in milliseconds
int cupSizeSelection = 0;  // Variable to store whcih size the user selects (0 is small, 1 is large)








//					MAIN CODE STARTS HERE
//			MAIN CODE STARTS HERE
//	MAIN CODE STARTS HERE
//			MAIN CODE STARTS HERE
//					MAIN CODE STARTS HERE










void setup() {
//Setup - Runs once
  Serial.begin(9600);
  Serial.println("Void setup is running");
  
// PinModes - Most are set by library used like Encoder automatically setting pins to pullup
  pinMode(elevatorRackLimitSwitch, INPUT_PULLUP); // Remember, HIGH means open circuit because of Pullup
  pinMode(heatingCoil, OUTPUT);
  pinMode(airPump, OUTPUT);
  pinMode(waterPump, OUTPUT);
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  lcd.clear(); // Start by clearing the LCD
  lcd.print("OpTeaMus Prime"); // Print a message to the LCD.
  grabberServo.attach(grabberServoPin);
  pivotServo.attach(pivotServoPin);
  // Initialize debouncers with the shared debounce interval
  loadButtonDebouncer.attach(loadButton, INPUT_PULLUP);
  loadButtonDebouncer.interval(debounceInterval);
  nextButtonDebouncer.attach(nextButton, INPUT_PULLUP);
  nextButtonDebouncer.interval(debounceInterval);
}

void loop() {
// Main loop calls functions declared below
  Serial.println("The main loop function is starting");
  startupInit();
  loadGrabber();
  teaSelection();
  progAdjust();
  selectCupSize();
  preFlight();
  heatWater();
  steepFunction();
  disposeBag();
  shutDown();
}






// Main functions written below are called by main loop






void startupInit() {
  Serial.println("startupInit fucnction is running");
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
  elevatorRack.step(-10);  // After the limit switch is activated, move thee gantry down again a bit. Adjust as needed.
  pivotServo.write(SOUTH); // Now that the the elevator is at the top position, rotate grabber to SOUTH
}

void loadGrabber() {
  Serial.println("loadGrabber fucnction is running");
  // Print instructions to the LCD
  lcd.clear();
  lcd.print("Hold load btn");
  lcd.setCursor(0, 1);
  lcd.print("Then next");

  grabberServo.write(CLOSE);  // Start by closing the grabber

  while (digitalRead(nextButton) == HIGH) {
    // Update debouncer for loadButton
    loadButtonDebouncer.update();

    // Check debounced button state
    if (loadButtonDebouncer.fell()) {
      // Load button pressed
      grabberServo.write(OPEN);
      // Add additional logic or code to handle the button press action
    } else {
      // Load button not pressed, keep the grabber closed
      grabberServo.write(CLOSE);
	  delay(generalDelay); // Wait for the servo to close and move on to teaSelection 
	  break;  // Exit the loop if nextButton is pressed
    }
  }
}


void teaSelection() {
  Serial.println("teaSelection fucnction is running");
  // Clear the LCD display
  lcd.clear();

  // Display the heading on the first row
  lcd.print("Select Tea");

  // Display the current tea name on the second row
  lcd.setCursor(0, 1);
  lcd.print(teaParams[currentTeaIndex].name);

  // Set the selectedTeaName variable
  strcpy(selectedTeaName, teaParams[currentTeaIndex].name); // Copy the tea name to the selectedTeaName variable

  // While the nextButton is not pressed, allow the user to scroll through tea options
  while (nextButtonDebouncer.read() == HIGH) {
    // Read changes from the selectorKnob
    int selectorKnobChange = selectorKnob.read();

    // Update selectorKnobValue based on the change
    int selectorKnobValue = selectorKnobChange;

    // Ensure the selectorKnob value stays within valid bounds
    selectorKnobValue = constrain(selectorKnobValue, 0, sizeof(teaParams) / sizeof(teaParams[0]) - 1);

    // If the selectorKnob value changes, update the selected tea and LCD display
    if (selectorKnobChange != 0) {
      currentTeaIndex = selectorKnobValue;
      selectedTeaTime = teaParams[currentTeaIndex].time;
      selectedTeaTemp = teaParams[currentTeaIndex].temp;

      // Copy the tea name to the selectedTeaName variable
      strcpy(selectedTeaName, teaParams[currentTeaIndex].name);

      // Clear the LCD and display the updated information
      lcd.clear();
      lcd.print("Select Tea");
      lcd.setCursor(0, 1);
      lcd.print(selectedTeaName);
    }

    // Check and debounce nextButton
    nextButtonDebouncer.update();

    if (nextButtonDebouncer.fell()) {
      // Call the progAdjust function when the nextButton is pressed
      progAdjust();
      return;  // Exit the teaSelection function
    }

    // Delay to avoid rapid changes due to noise
    delay(generalDelay);
  }
}


void progAdjust() {
  Serial.println("progAdjust fucnction is running");
  // Allows the user to tweak or adjust teaTime variable by using the rotary encoder (rotaryInput)
  // to add or subtract time from teaTime variable in increments of 30 seconds (30000ms)

  // Debounce object for nextButton
  nextButtonDebouncer.update();

  // Display "Adjust steep" on the first line
  lcd.clear();
  lcd.print("Adjust steep");

  while (nextButtonDebouncer.read() == HIGH) {
    // Update encoder value based on the rotary input
    encoderValue += (digitalRead(rotaryInput) == HIGH) - (digitalRead(rotaryInput) == LOW);

    // Calculate adjusted steep time
    int adjustedTime = selectedTeaTime + encoderValue * steepTimeAdjustInterval;

    // Display plus or minus and the total time dynamically on the second line
    lcd.setCursor(0, 1);
    lcd.print(encoderValue > 0 ? "+" : "-");
    lcd.print(abs(adjustedTime) / 1000);
    lcd.print("s");

    // Delay to avoid rapid changes due to noise
    delay(generalDelay);

    // Check and debounce nextButton
    nextButtonDebouncer.update();

    // Update selectedTeaTime if the encoder value changes
    if (encoderValue != encoderLastValue) {
      selectedTeaTime = adjustedTime;
      encoderLastValue = encoderValue;
    }
	if (nextButtonDebouncer.fell()) {
    // Exit the function if the nextButton is pressed
    return;
    }
  }
}


void selectCupSize() {
  Serial.println("selectCupSize fucnction is running");
  // Clear the LCD display
  lcd.clear();

  // Display the cup size selection prompt on the first row
  lcd.print("Select Cup Size");

  // Display the current cup size on the second row
  lcd.setCursor(0, 1);
  lcd.print("Small   Large");

  // Rotary encoder variables
  int encoderValue = 0;
  int encoderLastValue = 0;

  // While the nextButton is not pressed, allow the user to scroll through cup size options
  while (nextButtonDebouncer.read() == HIGH) {
    // Read changes from the selectorKnob
    int selectorKnobChange = selectorKnob.read();

    // Update selectorKnobValue based on the change
    int selectorKnobValue = selectorKnobChange;

    // Ensure the selectorKnob value stays within valid bounds (0 for Small, 1 for Large)
    selectorKnobValue = constrain(selectorKnobValue, 0, 1);

    // If the selectorKnob value changes, update the LCD display
    if (selectorKnobChange != 0) {
      lcd.setCursor(0, 1);
      lcd.print(selectorKnobValue == 0 ? ">Small   Large" : " Small   >Large");

      // Update the last encoder value
      encoderLastValue = encoderValue;
    }

    // Check and debounce nextButton
    nextButtonDebouncer.update();

    if (nextButtonDebouncer.fell()) {
      // User made a selection, update the global variable
      cupSizeSelection = selectorKnobValue;

      // Exit the function
      return;
    }
  }
}


void preFlight() {
  Serial.println("preFlight fucnction is running");
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

  // Update the debouncer before checking the button state
  nextButtonDebouncer.update();

  while (nextButtonDebouncer.read() == HIGH) {
    delay(generalDelay);
  }
}


void pumpWater() {  // Function to pump water from the cold reservoir to the boiler
  Serial.println("pumpWater fucnction is running");
  digitalWrite(waterPump, HIGH); // Activate the water pump

  // Check which water fill probe to use based on cup size selection
  int waterFillProbe;

  if (cupSizeSelection == 0) {
    waterFillProbe = waterFill;
  } else {
    waterFillProbe = waterFillMax;
  }

  // Wait until the water reaches the selected water fill probe level
  while (digitalRead(waterFillProbe) == LOW) {
    // Delay to avoid rapid checking
    delay(generalDelay);
  }

  digitalWrite(waterPump, LOW); // Stop the water pump
}


// Function to heat water in the boiler
void heatWater() {
  Serial.println("heatWater fucnction is running");
  digitalWrite(heatingCoil, HIGH); // Activate the heating coil

  // Wait for the heating coil to warm up (adjust as needed)
  delay(startupDelay);

  lcd.clear();
  lcd.print("Heating Water");

  while (true) {
    int temperatureReading = analogRead(temperatureSensor);
    float temperatureCelsius = calculateTemperature(temperatureReading);

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperatureCelsius);
    lcd.print(" C   ");

    // Check if the current temperature is below the target temperature
    if (temperatureCelsius < selectedTeaTemp) {
      // Continue heating
      // You may need to adjust the heating process based on your system characteristics
    } else {
      // Stop heating
      digitalWrite(heatingCoil, LOW);
      break;
    }

    // Delay to avoid rapid checking
    delay(generalDelay);
  }
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