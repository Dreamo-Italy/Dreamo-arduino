/*

    Copyright (C) 2017 Libelium Comunicaciones Distribuidas S.L.
   http://www.libelium.com

    By using it you accept the MySignals Terms and Conditions.
    You can find them at: http://libelium.com/legal

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Version:           0.1
    Design:            David Gascon
    Implementation:    Luis Martin / Victor Boria
*/

/* THIS SKETCH GETS THE DATA FROM THE GALVANIC SKIN RESPONSE SENSOR AND THE ECG SENSOR,
 THAN IT SENDS CONDUCTANCE AND RESISTANCE VALUES BY MEANS OF A SERIAL LINE.
 THERE'S AN "OFFICE EXCEL" PLUGIN CALLED "PLX-DAQ" TO READ THE DATA FROM THE SERIAL LINE
 AND STORE IT IN A TABLE. */ 
 
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h" 


 
//High pass bessel filter order=5 alpha1=1.1627906976744 50 hz
class  FilterBeHp5
{
  public:
    FilterBeHp5()
    {
      for(int i=0; i <= 5; i++)
        v[i]=0.0;
    }
  private:
    float v[6];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = v[3];
      v[3] = v[4];
      v[4] = v[5];
      v[5] = (2.834655874754141158e-1 * x)
         + (0.06556815889477894677 * v[0])
         + (-0.53280555960268660698 * v[1])
         + (1.77085982088868343354 * v[2])
         + (-3.02516539564935360573 * v[3])
         + (2.67649986417774821135 * v[4]);
      return 
         (v[5] - v[0])
        +5 * (v[1] - v[4])
        +10 * (v[3] - v[2]);
    }
};
 

//Constants
const float baudRate = 38400;
const float samplingFrequency = 100;

//Variables
float elapsedTime = 0;
float samplingPeriod = (1.0 * 1000000 / samplingFrequency) ;
float conductance, ecg, ecg_filtered;
float resistance;
float conductanceVol;
float initTime;
boolean firstRun = true;
FilterBeHp5 *myfilter;


void setup() 
{// Setup
 
  myfilter = new FilterBeHp5();
  
  MySignals.begin();
  Serial.begin(baudRate); 
  while (!Serial) {}; 
  Serial.println("CLEARDATA"); //clears up any data left from previous projects
  Serial.println("LABEL, Time, Timer, Conductance, ECG, ECG_filtered, Sampling frequency, Baud rate");  
  //always write LABEL, so excel knows the vnext things will be the names of the columns 
  
  Serial.println("RESETTIMER"); //resets timer to 0 
  elapsedTime = micros() + samplingPeriod;

}

void loop() {    
  
  if( micros() >= elapsedTime )
  {   
    //get the values from the sensors  
    conductance = MySignals.getGSR(CONDUCTANCE);
    //resistance = MySignals.getGSR(RESISTANCE);
    //conductanceVol = MySignals.getGSR(VOLTAGE);
    
    ecg = MySignals.getECG(VOLTAGE);
    ecg_filtered = myfilter-> step(ecg);
  
    
    Serial.print("DATA, TIME , TIMER,"); //writes the time in the first column A and the time since the measurements started in column B
    Serial.print(conductance);  //writes the CONDUCTANCE in the third column
    Serial.print(",");
    Serial.print(ecg, 2);
    Serial.print(",");
    Serial.print(ecg_filtered, 2); 
    
    if ( firstRun ){
       Serial.print(",");
       Serial.print(samplingFrequency, 4); 
       Serial.print(",");
       Serial.print(baudRate, 4); 
       firstRun = false;
    }

    Serial.println("\n"); //go into the next row on the next run  
    
    elapsedTime += samplingPeriod;
  }
  
} // end loop



