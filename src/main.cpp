#include <Arduino.h>
#define IR_USE_AVR_TIMER 1
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 12
#define enter 4127850240 // Kod klávesy enterr 

int green = 10;
int blue = 6;
int red = 9;
String stringNum;
int bar[] = {green, blue, red}; // Piny pro RGB LED

// Deklarace funkce
String recievedNum(unsigned long irData);

// Definice kódů pro jednotlivé barvy
unsigned long greenCode = 4161273600; // Kód pro zelenou
unsigned long blueCode = 3141861120; // Kód pro modrou
unsigned long redCode = 3125149440; // Kód pro červenou

int intensity = 255; // Výchozí intenzita světla

void setup()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);
  for (int i = 0; i < 3; i++)
  {
    pinMode(bar[i], OUTPUT); // Nastavení pinů pro RGB LED jako výstupy
  }
}

void lightUp(int colorPin, int intensity)
{
  analogWrite(colorPin, intensity); // Nastavení intenzity světla
}

void loop()
{
  if (IrReceiver.decode())
  {
    unsigned long irData = IrReceiver.decodedIRData.decodedRawData;
    if (irData > 0)
    {
      Serial.print(irData);
      Serial.print(",");
      delay(100); // Počkej 100 ms, aby se zabránilo rychlému opakování tlačítka
      
      if (irData == enter) // Pokud bylo stisknuto tlačítko pro potvrzení intenzity
      {
        int selectedColor = -1;
        // Čekáme na stisknutí tlačítka pro výběr barvy
        while (true)
        {
          if (IrReceiver.decode())
          {
            unsigned long irDataColor = IrReceiver.decodedIRData.decodedRawData;
            if (irDataColor > 0)
            {
              Serial.print(irDataColor);
              Serial.print(",");
              if (irDataColor == greenCode || irDataColor == blueCode || irDataColor == redCode)
              {
                if (irDataColor == greenCode) selectedColor = green;
                else if (irDataColor == blueCode) selectedColor = blue;
                else if (irDataColor == redCode) selectedColor = red;
                break;
              }
            }
            IrReceiver.resume();
          }
        }
        
        if (selectedColor != -1)
        {
          lightUp(selectedColor, intensity); // Rozsvícení zvolené barvy s danou intenzitou
        }
      }
      else // Střádání čísel pro intenzitu
      {
        stringNum = stringNum + recievedNum(irData);
        Serial.println(stringNum);
        intensity = stringNum.toInt(); // Nastavení intenzity na zadanou hodnotu
        if (intensity > 255) intensity = 255; // Ochrana proti přetečení
        Serial.print("Intensity set to: ");
        Serial.println(intensity);
      }
    }
    IrReceiver.resume();
  }
}

String recievedNum(unsigned long irData)
{
  unsigned long codes[] = { // Definice kodu pro intenzitu
    0x0,        // 0
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
  
  int codesSize = sizeof(codes) / sizeof(codes[0]); // Zjištění počtu prvků v poli codes

  for (int i = 0; i < codesSize; i++) // projde pole codes, když se shoduje s kodem z přijimače vrátí pořadové číslo z codes
  {
    if (irData == codes[i])
    {
      return String(i); // Vracíme pořadové číslo kódu
    }
  }
  return ""; // Pokud kód není nalezen, vrátíme prázdný řetězec
}



