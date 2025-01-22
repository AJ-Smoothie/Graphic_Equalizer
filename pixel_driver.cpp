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


void pixelGrid::play(pixelObject *newTruck)
{ 
  static pixelObject lastTruck[7];

  for (int c = 0; c < 7; c++)
    {
      //* We don't even need to worry about turning off the zeroth pixel. playPeaks takes care of that.
      // // let's start by handling the 0 case
      // //! This can probably go because we'll set the bottom to a peak color anyways.
      if (newTruck[c].volume == 0)
        {
          _neoPixels->setPixelColor(xyRemap(c+1, 0), packColor(0,0,0,100));
          continue; // skip this iteration of the loop.
        }

      if (newTruck[c].volume > lastTruck[c].volume)
        {
          for (int i = lastTruck[c].volume; i <= newTruck[c].volume; i++)
            _neoPixels->setPixelColor(xyRemap(c+1, i-1), newTruck[c].color); //! i-1 hides the last pixel
        }
      
      if (newTruck[c].volume < lastTruck[c].volume)
        {
          for (int i = lastTruck[c].volume; i > newTruck[c].volume; i--)
            _neoPixels->setPixelColor(xyRemap(c+1, i-1), packColor(0,0,0,0)); //! i-1 to match above
        }
    }
  
  for (int i = 0; i < 7; i++) lastTruck[i].volume = newTruck[i].volume;

  playPeak(newTruck);
}

void pixelGrid::playPeak(pixelObject *truck)
{
  //static bool startFlags[7];
  //static int peaks[7];

  for (int c = 0; c < 7; c++)
    {
      if (truck[c].volume > truck[c].peak) 
        {
          truck[c].peak = truck[c].volume;
          _neoPixels->setPixelColor(xyRemap(c+1, truck[c].peak), truck[c].peakColor);
        }

      if (truck[c].volume < truck[c].peak) 
        {
          // if the volume dips below the peak, start our timer.
          if (!truck[c].startFlag)
            {
              truck[c].startTime = millis();
              truck[c].startFlag = true;
            }
          
          if (millis() - truck[c].startTime > truck[c].holdTime)
            {
              for (int i = truck[c].peak; i > truck[c].volume; i--)
                {
                  _neoPixels->setPixelColor(xyRemap(c+1, i), packColor(0, 0, 0, 0));  // turn off peak pixel
                  _neoPixels->setPixelColor(xyRemap(c+1, i-1), truck[c].peakColor);   // move peak pixel down
                }
              truck[c].startFlag = false;
            }
        }
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
