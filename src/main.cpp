#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include "myClassAndFunctions.h"

Adafruit_BME280 bme;
DadosTemporarios t;
FiltraNaN filtroNaN;
DadosSensores media;
OperadorMedia operacaoMedia;
DadosSensores *pMedia = &media;
OperadorMedia *pOperacaoMedia = &operacaoMedia;

void visor(DadosSensores *dadosMedia, Temporizador *dadosTempo, OperadorMedia *contador);

double getTemp(int sensor);

void servidorDados(int bytesRecebidos, DadosSensores *dadosMedia);

const int led = 13;
const int sensorK10 = 0;
int bytesRecebidos;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(sensorK10, INPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  bme.begin(0x76);
  display.clearDisplay();
}

void hardWorker() {
  static unsigned long tempoCorrente = millis();
  static DadosSensores soma;
  static Temporizador tempo;
  Temporizador *pTempo = &tempo;
  tempo.minuto = millis() / tempo._60MIL_MILISEGUNDOS;
  tempo.hora = tempo.minuto / tempo._60_MINUTOS;
  tempo.dia = tempo.hora / tempo._24_HORAS;
  
  if ((millis() - tempoCorrente) < 1000) {
    visor(pMedia, pTempo, pOperacaoMedia);
  }
  if ((millis() - tempoCorrente) > 1000) {
    tempoCorrente = millis();
  }

  if (operacaoMedia.contador < operacaoMedia.divisor) {
    soma.umidade += filtroNaN.umi_NaN(bme.readHumidity(), t.pt_U);
    soma.pressao += filtroNaN.press_NaN((bme.readPressure() / 100.0F), t.pt_P);
    soma.tempInterna += filtroNaN.temp_NaN(bme.readTemperature(), t.pt_T);
    soma.tempExterna += filtroNaN.t10k_NaN(getTemp(sensorK10), t.pt_10);
    operacaoMedia.contador++;
    digitalWrite(led, 0);
  }
  else {
    media.umidade = operacaoMedia.calcularMedia(soma.umidade);
    media.pressao = operacaoMedia.calcularMedia(soma.pressao);
    media.tempInterna = operacaoMedia.calcularMedia(soma.tempInterna);
    media.tempExterna = operacaoMedia.calcularMedia(soma.tempExterna);
    operacaoMedia.contador = 0;
    soma.umidade = soma.pressao = soma.tempInterna = soma.tempExterna = 0;
    digitalWrite(led, 1);
  }
}

void loop() {
  hardWorker();
  servidorDados(bytesRecebidos, pMedia);
}
