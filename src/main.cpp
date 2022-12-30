#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include "myClassAndFunctions.h"

const int led = 13;
const int sensorK10 = 0;
double mediaUmi;
double mediaTemp;
double mediaPress;
double media10k;
int bytesRecebidos;

Adafruit_BME280 bme;
Temporarios t;
FiltraNaN filtroNaN;

void setup() {
  
  pinMode(led, OUTPUT);
  pinMode(sensorK10, INPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  bme.begin(0x76);
  display.clearDisplay();
  delay(10);
}

double getTemp(int sensor);
void emissorDados(
  int bytesRecebidos,
  double mediaUmi, 
  double mediaPess, 
  double mediaTemp, 
  double media10k);

void hardWorker() {
  static unsigned long tempoCorrente0 = millis();
  static double soma10k, somaUmi, somaTemp, somaPress;
  static int cont = 0;
  static int minuto, hora, dias;
  const int divisor = 20;
  minuto = hora = dias = 0;
  minuto = millis() / 60000;
  hora = minuto / 60;
  dias = hora / 24;
  
  if (cont < divisor) {
    somaUmi += filtroNaN.umi_NaN(bme.readHumidity(), t.pt_U);
    somaTemp += filtroNaN.temp_NaN(bme.readTemperature(), t.pt_T);
    somaPress += filtroNaN.press_NaN((bme.readPressure() / 100.0F), t.pt_P);
    soma10k += filtroNaN.t10k_NaN(getTemp(sensorK10), t.pt_10);
    cont++;
    digitalWrite(led, 0);
  }
  else {
    
    mediaUmi = somaUmi / divisor;
    mediaPress = somaPress / divisor;
    mediaTemp = somaTemp / divisor;
    media10k = soma10k / divisor;
    cont = 0;
    somaUmi = somaTemp = somaPress = soma10k = 0;
    digitalWrite(led, 1);
  }

  if ((millis() - tempoCorrente0) < 999) {
    visor(mediaUmi, mediaPress, mediaTemp, media10k, dias, hora, minuto, cont);
  }
  
  if ((millis() - tempoCorrente0) > 1000) {

    tempoCorrente0 = millis();
  }
}

void loop() {
  hardWorker();
  emissorDados(
    bytesRecebidos,
    mediaUmi,
    mediaPress, 
    mediaTemp, 
    media10k);
}
