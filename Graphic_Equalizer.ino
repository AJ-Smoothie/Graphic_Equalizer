#include "pixel_driver.h"


int strobePin = 2;
int rstPin = 3;
int multiPlexPin = A7;
int multiPlex = 0;

const int XLEN = 9;
const int YLEN = 9;
pixelGrid grid(6, XLEN, YLEN);

// create a pixel object. The struct can be found in pixel Driver
// we are also going to make a pointer so we can share the struct anywhere
pixelObject *pixelArrayPtr, pixel[7]; 
unsigned long showStartTime; // this will keep track of everytime we call (show)

float hue = 0.0;
int rgbHoldTime = 25;
float wVal = 0.0;
unsigned long cMillis;
unsigned long pMillis;

int testColumn = 0;

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

  // //We're cheating here and setting colors for the columns
  // pixel[0].color = grid.packColor(0, 0, 0, 100);
  // pixel[1].color = grid.packColor(100, 0, 75, 0);
  // pixel[2].color = grid.packColor(0, 100, 0, 0);
  // pixel[3].color = grid.packColor(0, 0, 100, 0);
  // pixel[4].color = grid.packColor(50, 20, 0, 20);
  // pixel[5].color = grid.packColor(0, 0, 150, 70);
  // pixel[6].color = grid.packColor(0, 70, 50, 0);
  
  for (int i = 0; i < 7; i++)
    {
      pixel[i].peakColor = grid.packColor(255, 0, 0, 0);
    }
}

void loop()
{

  cMillis = millis();
  if (cMillis - pMillis > rgbHoldTime)
    {     
      hue += 0.01;
      if (hue >= 1.0) hue = 0.0;      
      pMillis = cMillis;
    }

  // if (Serial.available())
  //   {
  //     char c = Serial.read();
  //     switch (c)
  //       {
  //         case 'a': testColumn++; break;
  //         case 'b': testColumn--; break;
  //       }
  //     Serial.print("Volume: "); Serial.println(testColumn);
  //     pixel[3].volume = testColumn;
  //   }

  int rgbw[4];
  hsv2rgbw(hue, 1.0, 0.5, rgbw);
  for (int c = 0; c < 7; c++)
    {
      // send over the sexiness for color changing
      pixel[c].color = grid.packColor(rgbw[0], rgbw[1], rgbw[2], rgbw[3]);
    }


  getVolume(); // gets the volume data for all 7 columns
  grid.play(pixel); // sends the ptr
  grid.show();
}



void getVolume()
{
  int sampleCount = 20;

  for (int i = 0; i < 7; i++)
    {
      digitalWrite(strobePin, HIGH); // advance strobe by 1
      delayMicroseconds(20);
      digitalWrite(strobePin, LOW);

      float rawVolume = 0;
      for(int j = 0; j < sampleCount; j++) rawVolume += analogRead(multiPlexPin);
      rawVolume = (rawVolume / sampleCount); // val now contains the average reading.
      pixel[i].volumePercent = rawVolume / (1000.0 / 100.0); // volume is percentage now!
      if (pixel[i].volumePercent > 100) pixel[i].volumePercent = 100; // safeguard

      
      const float threshold = .20; // amount required to light up the first tile      
      pixel[i].volume = 0; // this will stay true if it's below the threshold.
      if (pixel[i].volumePercent >= (int)round(threshold * 100))
        {
          // subtract 20% of pixel volume from itslef, creating an offset.
          float offset = pixel[i].volumePercent - (pixel[i].volumePercent * threshold);
          float divisorRatio = (100 - (100 * threshold)) / 8;
          pixel[i].volume = offset / divisorRatio + 0.5; // 11.43
          // volume is now succesfully remapped to 0-8. Zero is off, and you must
          // cross 20% threshold to activate the first LED.
        }

      // Serial.print("VolP: "); Serial.print(pixel[i].volumePercent); Serial.print("->"); 
      // Serial.print(pixel[i].volume); Serial.print("\t"); 
    }
  // Serial.println();
}

// float fract(float x) { return x - int(x); }
// float mix(float a, float b, float t) { return a + (b - a) * t; }
// int* hsv2rgb(float h, float s, float b, int* rgb) 
// {
//   float val1 = s * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
//   float val2 = s * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
//   float val3 = s * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);

//   rgb[0] = (int)((1.0 - val1) * 255);
//   rgb[1] = (int)((1.0 - val2) * 255);
//   rgb[2] = (int)((1.0 - val3) * 255);
//   return rgb;
// }

float fract(float x) { return x - int(x); }
float mix(float a, float b, float t) { return a + (b - a) * t; }
int* hsv2rgbw(float h, float s, float b, int* rgbw) 
{
  wVal += 0.2;
  if (wVal >= 2.0) wVal = 0.0;
  float val1 = s * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  float val2 = s * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  float val3 = s * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  float val4 = s * mix(wVal, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  

  rgbw[0] = (int)((1.0 - val1) * 255);
  rgbw[1] = (int)((1.0 - val2) * 255);
  rgbw[2] = (int)((1.0 - val3) * 255);
  rgbw[3] = (int)((1.0 - val4) * 255);
  
  return rgbw;
}