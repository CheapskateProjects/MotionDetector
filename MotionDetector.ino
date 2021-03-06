/*
  Movement detector which supports reading multiple PIR sensors and reporting their changes to AWS IoT service through IoT gateway. 

  This detector should be used with AWS gateway ( https://github.com/CheapskateProjects/ESP8266AWSIoTGateway ) to automatically send results to IoT cloud. 

  Serial data example: 
  PIR1;{"state":{"reported":{"movement":"true"}}}
  
    
  created   Mar 2017
  by CheapskateProjects

  ---------------------------
  The MIT License (MIT)

  Copyright (c) 2017 CheapskateProjects

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

struct sensor
{
  int pin;
  bool wasHigh;
  bool wasLow;
  long unsigned int lowTime;
};
typedef struct sensor Sensor;

// This implementation expects used data pins to be in sequence (3, 4, 5, etc...). Give first data pin here. 
const int firstPin = 3;
// How many sensors do you want to use with this device?
const int numberOfSensors = 2;
Sensor sensors[numberOfSensors];

void setup()
{
  // Correct rate for our gateway
  Serial.begin(115200);

  
  for(int i = 0; i < numberOfSensors; i++)
  {
    sensors[i].pin = 3 + i;
    sensors[i].wasHigh = false;
    sensors[i].wasLow = false;
    sensors[i].lowTime = millis();
    pinMode(sensors[i].pin, INPUT);
  }

  // Wait for sensor to initialize and calibrate
  delay(30000);
} 

void loop()
{
  for(int i = 0; i < numberOfSensors; i++)
  {
    if(digitalRead(sensors[i].pin) == HIGH)
    {
      sensors[i].wasHigh = true;
      sensors[i].lowTime = 0;
      
      if(sensors[i].wasLow)
      {
        // Previously low. Should send message about movement!
        Serial.print("PIR");
        Serial.print(i+1);
        Serial.print(";{\"state\":{\"reported\":{\"movement\":\"true\"}}}\n");
        sensors[i].wasLow = false;
      }
    }
    else
    {      
      if(sensors[i].wasHigh)
      {
        sensors[i].lowTime = millis();// Capture begining of low
        sensors[i].wasHigh = false;
      }
      else
      {
        if(millis() - sensors[i].lowTime > 30000 && !sensors[i].wasLow)
        {
          Serial.print("PIR");
          Serial.print(i+1);
          Serial.print(";{\"state\":{\"reported\":{\"movement\":\"false\"}}}\n");
          sensors[i].wasLow = true;
        }
      }
    }
  }
}
