// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>



//EL pin del ESP es el GPIO14 (creo)
#define PIN           14

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      96

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {


  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {


 
  // Bucle 'Serpiente' (para probarlo)
  
for(int i=0;i<96;i++){
  
  pixels.setPixelColor(i, pixels.Color(1,1,1)); 
  delay(10);
  pixels.show();
}
/*
rainbowCycle(20);
*/

/*
for(int i=0;i<96;i++){
  
  pixels.setPixelColor(i, pixels.Color(10,10,10)); 
  delay(10);
  pixels.show();
}

for(int i=0;i<96;i++){
  
  pixels.setPixelColor(i, pixels.Color(0,0,0)); 
  delay(10);
  pixels.show();
}*/

  
  
  
  /*
for(int i=0;i<96;i++){
  
  pixels.setPixelColor(i, pixels.Color(0,0,1)); 

   
}
pixels.show();
delay(500);



for(int i=0;i<96;i++){
 
  pixels.setPixelColor(i, pixels.Color(0,1,0)); 
 
}
pixels.show();
delay(500);


for(int i=0;i<96;i++){
 
  pixels.setPixelColor(i, pixels.Color(1,0,0)); 
 
}
pixels.show();
delay(500);

for(int i=0;i<96;i++){
 
  pixels.setPixelColor(i, pixels.Color(1,1,1)); 
 
}
pixels.show();
delay(500);*/




}//loop


/*
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

*/


