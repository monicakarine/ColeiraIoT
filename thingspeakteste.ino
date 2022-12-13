#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define GPS_RX 8
#define GPS_TX 9
#define RX 6
#define TX 7
TinyGPS gps;
String AP = "Redmi Note 5";       // AP NAME
String PASS = "3040501991"; // AP PASSWORD
String API = "GC27IQG9E11UHZ17";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
float valSensor = 0;
float valSensor2 = 0;

SoftwareSerial esp8266(RX,TX); 
AltSoftSerial gpsSerial(8, 9);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
 read_gps();
 String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(valSensor);
 sendCommand("AT+CIPMUX=1",7,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+6),8,">");
 esp8266.println(getData);
 String getData2 = "GET /update?api_key="+ API +"&"+ field2 +"="+String(valSensor2);
 sendCommand("AT+CIPMUX=1",7,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData2.length()+6),8,">");
 esp8266.println(getData2);
 delay(2500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void read_gps(){
  bool newData = false;
  unsigned long chars;
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      // Serial.write(c); //apague o comentario para mostrar os dados crus
      if (gps.encode(c)) // Atribui true para newData caso novos dados sejam recebidos
        newData = true;
    }
  }
  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    valSensor = flat*1000;
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    valSensor2 = flon*1000;
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    Serial.println();
    Serial.println();
  }
  else
  {
    Serial.print("Não entrou");
  }
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
