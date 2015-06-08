//Comunicacion Serial para una trama de dato, inicando en '#' y terminando en '%'

int led = 8;
int x;
String inputString = "";
char inChar;
char number_sof;
char info_byte;
char number_eof;
int string_size;  //string_size de la trama desde java
int tramaok;      //condicion tramaok=0 trama sin estado, tramaok=1 trama funcionando,  tramaok=2 falla
void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT); 
  tramaok = 0;
}

void deco_trama()
{
  


}

void loop()
{
//si existe datos disponibles y se cumple condicion
  if (Serial.available()>0 && tramaok == 0)
  {   
    //leemos la opcion enviada
    inChar = (char)Serial.read();   //Se leen los caracteres en el buffer
    /*******************/
    if(inChar == '#')                //Si cumple con inicio de trama
    {
      do                             //
      {
        inChar = (char)Serial.read();    //Se rellena con datos basura para complentar cuando no hay recepcion
        inputString += inChar;
        string_size = inputString.length();
      }
      while((inChar != '%') && (string_size <= 10)); 
      
      if(inChar == '%')
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
    deco_trama();
    Serial.println(inputString);
    tramaok = 0;
    inputString="";
  }
  else if(tramaok == 2)
  {
    Serial.println("Error de trama");
    Serial.println(inputString);
    inputString="";
    tramaok = 0;
  }
  
  Serial.println("Hola");
  delay(500); 
}    
   /* x=size(inChar);
    Serial.print(x);
    
   
    
    else if(inChar == '%')
    {
    inputString=""; 
    }
     
    if(inChar == '%') 
    {
      string_size = inputString.length();
      Serial.println(string_size); 
      number_eof = inputString[string_size - 1];
      Serial.println(number_eof);
      info_byte = inputString[string_size - 2];
      number_sof = inputString[string_size -3];
      
      if(number_sof == '#' && info_byte == 'p')
      {
        digitalWrite(led, HIGH);     
        Serial.println("ON");
        inputString = "";            //Vacias String
      }      
      else if(number_sof == '#' && info_byte == 'a')
      {
        
        digitalWrite(led, LOW);     
        Serial.println("OFF");
        inputString = "";           //Vacias String
      }     
    }    
  }
}*/
