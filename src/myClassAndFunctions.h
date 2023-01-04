#ifndef BIBLIOTECA_PADRAO_ESTACAO
#define BIBLIOTECA_PADRAO_ESTACAO

#include <Arduino.h>
#include <math.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // Oled display width(largura), in pixels
#define SCREEN_HEIGHT 64 // Oled display height(altura), in pixels
#define Oled_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, Oled_RESET);

typedef struct{
  int contador = 0;
  const double divisor = 20;
}OperadorMedia;

typedef struct{
  const unsigned long _60MIL_MILISEGUNDOS = 60000;
  const unsigned long _60_MINUTOS = 60;
  const unsigned long _24_HORAS = 24;
  unsigned long dia;
  unsigned long hora;
  unsigned long minuto;
}Temporizador;

typedef struct{
  double umidade;
  double pressao;
  double tempInterna;
  double tempExterna;
}DadosSensores;

double getTemp(int sensor);

void servidorDados(int bytesRecebidos, DadosSensores *dadosMedia);

void visor(DadosSensores *dadosMedia, Temporizador *dadosTempo, OperadorMedia *contador);

typedef struct {
  private:
  double t_Umi;
  double t_Temp;
  double t_Press;
  double t_10k;

  public:
  double *pt_U = &t_Umi;
  double *pt_T = &t_Temp;
  double *pt_P = &t_Temp;
  double *pt_10 = &t_10k;

}DadosTemporarios;

typedef struct {
  private:
  double _Umidade;
  double _TempInterna;
  double _Press;
  double _TempExterna;
  int contador;
  int numeroDeVoltas = 1000;

  public:
  double umi_NaN (double umidade, double *pUmidade) {
    _Umidade = umidade;
    if (!isnan(_Umidade)) {
      *pUmidade = _Umidade;
    }
    contador = 0;
    while (isnan(_Umidade) && contador < numeroDeVoltas){
      _Umidade = umidade;
      contador ++;
    }
    return (contador == numeroDeVoltas) ? *pUmidade : _Umidade;
  }

  double temp_NaN (double tempInterna, double *pTempInterna) {
    _TempInterna = tempInterna;
    if (!isnan(_TempInterna)) {
      *pTempInterna = _TempInterna;
    }
    contador = 0;
    while (isnan(_TempInterna) && contador < numeroDeVoltas) {
      _TempInterna = tempInterna;
      contador++;
    }
    return (contador == numeroDeVoltas) ? *pTempInterna : _TempInterna; 
  }
  double press_NaN (double press, double *pPressao) {
    _Press = press;
    if (!isnan(_Press)) {
      *pPressao = _Press;
    }
    contador = 0;
    while (isnan(_Press) && contador < numeroDeVoltas) {
      _Press = press;
      contador++;
    }
    return (contador == numeroDeVoltas) ? *pPressao : _Press;
  }

  double t10k_NaN(double tempExterna, double *pTempExterna) {
    _TempExterna = tempExterna;
    if (!isnan(_TempExterna)) {
      *pTempExterna = _TempExterna;
    }
    contador = 0;
    while (isnan(_TempExterna) && contador < numeroDeVoltas) {
      _TempExterna = tempExterna;
      contador++;
    }
    return (contador == numeroDeVoltas) ? *pTempExterna : _TempExterna;
  }
}FiltraNaN;

double getTemp(int sensor) {
  
  int RawADC = analogRead(sensor);
  long Resistance;
  double Temp;
  Resistance=((10240000/RawADC) - 10000);
  Temp = log(Resistance);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius
  return Temp;  
}

void servidorDados(int bytesRecebidos, DadosSensores *dadosMedia) {
  if (Serial.available()){
    bytesRecebidos = Serial.read();
  
    if (bytesRecebidos == 'u'){
      Serial.print("u ");
      Serial.println(dadosMedia->umidade, 2);
    }
    if (bytesRecebidos == 'p'){
      Serial.print("p ");
      Serial.println(dadosMedia->pressao, 2);
    }
    if (bytesRecebidos == '1'){
      Serial.print("1 ");
      Serial.println(dadosMedia->tempInterna, 2);
    }
    if (bytesRecebidos == '2'){
      Serial.print("2 ");
      Serial.println(dadosMedia->tempExterna, 2);
    }
  }
}

void visor(DadosSensores *dadosMedia, Temporizador *dadosTempo, OperadorMedia *contador) {
  display.invertDisplay(0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(2, 5);
  display.println("ESTACAO METEREOLOGICA");

  display.setCursor(1, 17);
  display.print("U:");
  display.print(dadosMedia->umidade);
  display.println("%");
 
  display.setCursor(1, 27);
  display.print("P:");
  display.print(dadosMedia->pressao);
  display.println("hPa");
  
  display.setCursor(1, 37);
  display.print("T.Int:");
  display.print(dadosMedia->tempInterna);
  display.println("C");
  
  display.setCursor(1, 47);
  display.print("T.Ext:");
  display.print(dadosMedia->tempExterna);
  display.println("C");
  
  display.setCursor(1, 57);
  display.print("D:");
  display.print(dadosTempo->dia);
  display.print(",H:");
  display.print(dadosTempo->hora);
  display.print(",M:");
  display.println(dadosTempo->minuto);
  
  display.setCursor(78, 17);
  display.print("Contador");
  display.setCursor(84, 27);
  display.print("Media");
  display.setCursor(92, 37);
  display.print(contador->contador);

  display.display();
  display.clearDisplay();
}
#endif
