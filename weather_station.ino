#include <DHT.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


char auth[] = "kBibQ6PfRKxu1BMt0BROKGp-KsOx0NOR";
#define DHTPIN 2// Digital pin 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
//dichiaro il pin di entrata del sensore di luminosità in una variabile
const char* ssid = "S&S-WiFi"; //Enter SSID
const char* password = "$ilvia$ergio2019"; //Enter Password
int status = WL_IDLE_STATUS;
char server[] = "sclergio.altervista.org";


long interval = 1000; // READING INTERVAL

EthernetClient client;
String data;

void setup(void)
{

  Serial.begin(115200);
  dht.begin(); //inizializzo il sensore di umidità
  connectWifi();
  Blynk.begin(auth, ssid, password);
}

void loop()
{
  Blynk.run();
  readValue();
  sendValue();
  delay(interval); // WAIT TIME BEFORE SENDING AGAIN
}
void readValue()
{
  float temperatura = dht.readTemperature();
  float umidita = dht.readHumidity();
  float pressione = 0; //bmp.readPressure();
  float rugiada = calculate_rugiada(temperatura, umidita);
  float humidex = calculate_humidex(temperatura, umidita);
  data = "temperatura=" + String(temperatura);
  data += "+umidita=" + String(umidita);
  data += "+pressione=" + String(pressione);
  data += "+rugiada=" + String(rugiada);
  data += "+humidex=" + String(humidex);
  Blynk.virtualWrite(V5,temperatura);
  Blynk.virtualWrite(V6,umidita);
  Blynk.virtualWrite(V7,humidex);
}
void sendValue()
{
  HTTPClient http;    //Declare object of class HTTPClient
 
  String postData;

  //Post Data
  postData = "?value=" + data;

  http.begin("http://sclergio.altervista.org/write_data.php"+postData);              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  Serial.println(postData); 
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
 
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
 
  http.end();  //Close connection
}
double calculate_rugiada(double temperatura, double umidita)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * temperatura) / (b + temperatura) + log(umidita / 100);
  double Td = (b * temp) / (a - temp);
  return Td;
}

float calculate_humidex(float temperature,float humidity) {
  float e;
  e = (6.112 * pow(10,(7.5 * temperature/(237.7 + temperature))) * humidity/100); //vapor pressure
  float humidex = temperature + 0.55555555 * (e - 10.0); //humidex
  return humidex;
} 

void connectWifi() {
  // Attempt to connect to wifi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    // Wait 10 seconds for connection
    delay(10000);
  }
}
