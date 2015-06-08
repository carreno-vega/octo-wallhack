#include <avr/io.h>
#include <avr/interrupt.h>

int led1 = 9;
int led2 = 10;
int aux_timer1; 
int seconds;
int led = 8;
int arreglo[10];
byte inbyte;                                  //Caracter de entrada comunicacion serial
byte flag_cal;                                 //valor de bandera de calibracion
byte val_cal1;                                 //valor de calibracion 1
byte val_cal2;                                 //valor de calibracion 2
byte inst1;                                    //valor de Instancia1
byte inst2;                                    //valor de Instancia2
byte inst3;                                    //valor de Instancia3
byte estado;                                   //valor de Estado
byte crc;                                      //valor CRC enviado 
byte sof_s;
byte flag_call_s;
byte val_cal1_s;                                 //valor de calibracion 1
byte val_cal2_s;                                 //valor de calibracion 2
byte inst1_s;                                    //valor de Instancia1
byte inst2_s;                                    //valor de Instancia2
byte inst3_s;                                    //valor de Instancia3
byte estado_s;                                   //valor de Estado
byte crc_s; 
byte eof_s;
int contador;


void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);   
  digitalWrite(led,LOW);  
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  
  tramaok = 0;
  contador=0;
  aux_timer1 = 0;
  seconds = 0;
  sof_s='#';
  flag_call_s = 1;
  val_cal1_s = 2;                                 //test valor de calibracion 1
  val_cal2_s = 3;                                 //t valor de calibracion 2
  inst1_s = 4;                                    //t valor de Instancia1
  inst2_s = 5;                                    //t valor de Instancia2
  inst3_s = 6;                                    //t valor de Instancia3
  estado_s = 7;                                   //t valor de Estado
  crc_s = 8; 
  eof_s = '%';
  cli();          // Desaciva las interrupciones globales
  TCCR1A = 0;     // pone el regitro TCCR1A entero a 0
  TCCR1B = 0;     // pone el registro TCCR1B entero a 0
  OCR1A = 15624;     //  Comparación cada un seg 15624, 31249 para dos seg,   www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);  //datasheet 32U4 
  sei();
}
int deco_trama()
{
  flag_cal    = arreglo[1];  //bandera de calibracion
  val_cal1    = arreglo[2];  //validar calibracion 1
  val_cal2    = arreglo[3];  //validar calibracion 2
  inst1       = arreglo[4];  //Instancia1
  inst2       = arreglo[5];  //Instancia2
  inst3       = arreglo[6];  //Instancia3
  estado      = arreglo[7]; //Estado
  crc         = arreglo[8]; // Verificacion por redundancia ciclica establecida en este programa 
}

void send_trama()
{
  seconds = 0;
  sof_s='#';
  flag_call_s = 1 + flag_call_s;
  val_cal1_s = 2 + val_cal1_s;                                 //valor de calibracion 1
  val_cal2_s = 3;                                 //valor de calibracion 2
  inst1_s = 4;                                    //valor de Instancia1
  inst2_s = 5;                                    //valor de Instancia2
  inst3_s = 6;                                    //valor de Instancia3
  estado_s = 7 + estado_s;                                   //valor de Estado
  crc_s = 8; 
  eof_s = '%';
  Serial.write(sof_s);
  Serial.write(flag_call_s);
  Serial.write(val_cal1_s);
  Serial.write(val_cal2_s);
  Serial.write(inst1_s);
  Serial.write(inst2_s);
  Serial.write(inst3_s);
  Serial.write(estado_s);
  Serial.write(crc_s);
  Serial.write(eof_s); 
}

int read_trama()
{
  digitalWrite(led2,HIGH);
  arreglo[contador] = Serial.read();   
  if(arreglo[contador] == '#')                                    
  {
    do                                                 
    {
      contador++;
      arreglo[contador] = Serial.read();                    
    }
    while(contador < 9);    
    if((arreglo[0] == '#') && (arreglo[9] == '%'))
    {
      contador = 0;
      digitalWrite(led1,HIGH);
      
      if(arreglo[1] == 12)
      {    
        digitalWrite(led,HIGH); 
      }
      else
      {
        digitalWrite(led,LOW);
      }
      return(1);
    }
    else
    {
      digitalWrite(led1,LOW); 
      contador = 0;
      return(0);      
    }
  } 
}

ISR(TIMER1_COMPA_vect)   //Flag correspondiente a timer1 comparacion
{                        
    aux_timer1=1;     
}

void loop()
{
  if (Serial.available() > 0)
  {   
    if(read_trama())
    {
      deco_trama();
    }
    else
    {
      Serial.println("Error de trama");
    } 
  }

  if(aux_timer1 == 1)
  {  
    seconds++;
    if (seconds == 1)   
    {
      send_trama();
    }
    aux_timer1 = 0;
  }
  delay(100);
}  



