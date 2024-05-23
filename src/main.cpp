#include <Arduino.h>
#define IR_USE_AVR_TIMER 1
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 12 
#define enter 4127850240
//#define enter 2540177159  

int green = 6;
int blue = 9;
int red = 10;

String stringNum; // Řetězec pro uchování zadávané hodnoty intenzity

// Pole pinů pro RGB LED
int bar[] = {green, blue, red};

// Kódy pro jednotlivé barvy
unsigned long greenCode = 4161273600; // Kód pro zelenou
unsigned long blueCode = 3141861120; // Kód pro modrou
unsigned long redCode = 3125149440; // Kód pro červenou
/*unsigned long redCode = 2473330439; Kody na domaci ovladac
unsigned long blueCode = 3910534919;
unsigned long greenCode = 3943958279;*/ 

int intensity = 255; // Výchozí intenzita světla

//funkce pro převod IR kódu na číslo
String recievedNum(unsigned long irData);

void setup() {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Inicializace IR přijímače
  Serial.begin(9600); // Inicializace sériové komunikace
  for (int i = 0; i < 3; i++) {
    pinMode(bar[i], OUTPUT); // Nastavení pinů pro RGB LED jako výstupy
  }
}

void lightUp(int colorPin, int intensity) {
  analogWrite(colorPin, intensity); // Nastavení intenzity světla na daném pinu
}

void loop() {
  if (IrReceiver.decode()) { // Pokud je přijat IR signál
    unsigned long irData = IrReceiver.decodedIRData.decodedRawData;
    if (irData > 0) {
      Serial.print(irData); // Vypiš přijatý IR kód
      Serial.print(",");
      delay(100); // Počkej 100 ms, aby se zabránilo rychlému opakování tlačítka

      if (irData == enter) { // Pokud bylo stisknuto tlačítko pro potvrzení intenzity
        int selectedColor = -1;
        // Čekáme na stisknutí tlačítka pro výběr barvy
        while (true) {
          if (IrReceiver.decode()) {
            unsigned long irDataColor = IrReceiver.decodedIRData.decodedRawData;
            if (irDataColor > 0) {
              Serial.print(irDataColor); // Vypiš přijatý IR kód pro barvu
              Serial.print(",");
              // Zkontroluj, jestli přijatý kód odpovídá jedné z definovaných barev
              if (irDataColor == greenCode || irDataColor == blueCode || irDataColor == redCode) {
                if (irDataColor == greenCode) selectedColor = green;
                else if (irDataColor == blueCode) selectedColor = blue;
                else if (irDataColor == redCode) selectedColor = red;
                break;
              }
            }
            IrReceiver.resume(); // Připrav se na další IR signál
          }
        }

        if (selectedColor != -1) {
          lightUp(selectedColor, intensity); // Rozsvícení zvolené barvy s danou intenzitou
        }
        
        // Resetování hodnoty stringNum a intensity po potvrzení barvy
        stringNum = "";
        intensity = 255;
      } else { // Střádání čísel pro intenzitu
        stringNum = stringNum + recievedNum(irData); // Přidání přijatého čísla k řetězci
        Serial.println(stringNum); // Vypiš aktuální hodnotu stringNum
        intensity = stringNum.toInt(); // Nastavení intenzity na zadanou hodnotu
        if (intensity > 255) intensity = 255; // Ochrana proti přetečení
        Serial.print("Intensity set to: "); // Vypiš nastavenou intenzitu
        Serial.println(intensity);
      }
    }
    IrReceiver.resume(); // Připrav se na další IR signál
  }
}

String recievedNum(unsigned long irData) {
  // Pole kódů pro jednotlivé číslice
  unsigned long codes[] = {
    // 0 ?
    4077715200, // 1
    3877175040, // 2
    2707357440, // 3
    4144561920, // 4
    3810328320, // 5
    2774204160, // 6
    3175284480, // 7
    2907897600, // 8
    3041591040, // 9
    /*3994093319, // 0 kody na domací ovladac
    4211345159, // 1
    4194633479, // 2
    4177921799, // 3
    4144498439, // 4
    4127786759, // 5
    4111075079, // 6
    4077651719, // 7
    4060940039, // 8
    4044228359  // 9*/
  };

  int codesSize = sizeof(codes) / sizeof(codes[0]); // Zjištění počtu prvků v poli codes

  for (int i = 0; i < codesSize; i++) { // Projdi pole codes
    if (irData == codes[i]) { // Pokud se shoduje s kódem z přijímače
      return String(i); // Vracíme pořadové číslo kódu
    }
  }
  return ""; // Pokud kód není nalezen, vrátíme prázdný řetězec
}






