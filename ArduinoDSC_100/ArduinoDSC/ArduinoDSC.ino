/*
  Arduino based Digital Setting Circle
 
 Michael Fulbright 2010-06-06
 
 <mike.fulbright at pobox.com>
 
 Feel free to modify and share - please let me know if you make any changes so I can keep my version up to date
 
 2010-06-06:  Initial relase - supports 2X encoding
 
 */

// constants won't change. They're used here to 
// set pin numbers:

const int AZ_enc_A = 2;  // digital pin 6 = PD6
const int AZ_enc_B = 4;  // digital pin 7 = PD7

const int ALT_enc_A = 3;   // digital pin 9 = PB1
const int ALT_enc_B = 5;  // digital pin 10 = PB2

const int RES = 4000;  // resolution of encoders

byte beenAligned = 0;  

long ALT_pos = RES/2;  // initial position of encoders is 
long AZ_pos = RES/2;   // half their resolution

void setup() {

  // initialize the encoder inputs
  pinMode(ALT_enc_A, INPUT);
  pinMode(ALT_enc_B, INPUT);
  pinMode(AZ_enc_A, INPUT);
  pinMode(AZ_enc_B, INPUT);
  
  
  pinMode(13,OUTPUT);


  attachInterrupt(ALT_enc_A, az_A, CHANGE);
  attachInterrupt(ALT_enc_B, az_B, CHANGE);
  
  attachInterrupt(AZ_enc_A, alt_A, CHANGE);
  attachInterrupt(AZ_enc_B, alt_B, CHANGE);
  

  Serial.begin(9600); 

  // Pin change interrupt control register - enables interrupt vectors
  // Bit 2 = enable PC vector 2 (PCINT23..16)
  // Bit 1 = enable PC vector 1 (PCINT14..8)
  // Bit 0 = enable PC vector 0 (PCINT7..0)
  //PCICR |= (1 << PCIE2);
  //PCICR |= (1 << PCIE0);

  // Pin change mask registers decide which pins are enabled as triggers
  //PCMSK2 |= (1 << PCINT23);  // arduino pin 7 = PD7 = az encoder A
  //PCMSK0 |= (1 << PCINT1);   // arduino pin 9 = PB1 = alt encoder A 

  // enable interrupts
  //interrupts();
}

void loop(){


  // see if command sent and reply if appropriate
  //

  byte inchar;
  while (!Serial.available())
  {
    delay(50);
  }
  inchar = Serial.read();

  // throw away rest of command - we don't need it
  Serial.flush();

  if (inchar == 'Q')
  {
    printEncoderValue(AZ_pos);
    Serial.print("\t");
    printEncoderValue(ALT_pos);
    Serial.print("\r");
  }
  else if (inchar == 'R' || inchar == 'Z' || inchar == 'I' || inchar == 'z')
  {
    // ignore command - just return proper code
    if (inchar == 'R' || inchar == 'I')  
      Serial.print("R");
    else if (inchar == 'Z')
      Serial.print("*"); 
    else if (inchar == 'z')
      Serial.print("r");
  }
  else if (inchar == 'r') 
  {
    // print out resolution - hardcoded to 4000 tics/rev
    Serial.print("04000\t04000\r");
  }
  else if (inchar == 'V')
  {
    //version
    Serial.print("V1.0.0\r");
  }
  else if (inchar == 'T')
  {
    // test mode - output resolutions and error count
    Serial.print("04000,04000,00000\r");
  }
  else if (inchar == 'q')
  {
    // error count
    Serial.print("00000\r");
  }
  else if (inchar == 'P')
  {
    // encoder power up
    Serial.print("P");
  }
  else if (inchar == 'p')
  {
    // 
    // dave eks error command
    Serial.print("00");
  } 
  else if (inchar == 'h')
  {
    // report resolution in Dave Eks format
    //Serial.print(0xA0, BYTE);
    Serial.print(0xA0);
    //Serial.print(0x0F, BYTE);
    Serial.print(0x0F);
    //Serial.print(0xA0, BYTE);
    Serial.print(0xA0);
    //Serial.print(0x0F, BYTE);
    Serial.print(0x0F);
  }
  else if (inchar == 'y')
  {
    // report encoders in Dave Eks format
    printHexEncoderValue(ALT_pos);
    printHexEncoderValue(AZ_pos);
  }  
  else if (inchar == 'a')
  {
    if (beenAligned)
      Serial.print("Y");
    else
      Serial.print("N");
  }
  else if (inchar == 'A')
  {
    beenAligned = 1;
  }

//#ifdef DEBUG
  Serial.print(digitalRead(AZ_enc_A));
  Serial.print(" ");
  Serial.print(digitalRead(AZ_enc_B));
  Serial.print(" ");
  Serial.print(digitalRead(ALT_enc_A));
  Serial.print(" ");
  Serial.print(digitalRead(ALT_enc_B));
  Serial.println();

  //return;

  Serial.print(AZ_pos);
  Serial.print(" ");
  Serial.print(ALT_pos);
  Serial.println();

  delay(500);
//#endif


}


// print encoder value with leading zeros
void printEncoderValue(long val)
{
  unsigned long aval; 

  if (val < 0)
    Serial.print("-");
  else
    Serial.print("+");

  aval = abs(val);

  if (aval < 10)
    Serial.print("0000");
  else if (aval < 100)
    Serial.print("000");
  else if (aval < 1000)
    Serial.print("00");
  else if (aval < 10000) 
    Serial.print("0");

  Serial.print(aval);  
}

void printHexEncoderValue(long val)
{
  byte low, high;

  high = val / 256;

  low = val - high*256;

  //Serial.print(low, BYTE);
  //Serial.print(high, BYTE);
  Serial.print(low);
  Serial.print(high);

  return;
}


void az_A()
{
  digitalWrite(13,HIGH);
    delay(200);
  digitalWrite(13,LOW);
    delay(200);
 
 
  if (digitalRead(AZ_enc_A) == digitalRead(AZ_enc_B))
  {
    AZ_pos++;
    if (AZ_pos >= RES)
      AZ_pos = 0;
  }
  else
  {
    AZ_pos--;

    if (AZ_pos < 0)
      AZ_pos = RES - 1;
  } 
}

void az_B()
{
   digitalWrite(13,HIGH);
    delay(200);
  digitalWrite(13,LOW);
    delay(200);
 
 
  if (digitalRead(AZ_enc_A) == digitalRead(AZ_enc_B))
  {
    AZ_pos++;
    if (AZ_pos >= RES)
      AZ_pos = 0;
  }
  else
  {
    AZ_pos--;

    if (AZ_pos < 0)
      AZ_pos = RES - 1;
  } 
}

void alt_A()
{
   digitalWrite(13,HIGH);
    delay(200);
  digitalWrite(13,LOW);
    delay(200);
 
  
   if (digitalRead(ALT_enc_A) == digitalRead(ALT_enc_B))
  {
    ALT_pos++;

    if (ALT_pos >= RES)
      ALT_pos = 0;
  }
  else
  {
    ALT_pos--;

    if (ALT_pos < 0)
      ALT_pos = RES - 1;
  }
}

void alt_B()
{
  digitalWrite(13,HIGH);
    delay(200);
  digitalWrite(13,LOW);
    delay(200);
  
   if (digitalRead(ALT_enc_A) == digitalRead(ALT_enc_B))
  {
    ALT_pos++;
    if (ALT_pos >= RES)
      ALT_pos = 0;
  }
  else
  {
    ALT_pos--;

    if (ALT_pos < 0)
      ALT_pos = RES - 1;
  }
}


/*
// we have to write our own interrupt vector handler..
ISR(PCINT2_vect){ 

  if (digitalRead(AZ_enc_A) == digitalRead(AZ_enc_B))
  {
    AZ_pos++;
    if (AZ_pos >= RES)
      AZ_pos = 0;
  }
  else
  {
    AZ_pos--;

    if (AZ_pos < 0)
      AZ_pos = RES - 1;
  } 
}


ISR(PCINT0_vect){

  if (digitalRead(ALT_enc_A) == digitalRead(ALT_enc_B))
  {
    ALT_pos++;

    if (ALT_pos >= RES)
      ALT_pos = 0;
  }
  else
  {
    ALT_pos--;

    if (ALT_pos < 0)
      ALT_pos = RES - 1;
  }

}

*/








