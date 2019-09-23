#include <FastLED.h>
#define LED_PIN 6             // Arduino pin connected to data pin of LEDS
#define COLOR_ORDER GRB       // Strip Color order 
#define CHIPSET WS2812B       // Strip Chipset
#define BRIGHTNESS 255
#define NUM_LEDS 299          // Number of Leds - 1

// Size of 2 or less will cause bugs, for physics to work stick to under 10 at the moment
#define LED_ONE_LENGTH 20     // Length of block 1
#define LED_TWO_LENGTH 20      // Length of block 2

// Variables for led block control and collision
unsigned int red, green, blue;
unsigned int led1_index = 0 + LED_ONE_LENGTH - 1;
unsigned int led2_index = NUM_LEDS - LED_TWO_LENGTH;

// Collision with eachother
boolean noCollision = true;

// Collision with ends
boolean led1Collision = false;
boolean led2Collision = false;

// Because of how the velocity is implemented, a higher number means a lower velocity
// Physics calculations are improvised to use the inverse of the velocity
int led1_velocity = 4;
int led2_velocity = -4;

// Define led array
CRGB leds[NUM_LEDS];

// Execution count
unsigned long executions = 0;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);

  // Define LED properties and brightness
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS + 1);
  FastLED.setBrightness(BRIGHTNESS);

  // Set all LEDs off
  for (int i = 0; i < NUM_LEDS ; i++) {
    leds[i] = CRGB(0, 0, 0);
  }

  // Randomize first LED block color
  red = random(255);
  green = random(255);
  blue = random(255);

  // Set first block starting position
  for (int i = 0; i < LED_ONE_LENGTH; i++) {
    leds[i] = CRGB(red, green, blue);
  }

  // Randomize second block color
  red = random(255);
  green = random(255);
  blue = random(255);

  // Set second block starting position
  for (int i = NUM_LEDS - LED_TWO_LENGTH; i < NUM_LEDS; i++) {
    leds[i] = CRGB(red, green, blue);
  }

  FastLED.show();
}

void loop() {
  // Check if block 1 has collided with the end, and switch direction if so
  if (led1Collision) {
    led1_index = led1_index + LED_ONE_LENGTH - 1;
    led1_velocity = -1 * led1_velocity;

    // Randomize color change
    red = random(255);
    green = random(255);
    blue = random(255);

    for (int i = led1_index; i >= led1_index - LED_ONE_LENGTH + 1; i--) {
      leds[i] = CRGB(red, green, blue);
    }

    shiftForwards(led1_index, 1);
  }

  // Check if block 2 has collided with the end, and switch direction if so
  if (led2Collision) {
    led2_index = led2_index - LED_TWO_LENGTH + 1;
    led2_velocity = -1 * led2_velocity;

    // Randomize color change
    red = random(255);
    green = random(255);
    blue = random(255);

    for (int i = led2_index; i <= led2_index + LED_ONE_LENGTH - 1; i++) {
      leds[i] = CRGB(red, green, blue);
    }

    shiftBackwards(led2_index, 2);
  }


  // Check if blocks collided
  if (noCollision) {

    // if there is no collision shift leds

    // checks which direction to shift block 1
    if (executions % abs(led1_velocity) == 0) {
      if (led1_velocity > 0) {
        shiftForwards(led1_index, 1);
      } else {
        shiftBackwards(led1_index, 1);
      }
    }

    // checks which direction to shift block 2
    if (executions % abs(led2_velocity) == 0) {
      if (led2_velocity > 0) {
        shiftForwards(led2_index, 2);
      } else {
        shiftBackwards(led2_index, 2);
      }
    }

  } else {

    // calculate velocities
    int temp_led1 = led1_velocity;
    int temp_led2 = led2_velocity;

    // Have to use a temp variable b/c the calculations are dependent on eachothers velocities
    int temp_1 = calculate_velocities(1);
    int temp_2 = calculate_velocities(2);

    led1_velocity = temp_1;
    led2_velocity = temp_2;


    // if block 1's direction reversed, swap index
    if (led1_velocity < 0 && temp_led1 > 0) {
      led1_index = led1_index - LED_ONE_LENGTH + 1; //switch led1 backwards
    }

    // if block 2's direction reversed, swap index
    if (led2_velocity > 0 && temp_led2 < 0) {
      led2_index = led2_index + LED_TWO_LENGTH - 1; //switch led2 forwards
    }

    // Check direction and shift block 1
    if (led1_velocity > 0) {
      shiftForwards(led1_index, 1);
    } else {
      shiftBackwards(led1_index, 1);
    }

    // Check direction and shift block 2
    if (led2_velocity > 0) {
      shiftForwards(led2_index, 2);
    } else {
      shiftBackwards(led2_index, 2);
    }
  }

  // Check for collisions
  noCollision = checkCollision(led1_index, led2_index);
  led1Collision = checkLED_ONE_Collision(led1_index);
  led2Collision = checkLED_TWO_Collision(led2_index);

  // resets execution count to avoid overflow
  if (executions > 10000) {
    executions = 0;
  }

  FastLED.show();
  executions++;
}


// Shifts LEDS from low index to high index
void shiftForwards(int startIndex, int led) {
  if (led == 1) {
    for (int i = startIndex + 1; i >= startIndex - LED_ONE_LENGTH + 1; i--) {
      leds[i] = leds[i - 1];
    }
    leds[startIndex - LED_ONE_LENGTH + 1] = CRGB(0, 0, 0);
    led1_index++;
  } else {
    for (int i = startIndex + 1; i >= startIndex - LED_TWO_LENGTH + 1; i--) {
      leds[i] = leds[i - 1];
    }
    leds[startIndex - LED_TWO_LENGTH + 1] = CRGB(0, 0, 0);
    led2_index++;
  }
}

// Shifts LEDS from high index to low index
void shiftBackwards(int startIndex, int led) {
  if (led == 1) {
    for (int i = startIndex - 1; i < startIndex + LED_ONE_LENGTH; i++) {
      leds[i] = leds[i + 1];
    }
    leds[startIndex + LED_ONE_LENGTH - 1] = CRGB(0, 0, 0);
    led1_index--;
  } else {
    for (int i = startIndex - 1; i < startIndex + LED_TWO_LENGTH; i++) {
      leds[i] = leds[i + 1];
    }
    leds[startIndex + LED_TWO_LENGTH - 1] = CRGB(0, 0, 0);
    led2_index--;
  }
}

// check is the blocks have collided, return false if they have collided (because of how noCollision variable is programed)
boolean checkCollision(int led1_index, int led2_index) {
  if (abs(led2_index - led1_index) <= 2) {
    return false;
  } else {
    return true;
  }
}

// Check block 2 for a collision against the wall(led[0])
boolean checkLED_ONE_Collision(int led1_index) {
  if (led1_index <= 2 && led1_velocity < 0) {
    return true;
  } else {
    return false;
  }
}

// Check block 2 for a collision against the wall(led[NUM_LEDS - 2])
boolean checkLED_TWO_Collision(int led2_index) {
  if (led2_index >= 297 && led2_velocity > 0) {
    return true;
  } else {
    return false;
  }

}

int calculate_velocities(int led) {

  if (led == 1) {
    double vel1_a, vel1_b;
    // Calculate block 1's new velocity
    // Account for velocity equaling zero
    if (led1_velocity == 0) {
      vel1_a = 0.0;
    } else {
      vel1_a = ((LED_ONE_LENGTH - LED_TWO_LENGTH) / ((double)(LED_ONE_LENGTH + LED_TWO_LENGTH))) * (10 * 1.0 / led1_velocity);
    }

    // Account for velocity equaling zero
    if (led2_velocity == 0) {
      vel1_b = 0;
    } else {
      vel1_b = ((2.0 * LED_TWO_LENGTH) / (double(LED_ONE_LENGTH + LED_TWO_LENGTH))) * (10 * 1.0 / led2_velocity);
    }

    double final1 = vel1_a + vel1_b;
    Serial.print("New led1 velocity in function: ");
    Serial.println(final1);
    return round(final1);

  } else {
    int vel2_a, vel2_b;
    // Calculate block 2's new velocity
    // Account for velocity equaling zero

    if (led1_velocity == 0) {
      vel2_a = 0;
    } else {
      vel2_a = ((2.0 * LED_ONE_LENGTH) / ((double)(LED_ONE_LENGTH + LED_TWO_LENGTH))) * (10 * 1.0 / led1_velocity);
    }

    // Account for velocity equaling zero
    if (led2_velocity == 0) {
      vel2_b = 0;
    } else {
      vel2_b = ((LED_ONE_LENGTH - LED_TWO_LENGTH) / ((double)(LED_ONE_LENGTH + LED_TWO_LENGTH))) * (10 * 1.0 / led2_velocity);
    }

    double final2 = vel2_a - vel2_b;
    Serial.print("New led2 velocity in function: ");
    Serial.println(final2);
    return round(final2);

  }




}
