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
  _neoPixels->clear(); // erases all the pixels, but doesn't SHOW them!

  for (int c = 0; c < 7; c++)
    {
      //& Since we are doing < and not <=, we aren't showing the top pixel.
      for (int i = 0; i < newTruck[c].volume; i++)
        {
          _neoPixels->setPixelColor(xyRemap(c+1, i), newTruck[c].color);
        }
    }
  playPeak(newTruck);
}

void pixelGrid::playPeak(pixelObject *truck)
{
  for (int c = 0; c < 7; c++)
    {
      // always show the peak.
      _neoPixels->setPixelColor(xyRemap(c+1, truck[c].peak), truck[c].peakColor);
      // if there is higher peak, get it and save it.
      if (truck[c].volume > truck[c].peak) 
        {
          truck[c].peak = truck[c].volume;
          truck[c].counter = 0; // we just go an update, so reset counter
        }
      // if the volume is lower than the peak, then start incrementing the counter
      if (truck[c].volume < truck[c].peak) truck[c].counter++;
      if (truck[c].counter >= truck[c].increments)
        {
          truck[c].peak = truck[c].volume;
          truck[c].counter = 0;
          for (int i = truck[c].peak; i > truck[c].volume; i--)
            {
              _neoPixels->setPixelColor(xyRemap(c+1, i), packColor(0, 0, 0, 0));
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

void pixelGrid::show() { _neoPixels->show(); }
