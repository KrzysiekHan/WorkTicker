
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
/*-------------------------------------------------------------------------------------------
                    D E F I N I C J E
  -------------------------------------------------------------------------------------------*/
//Wejscie danych paska WS2812
#define PIN        8
//Ilosc pixeli paska do obsluzenia
#define NUMPIXELS  10
//Inicjalizacja biblioteki
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define EEPROMSTATE 0 //możliwość wyłączenia zapisu do eeprom
#define DEBUG 0
// * RX is digital pin 10 (connect to TX of other device)
// * TX is digital pin 11 (connect to RX of other device)
SoftwareSerial Genotronex(10, 11);
/*-------------------------------------------------------------------------------------------
                S T R U K T U R Y   I   T Y P Y
  -------------------------------------------------------------------------------------------*/
//TYP DO PRZECHOWANIA KOLORKA
typedef struct
{
  int R;
  int G;
  int B;
} TLed;

//STRUKTURA DO ZAPISYWANIA DANYCH W EEPROMIE
struct eepromdata {
  int eeMoc = 0;
  int eeCzas = 0;
};
/*-------------------------------------------------------------------------------------------
                    Z M I E N N E
  -------------------------------------------------------------------------------------------*/
//PRZERWANIE
const byte interruptPin = 2;
volatile byte state = LOW;


//TIMER DO RTOS'A XD
unsigned long previousMillis = 0;    //przechowalnia ostatniego czasu
const long interval = 100;           //czestotliwosc tykacza
byte TICK100MS = LOW;

//INNE
int step = 0;
int licznik = 0;
int LiczbaLed = 0;
int Moc = 100;
float FMoc = 0;
int MocVis = 0;
int Czas = 127;

//EEPROM
int adresCzas = 10;
int adresMoc = 20;


//KOMUNIKACJA SZEREGOWA
String odebraneDane = ""; //Pusty ciąg odebranych danych
char polecenie = 'x';
String inString = "";
float flocik = 0;
String readString, str1, str2;


//MASZYNA STANU
// 0 - poczatek
// 1 - trwa odliczanie czasu - zapalanie kolejnych diod
// 2 - czas odliczony - wszystkie diody czerwone
int StatusProgramu = 0;


//DEFINICJA KOLORKÓW OD CZERWONEGO DO ZIELONEGO
TLed Diody[10] = {
  {43, 194, 0},
  {82, 198, 0},
  {123, 202, 0},
  {166, 206, 0},
  {210, 209, 0},
  {213, 171, 0},
  {217, 132, 0},
  {221, 91, 0},
  {225, 48, 0},
  {229, 4, 0},
};
/*-------------------------------------------------------------------------------------------
              F U N K C J E    O B S Ł U G I    P R Z E R W A N
  -------------------------------------------------------------------------------------------*/
void ISRbutton()
{
  LiczbaLed = 0;
  StatusProgramu = 0;
  licznik = 0;
  resetleds();
}
/*-------------------------------------------------------------------------------------------
              F U N K C J E
  -------------------------------------------------------------------------------------------*/
//WYŚLIJ PRZYGOTOWANE ZMIENNE DO PASKA I USTAW JASNOŚĆ
void Show()
{
  //pixels.setBrightness(255);
  pixels.show();
}

//FUNKCJA TYKAJĄCA CO 100MS UMOŻLIWIAJĄCA NIEBLOKUJĄCE DZIAŁANIE PROGRAMU
void TICK()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    TICK100MS = HIGH;
    licznik = licznik + 1;
  }
}

//PRZELICZ TICK
void ObliczInterwalLed ()
{
  //czas otrzymany z telefonu w ms podzielony jest przez 100ms a następnie przez ilość pixeli
  //to daje ilość ticków koniecznych do odliczenia aby zmienić stan kolejnej diody
  step = (((Czas * 1000) / 100) / (NUMPIXELS + 1));
}


//ZAPAL WSZYSTKIE DIODY NA CZERWONO
void allred ()
{
  for (int i = 0; i < 10; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  Show();
}

//ZGAS WSZYSTKIE DIODY
void resetleds()
{
  for (int i = 0; i < 10; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  Show();
}

//OD ZIELONEGO DO CZERWONEGO
void redtogreen(int IleDiod)
{
  for (int i = 0; i < IleDiod; i++)
  {
    pixels.setPixelColor(i, pixels.Color(Diody[i].R, Diody[i].G, Diody[i].B));
  }
  Show();
}

//KOMUNIKACJA SZEREGOWA

void Szeregowa2() {
  //odbierz komunikat
  while (Genotronex.available()) {
    delay(10);
    if (Genotronex.available() > 0) {
      char c = Genotronex.read(); //odczyt kolejnego bajtu
      readString += c; //dopisanie bajtu do stringa i inkrementacja pozycji
    }
  }

  if (readString.length() > 3) {
    if (DEBUG) {
      Genotronex.println(readString); //see what was received
    }
    str1 = readString.substring(0, 3); //get the first four characters
    str2 = readString.substring(3, 6); //get the next four characters

    if (DEBUG) {
      Genotronex.println(str1);  //print ot serial monitor to see results
      Genotronex.println(str2);
    }
    int n1; //declare as number
    int n2;

    char carray1[5]; //magic needed to convert string to a number
    str1.toCharArray(carray1, sizeof(carray1));
    n1 = atoi(carray1);

    char carray2[5];
    str2.toCharArray(carray2, sizeof(carray2));
    n2 = atoi(carray2);

    if (DEBUG) {
      Genotronex.println("parsowanie: ");
      Genotronex.println(n1);
      Genotronex.println(n2);
    }
    readString = "";

    //ustawianie czasu i zapis do eeprom
    Czas = n1;
    Czas < 0 ? Czas = 0 : Czas = Czas;
    Czas > 600 ? Czas = 600 : Czas = Czas;
    if (EEPROMSTATE) {
      EEPROM.put(adresCzas, Czas);
    }
    if (DEBUG) {
      Genotronex.print("czas: ");
      Genotronex.println(Czas);
    }

    //obliczanie mocy i zapis do eeprom
    Moc = n2;
    Moc < 0 ? Moc = 0 : Moc = Moc;
    Moc > 100 ? Moc = 100 : Moc = Moc;
    if (DEBUG) {
      Genotronex.print("moc: ");
      Genotronex.println(Moc);
    }
    MocVis = Moc;
    FMoc = (float)Moc * 2.55; //jakieś cyrki bo biblioteka pixels chce wartość od 0 do 255 XD
    if (EEPROMSTATE) {
      EEPROM.put(adresMoc, Moc);
    }
    Moc = (int)FMoc;
    pixels.setBrightness(Moc);
    pixels.show();
    ISRbutton();
  }
  else //string krótszy niż 3 znaki
  {
    if (readString.length() > 0) {
      if (DEBUG) {
        Genotronex.println(readString);
      }
      if (readString.equals("i")) {
        Genotronex.print(";");
        Genotronex.print(Czas);
        Genotronex.print(";");
        Genotronex.print(MocVis);
      }
      if (readString.equals("r")) {
        ISRbutton();
      }
    }
    readString = "";
  }
}
/*-------------------------------------------------------------------------------------------
                    S E T U P
  -------------------------------------------------------------------------------------------*/
void setup() {
  Genotronex.begin(9600);
  Genotronex.setTimeout(200);
  //Serial.begin(9600); //Ustawienie prędkości transmisji
  //Serial.setTimeout(200);
  pixels.begin(); //Inicjalizacja paska
  //pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), ISRbutton, FALLING);
  if (EEPROMSTATE) {
    EEPROM.get(adresMoc, Moc);
    MocVis = Moc;
    EEPROM.get(adresCzas, Czas);
    pixels.setBrightness(Moc);
  }
  if (DEBUG) {
    Genotronex.println(Moc);
    Genotronex.println(Czas);
  }
}
/*-------------------------------------------------------------------------------------------
                    P Ę T L A   G Ł Ó W N A
  -------------------------------------------------------------------------------------------*/
void loop() {
  Szeregowa2();
  TICK();//funkcja ustawia flagę TICK100MS co 100ms i inkrementuje licznik
  ObliczInterwalLed();//oblicza zmienną step
  if ((licznik % step) == 0)
  {
    LiczbaLed = (licznik / step);
    if (LiczbaLed <= 10)
    {
      redtogreen(LiczbaLed);
    }
    else
    {
      allred();
    }
  }
  TICK100MS = LOW;//skasowanie flagi ustawianej przez funkcję TICK
}

