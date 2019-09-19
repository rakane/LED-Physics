#include <FastLED.h>
#define LED_PIN 6             // Arduino pin connected to data pin of LEDS
#define COLOR_ORDER GRB       // Strip Color order 
#define CHIPSET WS2812B       // Strip Chipset
#define NUM_LEDS 300          // Number of Leds
#define LED_ONE_LENGTH 6      // Length of block 1
#define LED_TWO_LENGTH 6      // Length of block 2


// Variables for led block control and collision
int red, green, blue;
int led1_index = 0 + LED_ONE_LENGTH - 1;
int led2_index = NUM_LEDS - LED_TWO_LENGTH;

//Collision with eachother
boolean noCollision = true;

// Collision with ends
boolean led1Collision = false;
boolean led2Collision = false;

// led direction defines true as the forwards, the direction from led[0] to led[NUM_LEDS - 2]
boolean led1_direction = true;
boolean led2_direction = false;

// Define led array
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  // Set all LEDs off
  for(int i = 0; i < NUM_LEDS ; i++) {
      leds[i] = CRGB(0,0,0);
    }

  // Randomize first LED block color
  red = random(255);
  green = random(255);
  blue = random(255);

  // Set first block starting position
  for(int i = 0; i < LED_ONE_LENGTH; i++) {
    leds[i] = CRGB(red, green, blue);
  }

  // Randomize second block color
  red = random(255);
  green = random(255);
  blue = random(255);

  // Set second block starting position
  for(int i = NUM_LEDS - 1 - LED_TWO_LENGTH; i < NUM_LEDS; i++) {
    leds[i] = CRGB(red, green, blue);
  }

  Serial.print("First Strip index: ");
  Serial.println(led1_index);
  Serial.print("Second Strip index: ");
  Serial.println(led2_index);
  FastLED.show();  
  delay(1000);
}

void loop() {

  // Check if block 1 has collided with the end, and switch direction if so
  if(led1Collision) {
    led1_index = led1_index + LED_ONE_LENGTH - 2;
    led1_direction = true;
  }

  // Check if block 2 has collided with the end, and switch direction if so
  if(led2Collision) {
    led2_index = led2_index - LED_TWO_LENGTH + 2;
    led2_direction = false;
  }


  // Check if blocks collided
  if(noCollision) {
    // if there is no collision shift leds

    // checks which direction to shift block 1
    if(led1_direction) {
      shiftForwards(led1_index, 1);
    } else {
      shiftBackwards(led1_index, 1);
    }

    if(!led2_direction) {
      shiftBackwards(led2_index, 2);
    } else {
      shiftForwards(led2_index, 2);
    }
  } else {
      led1_index = led1_index - LED_ONE_LENGTH + 2;
      led2_index = led2_index + LED_TWO_LENGTH - 2;
      led1_direction = false;
      led2_direction = true;
  }

  
  noCollision = checkCollision(led1_index, led2_index);
  led1Collision = checkLED_ONE_Collision(led1_index);
  led2Collision = checkLED_TWO_Collision(led2_index);
  
  FastLED.show();
}


// Shifts LEDS from low index to high index
void shiftForwards(int startIndex, int led) {
  if(led == 1) {
     for(int i = startIndex + 1; i >= startIndex - LED_ONE_LENGTH + 1; i--) {
      leds[i] = leds[i - 1];
     }
     leds[startIndex - LED_ONE_LENGTH + 1] = CRGB(0, 0, 0);
     led1_index++;
  } else {
      for(int i = startIndex + 1; i >= startIndex - LED_TWO_LENGTH + 1; i--) {
        leds[i] = leds[i - 1];
      }
      leds[startIndex - LED_TWO_LENGTH + 1] = CRGB(0, 0, 0);
      led2_index++;
  }
}

// Shifts LEDS from high index to low index
void shiftBackwards(int startIndex, int led) {
  if(led == 1) {
    for(int i = startIndex - 2; i < startIndex + LED_ONE_LENGTH - 1; i++) {
      leds[i] = leds[i+1];
    }
    leds[startIndex + LED_ONE_LENGTH - 2] = CRGB(0, 0, 0);
    led1_index--; 
  } else {
      for(int i = startIndex - 2; i < startIndex + LED_TWO_LENGTH - 1; i++) {
        leds[i] = leds[i+1];
      }
      leds[startIndex + LED_TWO_LENGTH - 2] = CRGB(0, 0, 0);
      led2_index--; 
    }
}

// check is the blocks have collided, return false if they have collided (because of how noCollision variable is programed)
boolean checkCollision(int led1_index, int led2_index) {
  if(abs(led1_index - led2_index) <= 1 && led1_direction && !led2_direction) {
    return false;
  } else {
    return true;
  }
}

// Check block 2 for a collision against the wall(led[0])
boolean checkLED_ONE_Collision(int led1_index) {
  if(led1_index == 0 && !led1_direction) {
    return true;
  } else {
    return false;
  }
}

// Check block 2 for a collision against the wall(led[NUM_LEDS - 2])
boolean checkLED_TWO_Collision(int led2_index) {
  if(led2_index == 298 && led2_direction) {
    return true;
  } else {
    return false;
  }

}
