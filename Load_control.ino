
/* Load_control
 --------------
 8.09.2012 - YM
 Contrôleur de charge. 
 Le relai coupe la puissance, lorsque le seuil inférieur est atteint. 
 Note: carte Stalker = "Arduino Pro 3,3V 8 MHz"
 */

#define VERSION "1.20"
#include <SerialLCD.h>
#include <SoftwareSerial.h>

// initialise la librairie LCD série
// connexion série sur ports 11 et 12
SerialLCD slcd(11,12);

// détermnine les pins E/S
const int Buzzer = 3;
const int RelayPow = 2;
const int Led = 7;
const int Btn = 4;
const int BtnStart = 5;

// analogique
const int I_read = 0; // conversion I->A/D
const int Pot = 1;  // potentiomètre de réglage

// variables globales
byte start = 0;
#define TEMPO_END_LOAD 6
byte endLoad = 0;
int i_min = 130;
int i_AC = 0;  // courant AC en mA
int nTimeCharge = 0;    // t charge en secondes
int mTimeCharge = 0;    // t charge en minutes
long mAS = 0L;  //mA * sec
long mAM = 0L;  //mA * min

// Etat des entrées, mémoriser les transitions
boolean BLoad = LOW;
boolean B_affiche = 0;

void setup() 
{
  // set up 
  pinMode(Buzzer, OUTPUT);
  pinMode(RelayPow, OUTPUT);  
  pinMode(Led, OUTPUT);
  pinMode(Btn, INPUT);
  pinMode(BtnStart, INPUT);

  Serial.begin(9600);
  
  slcd.begin();
  // Message de démarrage sur le LCD.
  slcd.print("Load Ctrl " VERSION);
}


// Monitoring sur PC seriel
void serial_write()
{
  if (BLoad)
  {
	Serial.print("\nI:");
	Serial.print(i_AC, DEC);
	Serial.print("\tn:");
	Serial.print(nTimeCharge,DEC);
    if (endLoad == 0)
	{
		Serial.print("\nEnergie:\t");
		Serial.print(energy_computed(mAS), DEC);
	} 
  }
}

/* pulse_1Sec()
   ------------
   Appelé chaque seconde
   Enclenche/maintient le relais tant que le
   courant est supérieur à la limite.
*/
void pulse_1Sec()
{  
  static byte run_S = 0;  // seconde courante
  run_S++;  // compte en continu
  if (run_S == 4)  // toute les 4 secondes
  {
    run_S = 0;
    B_affiche = 1 - B_affiche; // permute
  }
  // lire le courant consommé par la charge  
  i_AC = analogRead(I_read);
  // Q:état du bouton actif ou courant suffisant?
  if (start || i_AC >= i_min)
  {  
      BLoad = HIGH; // R: oui, indiquer
      endLoad = TEMPO_END_LOAD;
  }
  else
  {  // R: non, tempo de fin
	if (endLoad == 0)  //Q: terminé?
	  BLoad = LOW;    // R: oui, indiquer
	else
	  endLoad--;      // R: non, diminuer tempo
  }
  
  if (BLoad)    // Q: en charge?
  {             // R: oui
    nTimeCharge++;  // compte le temps de charge
    mAS += i_AC;    // accumule le courant
    if (nTimeCharge == 60)// Q: minute ecoulée?
    {                  // R: oui
      nTimeCharge = 0; // RAZ compteur
      mTimeCharge++;   // inc minute de charge
      mAM = mAM + (mAS / 60);
      mAS = 0;        // RAZ energie par seconde
    }
  }
  
  digitalWrite(RelayPow, BLoad);  // piloter le relais
  digitalWrite(Led, BLoad);       // et la LED
  
  lcd_write();  // imprimer l'état
  serial_write();  
  
  if (start)  // Q:tempo active?
    start--;  // A: oui, décompter
	
  if (endLoad < TEMPO_END_LOAD) // Q:tempo fin?
  {                             // R:oui activer buzzer
    if (endLoad & 0x1)
      digitalWrite(Buzzer, HIGH);
    else
      digitalWrite(Buzzer, LOW);
  }
  else                          // R:non
  {
    digitalWrite(Buzzer, LOW);  // buzzer inactif
  }

}// fin de pulse_1Sec()

//Boucle principale
const unsigned long cycleTime = 1000; // t boucle: 1 sec.
unsigned long previousMillis = 0UL;

void loop() 
{
  unsigned long currentMillis = millis();  // t écoulé
  if (currentMillis - previousMillis > cycleTime)
  {
    previousMillis = currentMillis;
    pulse_1Sec();
  }

  if (digitalRead(BtnStart) && start == 0)// voir si le bouton est pressé?
    start = 5;   // tempo
}// fin de la boucle principale

