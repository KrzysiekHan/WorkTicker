
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
#define PIN        6
//Ilosc pixeli paska do obsluzenia
#define NUMPIXELS  10
//Inicjalizacja biblioteki
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define EEPROMSTATE 1 //możliwość wyłączenia zapisu do eeprom

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
int Moc = 10;
float FMoc = 0;
int Czas = 10;

//EEPROM
int adresCzas = 10;
int adresMoc = 20;


//KOMUNIKACJA SZEREGOWA
String odebraneDane = ""; //Pusty ciąg odebranych danych
char polecenie = 'x';
String inString = "";
float flocik = 0;


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
  step = (((Czas*1000) / 100) / NUMPIXELS);
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
void Szeregowa ()
{
  if (Serial.available() > 0) {
    polecenie = Serial.read();
    switch (polecenie) {
      //ustawienie czasu
      case 't':
        Czas = Serial.parseInt();
        Czas<0?Czas=0:Czas=Czas;
        Czas>600?Czas=600:Czas=Czas;
        if (EEPROMSTATE){
          EEPROM.put(adresCzas,Czas); 
        }
        Serial.print("czas: ");
        Serial.println(Czas);
        
        ISRbutton();
        break;

      //ustawienie mocy
      case 'p':
        Moc = Serial.parseInt();
        Moc<0?Moc=0:Moc=Moc;
        Moc>100?Moc=100:Moc=Moc;
        Serial.print("moc: ");
        Serial.println(Moc);
        FMoc=(float)Moc*2.5; //jakieś cyrki bo biblioteka pixels chce wartość od 0 do 255 XD
        Moc=(int)FMoc;
        if (EEPROMSTATE){
          EEPROM.put(adresMoc,Moc);
        }
        pixels.setBrightness(Moc);
        pixels.show();
        break;

      //reset
      case 'r':
        Serial.println("zresetowano");
        ISRbutton();
        break;

      //połączono z telefonem, odeslij parametry
      case 'i':
        Serial.print(Moc);
        Serial.print(";");
        Serial.println(Czas);
        break;

      default:
        Serial.println("nieznane polecenie");
        break;
    }
    Serial.flush();
  }
}
/*-------------------------------------------------------------------------------------------
                    S E T U P
  -------------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(9600); //Ustawienie prędkości transmisji
  Serial.setTimeout(200);
  pixels.begin(); //Inicjalizacja paska
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISRbutton, FALLING);
  EEPROM.get(adresMoc, Moc);
  EEPROM.get(adresCzas, Czas);
  Serial.println(Moc);
  pixels.setBrightness(Moc);
  Serial.println(Czas);
}
/*-------------------------------------------------------------------------------------------
                    P Ę T L A   G Ł Ó W N A
  -------------------------------------------------------------------------------------------*/
void loop() {

  Szeregowa();

  TICK();//funkcja ustawia flagę TICK100MS co 100ms i inkrementuje licznik
  ObliczInterwalLed();//oblicza zmienną step
  if ((licznik % step) == 0)
  {
    LiczbaLed = (licznik / step);
    if (LiczbaLed <= 10)
    {
      redtogreen(LiczbaLed);
    }
    else {
      allred();
    }

  }

  TICK100MS = LOW;//skasowanie flagi ustawianej przez funkcję TICK

}

