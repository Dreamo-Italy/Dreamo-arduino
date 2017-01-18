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

//Libraries
#include <eHealth.h>

//Variables
  float period = (1.0 / 256.0);
  float durata = 0.6;
  float initTime, fine, inizio;
  float conductance, conductanceOld, ecg, ECG,ECG255;
  int  flag;
  int inByte = 0; 
  FilterBeHp5 *myfilter;

  
void setup() {
  // Setup
  myfilter=new FilterBeHp5();
  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

establishContact();  // send a byte to establish contact until receiver responds
  }

  
void loop() {


   // Set initial time
   initTime = micros() / 1000000.0; // converto in secondi
   
    // Read sensor values
     conductance = eHealth.getSkinConductance();
     ecg =  eHealth.getECG();
     ECG=myfilter->step(ecg);
     


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

if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    Serial.write(int(100*conductance));
    Serial.write(int(100*ECG255));
    
}
    // Time Loop Control
    float elapsedTime = (micros()/1000000.0 - initTime) ;
    while (  elapsedTime <= period ) { 
    elapsedTime = (micros()/1000000.0 - initTime); };
    
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(30);
  }
}

  
