// THIS SKETCH GETS THE DATA FROM THE GALVANIC SKIN RESPONSE SENSOR AND THE ECG SENSOR,
// THAN IT SENDS CONDUCTANCE AND RESISTANCE VALUES BY MEANS OF A SERIAL LINE.
 
 
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
 
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h" 

//Constants
const float baudRate = 38400;
const float samplingFrequency = 100;

//Variables
float samplingEnd = 0;
float samplingPeriod = (1.0 * 1000000 / samplingFrequency) ;
float conductance, ecg, ecg_filtered;
FilterBeHp5 *myfilter;
  
void setup() 
{// Setup 
  myfilter = new FilterBeHp5();
  
  Serial.begin(baudRate); 
  MySignals.begin();

  while (!Serial) {}; 
  
  samplingEnd = micros() + samplingPeriod;
}
  
void loop() 
{
 if( micros() >= samplingEnd )
  {   
    //get the values from the sensors  
    conductance = MySignals.getGSR(CONDUCTANCE);
    ecg = MySignals.getECG(VOLTAGE);
    ecg_filtered = myfilter-> step(ecg);   
       
   // Send sensor values:
   Serial.print( conductance );
   Serial.print("\t");
   Serial.println( ecg_filtered );

    samplingEnd += samplingPeriod;
    }    
 } // end loop
