#include <WiFi.h>             //Importamos la libreria de wifi
#include <NTPClient.h>        //importamos la librería del cliente NTP
#include <WiFiUdp.h>          //importamos librería UDP para comunicar con la libreria NTP

                              //Liberia para la pantalla OLED SH1106
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c        //Se inicializa con el I2C, con contraseña 0x3C de la OLED
#define SCREEN_WIDTH 128        //Ancho en pixeles de la OLED
#define SCREEN_HEIGHT 64        //Altura en pixeles de la OLED
#define OLED_RESET -1

// Declaración para la pantalla SH1106 conectada a I2C (pines SDA, SCL)
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Ajuste de wifi
#define WIFI_SSID   "TS-WISP/GUERRERO_RESENDIZ"    //Nombre de la red
#define WIFI_PASSWORD   "Gue.re20"                 //Contraseña
#define JST 3600*9

//declaración de variables
unsigned long epochTime;
String formattedTime = "";
int currentHour = 0;
int currentMinute = 0;
int currentSecond = 0;
String weekDay = "";
struct tm *ptm ;
int monthDay = 0;
int currentMonth = 0;
String currentMonthName = "";
int currentYear = 0;
String currentDate = "";
String message = "";

//Definir cliente NTP para obtener tiempo
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Dias de la semana
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Nombre de meses
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup() {
  //código de configuración para ejecutar
  Serial.begin(9600);
  delay(250);
  Serial.print("\n\nRecargando:\n");

  // Dirección 0x3C por defecto
  display.begin(i2c_Address, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  //Iniciando Wifi
  drawLog("Conectando Wifi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.printf("Conectando, direccion IP: ");  //Busca que exista coneccion con la direccion IP
  Serial.println(WiFi.localIP());
  drawLog("WiFi conectado!");                   //Se encuentra conectado con el Wifi
  display.clearDisplay();
  message = "Conectado!";
  //init and get the time
  timeClient.begin();
  timeClient.setTimeOffset(21600);
  delay(1000);
  Mensajes();
}

void Mensajes() {
  loop();
  display.setCursor(0, 0);
  display.println("Dispositivo: Friends");
  display.println("Dia: " + weekDay);
  display.print("Fecha: ");
  display.println(currentDate);
  display.print("Hora: ");
  display.println(formattedTime);
  display.print("Estado: ");
  display.println(message);
  display.display();
  delay(2000);
}

void loop() {
  //LLamar los formatos de tiempo
  timeClient.update();                          //Actualiza
  epochTime = timeClient.getEpochTime();
  formattedTime = timeClient.getFormattedTime();//Formato de tiempo
  Serial.print("Formato de tiempo: ");
  Serial.println(formattedTime);
  currentHour = timeClient.getHours();          //Horas
  currentMinute = timeClient.getMinutes();      //Minutos
  currentSecond = timeClient.getSeconds();      //Segundos
  weekDay = weekDays[timeClient.getDay()];      //Dia

  //Get a time structure
  tm *ptm = gmtime ((time_t *)&epochTime);
  monthDay = ptm->tm_mday;
  currentMonth = ptm->tm_mon + 1;
  currentMonthName = months[currentMonth - 1];
  currentYear = ptm->tm_year + 1900;

  //Print complete date:
  currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.println(currentDate);
  Serial.println("");
  delay(2000);
}

void drawLog(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  drawText(msg);
  display.display();
  delay(1);
}

void drawText(const char* text) {
  for (uint8_t i = 0; i < strlen(text); i++) {
    display.write(text[i]);
  }
}
