//
#include "pat9125.h"
#include "config.h"
#include <avr/wdt.h>
//

#define DIAG_LED_PIN 14
#define MODE_PIN     15

bool dial_pin_state;
char cmd;

//
PAT9125 PAT[UNIT_COUNT] = {
  PAT9125(ADR, 0),
  PAT9125(ADR, 1),
  PAT9125(ADR, 2),
  PAT9125(ADR, 3),
  PAT9125(ADR, 4),
  PAT9125(ADR, 5)
};

byte FINDA_PIN[UNIT_COUNT] = {
  FINDA_PIN0,
  FINDA_PIN1,
  FINDA_PIN2,
  FINDA_PIN3,
  FINDA_PIN4,
  FINDA_PIN5
};
//
bool finda_in[UNIT_COUNT] = {};     //binary finda
long x_value[UNIT_COUNT]  = {};     //
long y_value[UNIT_COUNT]  = {};     //
int s_value[UNIT_COUNT]   = {};     //shutter value
int b_value[UNIT_COUNT]   = {};
//

void setup() {
  delay(5000);
  pinMode(MODE_PIN, INPUT);
  digitalWrite(MODE_PIN, HIGH);
  pinMode(DIAG_LED_PIN, OUTPUT);
  Serial.begin(SERIAL_SPEED);
  init_all();
  set_res_all();
  Serial.println("start");

}

void init_all() {
  for (int i = 0; i < UNIT_COUNT; i++) {
    PAT[i].pat9125_init();
    PAT[i].pat9125_reset();
    pinMode(FINDA_PIN[i], INPUT);
#if PULLUP_FINDA
    digitalWrite(FINDA_PIN[i], HIGH);
#endif
    delay(10);
  }

}

void set_res_all() {
  for (int i = 0; i < UNIT_COUNT; i++) {
    PAT[i].pat9125_set_res(XRES, YRES);
    delay(10);
  }
}

void update_all() {
  //read PAT
  for (int i = 0; i < UNIT_COUNT; i++) {
    PAT[i].pat9125_update();
    finda_in[i] = (digitalRead(FINDA_PIN[i]) ^ INVERSE_FINDA);
    delay(1);
  }

}

void to_array() {
  for (int i = 0; i < UNIT_COUNT; i++) {
    s_value[i]  = PAT[i].pat9125_s;

    if (b_value[i] == -1) {
      x_value[i]  = -1;
      y_value[i]  = -1;
      b_value[i]  = -1;
      //PAT[i].pat9125_reset();
    }
    else {
      x_value[i]  = PAT[i].pat9125_x;
      y_value[i]  = PAT[i].pat9125_y;
      b_value[i]  = PAT[i].pat9125_b;
    }

  }
}

void serial_out_ascii() {
  for (int i = 0; i < USED; i++) {
    Serial.print(i);
    Serial.print(",");
    Serial.print(finda_in[i]);
    Serial.print(",");
    Serial.print(x_value[i]);
    Serial.print(",");
    Serial.print(y_value[i]);
    Serial.print(",");
    Serial.print(b_value[i]);
    Serial.print(",");
    Serial.print(s_value[i]);
    if (i < USED -1 ) Serial.print(",");
  }
  Serial.println();
}

void process_line() {
  digitalWrite(DIAG_LED_PIN, HIGH);
  char cmd = Serial.read();
  uint8_t val;
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'R':
      val = Serial.parseInt();
      PAT[val].pat9125_reset();
      Serial.print("reset:");
      Serial.println(val);
      break;
    case 'X':
      val = Serial.parseInt();
      for (int i = 0; i < UNIT_COUNT; i++) {
        PAT[i].pat9125_set_res_x(val);
      }
      Serial.print("set X resolution:");
      Serial.println(val);
      break;
    case 'Y':
      val = Serial.parseInt();
      for (int i = 0; i < UNIT_COUNT; i++) {
        PAT[i].pat9125_set_res_y(val);
      }
      Serial.print("set Y resolution:");
      Serial.println(val);
      break;
    case 'H':
      help();
      delay(5000);
      break;
    case 'Q':
      Serial.println("software reset");
      wdt_enable(WDTO_15MS);
      break;
  }
  while (Serial.read() != 10);
  digitalWrite(DIAG_LED_PIN, LOW);
}

void help() {
  Serial.println(F("R<n> reset sensor n"));
  Serial.println(F("X<n> set resolution n"));
  Serial.println(F("Y<n> set resolution n"));
  Serial.println(F("Q reset "));

}

void loop() {
  if (Serial.available()) process_line();
  update_all();
  to_array();
  //delay(1);
  serial_out_ascii();
}
