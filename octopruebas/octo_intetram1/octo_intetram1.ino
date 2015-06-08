#include <avr/io.h>
#include <avr/interrupt.h>

int aux_timer1; 
int seconds;

int led = 8;
String inputString = "";                      //String de entrada - comunicacion serial
char inChar;                                  //Caracter de entrada comunicacion serial
char flag_cal;                                 //valor de bandera de calibracion
char val_cal1;                                 //valor de calibracion 1
char val_cal2;                                 //valor de calibracion 2
char inst1;                                    //valor de Instancia1
char inst2;                                    //valor de Instancia2
char inst3;                                    //valor de Instancia3
char estado;                                   //valor de Estado
char crc;                                      //valor CRC enviado 

char sof_s;
char flag_call_s;
char val_cal1_s;                                 //valor de calibracion 1
char val_cal2_s;                                 //valor de calibracion 2
char inst1_s;                                    //valor de Instancia1
char inst2_s;                                    //valor de Instancia2
char inst3_s;                                    //valor de Instancia3
char estado_s;                                   //valor de Estado
char crc_s; 
char eof_s;



char crc_value;                               // Valor calculado para Verificacion por redundancia ciclica, calculo establecido entre programas
int string_size;  //string_size de la trama desde java
byte tramaok;      //condicion tramaok=0 trama sin estado, tramaok=1 trama funcionando,  tramaok=2 falla
void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT); 
  digitalWrite(led,LOW); 

  tramaok = 0;
  
/* Varibles para Interrupcion Timer1 */
  aux_timer1 = 0;
  seconds = 0;
/* --fin varibles Timer1*/ 

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
  
  
  // inicializar Timer1
  cli();          // Desaciva las interrupciones globales
  TCCR1A = 0;     // pone el regitro TCCR1A entero a 0
  TCCR1B = 0;     // pone el registro TCCR1B entero a 0
   
  // Se pone el numero con el que se desea hacer la comparacion:
  OCR1A = 15624;     //  Comparación cada un seg 15624, 31249 para dos seg,   www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
  // enciende el modo CTC:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler -> nuevafrecuencia=frecuenciaoscilador/1024:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // Habilira la interripcion por comparacion de tiempo para timer1
  TIMSK1 |= (1 << OCIE1A);  //datasheet 32U4 
  // Activa interrupciones globales
  sei();
  // Fin inicializar Timer1
}

//---------------------------------------------------------------------------------------------------------------------
//Funcion decodificar trama desde java.  Se le asigna a cada byte de informacion una variable que luego se intepretara

int deco_trama()
{
  flag_cal    = byte(inputString[1]);  //bandera de calibracion
  val_cal1    = inputString[2];  //validar calibracion 1
  val_cal2    = inputString[3];  //validar calibracion 2
  inst1       = inputString[4];  //Instancia1
  inst2       = inputString[5];  //Instancia2
  inst3       = inputString[6];  //Instancia3
  estado      = inputString[7]; //Estado
  crc         = inputString[8]; // Verificacion por redundancia ciclica establecida en este programa
  
  crc_value = flag_cal + (val_cal1/5) + (val_cal2/5) + inst1 + inst2 + inst3 + (estado/5);  //calculo de crc en arduino
  Serial.println("crc =");
  Serial.println(crc); 
  Serial.println("crc_value ");
  Serial.println(crc_value);
  
  
  if(flag_cal == '12')
  {    
    digitalWrite(led,HIGH); 
  }
  else
  {
    digitalWrite(led,LOW);
  }
  /*Serial.println("flag_cal = ");
  Serial.println(flag_cal);*/
  
  if(crc == crc_value)      //Comprobacion entre variable recibido crc y calculado crc_value
  {
    Serial.println("return 1");
    return 1;
  }
  else
  {
    Serial.println("return 0");
    return 0;
  }

  
 
}

//---------------------------------------------------------------------------------------------------------------------

void loop()
{

//---------------------------------------------------------------------------------------------------------------------
    //Comienzo de comunicacion Serial - Reciviendo Trama de datos
  if (Serial.available()>0 && tramaok == 0)
  {   
    inChar = (char)Serial.read();   //Se leen los caracteres en el buffer
    
    /************************************************************************/
    /* Condicion para comprobar la trama, comenzando por el inicio de trama (#) */
    if(inChar == '#')                                     //Si caracter de entrada cumple con inicio de trama
    {
      inputString += inChar;
      do                                                 // Hacer
      {
        inChar = (char)Serial.read();                    
        inputString += inChar;                           //Se apilan los caracteres en el inputString
        string_size = inputString.length();              //Tamaño del String
        
        //el inputString se rellena con datos basura para completar cuando no hay recepcion
      }
      while(string_size <= 9);     //mientras se cumpla condicion de que caracter distinto de '%' 
       
     
     Serial.println(inputString);     //y tamaño del string menos a 9 (string parte de 0 y no esta incluido # inicializacion de trama), vuelve a  do
     
      if((inputString[0] == '#') && (inputString[9] == '%'))
      {
        tramaok = 1;
      }
      else
      {
        tramaok = 2;
       
      }
    }
  }
  if(tramaok == 1)
  {
    if (deco_trama())
    {
      Serial.println("trama buena");
      Serial.println(inputString);
      flag_cal    = inputString[0];  //bandera de calibracion
      val_cal1    = inputString[1];  //validar calibracion 1
      val_cal2    = inputString[2];  //validar calibracion 2
      inst1       = inputString[3];  //Instancia1
      inst2       = inputString[4];  //Instancia2
      inst3       = inputString[5];  //Instancia3
      estado      = inputString[6]; //Estado
      tramaok = 0;
      inputString="";
    }
    
    else
    {
      tramaok = 0;
      inputString="";
    }
  }
  else if(tramaok == 2)
  {
    Serial.println("Error de trama");
    Serial.println(inputString);
    inputString="";
    tramaok = 0;
  }
  
  /* Fin de comprobacion de trama sof, eof y crc           */
  /*-------------------------------------------------------*/
 
 /**********************************************************/
 /*  Incio Interpretacion de variables de trama           */
   
  

 
/*--------------------------------------------------------------------*/
/* ------  Enviar datos cada n segundos utilizando interrupcion-------*/
  if(aux_timer1 == 1)
  {  
    seconds++;
    if (seconds == 1)   //Considerar que se demora en este caso 1 seg en alcanzar el estado de comparacion por lo que para 3 seg la condicion debe ser para 2
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
    aux_timer1 = 0;
  }
}  
//---------------------------------------------------------------------------------------------------------------------
// Interrupcion timer1
ISR(TIMER1_COMPA_vect)   //Flag correspondiente a timer1 comparacion
{                        
    aux_timer1=1;     
}


