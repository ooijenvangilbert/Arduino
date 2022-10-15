#include <Adafruit_NeoPixel.h>
#include <PDM.h>

#include "darter.h"

#include <List.hpp>

#define DATA_PIN 0
#define NUM_LEDS 300 

Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 16000;

// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[512];

// Number of audio samples read
volatile int samplesRead;

// threshold for audio
int threshold = 300;

// increment of the led steps
int increment = 3;

List<Darter> _data;

void setup() 
{
  Serial.begin(115200);
    
  // put your setup code here, to run once:
  pixels.begin();
  pixels.show();

  // Configure the data receive callback
  PDM.onReceive(onPDMdata);

  // Optionally set the gain
  // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
  // PDM.setGain(30);

  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
  // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
  if (!PDM.begin(channels, frequency)) 
  {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  _data.clear();
}

void loop() 
{
  if (samplesRead)
  {
    for (int i = 0; i < samplesRead; i++) 
    {     
        if (abs(sampleBuffer[i]) >threshold)
        {
          int randNumberR = random(0, 128);  //RED random number variable
          int randNumberG = random(0, 128);  //GREEN random number variable
          int randNumberB = random(0, 128);  //BLUE random number variable
          Darter dart(1,randNumberR,randNumberG,randNumberB);
          _data.add(dart);
          i = samplesRead; // escape adding stuff.
          memset(sampleBuffer, 0, sizeof(sampleBuffer)); // clearing the buffer, doesn't do much
        }
    }

    // Clear the read count
    samplesRead = 0;
  }

  for(int i = 0; i < _data.getSize(); i++)
  {
    Darter dart = _data[i];
    int incoming = dart.pos;

    pixels.setPixelColor(dart.pos, pixels.Color(0, 0, 0));
    pixels.setPixelColor(dart.pos + increment, pixels.Color(dart.r,dart.g, dart.b));
    incoming = incoming + increment;
    _data.removeFirst(); // remove this instance from the list
    
    if (incoming < NUM_LEDS)
    {
      // not the end of the strip, add to the list again.
      Darter dar (incoming,dart.r,dart.g,dart.b);
      _data.add(dar); 
    }
  }

  pixels.show();
}

/**
 * Callback function to process the data from the PDM microphone.
 * NOTE: This callback is executed as part of an ISR.
 * Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() 

{
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
