#include "LiquidCrystal.h"

//Code pour la captation du pouls fortement inspiré de  Pulse Sensor Amped 1.5    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com
//Code pour le joystick inspiré de http://idehack.com/blog/utiliser-un-joystick-avec-larduino/ (pour les noms de variable...)
//LiquidCrystal lcd(11,10,9,8,7,6,5,4,3,2); // liaison 8 bits de données
LiquidCrystal lcd(11,10,5,4,3,2); // liaison 4 bits de données

#define nb_questions 16

#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER  2

/*Problème de stockage :
 * Vue la faible capacité mémoire dynamique de l'arduino, il est très difficile d'y mettre beaucoup de questions, car celles-ci seraient NULL 
 * ou alors les autres chaines de caractère aussi (je pense aux noms). Solution :
 * soit on supprime encore des questions,
 * soit on n'utilise pas la classe String, qui est assez coûteuse notamment pour les fonctions liées à celle-ci.
   Le but aurait de base été de proposer une cinquantaine de questions (base solide) voire une centaine. Nous avons déchanté.
   Finalement, au prix d'une question, tout marche. Hourra !*/
   
String questions[nb_questions]={
"Es-tu jaloux ?",
"As-tu deja menti sur ton CV ?",
"T a-t-on deja surpris a voler ?",
"Couches-tu le premier soir ?",
"Es-tu plutot soumis ou dominateur ?",
"As-tu peur du noir ?",
"As-tu deja vole de l’argent a tes parents ?",
"As-tu deja souhaite la mort de quelqu’un ?",
"As-tu honte de tes amis ?",
"T a t'on deja pose un lapin ?",
"T es-tu deja drogue ?",
"Es-tu fidele ? ",
"Fais un compliment honnete a chaque joueur.",
"As-tu déjà triche a un test?",
"T es tu deja imagine avec le/la petite copine d un ami?",
"Pleures-tu devant les films ou series ?"
};
/*Certaines anciennes questions sont stockées ici.*/
//"As-tu déjà rompu avec une personne juste avant un jour férié afin de ne pas avoir à lui acheter un cadeau?",
//"As tu déjà accusé quelqu’un a ta place après avoir pété ?",
//"As-tu un ami imaginaire ?",
//"As-tu déjà goûté une nourriture pour animal?",
//"As-tu menti pendant ce jeu ?",
//"As tu deja revendu ou offert un kdo que tu as reçu ?",
//te laves tu tous lesj ours
//"As-tu déjà pensé à tromper ton petit ami ou ta petite amie ?",
//"T es tu deja reveille sans te souvenir de ta soiree ?",

///////////////////////////////////////////Variables/////////////////////////////////
//les pins pour les LEDs vie + mensonge
int led1 = 13;
int led2 = 12;
int led3 = 8;
int led_mensonge=9;

//les pins pour le joystick
int VRx = A0;
int VRy = A1;
int SW = 7;

int i=0, j=0; //variables pour le joystick
int x=0, y=0; //variables pour position du curseur sur l'écran

//des variables pour la position du joystick
int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;

int pulsePin = 3;                 // Pulse Sensor purple wire connected to analog pin 0

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// SET THE SERIAL OUTPUT TYPE TO YOUR NEEDS
// PROCESSING_VISUALIZER works with Pulse Sensor Processing Visualizer
//      https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer
// SERIAL_PLOTTER outputs sensor data for viewing with the Arduino Serial Plotter
//      run the Serial Plotter at 115200 baud: Tools/Serial Plotter or Command+L
static int outputType = SERIAL_PLOTTER;

//////////////////////////////////////Fonctions/////////////////////////////////////////////////////////////
int fonc_nb_joueurs(){//choisir le nombre de joueurs
  //int xPosition=0, yPosition=0, SW_state=0, mapX=0, mapY=0;
  while(1){
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      mapX = map(xPosition, 0, 1023, -512, 512);
      mapY = map(yPosition, 0, 1023, -512, 512);
      if(SW_state==0){
        int nb_joueurs=choix_joueurs();
        lcd.clear();
        lcd.home();
        lcd.print(nb_joueurs);
        lcd.print(" joueurs ?");
        ecrire_reponse();
        //boolean reponse=choisir_reponse();
        if(choisir_reponse()){
          lcd.noBlink();
          lcd.clear();
          return nb_joueurs;
        }
        else{
          lcd.clear();
          afficher_initialisation();
          i=0;
          j=0;
          x=0;
          y=0;
        }
      }
      //on regarde en fonction de la poussée du joystick, où passe le curseur ?
      if(mapX>100&&i==1){// une certaine sensibilité
        i--;
        x--;
      }
      else if(mapX<-100&&i==0){
        i++;
        x++;
      }
      if(mapY>100&&j<3){
        j++;
        y=y+4;
      }
      else if(mapY<-100&j>0){
        j--;
        y=y-4;
      }
      lcd.setCursor(4*j,i);
      delay(200);
  }
}

int choix_joueurs(){
  if(x==0&&y==0){
    return 1;
  }
  if(x==0&&y==4){
    return 2;
  }
  if(x==0&&y==8){
    return 3;
  }
  if(x==0&&y==12){
    return 4;
  }
  if(x==1&&y==0){
    return 5;
  }
  if(x==1&&y==4){
    return 6;
  }
  if(x==1&&y==8){
    return 7;
  }
  if(x==1&&y==12){
    return 8;
  }
}

void afficher_initialisation(){
  lcd.blink();
  int i, j=1;
  for(i=0;i<16;i=i+4){
    lcd.setCursor(i,0);
    lcd.print(j);
    j++;
  }
  for(i=0;i<16;i=i+4){
    lcd.setCursor(i,1);
    lcd.print(j);
    j++;
  }
  lcd.home();
}

void ecrire_reponse(){
  lcd.setCursor(0,1);
  lcd.print("Oui     ");
  lcd.print("Non");
  lcd.setCursor(0,1);
}

boolean choisir_reponse(){
  delay(1000);
  lcd.setCursor(0,1);
  y=0;
  while(1){
    yPosition = analogRead(VRy);
    SW_state = digitalRead(SW);
    mapY = map(yPosition, 0, 1023, -512, 512);
    if(SW_state==0){
      int reponse=y;
      if(y==0){
        Serial.println("Oui");
        return 1;
      }
      else if(y==1){
        Serial.println("Non");
        return 0;
      }
      Serial.println(y);
      break;
    }
    if(mapY>100&&y==0){
      y++;
    }
    else if(mapY<-100&&y==1){
      y--;
    }
    lcd.setCursor(8*y,1);
  }
}

volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P =512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 530;                // used to find instant moment of heart beat, seeded
volatile int amp = 0;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM


void interruptSetup(){  // CHECK OUT THE Timer_Interrupt_Notes TAB FOR MORE ON INTERRUPTS 
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}


// THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE.
// Timer 2 makes sure that we take a reading every 2 miliseconds
ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T){                        // T is the trough
      T = Signal;                         // keep track of lowest point in pulse wave
    }
  }

  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250){                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      //digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        sei();                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }


      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable

      for(int i=0; i<=8; i++){                // shift data in the rate array
        rate[i] = rate[i+1];                  // and drop the oldest IBI value
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
    //digitalWrite(blinkPin,LOW);            // turn off pin 13 LED
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 530;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  sei();                                   // enable interrupts when youre done!
}// end isr

void serialOutput(){   // Decide How To Output Serial.
  switch(outputType){
    case PROCESSING_VISUALIZER:
      sendDataToSerial('S', Signal);     // goes to sendDataToSerial function
      break;
    case SERIAL_PLOTTER:  // open the Arduino Serial Plotter to visualize these data
      Serial.print(BPM);
      Serial.print(",");
      Serial.print(IBI);
      Serial.print(",");
      Serial.println(Signal);
      break;
    default:
      break;
  }

}

//  Decides How To OutPut BPM and IBI Data
void serialOutputWhenBeatHappens(){
  switch(outputType){
    case PROCESSING_VISUALIZER:    // find it here https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer
      sendDataToSerial('B',BPM);   // send heart rate with a 'B' prefix
      sendDataToSerial('Q',IBI);   // send time between beats with a 'Q' prefix
      break;

    default:
      break;
  }
}

//  Sends Data to Pulse Sensor Processing App, Native Mac App, or Third-party Serial Readers.
void sendDataToSerial(char symbol, int data ){
    Serial.print(symbol);
    Serial.println(data);
  }

void setup()
{
   Serial.begin(9600);
   pinMode(VRx, INPUT);
   pinMode(VRy, INPUT);
   pinMode(SW, INPUT_PULLUP);
   pinMode(led1,OUTPUT);
   pinMode(led2, OUTPUT);
   pinMode(led3, OUTPUT);
   pinMode(led_mensonge, OUTPUT);
   
   lcd.begin(16,2); // utilisation d'un écran 16 colonnes et 2 lignes
   randomSeed(analogRead(0));
   interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
   afficher_initialisation();
}

int nb_joueurs=0;
int vies[8]={3,3,3,3,3,3,3,3};//nombre de vies des joueurs, initialisé à 3, descendant dès mensonge
int pouls[8]={60,60,60,60,60,60,60,60};//pouls calculé en début de partie pour chaque joueur
String noms[8]={"","","","","","","",""};

//pour gérer des variables non modifiables par suite
int lock_variable=1;

void loop(){
  if(lock_variable){
    nb_joueurs=fonc_nb_joueurs();
    Serial.print("Ceci est le nombre de joueurs :");
    Serial.println(nb_joueurs);
    delay(1000);
    for(int i=0;i<nb_joueurs;i++){
      lcd.clear();
      lcd.print("Nom du joueur ");
      lcd.print(i+1);
      wait_joystick_input();
      delay(500);
      noms[i].concat(choix_nom());
      Serial.println(noms[i]);
      lcd.clear();
      lcd.home();
      lcd.print("Voici ");
      lcd.print(noms[i]);
      delay(1500);
      lcd.clear();
      lcd.home();
      lcd.print("Pouls de ");
      lcd.print(noms[i]);
      wait_joystick_input();
      pouls[i]=initialiser_pouls();
    }
    lock_variable=0;
  }
  delay(1000);
  for(int k=0; k<nb_joueurs; k++){
    affichage_vies(k);
    lcd.print(noms[k]);
    lcd.print(" a toi !");
    wait_joystick_input();
    lcd.clear();
    int rand=random(nb_questions-1); //car il y a 22 questions
    lcd.setCursor(3,0);
    lcd.print(questions[rand]);
    //delay(1000);
    for(int a=0;a<questions[rand].length()-10;a++){
      lcd.scrollDisplayLeft();
      delay(300);
    }
    int mensonge=captation_pouls(k);
    if(mensonge)
      vies[k]--;
    lcd.clear();
    lcd.home();
  }
}

void affichage_vies(int k){
  switch(vies[k]){
    case 3:
      digitalWrite(led1,HIGH);
      digitalWrite(led2,HIGH);
      digitalWrite(led3,HIGH);
      break;
    case 2:
      digitalWrite(led1,HIGH);
      digitalWrite(led2,HIGH);
      digitalWrite(led3,LOW);
      break;
    case 1:
      digitalWrite(led1,HIGH);
      digitalWrite(led2,LOW);
      digitalWrite(led3,LOW);
      break;
    default:
      digitalWrite(led1,LOW);
      digitalWrite(led2,LOW);
      digitalWrite(led3,LOW);
      fin();
      break;  
  }
}

void scroll_question(int n, int i){
  lcd.setCursor(3,0);
  for(int j=0; j<n; j++){
      lcd.scrollDisplayLeft();
      delay(333);
  }
  lcd.clear();
}

void wait_joystick_input(){
  SW_state=digitalRead(SW);
  while(SW_state)
    SW_state=digitalRead(SW);
}

int captation_pouls(int k){
  lcd.clear();
  int mensonge=0;
  for(int i=30;i>0;i--){
    afficher_timer(i);
    BPM=valeur_pouls();
    if(BPM>=pouls[k]*1.3){
      mensonge=1;//on considère que le joueur ment dès lors qu'il y a une augmentation relative de son pouls mesuré de *1.3 (arbitraire)
      digitalWrite(led_mensonge,HIGH);
    }
    else{
      digitalWrite(led_mensonge,LOW);
    }
   }
   //delay(1000);
   return mensonge;
}
  
int valeur_pouls(){
  serialOutput() ;

    if (QS == true){     // A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
        //fadeRate = 255;         // Makes the LED Fade Effect Happen
                                // Set 'fadeRate' Variable to 255 to fade LED with pulse
        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
        QS = false;                      // reset the Quantified Self flag for next time
    }
    return BPM;
}
void fin(){
  lcd.clear(); 
  lcd.print("Gage et fin");
  while(1);
}

void afficher_timer(int i){
  lcd.setCursor(3,0);
  if(i<=9){      
    lcd.print(" ");
    lcd.print(i);
  }
  else{
    lcd.print(i); 
  }
  delay(1000);//à vérifier
}

int initialiser_pouls(){
  lcd.clear();
  int moyenne=0, pouls;
  for(int i=10;i>0;i--){
    afficher_timer(i);
    moyenne+=valeur_pouls();
  }
  Serial.println("Le pouls est : ");
  Serial.println(moyenne/10);
  lcd.clear();
  return moyenne/10;
}

String choix_nom(){
  afficher_nom();
  lcd.blink();
  unsigned int ecriture=0;//caractere : int en char en ASCII
  char caractere;
  int x=0, y=4;//gérer l'affichage sur l'écran LCD
  String nom;
  while(1){
    xPosition = analogRead(VRx);
    yPosition = analogRead(VRy);
    SW_state = digitalRead(SW);
    mapX = map(xPosition, 0, 1023, -512, 512);
    mapY = map(yPosition, 0, 1023, -512, 512);
    if(nom.length()>=3){
      SW_state=0;
      caractere=124;
    }
    if(SW_state==0){
        if(nom.length()<3)
          caractere=16*x+y+93;//car a=97 en ASCII, or a est sur la 4ème colonne 0ème ligne (en partant de zéro)
        if(caractere==123&&nom.length()>0){//retour, efface caractere
          ecriture--;
          lcd.setCursor(ecriture,0);
          lcd.print("_");
          nom.remove(ecriture);
        }
        else if(caractere==124){
          lcd.clear();
          lcd.print("Validation ");
          lcd.print(nom);
          lcd.print(" ?");
          delay(1000);
          ecrire_reponse();
          if(choisir_reponse()){
            lcd.clear();
            lcd.noBlink();
            Serial.println(nom);
            lcd.print(nom);
            return nom;
          }
          else{
            lcd.clear();
            afficher_nom();
            x=0;
            y=4;
            ecriture=0;
            nom="";
            delay(200);
          }
        }
        else{
          lcd.setCursor(ecriture,0);
          lcd.print(caractere);
          lcd.setCursor(y,x);
          //nom.concat(caractere);
          nom+=caractere;
          Serial.print("Caractere :");
          Serial.println(caractere);
          ecriture++;
        }
    }
    if(mapX>100&&x==1){
      x--;
      if(y<4){
        y=4;
      }
    }
    else if(mapX<-100&&x==0){
      x++;
    }
    if(mapY>100&&y<15){
      y++;
    }
    else if(mapY<-100&&((y>4&&x==0)||(y>0&&x==1))){
      y--;
    }
    lcd.setCursor(y,x);
    delay(200);
  }
}

void afficher_nom(){
  lcd.clear();
  lcd.home();
  lcd.print("___ abcdefghijkl");
  lcd.setCursor(0,1);
  lcd.print("mnopqrstuvwxyz←$");
  lcd.setCursor(4,0);
}
