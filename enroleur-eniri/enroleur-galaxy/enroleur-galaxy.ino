//----------------------------------------------------------
//
// Lecteur enroleur de badge pour lecteur galaxy honeywell
// Claude Boisgerault le 26/02/2017
// Version 0.99
// PROJET ENIRI licence MIT
// ce programme permet de lire un numéro de badge
// avec un lecteur en protocol wiegand.
// l'enroleur récupère les 34 bits envoyés et convertit 
// du bit 3 au bit 34 en décimal. 
// Le numéro du badge est envoyé en émulant le clavier du Pc
// par l'intermédiaire d'un Arduino Léonardo.
//
//----------------------------------------------------------



// Déclaration des constantes des broches

const int DATA_0 = 2; // Raccordement de data 0 sur Broche digital 2
const int DATA_1 = 3; //Raccordement de data 1 sur Broche digital 3


// --- Déclaration des variables globales ---

String numero_bin=""; // variable pour mémoriser les bits de lecture du badge
String ref=""; // variable tempon pour la lecture inversée des bits du badge
unsigned long temps_debut; // temps de référence
unsigned long temps_inter; // temps intermédiaire
unsigned long temps_timeout; // temps du timeout
unsigned long nu_badge_deci;// numéro du badge en décimal
int finlecture = 0; // flag pour sortir de la boucle do pour arrêter la lecture
boolean lecteur_hs = false; // lecteur connecté ou déconnecté
int valeur_d0 = 0; // variable pour lecture DATA_0
int valeur_d1 = 0; // variable pour lecture DATA_1
int resultat = 0; // variable pour calcul
// Variables des paramètrages de lecture 
int bit_start = 3; // Variable pour bit de départ de lecture du numéro du badge
int bit_end = 34; // Variable pour bit de fin de lecture du numéro du badge

//----------------------------------
// Chargement de la librairie
//----------------------------------

#include <Keyboard.h>

//----------------------------------
// initialisation du microcontroleur
//----------------------------------

void setup()   {
  pinMode (DATA_0, INPUT); //Broche digital 2 en mode entrée
  pinMode (DATA_1, INPUT); //Broche digital 3 en mode entrée
  Keyboard.begin();// on active l'émulateur clavier arduino
}

//-----------------------------
// début du programme principal
//-----------------------------

void loop() {
  
  valeur_d0 = digitalRead (DATA_0);
  valeur_d1 = digitalRead (DATA_1);
  resultat = valeur_d0 + valeur_d1;
   
  // On vérifie s'il y a au moins un data à 0
  if ( resultat < 2 )
  {
    // on vérifie s'il y a au moins un data à 1
    if (resultat > 0 )
    {
      finlecture = 1; /* le test de data 0 et 1 est ok on initialise la variable
                      finlecture pour lancer la boucle "do" "début lecture badge" */
     
      //---------------------------
      // boucle début lecture badge
      //---------------------------
      do
      {
        valeur_d0 = digitalRead (DATA_0); // on lit l'état de data 0 sur le lecteur
        valeur_d1 = digitalRead (DATA_1); // on lit l'état de data 1 sur le lecteur
        resultat = valeur_d0 + (valeur_d1 * 2);
        
        /*  resultat=3 > pas de valeur de bit envoyée
            resultat=2 > valeur du bit envoyée est de 0
            resultat=1 > valeur du bit envoyée est de 1
        */

        //---------------------------------------
        // on lit une valeur de bit de donnée à 0
        //---------------------------------------
        if ( resultat == 2 )
        {        
          // pour une lecture a l'envers des bits
          //numero_bin = "0" + ref;
          //ref = numero_bin;
          
          // lecture dans le bon sens
          numero_bin += "0";
          
          temps_debut = micros(); // top chrono temps référent
          // boucle de test timeout de data 0
          do
          {
            valeur_d0 = digitalRead (DATA_0); // on lit l'état de data 0 sur le lecteur
            temps_inter = micros(); // on prend le temps
            temps_timeout = temps_inter - temps_debut; // on calcul le temps passé
            if (temps_timeout > 100) // timeout dépassé on arrête la lecture du badge
            {
              finlecture = 0; // on oblige la sortie de la boucle "do" début lecture badge
            }
          } while (valeur_d0 == 0);
        }

        //----------------------------------------
        // on lit une valeur de  bit de donnée à 1
        //----------------------------------------
        if ( resultat == 1 )
        {
          // pour une lecture a l'envers des bits
          //numero_bin = "1" + ref;
          //ref = numero_bin;

          //lecture dans le bon sens
          numero_bin += "1";
          
          temps_debut = micros();
          // boucle de test timeout de data 1
          do
          {
            valeur_d1 = digitalRead (DATA_1);// on lit l'état de data 1 sur le lecteur
            temps_inter = micros(); // on prend le temps
            temps_timeout = temps_inter - temps_debut; // on calcul le temps passé
            if (temps_timeout > 100) // timeout dépassé on arrête la lecture du badge
            {
              finlecture = 0; // on oblige la sortie de la boucle "do" début lecture badge
            }

          } while (valeur_d1 == 0);
        }
        //-------------------------------------------------------
        // il n'y a plus de donnée  data 0 ou 1 envoyé
        //-------------------------------------------------------

        if ( resultat == 3)
        {
          temps_debut = micros();
          // boucle de test timeout entre 2 données de bit
          do
          {
            valeur_d0 = digitalRead (DATA_0); // on lit l'état de data 0 sur le lecteur
            valeur_d1 = digitalRead (DATA_1); // on lit l'état de data 1 sur le lecteur
            resultat = valeur_d0 + (valeur_d1 * 2);
            temps_inter = micros(); // on prend le temps
            temps_timeout = temps_inter - temps_debut; // on calcul le temps passé
            if (temps_timeout > 3000) // timeout dépassé on arrête la lecture du badge
            {
              finlecture = 0; // on oblige la sortie de la boucle "do" début lecture badge
              resultat = 0; // on oblige la sortie de la boucle "do" timeout
            }

          } while (resultat == 3);

        }

      } while (finlecture == 1); // on sort de la boucle "do" début lecture badge
      
      
      // ---------------------------------------------------------
      //   Fin de la lecture du badge
      // ---------------------------------------------------------
          
      nu_badge_deci = bin_deci(numero_bin);// on convertit le binaire en decimal
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.print(nu_badge_deci); // on tape le numéro du badge
      Keyboard.releaseAll();
      Keyboard.print("\n");// retour ligne
            
      // on rénitialise la variable numero_bin et ref pour une prochaine lecture
      numero_bin = "";
      ref = ""; 
          
    }
  }
} // fin de la boucle Void()



//-----------------------------------------------------------------------------------------------
//                                       Début de la zone "fonction"
//-----------------------------------------------------------------------------------------------


unsigned long bin_deci(String binaire){
 
  /* Convertit l'argument String 'binaire' en decimal 
     et retourne la valeur dans une variable du type
     unsigned long.     
  */
  unsigned long resultat = 0;
  int str_longeur = binaire.length();
  int pos_bin = 0;
  int fin = str_longeur - bit_start;
  int debut = str_longeur - bit_end;
  if(str_longeur >=34){  
    for (int compt = fin ; compt >= debut ; compt--)
    {
      if (binaire.charAt(compt) == '1') 
      {
        bitWrite(resultat, pos_bin, 1);
      }
      
      else
      {
        bitWrite(resultat, pos_bin, 0);      
      }
      
      pos_bin = ++pos_bin;      
    }
  }
  return resultat;
}



