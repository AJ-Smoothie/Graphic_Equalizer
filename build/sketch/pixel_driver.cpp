#line 1 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Spectrum_Analyzer\\pixel_driver.cpp"
#include "Arduino.h"
#include "pixel_driver.h"

pixelGrid::pixelGrid(int pin, int xlen, int ylen) // dataPin, X, Y
{
  columns = xlen; // now the object contains the sizes
  rows = ylen;
  pixelCount = columns * rows - 1; // we have to remove that one pixel
  _neoPixels = new Adafruit_NeoPixel(pixelCount, pin, NEO_GRBW + NEO_KHZ800);
}

pixelGrid::~pixelGrid()
{
  delete _neoPixels;
}

void pixelGrid::init()
{
  _neoPixels->begin();
}

void pixelGrid::test(int x, int y)
{
  _neoPixels->setPixelColor(xyRemap(x, y), _neoPixels->Color(150, 0, 0, 150));
  _neoPixels->show();
}

void pixelGrid::clear() { _neoPixels->clear(); }

void pixelGrid::playPeak(int column, pixelObject *pixel)
{
  // PixelOut peakLED; // creates an structure for the topmost LED  
  // peakLED.RED = color_ptr->peakColor[red];
  // peakLED.GREEN = color_ptr->peakColor[green];
  // peakLED.BLUE = color_ptr->peakColor[blue];
  // peakLED.XPOS = column;

  // colInt = color_ptr->interval; // for ease of use
    
  // newBrightness[colInt] = color_ptr->brightnessInput;  //save the brightness of each channel
  // //Serial.print(newBrightness[colInt]);

  // if (newBrightness[colInt] > peakVal[colInt])
  //   {
  //     peakVal[colInt] = newBrightness[colInt];
  //     pMillis[colInt] = millis();
  //   }
 
  // if (millis() - pMillis[colInt] >= color_ptr->holdTime)
  //   {
  //     if (peakVal[colInt] > newBrightness[colInt])
  //       {
  //         peakVal[colInt]--; // decrement until we reach the new lower brightness level
  //       }
  //     else 
  //       {
  //         pMillis[colInt] = millis(); // reset timer now that the leds match
  //       }      
  //   } 
  // peakLED.YPOS = peakVal[colInt]; //  sets the first fade pixel to be above the volume pixel
  // showPixel(&peakLED);  

}

void pixelGrid::play(pixelObject *newTruck)
{ 
  static pixelObject lastTruck[7]; // we are going to save the old pixel data here for comparison

  for (int c = 0; c < 7; c++)
    { 
      if (newTruck[c].volume < 0) continue; // if we aren't above the threshold, do nothing.
      if (newTruck[c].volume == lastTruck[c].volume) continue; // if nothing has changed, skip this iteration of the loop.
      //~ We are using C + 1 so that we don't use the first column, which is not used for volume.
      if (newTruck[c].volume > lastTruck[c].volume) 
        {
          // Don't freak out, C+1 is so that we don't show the first column as volumes
          for (int i = lastTruck[c].volume; i <= newTruck[c].volume; i++)
            {
              _neoPixels->setPixelColor(xyRemap(c + 1, i), newTruck[c].color);              
            }
        }
      else 
        {
          for (int i = lastTruck[c].volume; i >= newTruck[c].volume; i--)
            {
              _neoPixels->setPixelColor(xyRemap(c + 1, i), packColor(0,0,0,0));
            }
        }

      // // if the new volume is less than our previous volume, then let's turn pixels off until we reach that lower value
      // if (newTruck[c].volume < lastTruck[c].volume)
      //   {
      //     for (int i = lastTruck[c].volume; i > newTruck[c].volume; i--)
      //       {
      //         _neoPixels->setPixelColor(xyRemap(c + 1, i), packColor(0,0,0,0));
      //       }
      //   }         
    }

  //* Now we can show the peaks

  //playPeak(newTruck);

  for (int i = 0; i < 7; i++) lastTruck[i] = newTruck[i]; // copy current pixel state to the old one.
}

void pixelGrid::playPeak(pixelObject *truck)
{
  int interval = 3000;
  /**
   * First go and find all the peaks. If the volume is greater than the peak, then update the peak, and set the start time.
   * If the volume is less than the peak, then we need to check if the peak has been held for the hold time. If it has, then
   * we can start decrementing the peak until it matches the volume. If the volume is less than the peak, but the hold time has
   * not been reached, then we just keep the peak at the same value.
   */
  for (int c = 0; c < 7; c++)
    {
      // remove this at some point. Main should be controlling the colors
      truck[c].peakColor = packColor(100, 0, 0, 0);
      // First let's find any peaks.
      if (truck[c].volume > truck[c].peak) 
        {
          truck[c].peak = truck[c].volume; //& We actually don't need to do volume + 1, because we aren't showing the top pixel anyways
          truck[c].startTime = millis();
          _neoPixels->setPixelColor(xyRemap(c+1, truck[c].peak), truck[c].peakColor);     
        }
      // // if volume was less than peak, let's check how long the peak has been on.
      // else 
      //   {
      //     if (millis() - truck[c].startTime > interval)
      //       {
              
      //         // we still have to do c+1 to avoid the first column.
      //         for (int p = truck[c].peak; p > truck[c].volume; p--)
      //          {
      //           _neoPixels->setPixelColor(xyRemap(c+1, p), truck[c].peakColor);
      //          }
      //         //truck[c].peak = truck[c].volume + 1;
      //       }
      //   }
    }

  
  
}

byte pixelGrid::xyRemap(int x, int y)
{   
  if (x >= 9 || y >= 9) return -1;

  int columnSelect = xRowVals[x];  // use the lookup enum xRowVals to find the x column base number
  if (columnSelect != 0 && columnSelect % 2 == 0) // if the column number / 2 has a remainder, then it's a negative going column
    {
      int ledIndex = columnSelect - (y); 
      return ledIndex;
    }  
  else
    {
      int ledIndex = columnSelect + (y); 
      return ledIndex;
    }
}

uint32_t pixelGrid::packColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
  return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

int pixelGrid::decimalExtract(float value)
 {
  /* This function takes a float e.g 3.74 and casts that to an integer so
   * we only get the 1s place, and then we multiply that by 100. (300)
   * We then take the original float and * 100 to equal (374)
   * Now you can this number (374) and subrtact the (300) to get just the
   * decimals, being (74)
   */
  //Serial.print(value);
  int castToInteger = value; // 3.74 = 3
  castToInteger = castToInteger * 100; //300
  value = value * 100; //374
  int decimalValues = value - castToInteger; // (374 - 300 = 74)
  return map(decimalValues, 0, 99, 0, 255); // decimals never reach 100, so 99 to get full brightness at 255  
  //Serial.print(" Decimal Values = ");
  //Serial.print(decimalValues);
  //Serial.println(); 
}

void pixelGrid::updateGrid()
{
  _neoPixels->show();
  //_neoPixels->clear();
}
