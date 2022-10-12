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

List<darter> _data;

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
        if (abs(sampleBuffer[i]) >300)
        {
          Serial.println("adding");
         
          darter dart(0,0,0);
          _data.add(dart);
          i = samplesRead; // escape adding stuff.
        }
    }

    samplesRead = 0;
  }

  for(int i = 0; i < _data.getSize(); i++)
  {
    Serial.print(i);
    Serial.print("  ");
    int incoming = _data[i].getposition();
    Serial.print(incoming);
    Serial.println("updating");
    pixels.setPixelColor(incoming, pixels.Color(0, 0, 0));
    pixels.setPixelColor(incoming+1, pixels.Color(0, 128, 0));
    incoming++;
    _data[i].setposition(10);
    // if _data[i].getposition() > NUM_LEDS then can be removed from list
 
  }
  
  pixels.show();
  
  if (_data.getSize()>10)
  {
    _data.removeFirst();
  }
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
