//----------------------------------------------------------
//
// Lecteur de badge
// Claude Boisgerault le 22-06-13
// Version 0.9
// ce programme permet de raccorder un lecteur avec le
// protocole wiegand (data 0/data 1).le logiciel lit et 
// envoie le contenu des bits du badge par le port serie usb
// en 115200 baud vous pouvez le lire avec un terminal.
//----------------------------------------------------------

// Déclaration des constantes des broches
const int DATA_0 = 2; // Raccordement de data 0 sur Broche digital 2
const int DATA_1 = 3; //Raccordement de data 1 sur Broche digital 3

// --- Déclaration des variables globales ---
unsigned long temps_debut; // temps de référence
unsigned long temps_inter; // temps intermédiaire
unsigned long temps_timeout; // temps du timout
int valeur_d0 = 0; // variable pour lecture DATA_0
int valeur_d1 = 0; // variable pour lecture DATA_1
int resultat = 0; // variable pour calcul
int finlecture = 0; // flag pour pour la boucle do

//----------------------------------
// initialisation du microcontroleur
//----------------------------------
void setup()   {
	pinMode (DATA_0, INPUT); //Broche digital 2 en mode entrée
	pinMode (DATA_1, INPUT); //Broche digital 3 en mode entrée
	Serial.begin(115200); // vitesse du port série usb à 115200 baud
}

//-----------------------------
// début du programme principal
//-----------------------------
void loop(){
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
			//Serial.print ( "-lecture du badge-\n" );
			//---------------------------
			// boucle début lecture badge
			//---------------------------
			do 
			{
				valeur_d0 = digitalRead (DATA_0); // on lit l'état de data 0 sur le lecteur
				valeur_d1 = digitalRead (DATA_1); // on lit l'état de data 1 sur le lecteur
				resultat = valeur_d0 + (valeur_d1 * 2); /* resultat=3 > pas de valeur de bit envoyé
									resultat=2 > valeur de bit 0 envoyé 
									resultat=1 > valeur de bit 1 envoyé
									*/
				
        			//---------------------------------------
				// on lit une valeur de bit de donnée à 0
				//---------------------------------------
				if ( resultat == 2 )
				{  
					Serial.print("0"); // on envoie la nouvelle valeur de bit 0
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
							Serial.print("!00!"); // on envoie le code erreur !00! timeout  sur data 0
              						Serial.println( String (temps_timeout));
						}
					}while (valeur_d0 == 0);
				} 
				
				//----------------------------------------
				// on lit une valeur de  bit de donnée à 1
				//----------------------------------------
				if ( resultat == 1 )
				{
					Serial.print("1"); // on envoie la nouvelle valeur de bit 1
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
							Serial.print("!01!"); // on envoie le code erreur !01! "timeout sur data 1"
						}
						
					}while (valeur_d1 == 0);
				}
				//-------------------------------------------------------
				// il n'y a plus de donnée  data 0 ou 1 envoyé
				//-------------------------------------------------------

				if ( resultat == 3)
				{
					temps_debut = micros();
				   // boucle de test timeout entre 2 donnée de bit
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
							Serial.print("!03!"); // on envoie le code erreur !03! "timeout entre 2 donnée de bit"
              						resultat = 0; // on oblige la sortie de la boucle "do" timeout
						}

					}while (resultat == 3);
					
				}
			
			}while (finlecture == 1); // on sort de la boucle "do" début lecture badge
			Serial.print("#\n"); // on envoie le code de fin lecture badge avec retour ligne
		
		}
    
	}
} // fin de la boucle Void()


