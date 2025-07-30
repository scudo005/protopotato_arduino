/*
  The circuit:
 * LCD RS pin to digital pin 12 -> moved from pin 11 due to PWM corruption
 thanks to tone()
 * LCD Enable pin to digital pin 13 -> moved from pin 10 due to possible timer
 corruption
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 7 -> moved from pin 3 due to PWM corruption thanks
 to tone()
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * passive piezo speaker 5V to pin 8
 * button #1 to pin 0
 * button #2 to pin 1
*/

#include "pitches.h"
#include <LiquidCrystal.h>

const int rs = 12, en = 13, d4 = 5, d5 = 4, d6 = 7, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int piezoOut = 8;
const int prevButton = 0;
const int nextButton = 1;
int index = 0; // pointers are broken in this stupid ass language
bool doWeNeedToBlink_ISR = false;
byte hyphen[8]{
    0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000,
};
byte allBlack[8]{// black square
                 0b11111, 0b11111, 0b11111, 0b11111,
                 0b11111, 0b11111, 0b11111, 0b11111};
byte face_top_left[8]{
    0b00001, 0b00010, 0b00100, 0b00100, 0b01000, 0b01000, 0b10000, 0b10000,
};
byte face_bottom_left[8]{
    0b10000, 0b10000, 0b01000, 0b01000, 0b00100, 0b00100, 0b00010, 0b00001,
};
byte tilde[8]{
    0b00000, 0b00000, 0b00000, 0b01000, 0b10101, 0b00010, 0b00000, 0b00000,

};
byte smile_awake_left[8]{
    0b00000, 0b00000, 0b00000, 0b10000, 0b01000, 0b00101, 0b00010, 0b00000,
};
byte smile_awake_right[8]{
    0b00000, 0b00000, 0b00000, 0b00001, 0b00010, 0b10100, 0b01000, 0b00000,
};
byte face_top_right[8]{
    0b10000, 0b01000, 0b00100, 0b00100, 0b00010, 0b00010, 0b00001, 0b00001,
};
byte face_bottom_right[8]{
    0b00001, 0b00001, 0b00010, 0b00010, 0b00100, 0b00100, 0b01000, 0b10000,
};

ISR(TIMER1_COMPA_vect) // Interrupt Service/Software Routine. used to handle
                       // COMPA timer interrupt. used for blinking eyes
                       // animation. fires every 4096ms.
{
  OCR1A += 64000; // Advance The COMPA Register
  // Handle The Timer Interrupt
  doWeNeedToBlink_ISR = true; // calling functions inside the ISR is broken
                              // (timers don't work properly)
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, allBlack);
  lcd.createChar(1, face_top_left);
  lcd.createChar(2, face_bottom_left);
  lcd.createChar(3, tilde);
  lcd.createChar(4, smile_awake_left);
  lcd.createChar(5, smile_awake_right);
  lcd.createChar(6, face_top_right);
  lcd.createChar(7, face_bottom_right);
  pinMode(prevButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);
  randomSeed(analogRead(0));
  /*index = (int) (random(MIN_QUOTES, MAX_QUOTES));*/ // unused
  TCCR1A = 0;                                         // Init Timer1A
  TCCR1B = 0;                                         // Init Timer1B
  TCCR1B |= B00000101;                                // Prescaler = 1024
  OCR1A = 64000;       // Timer Compare1A Register
  TIMSK1 |= B00000010; // Enable Timer COMPA Interrupt
  Serial.begin(38400);
}

void loop() {
  anim_initialize();
  beep_startup(); // doing this (a different function for every step of the
                  // animation) is useful for testing every single part
                  // separately
  show_face_idle();
  // check_buttons();
  beep_goodbye();
  do_nothing();
}

/*void check_buttons() { // this function is unused, but I plan to harvest code
                       // from it to manage pressing buttons while in the
                       // various animations.
  bool areWeDone = false;
  while (!areWeDone) {
    delay(175); // so we don't change the index every time the CPU polls the IO
                // lines and founds a button is pressed. determined empirically.
    lcd.home();
    if (digitalRead(prevButton) == LOW && digitalRead(nextButton) == HIGH) {
      two_short_beeps();
      index--;
      print_quotes(index);
    } else if (digitalRead(nextButton) == LOW &&
               digitalRead(prevButton) == HIGH) {
      two_short_beeps();
      index++;
      print_quotes(index);
    } else if (digitalRead(prevButton) == LOW &&
               digitalRead(nextButton) == LOW) {
      lcd.clear();
      lcd.print("Goodbye!");
      areWeDone = true;
    }
  }
}*/

/*void print_quotes(int index) { // this too is never called. maybe one day I'll
                               // do something with it.
  lcd.clear();
  lcd.home();
  switch (index) {
  case 0:
    lcd.print("Qui godi");
    lcd.setCursor(0, 1);
    lcd.print("o vieni goduto");
    break;
  case 1:
    lcd.print("Sai che aspetto?");
    lcd.setCursor(0, 1);
    lcd.print("Il chiesi");
    break;
  case 2:
    lcd.print("cinghiale");
    lcd.setCursor(0, 1);
    lcd.print("- neonsn0w");
    break;
  default:
    index = (int)(random(MIN_QUOTES, MAX_QUOTES)); // no underflows or
                                                   // overflows!
    print_quotes(index);
    break;
  }
}*/

void do_nothing() {
  while (1 == 1) {
  }
}

void beep_startup() {
  tone(piezoOut, NOTE_C6, 500);
  lcd.clear();
  lcd.home();
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Protopotato!");
  delay(3000);
  lcd.clear();
}

void two_short_beeps() {
  tone(piezoOut, NOTE_C6, 50);
  delay(125);
  tone(piezoOut, NOTE_C5, 50);
}

void beep_goodbye() {
  lcd.clear();
  lcd.home();
  lcd.print("Goodbye!");
  noTone(piezoOut);
  delay(350);
  tone(piezoOut, NOTE_F6, 350);
  delay(500);
  tone(piezoOut, NOTE_C6, 350);
  delay(500);
  tone(piezoOut, NOTE_F5, 350);
}

void anim_initialize() { // I had to unroll a for loop or it wouldn't work
  lcd.setCursor(2, 0);
  lcd.print("Initializing");
  lcd.setCursor(12, 1);
  lcd.print("0%");
  delay(900);

  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("10%");
  delay(900);

  lcd.setCursor(1, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("20%");
  delay(900);

  lcd.setCursor(2, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("30%");
  delay(900);

  lcd.setCursor(3, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("40%");
  delay(900);

  lcd.setCursor(4, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("50%");
  delay(900);

  lcd.setCursor(5, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("60%");
  delay(900);

  lcd.setCursor(6, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("70%");
  delay(900);

  lcd.setCursor(7, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("80%");
  delay(900);

  lcd.setCursor(8, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("90%");
  delay(900);

  lcd.setCursor(9, 1);
  lcd.write(byte(0));
  lcd.setCursor(11, 1);
  lcd.print("100%");
  delay(750);
}

void show_face_idle() {
  bool goodbye = false;
  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.write(byte(1));

  lcd.setCursor(5, 1);
  lcd.write(byte(2));

  lcd.setCursor(6, 0);
  lcd.write("o");

  lcd.setCursor(7, 1);
  lcd.write(byte(4));

  lcd.setCursor(8, 1);
  lcd.write(byte(5));

  lcd.setCursor(9, 0);
  lcd.print("o");

  lcd.setCursor(10, 0);
  lcd.write(byte(6));

  lcd.setCursor(10, 1);
  lcd.write(byte(7)); // done

  recycle_custom_char_slot_0(hyphen);
  do {
    blink_eyes();
    if (digitalRead(prevButton) == LOW && digitalRead(nextButton) == LOW)
      goodbye = true;
    if (!digitalRead(prevButton) || !digitalRead(nextButton)) { // you can boop
      doWeNeedToBlink_ISR = false;                              // it!
      lcd.setCursor(6, 0);
      lcd.write(byte(3));

      lcd.setCursor(9, 0);
      lcd.write(byte(3));

      tone(piezoOut, NOTE_F7, 250);
      delay(250);
      tone(piezoOut, NOTE_F7, 250);
      delay(500);

      lcd.setCursor(6, 0);
      lcd.print("o");

      lcd.setCursor(9, 0);
      lcd.print("o");
      doWeNeedToBlink_ISR = true;
    }
    long rnd1 = random(OCR1A);
    long rnd2 =
        TCNT1 * TCCR0A * 10000 * OCR1A *
        123456789; // two numbers unlikely to be related but not too unlikely
    if (rnd1 % rnd2 == 1 &&
        rnd2 != 0) { // it is true when rnd1 = 1. it happens randomly but not
                     // too seldom. it can get really "talkative" at times.
      two_short_beeps();
    }

  } while (!goodbye);
}

void show_face_sleep_frame_1() { // this should get called when user has not
                                 // interacted for a while. (3 min?) unused for
                                 // now. should be called by COMPB ISR.
  lcd.setCursor(5, 0);
  lcd.write(byte(1));

  lcd.setCursor(5, 1);
  lcd.write(byte(2));

  lcd.setCursor(6, 0);
  lcd.write(byte(4));

  lcd.setCursor(8, 0);
  lcd.write(byte(4));
}

void blink_eyes() {
  if (doWeNeedToBlink_ISR) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.write(byte(1));

    lcd.setCursor(5, 1);
    lcd.write(byte(2));

    lcd.setCursor(7, 1);
    lcd.write(byte(4));

    lcd.setCursor(8, 1);
    lcd.write(byte(5));

    lcd.setCursor(9, 0);
    lcd.print("o");

    lcd.setCursor(10, 0);
    lcd.write(byte(6));

    lcd.setCursor(10, 1);
    lcd.write(
        byte(7)); // finished drawing face again (this is probably unnecessary)

    // decide whether to blink left or right eye
    int rng = random(100);
    if (rng > 50) {
      lcd.setCursor(6, 0);
      lcd.print("o"); // patch

      lcd.setCursor(9, 0);
      lcd.write(byte(3)); // tilde
      delay(150);

      lcd.setCursor(9, 0);
      lcd.write(
          byte(0)); // hyphen because the one in the LCD ROM doesn't look good
      delay(150);

      lcd.setCursor(9, 0);
      lcd.print("o");

    } else {
      lcd.setCursor(6, 0);
      lcd.write(byte(3)); // tilde
      delay(150);

      lcd.setCursor(6, 0);
      lcd.write(byte(0)); // hyphen
      delay(150);

      lcd.setCursor(6, 0);
      lcd.print("o");
    }
    doWeNeedToBlink_ISR = false; // all right
  } else
    doWeNeedToBlink_ISR = false; // just in case.
}

void recycle_custom_char_slot_0(
    byte custom_char[]) { // this function is needed because the LCD is in 4-bit
                          // mode and can only have character ID numbers ranged
                          // 0-7.
  lcd.createChar(0, custom_char);
}
