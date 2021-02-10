                                                                                                                                                              // Programación básica tarjetaPI

// Declaración  de variables

// Barrido del display-7seg y scanner del teclado (PORTL)
// Display (4 bits inferiores)
volatile int D4=  B11111110;  //PL0 unidades-columna 1 del teclado
volatile int D3=  B11111101;  //PL1 decenas-columna 2 del teclado
volatile int D2=  B11111011;  //PL2 centenas-columna 3 del teclado
volatile int D1=  B11110111;  //PL3 unidades de millar - columna 4 (no existe en nuestro teclado)



// Pulsadores pup, pdown, pleft, pright, penter y speaker(PORTC)
int pright = 30;  //PC7
int pdown = 31;   //PC6
int pleft = 32;   //PC5
int penter = 33;  //PC4
int pup = 34;     //PC3
                  //PC2
                  //PC1
int speaker =37;  //PC0

/*
// Teclado (4 bits superiores)
volatile int fila_R1=   B01110000;  //PL7 fila_R1 (123)
volatile int fila_R2=   B10110000;  //PL6 fila_R2 (456)
volatile int fila_R3=   B11010000;  //PL5 fila_R3 (789)
volatile int fila_R4=   B11100000;  //PL4 fila_R4 (*0#)


// Display de 7 segmentos (PORTA)
volatile int dp=29;  //PA7
volatile int g= 28;  //PA6
volatile int f= 27;  //PA5
volatile int e= 26;  //PA4
volatile int d= 25;  //PA3
volatile int c= 24;  //PA2
volatile int b= 23;  //PA1
volatile int a= 22;  //PA0
*/


// otras variables
volatile int  tecla_anterior=-1;
volatile int  tecla_actual=-1;
volatile int modoTurnomatic = 0; // 0: funcionamiento normal, modo turnomatic
volatile int estado=0;
volatile int contador=0;
volatile int incremento=1;
volatile int unidades=0;
volatile int decenas=0;
volatile int val;

// Parámetros del beep (speaker)
unsigned int sound_base = 100;  // 100 Hz de base
unsigned int sound = 100;       //sound = 100 Hz (emnpezamos con sonido base)
unsigned int duration = 200;    // duración del beep o sonido


  // código de 7-segmentos
  byte tabla7seg[] = {63,06,91,79,102,109,125,39,127,103,0};
  int vsound[]={262,277,293,311,330,349,370,392,440,493};



void setup(){
  Serial.begin(9600);
  
    //PA7-PA0 (dpgfedcba) salidas --> display 7-seg
    DDRA = B11111111;

    // PC7-PC0 --> pulsadores entrada PC7-PC3 (entrada)) PC4-PC5 (free, entrada), PC0 speaker (salida)
    DDRC = B00000001;
    // activación pull-up en las líneas de entrada
    PORTC = B11111110;
  
    // PL7-PL0 --> barrido display PL4-PL0 (salida) y scanner teclado (PL7-PL4)
    DDRL = B00001111;
    // activación líneas de pull-up en las entradas de PORTL
    PORTL = B11111111; 


// Habilitación del TIMER1 para interrumpir cada 10ms (100Hz)
  // Funcionamiento normal... todo a cero
  // Disable interrupts
  cli();
  // modo normal de funcionamiento
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0; // cuenta inicial a cero
  // mode CTC
  TCCR1B |= (1<<WGM12);
  // prescaler  N = 1024
  TCCR1B |= (1<<CS12)|(1<<CS10);
  // fintr = fclk/(N*(OCR1A+1)) --> OCR1A = [fclk/(N*fintr)] - 1
  // para fintr = 100Hz --> OCR1A = [16*10^6/(1024*100)] -1 = 155,25 --> 155
  
  OCR1A = 77; // para 200 Hz  programar OCR1A = 77 (se ve mejor a 200 Hz!)

  // enable timer1 compare interrupt
  TIMSK1 |=(1<<OCIE1A);
  // habilitamos interrupciones
  sei();
}


void loop(){
 
  // Programa principal de lectura de los pulsadores
  
   if (digitalRead(pup) == 0){
     
     // Incrementar
    incrementar();    
    do
    {
      val = digitalRead(pup);
      if (digitalRead(pdown)==0) reset();      
    }while(val == 0);
  }
  
    // decrementar
    if (digitalRead(pdown) == 0){
    // Decrementar
    decrementar();
    do
    {
      val = digitalRead(pdown);
      if (digitalRead(pup)==0) reset();
    }while(val==0);
  } 

// Comprueba efectos especiales
  if(modoTurnomatic == 1) piano();
  if(modoTurnomatic == 2) gusano();
}



// ************************************************************
// Funcionamiento del turnomatic en modo piano
void piano()
  {
    int decenas_old = decenas;
    int unidades_old = unidades;
    unidades = 10;
    decenas = 10;

     do{
      if(tecla_actual!=-1){
        //Serial.println(tecla_actual);
        //Serial.println(vsound[tecla_actual]);
        tone(speaker,vsound[tecla_actual],400);
        //delay(300);
        unidades=tecla_actual;
        tecla_actual=-1;
      }
     }while(digitalRead(pup) || digitalRead(pdown)); // seguimos mientras no están los dos pulsados
     while(!(digitalRead(pup) &&  digitalRead(pdown))); //cuando soltemos los dos pulsadores seguimos
     decenas = decenas_old;
     unidades = unidades_old;
     modoTurnomatic = 0;    // Retorno a modo= 0 --> turnomatic recupera su modo de funcionamiento normal.
  }
// ************************************************************  

// ************************************************************
// Funcionamiento del turnomatic en modo gusano
void gusano() {
  PORTA = 0x01;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x02;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x04;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x08;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x10;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x20;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x01;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x02;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x04;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x08;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x10;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);

  PORTA = 0x20;
  digitalWrite(49, HIGH);
  delay(250);
  digitalWrite(49, LOW);


  PORTA = 0xff;
  int val = 0;
  //Para cumplir que sean unicamente 3 segundos, entramos 3 veces al while con delays de 0,5s
  while(val < 3){
    //Encendemos decenas y unidades
    digitalWrite(49, HIGH);
    digitalWrite(48, HIGH);
    delay(500);

    //Apagamos las decenas y unidades
    digitalWrite(49, LOW);
    digitalWrite(48, LOW);
    delay(500);
    // Incrementamos
    val = val + 1;
  }
  // Retorno al funcionamiento normal del turnomatic
  modoTurnomatic = 0;
}
  
void incrementar()
{
      contador=decenas*10+unidades;
      contador=contador+incremento;
      if (contador >99)contador = contador-100;
      decenas = int(contador/10);
      unidades = contador % 10;
      beep();
}

void decrementar()
{
      contador=decenas*10+unidades;
      contador=contador-incremento;
      if (contador < 0)contador = contador+100;
      decenas = int(contador/10);
      unidades = contador % 10;
      beep();
}

void reset()
{
      unidades=0;
      decenas=0;
      beep();
}

void beep(){
  tone(speaker,sound,duration);
} 


// Rutina de servicio de la interrupción del TIMER1
ISR(TIMER1_COMPA_vect)
{
  // display-off
  PORTL=PORTL|B00001111; // D1D2D3D4 = 1111
  // variable de estado: 0(unidades-col1), 1(decenas-col2), 2(centenas-col3) y 3(millares-xx)
  switch(estado){
    case 0:
       // UNIDADES-COLUMNA 1
      // Actualizamos portA con unidades
      PORTA = tabla7seg[unidades];
      // Activamos unidades en PORTL (D1D2D3D4)
      PORTL = D4; // PORTL=1111 1110
      teclado(estado);
      estado=1;
     
      break;
    
    case 1:
      // DECENAS- COLUMNA 2
     // Actualizamos portA con decenas
      PORTA = tabla7seg[decenas];
     // Activamos decenas en PORTL (D1D2D3D4)
      PORTL=D3;   // PORTL=1111 1101
      teclado(estado);
      estado=2;
      break;
         
    case 2:
      // NO CENTENAS --> COLUMNA 3
      PORTA = tabla7seg[10];
      PORTL=D2;   // PORTL=1111 1011
      teclado(estado);
      estado=0;  // volvemos al "estado 0" ya que el estado 3 no es necesario.
      break;

    case 3:
      // NO MILLARES - NO COLUMNA 4 (nuestro teclado solo tiene 3 columnas, y los millares no se usan en el turnomatic)
      PORTA = tabla7seg[10];
      PORTL=D1;   // PORTL=1111 0111
      teclado(estado);
      estado=0;
      break;
      
  }
}


void teclado(int estado){
      int teclau, tecla;

       // Leemos teclado
       teclau = (PINL & 0xF0)>>4; // leemos filas del teclado
       if (teclau != 15) // hay pulsación y esperamos a que se suelte tecla
       {
         while( ((PINL & 0xF0)>>4) !=15);
       }
       
       switch(estado){
        case 0:
            // Barrido de la primera columna del teclado

              switch (teclau) {
                case 7:
                 tecla = 1;
                 break;
                case 11:
                 tecla = 4;
                 break;
                case 13:
                 tecla = 7;
                 break;
                case 14:
                 // "*" 0x2A=42
                 tecla  = '*';
                 break;
                }
                break;
      case 1:
             // barrido de la segunda columna del teclado
           
            switch (teclau) {
                case 7:
                 tecla = 2;
                 break;
                case 11:
                 tecla = 5;
                 break;
                case 13:
                 tecla = 8;
                 break;
                case 14:
                 tecla = 0;
                 break;              
               }
              break;
              
      case 2:
          // Barrido de la tercera columna

            switch (teclau) {
                case 7:
                 tecla = 3;
                 break;
                case 11:
                 tecla = 6;
                 break;
                case 13:
                 tecla = 9;
                 break;
                case 14:
                  // # (0x23)
                 tecla = '#';
                 break;  
              }
            break;
    }
    
// si teclau != 15 es que se ha pulsado una tecla
    if(teclau !=15){
      tecla_anterior=tecla_actual;
      tecla_actual=tecla;
      //Serial.println(tecla);
    }


    if (modoTurnomatic == 0){
      // Estamos en modo turnomatic y comprobamos si hay alguna alguna secuencia de teclas válida
    
       if(tecla_anterior == '*' && (tecla_actual >=0 && tecla_actual<=9))
         { // cambiar frecuencia sonido del beep
              sound = sound_base + tecla_actual * 400;
              if(tecla_actual == 0) sound=0;
              tecla_anterior = -1;
              tecla_actual = -1;
         }
         
       if(tecla_anterior == '*' && (tecla_actual == '#'))
         { // Poner el turnomatic en modo piano
              modoTurnomatic = 1;
              tecla_anterior = -1;
              tecla_actual = -1;
         }         

       if(tecla_anterior == '#' && (tecla_actual == '*')){
          // Poner el turnomatic en modo gusano
              modoTurnomatic = 2;
              tecla_anterior = -1;
              tecla_actual = -1;
       }
       
    }

}
