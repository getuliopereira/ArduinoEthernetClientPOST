/**
 Configura acesso a servidor PHP para envio de dados coletados de sensor de temperatura e umidade
 Intervalo de leitura: 5 minutos
 Forma de envio: HTML POST
 Formato da URL: http://localhost/arduinoServer/add.php
 Parametros do post: t=<temperatura em graus celsius>&u=<umidade relativa>
*/

#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>

// MAC address of the ethernet shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xA0, 0xF6 };
IPAddress ip(192, 168, 1, 112);
IPAddress dnsServer(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

byte serverAddress[] = {192, 168, 1, 250};
String serverURL = "192.168.1.250";
String sitename = "/arduinoServer";
int serverPort = 80;

EthernetClient client;

#define DHTPIN 2
#define DHTTYPE DHT11    // SENSOR TYPE - THE ADAFRUIT LIBRARY OFFERS SUPPORT FOR MORE MODELS
DHT dht(DHTPIN, DHTTYPE);

long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 1 * 5 * 1000; // READING INTERVAL (250000)

int t = 0;	// TEMPERATURE VAR
int h = 0;	// HUMIDITY VAR
String data;

void setup() {
  Serial.begin(115200);
  Serial.println("Trying an ip from dhcp. Please, wait a moment.");
  //if (Ethernet.begin(mac) == 0) {
  //Serial.println("Failed to configure Ethernet using DHCP");
  // no point in carrying on, so do nothing forevermore:
  Ethernet.begin(mac, ip, dnsServer, gateway, subnet);
  //} else {
  //  Serial.println("Ethernet address defined successfully!");
  //}

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    if ( thisByte < 3 ) {
      Serial.print(".");
    }
  }
  Serial.println();

  Serial.println("Initializing sensor....");
  dht.begin();
  delay(5 * 1000); // GIVE THE SENSOR SOME TIME TO START

  h = (int) dht.readHumidity();
  t = (int) dht.readTemperature();
  data = "";
  Serial.println("Enjoy!");
}

String msg;
int response;

void loop() {

  currentMillis = millis();
  if (currentMillis - previousMillis > interval) { // READ ONLY ONCE PER INTERVAL
    previousMillis = currentMillis;
    h = (int) dht.readHumidity();
    t = (int) dht.readTemperature();

    data = "t=" + String(t) + "&u=" + String(h);

    switch ( response = client.connect(serverAddress, serverPort) ) {
      case 1 : //SUCCESS:
        Serial.println("Connection Status: SUCCESS");

        client.println("POST " + sitename + "/add.php HTTP/1.1");
        client.println("Host: " + serverURL);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Content-Length: " + String(data.length()));
        client.println();
        client.print(data);
        break;
      case -1 : //TIMED_OUT:
        Serial.println("Connection Status: TIMED_OUT");
        break;
      case -2 : //INVALID_SERVER:
        Serial.println("Connection Status: INVALID_SERVER");
        break;
      case -3 : //TRUNCATED:
        Serial.println("Connection Status: TRUNCATED");
        break;
      case -4 : //INVALID_RESPONSE:
        Serial.println("Connection Status: INVALID_RESPONSE");
        break;
      default:
        Serial.print("Connection Status: UNDEFINED = ");
        Serial.println( response );
        Serial.println();
    }

    if (client.connected()) {
      client.stop(); // encerra a conexão com o servidor
    }
  }

  delay(1 * 10 * 1000); // WAIT FIVE MINUTES BEFORE SENDING AGAIN
}
