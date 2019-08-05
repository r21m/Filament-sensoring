//
#include "pat9125.h"
#include "config.h"
//

//
PAT9125 PAT[unit_count] = {
  PAT9125(ADR, 0),
  PAT9125(ADR, 1),
  PAT9125(ADR, 2),
  PAT9125(ADR, 3),
  PAT9125(ADR, 4),
  PAT9125(ADR, 5)
};

byte FINDA_PIN[unit_count] = {
  FINDA_PIN0,
  FINDA_PIN1,
  FINDA_PIN2,
  FINDA_PIN3,
  FINDA_PIN4,
  FINDA_PIN5
};
//
byte finda_in[unit_count] = {};     //binary finda
long x_value[unit_count]  = {};     //
long y_value[unit_count]  = {};     //
int s_value[unit_count]   = {};     //shutter value
int b_value[unit_count]   = {};      
//

void setup() {
  pinMode(MODE_PIN, INPUT);
  pinMode(DIAG_LED_PIN, OUTPUT);
    
  Serial.begin(SERIAL_SPEED);
  delay(1000);
  init_all();
  set_res_all();
  Serial.println("start");
}

void init_all() {
  for (int i = 0; i < unit_count; i++) {
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
  for (int i = 0; i < unit_count; i++) {
    PAT[i].pat9125_set_res(XRES, YRES);
    delay(10);
  }
}

void update_all() {
  //read PAT
  for (int i = 0; i < unit_count; i++) {
    PAT[i].pat9125_update();
    
#if INVERSE_FINDA
    finda_in[i] = digitalRead(FINDA_PIN[i]);
#else
    finda_in[i] = !digitalRead(FINDA_PIN[i]);
#endif
    delay(1);
  }

}

void to_array() {
  for (int i = 0; i < unit_count; i++) {
    s_value[i]  = PAT[i].pat9125_s;
    b_value[i]  = PAT[i].pat9125_b;
    
    if (s_value[i] == -1){
    x_value[i]  = -1;
    y_value[i]  = -1;  
    }
    else{
    x_value[i]  = PAT[i].pat9125_x;
    y_value[i]  = PAT[i].pat9125_y;  
    }
    
  }
}

void serial_out_ascii() {
  for (int i = 0; i < used; i++) {
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
    Serial.println(s_value[i]);
    delay(1);
  }
}

void loop() {

  update_all();
  to_array();
  delay(100);
  serial_out_ascii();

}
