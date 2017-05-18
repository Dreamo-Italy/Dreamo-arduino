/*
 * SKETCH FAKE: MANDA 2 VALORI SULLA LINEA SERIALE. SIMULA UN DISPOSITIVO IN TRASMISSIONE.
 * UTILE PER TESTARE LA COMUNICAZIONE CON PROCESSING
 * 
*/
void setup() {
  
   Serial.begin(38400);
   while (!Serial) {};
   
   //samplingEnd = micros() + samplingPeriod;
}
int i = 0;

void loop() {

  
  // Send sensor values:
    Serial.print( i&0xF0 );
    Serial.print("\t");
    Serial.println( i&0x0F );
    i ++;
    delay(10);
}
