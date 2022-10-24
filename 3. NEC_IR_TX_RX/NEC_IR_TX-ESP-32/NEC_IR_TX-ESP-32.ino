

 /*IR Transmitter Board mislabeled: signal => +VCC , VCC => GND, GND => Signal
  * 
  */
 /*NEC FRAME with respect to Time:
  *
  * [START OF FRAME]->[8BIT DEVICE ADDRESS]->[8BIT INVERSED DEVICE ADDRESS]->[8BIT CMD]->[8BIT INVERSE CMD]->[END OF FRAME]
  *                   LSB               MSB   LSB                       MSB   LSB   MSB   LSB          MSB
  *  0ms                                                                                                              67 ms
  *  Time -->
  */

 /* Get 32bit code of each key from NEC_IR_Reciever_Decoder Project
 *
 * 32bit data frame: [cmp_cmd]-[cmd]-[cmp_addr]-[addr] : will transmit this from LSB to MSB 
 *                   MSB                           LSB
 */


void high_pulse_brust_38khz(uint16_t duration);
void send_ir_signal(uint32_t button_code);

int IR_OUT =  2;
int VOL_UP_PIN = 18;
int VOL_DOWN_PIN =  19;
int CH_UP_PIN = 23;
int CH_DOWN_PIN = 22;
int MUTE_PIN = 21;
int POWER_PIN = 5;


 
uint32_t VOL_DOWN_DATA = 0b10001101011100101001101110001000;
uint32_t VOL_UP_DATA   = 0b10001110011100011001101110001000;
uint32_t CH_UP_DATA = 0b10001100011100111001101110001000;
uint32_t CH_DOWN_DATA = 0b10001011011101001001101110001000;
uint32_t POWER_DATA = 0b11110100000010111001101110001000;
uint32_t MUTE_DATA = 0b11101000000101111001101110001000;


void setup(){
  
  pinMode(IR_OUT, OUTPUT);
  
  pinMode(VOL_UP_PIN , INPUT_PULLUP);
  pinMode(VOL_DOWN_PIN , INPUT_PULLUP);
  pinMode(CH_UP_PIN , INPUT_PULLUP);
  pinMode(CH_DOWN_PIN , INPUT_PULLUP);
  pinMode(POWER_PIN , INPUT_PULLUP);
  pinMode(MUTE_PIN , INPUT_PULLUP);
  
  digitalWrite(IR_OUT,LOW);
}


void loop() {

  if (!digitalRead(VOL_UP_PIN)){
    send_ir_signal(VOL_UP_DATA);
    delay(100);
  }

  if(!digitalRead(VOL_DOWN_PIN)){
    send_ir_signal(VOL_DOWN_DATA);
    delay(100);
  }
  if(!digitalRead(CH_UP_PIN)){
    send_ir_signal(CH_UP_DATA);
    delay(100);
  }

   if(!digitalRead(CH_DOWN_PIN)){
    send_ir_signal(CH_DOWN_DATA);
    delay(100);
  }

   if(!digitalRead(POWER_PIN)){
    send_ir_signal(POWER_DATA);
    delay(100);
  }

   if(!digitalRead(MUTE_PIN)){
    send_ir_signal(MUTE_DATA);
    delay(100);
  }
  
 
}


void send_ir_signal(uint32_t button_code)
{
  volatile int len = 0;

  /* SOF */
  high_pulse_brust_38khz(9000);
  digitalWrite(IR_OUT,HIGH);
  delayMicroseconds(4500);

  /* 32 bit data frame [adddress + command] */
  while (len <=  31){


    if (button_code & (1 << len)){
     
      high_pulse_brust_38khz(563);
      digitalWrite(IR_OUT,HIGH);
      delayMicroseconds(1690);
      
    } else {
       high_pulse_brust_38khz(563);
       digitalWrite(IR_OUT,HIGH);
       delayMicroseconds(563);
    }

    len++;
  }
  /*EOF*/
  high_pulse_brust_38khz(563);
  digitalWrite(IR_OUT,HIGH);
}




/* Frequency Modulation with carrier signal of 38khz */
void high_pulse_brust_38khz(uint16_t duration)
{
  unsigned long time = micros();
  Serial.println(micros()-time);
  
  while ((micros() - time) < duration){
    
    digitalWrite(IR_OUT, HIGH);
    delayMicroseconds(11);
    digitalWrite(IR_OUT, LOW);
    delayMicroseconds(12);
  }
}
