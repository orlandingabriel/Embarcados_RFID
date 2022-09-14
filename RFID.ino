//Programa : RFID - Controle de Acesso leitor RFID
// Programa : Teste modulo wireless ESP8266
// Configuracao ID e senha da rede Wireless

#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define DEBUG true
#define SSID        "Orlandin"
#define PASSWORD    "orlandin322000"
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// Cria uma serial nas portas 2 (RX) e 3 (TX)
SoftwareSerial esp8266(2, 3);




char st[20];
 
void setup() 
{ 
  
  //Configuração do ESP8266 para WIFI
  Serial.begin(9600);
  esp8266.begin(19200);
 
  sendData("AT+RSTrn", 2000, DEBUG); // rst
  // Conecta a rede wireless
  sendData("AT+CWJAP="SSID","PASSWORD"rn", 2000, DEBUG);
  delay(3000);
  sendData("AT+CWMODE=1rn", 1000, DEBUG);
  // Mostra o endereco IP
  sendData("AT+CIFSRrn", 1000, DEBUG);
  // Configura para multiplas conexoes
  sendData("AT+CIPMUX=1rn", 1000, DEBUG);
  // Inicia o web server na porta 80
  sendData("AT+CIPSERVER=1,80rn", 1000, DEBUG);
  //


  //Configuração do RFID e do Relé
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  pinMode(8, OUTPUT);  //Seta porta 2 como saída 
  //


  
}
 
void loop() 
{
    // Verifica se o ESP8266 esta enviando dados
    if (esp8266.available())
    {
      if (esp8266.find("+IPD,"))
      {
      delay(300);
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Digital 8: ";
      int a = digitalRead(8);
      webpage += a;
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "rn";
 
      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "rn";
 
      sendData(closeCommand, 3000, DEBUG);
    }
  }

  // while (1){
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo= "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();
    if (conteudo.substring(1) == "09 EF 7D 83") //UID 1 - Chaveiro
    {
      Serial.println("Ola Gabriel !");
      Serial.println("Acesso liberado!");
      digitalWrite(8, HIGH); // ativa rele, abre a trava solenoide
      delay(3000);           // espera 3 segundos
      digitalWrite(8, LOW);  // desativa rele, fecha a trava solenoide
      delay(3000);
    }
   
    if (conteudo.substring(1) == "75 3C E2 2A") //UID 2 - Cartao
    {
      Serial.println("Ola Gabriel !");
      Serial.println("Acesso liberado!");
      digitalWrite(8, HIGH); // ativa rele, abre a trava solenoide
      delay(3000);           // espera 3 segundos
      digitalWrite(8, LOW);  // desativa rele, fecha a trava solenoide
      delay(3000);
    }
  } 
// }

String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
