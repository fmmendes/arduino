#ifndef STASSID
#define STASSID "Tangamandapio"        // set your SSID
#define STAPSK "Ev1t@nd0 a F4d!ga..."  // set your wifi password
#endif

/* Configuration of NTP */
#define NTP_SERVER "gps.ntp.br"

#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <time.h>  // time() ctime()

RTC_DS3231 rtc;
SoftwareSerial ESP_Serial(10, 11);

String resposta = "";

void setup() {
  Serial.begin(115200);
  ESP_Serial.begin(9600);
  Wire.begin();

  Serial.println("Inicializando...");
  delay(1000);

  Serial.println("Conectando a rede...");
  String CWJAP = "\"AT+CWJAP=\"";
  CWJAP += STASSID;
  CWJAP += "\",\"";
  CWJAP += STAPSK;
  CWJAP += "\"";
  sendCommand(CWJAP);
  readResponse(10000);

  delay(2000);  //espera de seguranca

  if (resposta.indexOf("OK") == -1) {  //procura na resposta se houve OK
    Serial.println("Atencao: Nao foi possivel conectar a rede WiFi.");
    Serial.println("Verifique se o nome da rede e senha foram preenchidos corretamente no codigo e tente novamente.");
  } else {

    Serial.println("Obtendo endereco de IP na rede...");
    sendCommand("AT+CIFSR");
    readResponse(1000);

    Serial.println("Configurando para multiplas conexoes...");
    sendCommand("AT+CIPMUX=1");
    readResponse(1000);

    Serial.println("Ligando servidor...");
    sendCommand("AT+CIPSERVER=1,80");
    readResponse(1000);

    Serial.print("Pronto, acesse o IP atraves de um dispositivo ligado na mesma rede do ESP8266.");
  }
}

void loop() {
  delay(1000);
  if (ESP_Serial.available()) {
    //.find le os dados vindos da serial ate o alvo, neste caso +IPD,
    if (ESP_Serial.find("+IPD,")) {
      delay(500);
      page();
    }
  }
}

void page() {
  char id = ESP_Serial.peek();  //pega ID da conexao

  DateTime now = rtc.now();

  //nossa pagina web em HTML
  String webpage = String("HTTP/1.1 200 OK\r\n") + "Content-Type: text/html\r\n" + "Connection: close\r\n" + "\r\n" + "<!DOCTYPE HTML>" + "<html>"
                   + "<h1>" + "Hello from Arduino!" + "</h1>"
                   + "<h2>Time</h2>"
                   + "<h3>" + now.year() + '/' + now.month() + '/' + now.day() + ' ' + now.hour() + ':' + now.minute() + ':' + now.second() + " (" + now.unixtime() + ")</h3>"
                   + "</html>" + "\r\n";

  String cipSend = "AT+CIPSEND=";
  cipSend += (int(id) - 48);
  cipSend += ",";
  cipSend += webpage.length();
  Serial.println(webpage.length());
  sendCommand(cipSend);
  readResponse(500);

  sendCommand(webpage);
  readResponse(750);

  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += (int(id) - 48);
  sendCommand(closeCommand);
  readResponse(750);
  print_now();
}

void print_now() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
}

void sendCommand(String cmd) {
  ESP_Serial.println(cmd);
}

void readResponse(unsigned int timeout) {
  unsigned long timeIn = millis();  //momento que entramos nessa funcao é salvo
  resposta = "";
  //cada comando AT tem um tempo de resposta diferente...
  while (timeIn + timeout > millis()) {
    if (ESP_Serial.available()) {
      char c = ESP_Serial.read();
      resposta += c;
    }
  }
  Serial.println(resposta);
}