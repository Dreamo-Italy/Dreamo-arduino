/* THIS SKETCH GETS THE DATA FROM THE GALVANIC SKIN RESPONSE SENSOR AND THE ECG SENSOR,
 THAN IT SENDS CONDUCTANCE AND RESISTANCE VALUES BY MEANS OF A SERIAL LINE.
 THERE'S AN "OFFICE EXCEL" PLUGIN CALLED "PLX-DAQ" TO READ THE DATA FROM THE SERIAL LINE
 AND STORE IT IN A TABLE. */ 
 
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
 
#include <eHealth.h>

//Constants
const float baudRate = 38400;
const float samplingFrequency = 100;

//Variables
float elapsedTime = 0;
float samplingPeriod = (1.0 * 1000000 / samplingFrequency) ;
float conductance, ecg, ecg_filtered;
float initTime;
boolean firstRun = true;
FilterBeHp5 *myfilter;


void setup() 
{// Setup
 
  myfilter = new FilterBeHp5();
  
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
    conductance = eHealth.getSkinConductance();
    ecg = eHealth.getECG();
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
