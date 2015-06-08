//Comunicacion Serial para una trama de dato, inicando en '#' y terminando en '%'

int led = 8;
int x;
String inputString = "";
char inChar;
int flag_cal;
int val_cal1;
int val_cal2;
int inst1;
int inst2;
int inst3;
int estado;
int crc;

//char number_sof;
//char info_byte;
//char number_eof;
int string_size;  //string_size de la trama desde java
int tramaok;      //condicion tramaok=0 trama sin estado, tramaok=1 trama funcionando,  tramaok=2 falla
void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT); 
  digitalWrite(led,LOW); 
  tramaok = 0;
}

//---------------------------------------------------------------------------------------------------------------------
//Funcion decodificar trama desde java.  Se le asigna a cada byte de informacion una variable que luego se intepretara

void deco_trama()
{
  flag_cal    = inputString[0];  //bandera de calibracion
  val_cal1    = inputString[1];  //validar calibracion 1
  val_cal2    = inputString[2];  //validar calibracion 2
  inst1       = inputString[3];  //Instancia1
  inst2       = inputString[4];  //Instancia2
  inst3       = inputString[5];  //Instancia3
  estado      = inputString[6]; //Esrado
  crc         = inputString[7]; // Verificacion por redundancia ciclica establecida en este programa
  
  if(flag_cal == 1)
  {
    
  digitalWrite(led,HIGH); 
  }
  
  Serial.println("flag_cal = ");
  Serial.println(flag_cal);
}

//---------------------------------------------------------------------------------------------------------------------

void loop()
{

//---------------------------------------------------------------------------------------------------------------------
    //Comienzo de comunicacion Serial - Reciviendo Trama de datos
  if (Serial.available()>0 && tramaok == 0)
  {   
    //se lee la opcion enviada
    inChar = (char)Serial.read();   //Se leen los caracteres en el buffer
    
    /************************************************************************/
    /* Condicion para comprobar la trama, comenzando por el inicio de trama (#) */
    if(inChar == '#')                //Si cumple con inicio de trama
    {
      do                             // Hacer
      {
        inChar = (char)Serial.read();    //Se rellena con datos basura para complentar cuando no hay recepcion
        inputString += inChar;
        string_size = inputString.length();
      }
      while((inChar != '%') && (string_size <= 8));  //mientras se cumpla condicion vuelve a  do
      
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
//---------------------------------------------------------------------------------------------------------------------

