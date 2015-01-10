    //===========================================================================
    //  Serveur avec shield Ethernet-W5100 et carte Arduino UNO
    //  DANS LE CADRE DU PROJET TUTORE DE JORDAN DESBUQUOIT ET MATHIEU BRUN 
    //  LICENCE SYRDES 2014/2015
    //
    //===========================================================================
    //
    // Le shield utilise les pins 10, 11, 12 et 13
    // Arduino utilise les entrées analogiques A0-A5

    #include <SPI.h>
    #include <Ethernet.h>   //Inclure la bibliothèque Ethernet
    #define MaxHeaderLength 120
    #include <SD.h>
    char c;

    
    EthernetServer serverConf(80);    //Initialise le serveur Ethernet Web, port 80 par défaut pour HTTP
    EthernetServer serverOsc(5678);    //Initialise le serveur Ethernet Osc, port 5678
    
    // Adresse MAC du shield (ici choisie par défaut), l'adresse IP dépend de votre réseau local
    // Attention l'adresse mac doit tre unique sur le réseau !
    byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x2E, 0x5D }; 
    IPAddress ip(10,10,10,2);   
    byte subnet[] = {255, 255, 0, 0 };  //masque de sous réseau
    //L'adresse MAC du shield est associée à une adresse IP dans la livebox (DHCP)

    
   String HttpHeader = ""; //initialise la chaine HttpHeader
    
   File myFile; // objet file 
   char dirName[]="/monrep"; // tableau de caracteres pour le nom du répertoire
    // nom du fichier court (8 caractères maxi . 3 caractères maxi) (dit format DOS 8.3)
   char fileName[] = "test.txt"; // tableau de caractères pour le nom du fichier
   char pathFile[] = "/monrep/test.txt"; // tableau de caractères pour le nom du fichier
   int test; //variable mémorisation résultat renvoyé par fonctions



   
    void setup()
    {
      //Affichage sur PC via console USB
      Serial.begin(9600);
      
      // laisser la broche SS (select ethernet) en sortie - obligatoire avec librairie SD
      pinMode(10, OUTPUT);
      
      //Démarrage du Server Web
      Ethernet.begin(mac, ip);     //Démarer la connexion Ethernet
      
      if (true) {
        serverConf.begin();
        Serial.println("Pedalier en mode configuration via reseau");
        }
      else {
        serverOsc.begin();
        Serial.println("Pedalier en mode OSC");
        }
      
      //initialisation de la carte SD avec broche 4
      SD.begin(4);
      if (!SD.exists(pathFile)) {
        Serial.println("Pas de carte SD inseree ou mauvais nom de fichier");
      }
      else {
        Serial.println("Carte SD ok");
      }
      
      //Fin du Setup
      Serial.println("Arduino initialise");
}

    void loop()
    {
      
      EthernetClient client = serverConf.available(); // Ecouter les clients entrants
      if (client) {
        
        boolean currentLineIsBlank = true; // Une requète HTTP terminée par une ligne vide
        while (client.connected()) {
          if (client.available()) {
            if ( c != '&'){
             c = client.read();
            //SD.remove(pathFile); // efface le fichier  prééxistant
             //discard the rest until \n
            }
            else if (c == '&'){
              c = '\n';
            }
         if (HttpHeader.length() < MaxHeaderLength)
         {
           //store characters to string
           HttpHeader = HttpHeader + c;
         }
            
            //Si on recoit un caractère NewLine , la requête HTTP est finie, on peut répondre
            if (c == '\n' && currentLineIsBlank) {
              // Envoyer un header de résponse standard http
                
              //SD.remove(fileName); // efface le fichier
              //Serial.println ("Fichier efface !");
              Serial.println(HttpHeader);
              client.println("HTTP/1.1 200 OK");    //C'est OK
              client.println("Content-Type: text/html");
              client.println();
              client.print("<form>");
              client.print("load:");
              client.print("<br/>");
              client.print("<input type=\"text\" name=\"load\"/>");//les antislashs permettent d'échaper les guillemets car la fonction print necessite une certaine synthaxe et ceux-ci créraient une erreur de compilation
              client.print("<br/>");
              client.print("<input type=\"text\" name=\"load\"/>");//les antislashs permettent d'échaper les guillemets car la fonction print necessite une certaine synthaxe et ceux-ci créraient une erreur de compilation
              client.print("<br/>");
              client.print("<input type=\"submit\" value=\"Submit\"/>");
              client.print("</form>");

              
              
              
              myFile=SD.open(pathFile,FILE_WRITE);
              myFile.seek(0);
              myFile.println (HttpHeader); //imprime le texte du formulaire dans le fichier
              myFile.close();
              //SD.remove(fileName); // efface le fichier
              //Serial.println ("Fichier efface !");
           HttpHeader="";// nétoie la boucle pour la prochaine chaine de caractère sinon celà imprime toujours le premier texte afficher dans le formulaire
              
             
              break;
            }
            if (c == '\n') {
              // On commence une nouvelle ligne
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              // On an un charactère sur la ligne courante
              currentLineIsBlank = false;
            }
          }
        }
        // Laisser au navigateur web le temps de recevoir les données
        delay(1);
        // Refermer la connexion:
        client.stop();
      }
    }

