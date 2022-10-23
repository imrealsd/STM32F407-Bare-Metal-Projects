
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
void Turn_up_volume(void);
void Turn_down_volume(void);

int IR_OUT =  2;
int VOL_UP_PIN = 18;
int VOL_DOWN_PIN =  19;


 
uint32_t VOL_DOWN_DATA = 0b10001101011100101001101110001000;
uint32_t VOL_UP_DATA   = 0b10001110011100011001101110001000;


void setup(){
  
  pinMode(IR_OUT, OUTPUT);
  pinMode(VOL_UP_PIN , INPUT_PULLUP);
  pinMode(VOL_DOWN_PIN , INPUT_PULLUP);
  digitalWrite(IR_OUT,LOW);
}


void loop() {

  if (!digitalRead(VOL_UP_PIN)){
    Turn_up_volume();
    delay(100);
  }

  if(!digitalRead(VOL_DOWN_PIN)){
    Turn_down_volume();
    delay(100);
  }
 
}


void Turn_up_volume(void)
{
  volatile int len = 0;

  /* SOF */
  high_pulse_brust_38khz(9000);
  digitalWrite(IR_OUT,HIGH);
  delayMicroseconds(4500);

  /* 32 bit data frame [adddress + command] */
  while (len <=  31){


    if (VOL_UP_DATA & (1 << len)){
     
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



void Turn_down_volume(void)
{
  volatile int len = 0;

  /* SOF */
  high_pulse_brust_38khz(9000);
  digitalWrite(IR_OUT,HIGH);
  delayMicroseconds(4500);

  /* 32 bit data frame [adddress + command] */
  while (len <=  31){


    if (VOL_DOWN_DATA & (1 << len)){
     
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
