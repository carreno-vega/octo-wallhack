#include <avr/io.h>
#include <avr/interrupt.h>

/*Varibles de prueba*/
int  led = 8;
int  led1 = 9;
int  led2 = 10;
int  led3 = 11;

/*Timer1*/
int  aux_timer1; 
int  seconds;
/*Buffer de trama recibida*/
int  arreglo[10];
int  contador;
/*Identificadores de trama*/
byte  id_trama;
byte  estado_rx;
/*Variables Set point*/
byte  ph_sp;
byte  od_sp;  
byte  temp_sp;
byte  rpm_sp;  
byte  aux1_sp;
byte  aux2_sp;
/* Variables Control Manual*/
byte  inst1;
byte  inst2;  
byte  inst3;  
byte  aux1_man;
byte  aux2_man;  
byte  aux3_man;
/* Variables de calibracion*/
byte  flag_cal; 
byte  val_cal1; 
byte  val_cal2;
byte  aux1_cal;
byte  aux2_cal;
byte  aux3_cal; 
/*Variables enviadas*/
byte  sof_tx;
byte  id_trama_tx;                // normal = 1, confirm = 2
byte  estado_tx;               // envia estado q se recibiò
byte  sens1_tx;
byte  sens2_tx;
byte  sens3_tx;
byte  sens4_tx;
byte  sens5_tx;
byte  aux_tx;
byte  eof_tx;

/**/


void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);    
  digitalWrite(led,LOW);  
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
  
  
  contador    = 0;
  aux_timer1  = 0;
  seconds     = 0;
  sens1_tx    = 0;
  sens2_tx    = 0;
  sens3_tx    = 0;
  /* Inicio del timer1*/
  cli();          // Desaciva las interrupciones globales
  TCCR1A  = 0;     // pone el regitro TCCR1A entero a 0
  TCCR1B  = 0;     // pone el registro TCCR1B entero a 0
  OCR1A   = 15624;     //  Comparación cada un seg 15624, 31249 para dos seg,   www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);  //datasheet 32U4 
  sei();
}
int deco_trama()
{
  id_trama    = arreglo[1];
  estado_rx   = arreglo[2]; 
  
  switch(id_trama)
  {
    case(1): //trama = setpoint
    { 
      digitalWrite(led,HIGH);
      digitalWrite(led1,LOW); 
      digitalWrite(led2,LOW);
      ph_sp       = arreglo[3];  
      od_sp       = arreglo[4];  
      temp_sp     = arreglo[5];  
      rpm_sp      = arreglo[6];  
      aux1_sp     = arreglo[7]; 
      aux2_sp     = arreglo[8];       
      break;
    }
    case(2): //trama = manual
    { 
      digitalWrite(led1,HIGH);
      digitalWrite(led,LOW); 
      digitalWrite(led2,LOW); 
      inst1     = arreglo[3];  
      inst2     = arreglo[4];  
      inst3     = arreglo[5];  
      aux1_man  = arreglo[6];  
      aux2_man  = arreglo[7];  
      aux3_man  = arreglo[8];
      if(inst1 == 1)
      {
        digitalWrite(led3,HIGH); 
      }
      else
      {
        digitalWrite(led3,LOW); 
      } 
      break;
    }
    case(3):  // trama = calibracion
    { 
      digitalWrite(led2,HIGH);
      digitalWrite(led,LOW); 
      digitalWrite(led1,LOW); 
      flag_cal   = arreglo[3];  
      val_cal1   = arreglo[4];  
      val_cal2   = arreglo[5];  
      aux1_cal   = arreglo[6];  
      aux2_cal   = arreglo[7];  
      aux3_cal   = arreglo[8]; 
      break;
    }
    default:break;  
  }
}
byte make_trama(byte a,byte b)
{
  sof_tx      = '#';
  id_trama_tx = a;                // normal = 1, confirm = 2
  estado_tx   = b;               // envia estado q se recibiò
  sens1_tx    = 1 + sens1_tx;
  sens2_tx    = 2;
  sens3_tx    = 3 + sens3_tx;
  sens4_tx    = 4;
  sens5_tx    = 5;
  aux_tx      = 6;
  eof_tx      = '%';
}
void send_trama()
{
  Serial.write(sof_tx);
  Serial.write(id_trama_tx);
  Serial.write(estado_tx);
  Serial.write(sens1_tx);
  Serial.write(sens2_tx);
  Serial.write(sens3_tx);
  Serial.write(sens4_tx);
  Serial.write(sens5_tx);
  Serial.write(aux_tx);
  Serial.write(eof_tx); 
}

int read_trama()
{
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
      return(1);
    }
    else
    {
      contador = 0;
      return(0);      
    }
  } 
}
/*Interrupcion Timer*/
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
      make_trama(2,estado_rx);  // 2 trama de conf, estado_rx estado de conf
      send_trama();
    }
    else
    {
      Serial.println("Error de trama");
      digitalWrite(led1,LOW);
      digitalWrite(led,LOW); 
      digitalWrite(led2,LOW);
      digitalWrite(led3,LOW);  
    } 
  }

  if(aux_timer1 == 1)
  {  
    seconds++;
    if (seconds == 1)   
    {
      make_trama(1,0);  // 1 trama normal, 0 estado ok
      send_trama();
      seconds = 0;
    }
    aux_timer1 = 0;
  }
  delay(100);
}  



