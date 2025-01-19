#include <Arduino.h>
#line 1 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
#include "pixel_driver.h"

int strobePin = 2;
int rstPin = 3;
int multiPlexPin = A7;
int multiPlex = 0;
int eqVolume[7];
int sampleCount = 10;

const int XLEN = 9;
const int YLEN = 9;
pixelGrid grid(6, XLEN, YLEN);


// create a pixel object. The struct can be found in pixel Driver
// we are also going to make a pointer so we can share the struct anywhere
pixelObject *pixelArrayPtr, pixel[7]; 


float hue = 0.0;
int rgbHoldTime = 25;
unsigned long cMillis;
unsigned long pMillis;

#line 25 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
void setup();
#line 42 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
void loop();
#line 103 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
float fract(float x);
#line 104 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
float mix(float a, float b, float t);
#line 105 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
int * hsv2rgb(float h, float s, float b, int* rgb);
#line 118 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
void getVolume();
#line 25 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\Spectrum_Analyzer.ino"
void setup()
{
  Serial.begin(115200);
  grid.init();

  pinMode(rstPin, OUTPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(multiPlexPin, INPUT);

  // reset the multiplexor
  digitalWrite(rstPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(rstPin, LOW);

  //pixelArrayPtr = &(*pixel); // pixelArrayPtr points to the first pixelStruct (*pixel) is the address of the first pixel struct
}

void loop()
{
  // cMillis = millis();
  // if (cMillis - pMillis > rgbHoldTime)
  //   {     
  //     hue += 0.01;
  //     if (hue >= 1.0) hue = 0.0;      
  //     pMillis = cMillis;
  //   }

  // for (int i = 0; i < 9; i++)
  //   {
  //     grid.test(i, 7);
  //     delay(500);
  //   }
  // grid.clear();

  getVolume(); // gets the volume data for all 7 columns

  //We're cheating here and setting colors for the columns
  pixel[0].color = grid.packColor(0, 0, 0, 100);
  pixel[1].color = grid.packColor(100, 0, 75, 0);
  pixel[2].color = grid.packColor(0, 100, 0, 0);
  pixel[3].color = grid.packColor(0, 0, 100, 0);
  pixel[4].color = grid.packColor(50, 20, 0, 20);
  pixel[5].color = grid.packColor(0, 0, 150, 70);
  pixel[6].color = grid.packColor(0, 70, 50, 0);
  // for (int i = 0; i < 7; i++)
  //   {
  //     pixel[i].color = grid.packColor(0, 0, 255, 0);
  //   }

  // play expects a pointer to the struct pixelObj. When we pass in an array, we are actually passing in the address of the first element
  // which is the same as passing in a pointer to the pixel struct.
  grid.play(pixel); // sends the ptr



  // pixel.columnColor[0] = 0;
  // pixel.columnColor[1] = 0;
  // pixel.columnColor[2] = 155;
  // pixel.columnColor[3] = 0;

  // for(int i = 0; i < XLEN; i++)
  //   { 
  //     color.interval = i;
  //     color.brightnessInput = eqVolume[i];      
  //     color.peakColor[0] = 255;
  //     color.peakColor[1] = 0;
  //     color.peakColor[2] = 0;
  //     color.holdTime = 750;
  //     hsv2rgb(hue, 1.0, 0.5, color.columnColor); // send over the sexiness for color changing
  //     grid.play(i, color_ptr);
  //     Serial.print(eqVolume[i]);
  //     Serial.print(",\t");
  //   }
  grid.updateGrid();
  //delay(1000);
  
}

float fract(float x) { return x - int(x); }
float mix(float a, float b, float t) { return a + (b - a) * t; }
int* hsv2rgb(float h, float s, float b, int* rgb) 
{
  float val1 = s * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  float val2 = s * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  float val3 = s * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);

  rgb[0] = (int)((1.0 - val1) * 255);
  rgb[1] = (int)((1.0 - val2) * 255);
  rgb[2] = (int)((1.0 - val3) * 255);
  return rgb;
}


void getVolume()
{
  for (int i = 0; i < 7; i++)
    {
      digitalWrite(strobePin, HIGH); // advance strobe by 1
      delayMicroseconds(20);
      digitalWrite(strobePin, LOW);

      /**
       * The last pixel is very unstable because of the fluctuations in the ADC reading. Increasing sample count helps, 
       * but then we loose time. So an easier way to go about it to just not show the last pixel. We do this by having the 
       * column rise up to a volume of 8. This is technically the last pixel (pixel #9). But in our play function, we don't
       * show pixels <=, just < then the greatest. So we are never showing the last pixel, hence our loudest volume is index
       * 7 or pixel #8. 
       * So now the math works out as such: The Largest ADC value we ever really see is ~1015. So we if take that number, 
       * and divide it by (1000/8) it comes out to 8.15. We then truncate by casting to int, so our loudest volume is 8.
       * Except we never actually show this last pixel :)
       */

      float rawVolume = 0;
      for(int j = 0; j < sampleCount; j++) rawVolume += analogRead(multiPlexPin);
      rawVolume = (rawVolume / sampleCount); // val now contains the average reading.
      pixel[i].volume = rawVolume / (1000.0 / 8.0); // convert the volume
      pixel[i].volume += pixel[i].volumeOffset; // shift everything back by 1
      // this extracts the decimal content of rawVolume. This is essentially the percentage the top pixel should be on.
      // int decimalExtract = ((rawVolume - (int)rawVolume) * 100) + 0.5;
      // int red = map(decimalExtract, 0, 100, 0, 255);
      // pixel[i].brightness = grid.packColor(red, 0, 0, 0);
      
    
      //if (pixel[i].volume > 8) pixel[i].volume = 8; // safeguard

      //Serial.print("V: "); Serial.print(rawVolume); Serial.print("->"); Serial.print(pixel[i].volume); Serial.print("\t");
    }
  //Serial.println();
}


