#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum colorOptions { red = 0, green, blue, white, off };
const int ledColumnns = 7;


struct pixelObject
{
  int volume;
  int volumeOffset = -2;
  uint32_t brightness;
  uint32_t color; // (R, G, B, W)
  uint32_t peakColor; // (R, G, B, W)
  int peak = 0;
  unsigned long startTime;

  int holdTime; 
  int interval;
};

class pixelGrid
{
  public:
    int pixelCount = 0;
    int rows = 0; // for the library to keep track of
    int columns = 0;

    int xRowVals[9] = { 0, 16, 17, 34, 35, 52, 53, 70, 71 }; // these numbers are the postive going bottom numbers


    Adafruit_NeoPixel* _neoPixels; 
    pixelGrid(int pin, int xlen, int ylen); // data pin, pixel count
    ~pixelGrid();    
    void init();
    
    // we are calling pixelobjects trucks. They are semis carying information.
    void play(pixelObject *newTruck); // we will pass in a pointer to the pixelObject
    void playPeak(pixelObject *truck); // Pass in a pointer to an array containing peaks
    void playPeak(int column, pixelObject *pixel);
      // int newBrightness[ledColumnns]; // max number currently
      // int peakVal[ledColumnns]; //has to start at 1 because the volume for columns starts on 1
      // int colInt = 0; // column interval
      // unsigned long pMillis[ledColumnns];
      // unsigned long cMills;


    uint32_t packColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w); // packs RGBW into a 32 bit variable
    int decimalExtract(float value);
    byte xyRemap(int x, int y); // converts the numbering into an xy grid and returns an index number  
    void test(int x, int y); 
    void clear();
    
    void updateGrid();
};


// for (int i = 0; i < 7; i++)
//     {
//       Serial.print("CH: ");
//       Serial.print(i);
//       Serial.print("-");
//       Serial.print(ptr[i].volume);
//       Serial.print("\t");
//     }
//   Serial.println();