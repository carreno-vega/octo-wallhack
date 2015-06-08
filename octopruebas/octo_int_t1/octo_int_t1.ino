//Programa de prueba para XbeeProv2
 
// avr-libc library includes
#include <avr/io.h>
#include <avr/interrupt.h>
 
#define LEDPIN 8
int aux_timer1; 
int seconds;
char a;

void setup()
{
  
  aux_timer1 = 0;
  seconds = 0;
  
  Serial.begin(9600); 
  
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN,LOW);
   
  // initialize Timer1
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
}
 
void loop()
{
  if(Serial.available())
  {
    a = Serial.read();
    Serial.println(a);
    
    if( a == 'o')
    digitalWrite(LEDPIN,LOW);
    
  }
  if(aux_timer1 == 1)
  {  
    seconds++;
    if (seconds == 2)   //Considerar que se demora en este caso 1 seg en alcanzar el estado de comparacion por lo que para 3 seg la condicion debe ser para 2
    {
      seconds = 0;
      digitalWrite(LEDPIN,HIGH);
      Serial.println("Hola");
    }
    aux_timer1 = 0;
  }
  

}
 
ISR(TIMER1_COMPA_vect)   //Flag correspondiente a timer1 comparacion
{                        
    aux_timer1=1;     
}





