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

//libraries for eHealth shield mySignals
#include <MySignals.h>
#include <MySignals_BLE.h>
#include <Wire.h>
#include <SPI.h>

//Variables
  float period = (1.0 / 256.0);
  float durata = 0.6;
  float initTime, fine, inizio;
  float conductance, conductanceOld, ecg, ECG,ECG255, bpm;
  int counter = 0, flag;
  bool alreadyDone = false;
  FilterBeHp5 *myfilter;


// Sensor Included 

  bool selected_ecg;
  bool selected_gsr;
  


void setup() {
  // Setup
  myfilter=new FilterBeHp5();
  MySignals.begin();
  Serial.begin(19200);

  //Bluetooth activation
  MySignals.initSensorUART();
  MySignals.enableSensorUART(BLE);
  MySignals_BLE.hardwareReset();
  MySignals_BLE.initialize_BLE_values();

  
  // SPM flag initialization
  MySignals.ECGFlagBPM = 1;
  // Interrupt every 10 ms
  MySignals.initInterrupt(6);

}

void loop() {

//   COMMUNICATION  
//  //1. SET MODE: SLAVE (VISIBLE TO APP)
//  while ((MySignals_BLE.ble_mode_flag == master_mode))
//  {
//    if (MySignals_BLE.setMode(slave_mode) == 0)
//    {
//      MySignals_BLE.ble_mode_flag = slave_mode;
//    }
//    else
//    {
//      MySignals_BLE.hardwareReset();
//      delay(100);
//      MySignals_BLE.initialize_BLE_values();
//    }
//  }

// 

   // Set initial time
   initTime = micros() / 1000000.0; // converto in secondi
   
    // Read sensor values
     conductance = MySignals.getGSR(CONDUCTANCE);
     ecg = MySignals.getECG(VOLTAGE);
     ECG=myfilter->step(ecg);
     bpm=MySignals.ECGDataBPMBalanced;


     // Conductance Control Value
     if (conductance == -1) conductance = conductanceOld; 
     else { conductanceOld = conductance; };

//   Control ecg
    if ((ECG < 0,05) && (flag==0)){
    ECG255=0;
    inizio=micros()/1000000.0;
    } else{
      fine=micros()/1000000.0;
      if ((fine-inizio)<durata){
        ECG255 = ECG;
        flag=1;
        }else{
          ECG255=0;
          flag=0;
          }
      }
     
   
   // Send sensor values:
   Serial.println( conductance );
   Serial.println( ECG );
   Serial.println( ECG255 );
   Serial.println( bpm );

    // Time Loop Control
    float elapsedTime = (micros()/1000000.0 - initTime) ;
    while (  elapsedTime <= period ) { 
    elapsedTime = (micros()/1000000.0 - initTime); };
    
}

