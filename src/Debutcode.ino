#include <FreqMeasure.h>
#include <LiquidCrystal.h> 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FreqMeasure.begin();
  initialisation();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("2");
  delay(1000);
}

void initialisation(){
  Serial.print("Combien de joueurs êtes-vous?");
  int nb_joueurs=Serial.read();
  if(nb_joueurs>8){
    Serial.print("Trop de joueurs !");
    initialisation();
  }

  String questions[4]={"Comment t'appelles-tu ?", "Quel âge as-tu?", "En quelle année es-tu né ?","Quel est ton métier ?"};
  int* donnees_joueurs;
  for(int i=0;i<nb_joueurs;i++){
    Serial.print("Joueur ");
    Serial.print(i+1);
    Serial.print(" à toi !\n");
    Serial.print("Place les capteurs sur toi !");
    delay(5000);//délai de 5 secondes, il faut lui laisser le temps
    while(!FreqMeasure.available()){
      Serial.print("Place les capteurs sur toi !");
      delay(5000);
    }
    Serial.print("Quelques questions vont vous être poséees. Essayez d'y répondre le plus sérieusement possible, sans quoi le jeu sera nul.");

    int frequenceCardiaque=0;
    int compte=0;
    for(int j=0;j<4;j++){
      Serial.print(questions[j]);
      for(int k=0;j<6;j++){
        delay(500);
        frequenceCardiaque=frequenceCardiaque+FreqMeasure.read();
        compte++;
        j++;
      }
    }
    donnees_joueurs[2*i]=frequenceCardiaque/compte;
  }
}
