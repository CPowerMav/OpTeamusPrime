#include "EncoderStepCounter.h"

#define ENCODER_PIN1 3
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 2
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)

const int nextButton = 23;

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
const int numTeas = sizeof(teaParams) / sizeof(teaParams[0]);

// Create instance for one full step encoder
// EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);
// Use the following for half step encoders
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2, HALF_STEP);

void setup() {
  Serial.begin(9600);
  // Initialize encoder
  encoder.begin();
  // Initialize interrupts
  attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
  attachInterrupt(ENCODER_INT2, interrupt, CHANGE);
  // Set button pin mode to input with pull-up
  pinMode(nextButton, INPUT_PULLUP);
}

// Call tick on every change interrupt
void interrupt() {
  encoder.tick();
}

// Initialize position to the first tea
int position = 0;

void loop() {
  signed char pos = encoder.getPosition();
  if (pos != 0) {
    if (pos > 0) {
      position++; // Clockwise rotation
      if (position >= numTeas) {
        position = 0; // Loop back to the beginning
      }
    } else {
      position--; // Counterclockwise rotation
      if (position < 0) {
        position = numTeas - 1; // Loop to the end
      }
    }
    encoder.reset();
    Serial.println(teaParams[position].name);
  }
  
  // Check if the button is pressed
  if (digitalRead(nextButton) == LOW) {
    Serial.println("Recipe Details:");
    Serial.print("Name: ");
    Serial.println(teaParams[position].name);
    Serial.print("Brewing Time: ");
    Serial.println(teaParams[position].time);
    Serial.print("Brewing Temperature: ");
    Serial.println(teaParams[position].temp);
    delay(500); // debounce delay
    while (digitalRead(nextButton) == LOW) {} // Wait for button release
  }
}
