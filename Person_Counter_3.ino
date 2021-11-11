/* Dr.TRonik YouTube
   Kasım2021
   Giriş / Çıkış 2 yönlü kişi sayıcı
   P10 16*32 Led Panel gösterge
   4 satır, 20 karekter I²C LCD Ekranlı
   Buzzer ile  farklı ses tonları
   https://www.youtube.com/watch?v=QjX1xY_hw9E&t=12s
*/

//Kütüphane ve diğer genel tanımlamalar:
#include <LiquidCrystal_I2C.h>  //I²C li LCD için gerekli...
#include <Wire.h>               //I²C için gerekli...
#include <SPI.h>                //DMD için gerekli...
#include <DMD.h>                //DMD kütüphanesini ekleyelim
#include <TimerOne.h>           //TimerOne kütüphanesini ekleyelim, DMD için gerekli...
#include "angka6x13.h"          //Panelde kullanılan sayı fontu...
#define DISPLAYS_ACROSS 1       //Yatayda 1 panel
#define DISPLAYS_DOWN 1         // Dikeyde 1 panel
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN); //1,1 Yani tek paneli tanımlama

LiquidCrystal_I2C lcd(0x3F, 20, 4); //Kullandığımız LCD I²C adresi ve karakter satır sayısı

int sensor_giris = 3; //Birinci sensör girişi, int ile tanımlamaya örnek...
int sensor_cikis = 4; //İkinci sensör girişi
int sayac = 0;        //Sayacımızın ilk değeri
int buzzer = 2;       //Buzzerin bağlı olduğu pin
int x = 140;          //Sensörlerin arasındaki mesafeye göre ayarlanmalı!
int koord_x ;         // P10 panelde gösterilen bir, iki ve üç haneli sayıların ortalanabilmesi için...

#define role_pin A0         //Röle pini tanımlandı, #define ile tanımlamaya örnek...
#define lamba A1            // Birinci grup aydınlatma
#define lamba_1 A2          // İkinci grup aydınlatma
#define button_pin 5        // Mod seçimi için kullanacağımız button
boolean button_durum = 1;   // Buton pininden veri okumak için değişkenimiz, başlangıçta HIGH, yani basılı değil...
int button_sayac = 0;       //Butona basılma sayısını tutacak değişkenimiz, başlangıçta değeri 0...
long ref = 0;               //delay() kullanamadığımız yerlerde millis() Kullanabilmek için...

void ScanDMD() // DMD kütüphanesi ile birlikte gelen ve Timer1 kesmesine attach edilen fonksiyon,
{
  dmd.scanDisplayBySPI();
}

//___SET_UP___SET_UP____SET_UP____SET_UP____SET_UP____SET_UP____SET_UP____SET_UP

void setup() {
    
  lcd.begin();  //LCD başlasın
  lcd.clear();  // ve ekran temizlensin
  Timer1.initialize( 4000 );           //3000 - 5000 arası ideal, bu değerlerin dışında titreme olabiliyor özellikle çoklu panellerde
  Timer1.attachInterrupt( ScanDMD );   //attach:  Timer1 kesmesine ScanDMD fonksiyonunu attach etme
  dmd.clearScreen( true );             //Panelde yanık led kalmasın
  
  //Pin mod ve durumlarını ayarlayalım
  digitalWrite (role_pin, HIGH); 
  digitalWrite (lamba, LOW);
  digitalWrite (lamba_1, LOW);
  pinMode(sensor_giris, INPUT);
  pinMode(sensor_cikis, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(role_pin, OUTPUT);
  pinMode(lamba, OUTPUT);
  pinMode(lamba_1, OUTPUT);
  
// Sensör data kabloları çıkmış ise uyarı:
  if ((digitalRead(sensor_giris) == LOW) || (digitalRead(sensor_cikis) == LOW)) {
    lcd.setCursor(0, 0);
    lcd.print(" **SENSOR ARIZASI**");
    lcd.setCursor(0, 1);
    lcd.print("**DATA GiRiSLERiNi**");
    lcd.setCursor(0, 2);
    lcd.print(" ***KONTROL EDiN***");
    while(1);
  }
  else //Her şey normal ise 5 saniye bilgi ekranı
  {
    lcd.setCursor(0, 0);
    lcd.print("*2 YONLU SAYICI V.1*");
    lcd.setCursor(0, 1);
    lcd.print("*Dr.TRonik  YouTube*");
    lcd.setCursor(0, 2);
    lcd.print("Lutfen Bir Mod Secin");
    lcd.setCursor(0, 3);
    lcd.print("  VARSAYILAN:1.MOD");
    delay(5000);
  }
}

//LOOP___LOOP___LOOP___LOOP___LOOP__LOOP___LOOP__LOOP___LOOP__LOOP

void loop() {
  //______Buton Sayacı_____
  button_durum = digitalRead(button_pin);
  if (button_durum == LOW) {
    button_sayac ++ ;
    delay(500);
    if (button_sayac > 3) //Eğer butona 4 defa basılırsa (0, 1, 2, 3)
    {
      button_sayac = 0; // 5. basılışta buton değeri başa döner ve 0 değerini alır
    }
  }
  //______Buton Sayacı_____SONU____

  if ((sayac <= 0) || (sayac > 999)) {
    sayac = 0 ;

  }
  //Serial.println(button_sayac);

  //_____SWITCH CASE BAŞLANGIÇ_____

  switch (button_sayac) {
    case 0:
      bi_directional();
      break;

    case 1:
      in();
      break;

    case 2:
      out();
      break;

    case 3:
      off();
      break;

    default:
      break;

  }
  //Sayac 5 den büyük olduğunda, röle aktif olsun
  if (sayac > 5) {
    digitalWrite (role_pin, LOW);
    lcd.setCursor(13, 0);
    lcd.print("Role:ON");
  } else {
    digitalWrite (role_pin, HIGH);
    lcd.setCursor(12, 0);
    lcd.print("Role:OFF");
  }
  //Sayac 9 dan büyük olduğunda birinci grup aydınlatma aktif olsun
  if (sayac > 9) {
    digitalWrite (lamba, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Lamp1:ON");
  } else {
    digitalWrite (lamba, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Lamp1:OFF");
  }
  //Sayac 12 den büyük olduğunda ikinci grup aydınlatma aktif olsun
  if (sayac > 12) {
    digitalWrite (lamba_1, HIGH);
    lcd.setCursor(11, 1);
    lcd.print("Lamp2:ON");
  } else {
    digitalWrite (lamba_1, LOW);
    lcd.setCursor(11, 1);
    lcd.print("Lamp2:OFF");
  }

  //___DMD KODLARI___

   if ((sayac <= 0) || (sayac > 999)) //Sayac 999 dan sonra 0'a dönsün
      {
        sayac = 0 ;
      }
   dmd.selectFont(angka6x13);
   //Hane sayısına göre sayac değerini panelde ortaya yazdırma
  if ((sayac >= 0) && (sayac < 10)) {
    koord_x = 13; //Tek hane
  }
  if ((sayac >= 10) && (sayac < 100)) {
    koord_x = 10; //Çift hane
  }
  if ((sayac >= 100) && (sayac < 1000)) {
    koord_x = 6; //Üç hane
  }
  char buffer[3];
  dmd.drawString(koord_x, 0, dtostrf(sayac, 3, 0, buffer), 3, GRAPHICS_NORMAL );

  //>>> dtostrf(float_değer, min string değeri, noktadan sonra istenen basamak sayısı, çıktının depolanacağı arabellek)
  //sayac verisi, data to string komutu ile 4 haneli bir string verisine çevrilerek, char buffer vasıtası ile
  //dmd.drawString fonksiyonu içinde yorumlanıp, istenilen x ve y koordinatlarına yazdırılır...

}
//___Fonksiyon tanımları___

void bi_directional () {
  if (millis() - ref >= 1000) //LCD ve paneldeki devamlılığı sağlayabilmek için delay kullanmadan...
  {
    ref = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1.MOD AKTiF");
    lcd.setCursor(0, 2);
    lcd.print(">>>GiRiS ve CIKIS<<<");
    lcd.setCursor(3, 3);
    lcd.print("Son SAYIM:");
    lcd.setCursor(14, 3);
    lcd.print(sayac);
  }
  if (digitalRead(sensor_giris) == LOW) {
    delay(x);
    if (digitalRead(sensor_cikis) == LOW) {
      dmd.clearScreen (true);

      sayac++;

      dmd.writePixel( 0, 0, GRAPHICS_NORMAL, 1 );//PANEL giriş
      dmd.writePixel( 1, 1, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 0, 2, GRAPHICS_NORMAL, 1 );

      if ((sayac <= 0) || (sayac > 999)) //999 dan sonra 0 dönsün
      {
        sayac = 0 ;
      }
      tone(buzzer, 250, 100);

    }
  }

  if (digitalRead(sensor_cikis) == LOW) {
    delay(x);
    if (digitalRead(sensor_giris) == LOW) {
      dmd.clearScreen (true);
      //
      sayac--;
      dmd.writePixel( 31, 0, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 30, 1, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 31, 2, GRAPHICS_NORMAL, 1 );

      tone(buzzer, 800, 100);
    }
  }
}

void in() {
  if (millis() - ref >= 1000) //
  {
    ref = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("2.MOD AKTiF");
    lcd.setCursor(0, 2);
    lcd.print(">>>Sadece GiRiS>>>");
    lcd.setCursor(3, 3);
    lcd.print("Son SAYIM:");
    lcd.setCursor(14, 3);
    lcd.print(sayac);
  }

  if (digitalRead(sensor_giris) == LOW) {
    delay(x);
    if (digitalRead(sensor_cikis) == LOW) {
      dmd.clearScreen (true);

      sayac++;

      if ((sayac <= 0) || (sayac > 999)) {
        sayac = 0 ;
      }
      dmd.writePixel( 0, 0, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 1, 1, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 0, 2, GRAPHICS_NORMAL, 1 );

      tone(buzzer, 250, 100);

    }
  }
}

void out() {
  if (millis() - ref >= 1000) //
  {
    ref = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("3.MOD AKTiF");
    lcd.setCursor(0, 2);
    lcd.print("<<<Sadece CIKIS<<<");
    lcd.setCursor(3, 3);
    lcd.print("Son SAYIM:");
    lcd.setCursor(14, 3);
    lcd.print(sayac);
  }
  
  if (digitalRead(sensor_cikis) == LOW) {
    delay(x);
    if (digitalRead(sensor_giris) == LOW) {
      dmd.clearScreen (true);
      
      sayac++;

      if ((sayac <= 0) || (sayac > 999)) {
        sayac = 0 ;
      }
      dmd.writePixel( 31, 0, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 30, 1, GRAPHICS_NORMAL, 1 );
      dmd.writePixel( 31, 2, GRAPHICS_NORMAL, 1 );

      tone(buzzer, 800, 100);
    }
  }
}

void off() {
  if (millis() - ref >= 1000) //
  {
    ref = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("4.MOD AKTiF");
    lcd.setCursor(3, 2);
    lcd.print(">>>-DiKKAT-<<<");
    lcd.setCursor(1, 3);
    lcd.print("!SAYIM YAPILMIYOR!");
  }
}
//Dr.TRonik YouTube... KASIM 2021...
