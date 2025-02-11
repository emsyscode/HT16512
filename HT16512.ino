/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/
//set your clock speed
#define F_CPU 16000000UL
#define VFD_in 5// If 0 write LCD, if 1 read of LCD
#define VFD_clk 6 // if 0 is a command, if 1 is a data0
#define VFD_stb 7 // Must be pulsed to LCD fetch data of bus

/*Global Variables Declarations*/
unsigned char day = 7;  // start at 7 because the VFD start the day on the left side and move to rigth... grid is reverse way
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;
unsigned char milisec = 0;
unsigned char points = 0;
unsigned char secs;
unsigned char digit;
uint8_t digitSu = 0x00;
uint8_t digitSd = 0x00;
uint8_t digitMu = 0x00;
uint8_t digitMd = 0x00;
uint8_t digitHu = 0x00;
uint8_t digitHd = 0x00;
uint8_t number = 0x00;
uint8_t var = 0x00;
unsigned char numSecsU;
unsigned char numSecsD;
unsigned char numMinuU;
unsigned char numMinuD;
unsigned char numHourU;
unsigned char numHourD;
uint8_t temp = 0x00;
//unsigned char grid;
unsigned char wordA = 0;
unsigned char wordB = 0;
unsigned int k=0;
unsigned int segments[] ={
  //....-decgfba-----abfgced
      0b01110111, //0   // 0b11101110
      0b00010010, //1   // 0b00100100  
      0b01101011, //2   // 0b11010110 
      0b01011011, //3   // 0b10110110 
      0b00011110, //4   // 0b00111100
      0b01011101, //5   // 0b10111010
      0b01111101, //6   // 0b11111010
      0b00010011, //7   // 0b00100110
      0b01111111, //8   // 0b11111110
      0b01011111, //9   // 0b00111110
      0b00000000, //10  // 0b00000000  // empty display
  };

void HT16512_init(void){
  delayMicroseconds(200); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!
  // Configure VFD display (grids)
  cmd_with_stb(0b00000001);//  (0b01000000)    cmd1 5 grids 16 segm
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
   }
   //digitalWrite(VFD_clk, LOW);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(1);
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     delayMicroseconds(1);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
   }
   digitalWrite(VFD_stb, HIGH);
   delayMicroseconds(1);
}
void test_VFD(void){
  /* 
  Here do a test for all segments of 5 grids
  each grid is controlled by a group of 2 bytes
  by these reason I'm send a burst of 2 bytes of
  data. 
  */
  clear_VFD();
      
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(0b00000001); // cmd 1 // 5 Grids & 16 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
        
         for (int i = 0; i < 6 ; i++){ // test base to 16 segm and 5 grids
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         }
    
      //cmd_without_stb(0b00000001); // cmd1 Here I define the 7 grids and 15 Segments
      //cmd_with_stb((0b10001000) | 7); //cmd 4
      digitalWrite(VFD_stb, HIGH);
      delay(1);
      delay(800);  
}
void writeLED(){
  byte mask = 0x01;
  for(byte led=0; led<4; led++){
    mask = (mask << led);
              digitalWrite(VFD_stb, LOW);
              delayMicroseconds(1);
              cmd_with_stb(0b00000010); // cmd 1 // 6 Grids & 16 Segments
              //
                digitalWrite(VFD_stb, LOW);
                delayMicroseconds(1);
                cmd_without_stb(0b01000001);
                cmd_without_stb(0b00000000 | mask); // LED's 0,1,2,3 is reverse mode 0=On 1=Off
              //
              digitalWrite(VFD_stb, HIGH);
              delay(500);
  }
}
void searchSegments(void){
  /* 
  Here do a test for all segments of 6 grids
  each grid is controlled by a group of 2 bytes
  by these reason I'm send a burst of 2 bytes of
  data. The cycle for do a increment of 3 bytes on 
  the variable "i" on each test cycle of FOR.
  */
  // to test 6 grids is 6*3=18, the 8 gird result in 8*3=24.
  clear_VFD();
      
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(0b00000001); // cmd 1 // 5 Grids & 16 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
        
         for (int i = 0; i < 6 ; i++){ // test base to 16 segm and 6 grids
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         }
    
      cmd_without_stb(0b00000001); // cmd1 Here I define the 7 grids and 15 Segments
      cmd_with_stb((0b10001000) | 7); //cmd 4
      digitalWrite(VFD_stb, HIGH);
      delay(1);
      delay(500);  
}
void testNumbers(void){
 
      for (uint8_t i = 0; i < 10 ; i++){ // test base to 16 segm and 6 grids
  clear_VFD();
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(0b00000001); // cmd 1 // 5 Grids & 16 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11001000)); //cmd 3 wich define the start address (00H to 15H)
        //
         cmd_without_stb(segments[i] << 2); // //Shift to left 2 bits if is grid 8 & 9 ////Shift to left 1 bits if is grid 6 & 7... etc
         cmd_without_stb(segments[i] << 1); // Data to fill table 5*16 = 80 bits
        //
      // cmd_without_stb(0b00000001); // cmd1 Here I define the 7 grids and 15 Segments
      cmd_with_stb((0b10001000) | 7); //cmd 4
      digitalWrite(VFD_stb, HIGH);
      delay(1);
      delay(500);
        }  
}
void clear_VFD(void){
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 14; n++){  // important be 10, if not, bright the half of wells./this on the VFD of 5 grids)
        cmd_with_stb(0b00000001); //       cmd 1 // 5 Grids & 15 Segments
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | n); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 6 grids * 15 segm = 80 bits on the table
            //
            //cmd_with_stb((0b10001000) | 7); //cmd 4
            digitalWrite(VFD_stb, HIGH);
            delayMicroseconds(100);
     }
}
void HT16512_RunWeels(){
  // word1 = (lngTmp & 0xff000000UL) >> 24;
  // word2 = (lngTmp & 0x00ff0000UL) >> 16;
  // word3 = (lngTmp & 0x0000ff00UL) >>  8;
  // word4 = (lngTmp & 0x000000ffUL) >>  0;

  uint8_t j = 0x00;
  uint8_t n = 0x00;
  char x;
  short v = 0b0000000000000010;  // The short have a size of 16 bits(2 bytes) The firts bit is the symbol of "DVD"
  //
        for (n=0; n < 13; n++){  //
          //
                           wordB = (v & 0xff00UL) >>  8;
                           wordA = (v & 0x00ffUL) >>  0;

                          //cmd1 Configure VFD display (grids) 
                          cmd_with_stb(0b00000001);//  5 grids
                          delay(1);  // 
                          
                          //cmd2 Write to memory display, increment address, normal operation 
                          cmd_with_stb(0b01000000);//Teste mode setting to normal, Address increment Fixed, Write data to display memory...
                          
                          digitalWrite(VFD_stb, LOW);
                          delay(1);
                          //cmd3 Address 00H - 15H 
                          cmd_without_stb(0b11000000);
                          delay(1); 
                          cmd_without_stb(wordA); //First 8 bits 0x00
                          cmd_without_stb(wordB); //Second 8 bits 0x01
              delay(1);
              digitalWrite(VFD_stb, HIGH);
              //cmd4 set DIMM/PWM to value
              cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)//0 min - 7 max  )(0b01010000)
              delay(50);
              v = v << 1;
              //Serial.println(v, BIN);
            }
}
/******************************************************************/
/************************** Update Clock **************************/
/******************************************************************/
void send_update_clock(void){
  if (secs >=60){
    secs =0;
    minutes++;
  }
  if (minutes >=60){
    minutes =0;
    hours++;
  }
  if (hours >=24){
    hours =0;
  }
    //*************************************************************
    digitSu = (secs%10);
    numSecsU=segments[digitSu];
    digitSd = (secs/10);
    numSecsD=segments[digitSd];
    //*************************************************************
    digitMu = (minutes%10);
    numMinuU=segments[digitMu];
    digitMd = (minutes/10);
    numMinuD=segments[digitMd];
    //**************************************************************
    digitHu = (hours%10);
    numHourU=segments[digitHu];
    digitHd = (hours/10);
    numHourD=segments[digitHd];
    //**************************************************************
    SegTo32Bits(); // This is to send the total of digits to VFD
}
void SegTo32Bits(){
  //Serial.println(number,HEX);
  // This block is very important, it solve the difference 
  // between segments from digit 1 and digit 2 from the same grid.
  // It is necessary because segment "a" is firts bit of byte one
  // and the other is the second bit of second byte.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(2);
      cmd_with_stb(0b00000001); // cmd 1 // 6 Grids & 15 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(2);
        cmd_without_stb(0b11000000); //cmd 3 wich define the start address (00H to 15H)
          // Here you can adjuste which grid represent the values of clock
          // each grid use 2 bytes of memory registers

          cmd_without_stb(0x00);  // dummy unit    //used to skip wheels grid
          cmd_without_stb(0x00);  // dummy dozens  //used to skip wheels grid

          cmd_without_stb(0x00);  // dummy unit    //used to skip Second grid
          cmd_without_stb(0x00);  // dummy dozens  //used to skip Second grid
        
          cmd_without_stb(numHourD << 1); 
          cmd_without_stb(numHourU << 1);    // Only this digit on the grid got a shift because the second  //  dozens

          cmd_without_stb(numMinuD << 1); 
          cmd_without_stb(numMinuU << 1);    // Only this digit on the grid got a shift because the second  //  dozens
          Serial.println(digit, DEC);
          temp = (numSecsU << 1);
          switch (digitSd){  //This is necessary because the segment "C" Dozen of Seconds is the bit 8 (this is of second byte)
            case 0: var = (temp | 0x01); break;
            case 1: var = (temp) ; break;
            case 2: var = (temp | 0x01) ; break;
            case 3: var = (temp | 0x01); break;
            case 4: var = (temp); break;
            case 5: var = (temp | 0x01); break;
            case 6: var = (temp | 0x01); break;
            case 7: var = (temp); break;
            case 8: var = (temp | 0x01); break;
            case 9: var = (temp); break;
            default: { 
              ;
            }
          }
          Serial.println(numSecsU, BIN);
          //..............-decgfbass
          cmd_without_stb(numSecsD << 2); // seconds unit
          cmd_without_stb(var); // seconds dozens  // Only this digit on the grid got a shift because the second  //  dozens
    
      digitalWrite(VFD_stb, HIGH);
      delayMicroseconds(2);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(2);
}
void adjustHMS(){
 // Important is necessary put a pull-up resistor to the VCC(+5VDC) to this pins (3, 4, 5)
 // if dont want adjust of the time comment the call of function on the loop
  /* Reset Seconds to 00 Pin number 3 Switch to GND*/
    // if((AdjustPins & 0x08) == 0 )
    // {
    //   _delay_ms(200);
    //   secs=00;
    // }
    
    // /* Set Minutes when SegCntrl Pin 4 Switch is Pressed*/
    // if((AdjustPins & 0x10) == 0 )
    // {
    //   _delay_ms(200);
    //   if(minutes < 59)
    //   minutes++;
    //   else
    //   minutes = 0;
    // }
    // /* Set Hours when SegCntrl Pin 5 Switch is Pressed*/
    // if((AdjustPins & 0x20) == 0 )
    // {
    //   _delay_ms(200);
    //   if(hours < 23)
    //   hours++;
    //   else
    //   hours = 0;
    // }
}
void readButtons(){
  int val = 0;       // variable to store the read value
  int dataIn=0;
  byte array[8] = {0,0,0,0,0,0,0,0};
  byte together = 0;
  unsigned char receive = 7; //define our transmit pin
  unsigned char data = 0; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  array[0] = 1;
  unsigned char btn1 = 0x41;
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(2);
      cmd_without_stb(0b01000010); // cmd 2 //Read Keys;Normal operation; Set pulse as 1/16
  
  pinMode(VFD_in, INPUT_PULLUP);  // Important this point! Here I'm changing the direction of the pin to INPUT data.
  delayMicroseconds(2);
  //PORTD != B01010100; // this will set only the pins you want and leave the rest alone at
  //their current value (0 or 1), be careful setting an input pin though as you may turn 
  //on or off the pull up resistor  
  //This don't send the strobe signal, to be used in burst data send
         for (int z = 0; z < 3; z++){
             //for (mask=00000001; mask > 0; mask <<= 1) { //iterate through bit mask
                   for (int h =8; h > 0; h--) {
                      digitalWrite(VFD_clk, HIGH);  // Remember wich the read data happen when the clk go from LOW to HIGH! Reverse from write data to out.
                      delayMicroseconds(2);
                     val = digitalRead(VFD_in);
                           if (val & mask){ // if bitwise AND resolves to true
                            array[h] = 1;
                           }
                           else{ //if bitwise and resolves to false
                           array[h] = 0;
                           }
                    digitalWrite(VFD_clk, LOW);
                    delayMicroseconds(2);
                   } 
              Serial.print(z);  // All the lines of print is only used to debug, comment it, please!
              Serial.print(" - " );
                                  for (int bits = 7 ; bits > -1; bits--) {
                                      Serial.print(array[bits]);
                                   }
                        if (z==1){
                          if(array[6] == 1){
                           hours++;
                          }
                        }
                          if (z==0){
                          if(array[2] == 1){
                           hours--;
                          }
                          }
                          if (z==0){
                          if(array[6] == 1){
                           minutes++;
                          }
                        }
                        if (z==0){
                          if(array[7] == 1){
                           minutes--;
                          }
                        }
                        if (z==0){
                          if(array[3] == 1){
                           secs++;
                          }
                        }
                          if (z==1){
                            if(array[7] == 1){
                              hours = 0;
                              minutes = 0;
                             secs=0;  // Set count of secs to zero to be more easy to adjust with other clock.
                            }
                          }
                  Serial.println();
          }  // End of "for" of "z"
      Serial.println();  // This line is only used to debug, please comment it!
 digitalWrite(VFD_stb, HIGH);
 delayMicroseconds(2);
 cmd_with_stb((0b10001000) | 7); //cmd 4
 delayMicroseconds(2);
 pinMode(VFD_in, OUTPUT);  // Important this point!  // Important this point! Here I'm changing the direction of the pin to OUTPUT data.
 delay(1); 
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_stb, OUTPUT);

  Serial.begin(115200);
  seconds = 0x00;
  minutes =0x00;
  hours = 0x00;
  /*CS12  CS11 CS10 DESCRIPTION
  0        0     0  Timer/Counter1 Disabled 
  0        0     1  No Prescaling
  0        1     0  Clock / 8
  0        1     1  Clock / 64
  1        0     0  Clock / 256
  1        0     1  Clock / 1024
  1        1     0  External clock source on T1 pin, Clock on Falling edge
  1        1     1  External clock source on T1 pin, Clock on rising edge
 */
  // initialize timer1 
  cli();           // disable all interrupts
  // initialize timer1 
  //noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
  TCNT1  = 0;
  // Use 62499 to generate a cycle of 1 sex 2 X 0.5 Secs (16MHz / (2*256*(1+62449) = 0.5
  OCR1A = 62498;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
 
  
  // // Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
  // // a little the value 62499 upper or lower if the clock have a delay or advnce on hours.
    
  //  a=0x33;
  //  b=0x01;
  CLKPR=(0x80);
  //Set PORT
  DDRD = 0xFF;  // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
  PORTD=0x00;
  DDRB =0xFF;
  PORTB =0x00;
  HT16512_init();
  test_VFD();
  clear_VFD();
  //only here I active the enable of interrupts to allow run the test of VFD
  //interrupts();             // enable all interrupts
  sei();
}
void loop() {
  testNumbers();
  //searchSegments();
  test_VFD();
  delay(1000);
  clear_VFD();
  for (uint8_t r =0x00; r < 3; r++){
    writeLED();
  }
   while(1){
      send_update_clock();
      delay(150);
      readButtons();
      delay(150);
       HT16512_RunWeels();
   }
}
ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
                            // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
      secs++;
}
