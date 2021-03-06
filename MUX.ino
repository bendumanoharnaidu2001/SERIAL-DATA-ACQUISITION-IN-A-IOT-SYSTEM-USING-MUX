#define MUX_A D4
#define MUX_B D3
#define MUX_C D2

#define ANALOG_INPUT A0


void setup() {
  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);     
  pinMode(MUX_C, OUTPUT);     
}

void changeMux(int c, int b, int a) {
  if(a==0){
       digitalWrite(MUX_A, LOW);
}else{
  digitalWrite(MUX_A, HIGH);
}
   if(b==0){
       digitalWrite(MUX_B, LOW);
}else{
  digitalWrite(MUX_B, HIGH);
}
   if(c==0){
       digitalWrite(MUX_C, LOW);
}else{
  digitalWrite(MUX_C, HIGH);
}
 
}

void loop() {
  float value;
  
  changeMux(0, 0, 0);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 0 pin of Mux
  
  changeMux(0, 0, 1);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 1 pin of Mux
  
  changeMux(0, 1, 0);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 2 pin of Mux

  changeMux(0, 1, 1);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 3 pin of Mux

  changeMux(1, 0, 0);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 4 pin of Mux

  changeMux(1, 0, 1);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 5 pin of Mux

  changeMux(1, 1, 0);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 6 pin of Mux

  changeMux(1, 1, 1);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 7 pin of Mux
  
}
