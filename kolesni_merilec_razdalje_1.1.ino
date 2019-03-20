/*********************************** VARS/LIBS ***********************************/
//----------------- LIBS -----------------//
#include <LiquidCrystal.h>

//----------------- PINS -----------------//
int sensA(10);
int sensB(9);
int m_plus(7);
int m_minus(8);
int enter(6);
int stop_start_signal(A1);

//----------------- VARS -----------------//
int counter(0);
double set_value(0.00);
double dist(0.00);
bool oneLoop(false);

//----------------- LCD -----------------//
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*********************************** SETUP LOOP ***********************************/
void setup() {
  //----------------- PINMODE -----------------//
  pinMode(sensA, INPUT_PULLUP);
  pinMode(sensB, INPUT_PULLUP);
  pinMode(m_plus, INPUT_PULLUP);
  pinMode(m_minus, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);
  pinMode(stop_start_signal, OUTPUT);
  // OUTPUT DA NA LOW, DA USTAVI MOTOR
  digitalWrite(stop_start_signal, LOW);

  //----------------- ZACETNO SPOROCILO -----------------//
  lcd.begin(16, 2);
  delay(1000);
  lcd.print("MERILEC");
  lcd.setCursor(4,1);
  lcd.print("RAZDALJE");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Nejc  SEDOVNIK");
  lcd.setCursor(3,1);
  lcd.print("Marec 2019");
  delay(3000);
}

/*********************************** FUNKCIJE ***********************************/

//----------------- LCD SPOROCILA -----------------//
// LCD NASTAVLJANJE DOLZINE //
void LCDdisplaySetup() {
  lcd.setCursor(0,0);
  lcd.print("Nastavi  dolzino");
  if(set_value >= 1000) {
    lcd.setCursor(2,1);
  }
  else if(set_value >= 100) {
    lcd.setCursor(3,1);
  }
  else if(set_value >= 10) {
    lcd.setCursor(4,1);
  } else {
    lcd.setCursor(5,1);
  }
  lcd.print(set_value);
  lcd.print(" m");
  lcd.display();
}

// LCD MED MERJENJEM //
void LCDdisplayRunning() {
  lcd.clear();
  if(dist >= 1000) {
    lcd.setCursor(2,0);
  }
  else if(dist >= 100) {
    lcd.setCursor(3,0);
  }
  else if(dist >= 10) {
    lcd.setCursor(4,0);
  } else {
    lcd.setCursor(5,0);
  }
  lcd.print(dist);
  lcd.print(" m");
  lcd.display();
}

// LCD KONEC/STOP //
void LCDdisplayFinished() {
  digitalWrite(stop_start_signal, LOW);
  lcd.clear();
  if(dist >= set_value) {
    lcd.setCursor(4,0);
    lcd.print("KONCANO!");
  } else {
    lcd.setCursor(6,0);
    lcd.print("STOP!");
  }
  lcd.display();
  delay(2000);
  lcd.clear();
}

//----------------- RESET VARS/PINS FUNKCIJA -----------------//
void reset() {
  digitalWrite(stop_start_signal, LOW);
  counter = 0;
  dist = 0.00;
  lcd.clear();
}

//----------------- RESET HARDWARE FUNKCIJA -----------------//
void(*resetFunc) (void) = 0;

//----------------- NASTAVLJANJE DOLZINE -----------------//
void setVal() {
  reset();
  while(digitalRead(enter) == HIGH) {   
    LCDdisplaySetup();
    counter = 0;
    delay(150);
    // DOLZINA PLUS //
    while(digitalRead(m_plus) == LOW && set_value < 9999.90) {
      delay(250);
      // RESET VSEH VARS IN LCD 
      if(digitalRead(m_minus) == LOW) {
        reset();
        set_value = 0;
        lcd.setCursor(5,0);
        lcd.print("RESET!");
        delay(2000);
        LCDdisplaySetup();
        break;
      }
      // PONOVNI ZAGON MIKROKONTROLERJA
      while(digitalRead(enter) == LOW) {
        delay(100);
        counter++;
        if(counter == 50) {
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("PONOVNI");
          lcd.setCursor(6,1);
          lcd.print("ZAGON");
          delay(2000);
          resetFunc();
        }
      }
      // SPREMEMBA HITROSTI NASTALJANJA DOLZINE
      if(counter > 40) {
        set_value += 100.00;
        LCDdisplaySetup();
      }
      else if(counter > 20) {
        set_value += 10.00;
        LCDdisplaySetup();
        counter++;
      } else {
        set_value += 0.10;
        LCDdisplaySetup();
        counter++;
      }
      // NASTAVI DOLZINO V KOLIKOR JE VECJA OD 9999.90
      if(set_value >= 9999.90) {
        set_value = 9999.90;
        lcd.clear();
        LCDdisplaySetup();
        break;
      }
    }
    
    // DOLZINA MINUS //
    while(digitalRead(m_minus) == LOW && set_value > 0) {
      delay(250);
      // RESET VSEH VARS IN LCD
      if(digitalRead(m_plus) == LOW) {
        reset();
        set_value = 0;
        lcd.setCursor(5,0);
        lcd.print("RESET!");
        delay(2000);
        LCDdisplaySetup();
        break;
      }
      while(digitalRead(enter) == LOW) {
        delay(150);
        counter++;
        // PONOVNI ZAGON MIKROKONTROLERJA (DRZIS PLUS IN ENTER)
        if(counter == 50) {
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("PONOVNI");
          lcd.setCursor(6,1);
          lcd.print("ZAGON");
          delay(2000);
          resetFunc();
        }
      }
      // CLEAR LCD,  PRIMERU DA SE DOLZINA PREIDE NA 3 ALI 2 MESTNO
      if(int(set_value) % 10 == 0 || int(set_value) % 9 == 0) {
        lcd.clear();
      }
      // SPREMEMBA HITROSTI SPREMINJANJA DOLZIN
      if(counter > 40) {
        set_value -= 100.00;
        LCDdisplaySetup();
      }
      else if(counter > 20) {
        set_value -= 10.00;
        LCDdisplaySetup();
        counter++;
      } else {
        set_value -= 0.10;
        LCDdisplaySetup();
        counter++;
      }
      // DOLZINO RESETIRA NA 0 V KOLIKOR PRIDE POD 0
      if(set_value <= 0.00) {
        set_value = 0;
        lcd.clear();
        LCDdisplaySetup();
        break;
      }
    }
  }

  // START/ZAGON MOTORJA //
  reset();
  lcd.setCursor(5,0);
  lcd.print("START!");
  delay(2000);
  digitalWrite(stop_start_signal, HIGH);
  lcd.clear();
}

/*********************************** LOOP ***********************************/
void loop() {
  setVal();
  LCDdisplayRunning();
  while(true) {
    // VRTENJE NAZAJ //
    if(digitalRead(sensA) == HIGH && digitalRead(sensB) == LOW && oneLoop == true) {
      counter--;
      // CE IZMERI 100 SIGNALOV PRESTAVI IZMERJENO DOLZINO ZA minus 0.1 OZ 10CM
      if(counter <= -100) {
        counter = 0;
        dist -= 0.1;
        LCDdisplayRunning();
      }
      oneLoop = false;
    }

    // VRTENJE NAPREJ //
    else if(digitalRead(sensA) == LOW && digitalRead(sensB) == HIGH && oneLoop == true) {
      counter++;
      // CE IZMERI 100 SIGNALOV PRESTAVI IZMERJENO DOLZINO ZA PLUS 0.1 OZ 10CM
      if(counter >= 100) {
        counter = 0;
        dist += 0.1;
        // CE JE IZMERJENA DOLZINA ENAKA NASTAVLJENI DOLZINI
        if(dist >= set_value) {
          LCDdisplayFinished();
          break;
        }
        LCDdisplayRunning();
      }
      oneLoop = false;
    }

    // EN DIGITALNI KROG (LOW/LOW, HIGH/LOW, HIGH/HIGH, LOW/HIGH, LOW/LOW - IN OBRATNO) - 4 SIGNALI - 800 SIGNALOV ZA EN KROG (20cm) //
    else if(digitalRead(sensA) == HIGH && digitalRead(sensB) == HIGH && oneLoop == false) {
      oneLoop = true;
    }

    // CE PRITISNES STOP GUMB //
    if(digitalRead(enter) == LOW) {
      LCDdisplayFinished();
      break;
    }
  }
}
