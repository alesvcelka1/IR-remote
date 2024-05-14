#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 12
#define enter 4127850240 // Kod klavesy enter
String stringNum;
int bar[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Definice pinů ledbar
unsigned long codes[] =                       // Definice kodu kláves
    {
        3910598400, // 0
        4077715200, // 1
        3877175040, // 2
        2707357440, // 3
        4144561920, // 4
        3810328320, // 5
        2774204160, // 6
        3175284480, // 7
        2907897600, // 8
        3041591040, // 9
};
int codesSize = sizeof(codes) / sizeof(codes[0]); // Zjištění počtů prvků v poli codes
int barSize = sizeof(bar) / sizeof(bar[0]);       // Zjištění počtů prvků v poli bar

void barOff()
{ // Vypnutí všech led
  for (int i = 0; i < barSize; i++)
  {
    digitalWrite(bar[i], LOW);
  }
}

void setBar(int intNum) // Vysvícení navoleného čísla
{
  barOff();
  for (int i = 0; i < barSize; i++)
  {
    digitalWrite(bar[i], bitRead(intNum, 9 - i));
  }
}

String recievedNum(unsigned long irData) // Vrátí číslo klávesy dle kodu
{
  for (int i = 0; i < codesSize; i++) // projde pole codes, když se shoduje s kodem z přijimače vrátí pořadové číslo z codes
  {
    if (irData == codes[i])
    {
      return String(i);
    }
  }
}

void setup()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Inicializace přijímače
  Serial.begin(9600);                                    // Inicializace seriové komunikace
  for (int i = 0; i < barSize; i++)
  {
    pinMode(bar[i], OUTPUT);
  }
}

void loop()
{
  if (IrReceiver.decode()) // Kontrola zda přišla data z přijímače
  {
    unsigned long irData = IrReceiver.decodedIRData.decodedRawData; // Uložení kodu z přijímače do irData
    if (irData > 0)                                                 // Kontrola falešných kodů
    {
      Serial.print(irData); // Výpis kodu na seriovou linku
      Serial.print(",");
      if (irData == enter)              // Pokud je zmačknut enter vypiš na display
      {                                 // funkce pro vypsání na display
        int intNum = stringNum.toInt(); // Přetypování String na int
        if (intNum < 1024)              // Kontrola rozsahu vstupního čísla
        {
          setBar(intNum);
          stringNum = ""; // Vyčištění nastřádaného čísla
        }
        else
        {
          Serial.println("Mimo rozsah");
          stringNum = ""; // Vyčištění nastřádaného čísla
        }
      }
      else // funkce pro střádání čísel
      {
        stringNum = stringNum + recievedNum(irData);
        Serial.println(stringNum);
      }
    }

    IrReceiver.resume(); // Enable receiving of the next value
  }
}