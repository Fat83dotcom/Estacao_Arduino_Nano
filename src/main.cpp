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
const int tempoIn = 90;
const int tempoOut = 900;

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

void run() {

  static double mediaUmi;
  static double mediaTemp;
  static double mediaPress;
  static double media10k;
  static unsigned long tempoCorrente0 = millis();
  static unsigned long tempoCorrente1 = millis();
  static double soma10k, somaUmi, somaTemp, somaPress;
  
  static int cont = 0;
  static int minuto, hora, dias;
  minuto = hora = dias = 0;
  const int divisor = 100;
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
  
  if ((millis() - tempoCorrente1) < tempoIn) {

    Serial.print("u ");
    Serial.println(mediaUmi, DEC);
    Serial.print("p ");
    Serial.println(mediaPress, DEC);
    Serial.print("1 ");
    Serial.println(mediaTemp, DEC);
    Serial.print("2 ");
    Serial.println(media10k, DEC);
  }
  if ((millis() - tempoCorrente1) > tempoOut) {

    tempoCorrente1 = millis();
  }
}

void loop() {
  run();
}