#include <EEPROM.h>
#include <SoftwareSerial.h>
//*********************************************Config Uart_PIN
SoftwareSerial RFID_Serial(2, 3); // RX, TX
//*********************************************Config PIN_RLY
const int PIN_RLY = A0;
long Delay_RLY = 3000; // Min = 1000 Ms
//*********************************************Config PIN_Buzzer
const int PIN_buzzer = 13;
//*********************************************Config Pin_Keypad
const int PIN_SW_Add = 12;
const int PIN_SW_Delete = 11;
//*********************************************Config Pin_LED
const int PIN_VCC_Led_Ok = 5;
const int PIN_GND_Led_Ok = 4;
const int PIN_VCC_Led_Error = 9;
const int PIN_GND_Led_Error = 8;
//*********************************************Config Value
String card;
int id_card = 0;
unsigned char i = 0;
unsigned char usart_data;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
boolean start_flag = 0, read_flag = 0, user_ok = 0, key = 0;
boolean key_Add, key_Delete;
//*********************************************Subprogram setup()
void setup() {
  RFID_Serial.begin(9600);
  pinMode(PIN_RLY, OUTPUT);
  pinMode(PIN_buzzer, OUTPUT);
  pinMode(PIN_VCC_Led_Ok, OUTPUT);
  pinMode(PIN_GND_Led_Ok, OUTPUT);
  pinMode(PIN_VCC_Led_Error, OUTPUT);
  pinMode(PIN_GND_Led_Error, OUTPUT);
  pinMode(PIN_SW_Add, INPUT_PULLUP);
  pinMode(PIN_SW_Delete, INPUT_PULLUP);
  digitalWrite(PIN_RLY, HIGH);
  digitalWrite(PIN_GND_Led_Ok, LOW);
  digitalWrite(PIN_GND_Led_Error, LOW);
}
//*********************************************Subprogram loop()
void loop() {
  key_Add = digitalRead(PIN_SW_Add);
  key_Delete = digitalRead(PIN_SW_Delete);
  Check_in();
  if (read_flag == 1) {
    if (key_Add == 0 && key_Delete == 1) {
      key = 1;
      Add_card();
    } else if (key_Add == 1 && key_Delete == 0) {
      key = 1;
      Delete_card();
    } else {
      check_in_Mode();
    }
  }
  if (key == 1 && key_Add == 1 && key_Delete == 1) {
    key = 0;
  }
}
//*********************************************Subprogram Check_in()
void Check_in() {
  if (RFID_Serial.available() && read_flag == 0) {
    usart_data = RFID_Serial.read();
    if (usart_data == 0x02) start_flag = 1;
    if (start_flag && i < 15) {
      digitalWrite(PIN_buzzer, HIGH);
      card = card + String(usart_data);
      i++;
    }
    if (usart_data == 0x03) {
      start_flag = 0;
      read_flag = 1;
      i = 0;
      delay (200);
      digitalWrite(PIN_buzzer, LOW);
    }
  }
}
//*********************************************Subprogram Add_card()
void Add_card() {
  if (read_flag == 1) {
    i = 1;
    user_ok = 0;
    while (i <= 10) {
      if (card == EEP_Read_String(i)) {
        user_ok = 1;
        break;
      }
      i++;
    }
    if (user_ok == 1) {
      _ERROR();
    } else {
      i = 1;
      user_ok = 0;
      while (i <= 10) {
        if (EEP_Read_String(i) == EEP_Read_String(11)) {
          user_ok = i;
          break;
        }
        i++;
      }
      if (i > 10) {
        _ERROR();
      } else {
        id_card = i;
        EEP_Write_String(id_card, card);
        _OK();
      }
    }
    user_ok = 0;
    read_flag = 0;
    i = 0;
    card = "";
  }
  Empty_Serial();
}
//*********************************************Subprogram Delete_card()
void Delete_card() {
  if (read_flag == 1) {
    i = 1;
    user_ok = 0;
    while (i <= 10) {
      if (card == EEP_Read_String(i)) {
        user_ok = 1;
        break;
      }
      i++;
    }
    if (user_ok == 1) {
      card = "";
      EEP_Write_String(i, card);
      _OK();
    } else {
      _ERROR();
    }
    user_ok = 0;
    read_flag = 0;
    i = 0;
    card = "";
  }
  Empty_Serial();
}
//*********************************************Subprogram check_in_Mode()
void check_in_Mode() {
  if (read_flag == 1) {
    i = 1;
    user_ok = 0;
    while (i <= 10) {
      if (card == EEP_Read_String(i)) {
        user_ok = 1;
        i = 0;
        break;
      }
      i++;
    }
    if (user_ok == 1) {
      digitalWrite(PIN_RLY, LOW);
      if (Delay_RLY < 1000) Delay_RLY = 1000;
      currentMillis = millis();
      previousMillis = currentMillis;
      _OK();
      digitalWrite(PIN_VCC_Led_Ok, HIGH);
      while (currentMillis - previousMillis < Delay_RLY) {
        currentMillis = millis();
      }
    } else {
      _ERROR();
    }
    digitalWrite(PIN_RLY, HIGH);
    user_ok = 0;
    read_flag = 0;
    i = 0;
    card = "";
  }
  Empty_Serial();
  digitalWrite(PIN_VCC_Led_Ok, LOW);
}
//*********************************************Subprogram Empty_Serial()
void Empty_Serial(){
  RFID_Serial.flush();
  if (RFID_Serial.readString() != "") {
    while (RFID_Serial.readString() != "") {
    }
  }
}
//*********************************************Subprogram _OK()
void _OK() {
  for (int _loop = 0 ; _loop < 3 ; _loop++) {
    digitalWrite(PIN_VCC_Led_Ok, HIGH);
    digitalWrite(PIN_buzzer, HIGH);
    delay (50);
    digitalWrite(PIN_VCC_Led_Ok, LOW);
    digitalWrite(PIN_buzzer, LOW);
    delay (50);
  }
}
//*********************************************Subprogram _ERROR()
void _ERROR() {
  digitalWrite(PIN_VCC_Led_Error, HIGH);
  digitalWrite(PIN_buzzer, HIGH);
  delay (2000);
  digitalWrite(PIN_VCC_Led_Error, LOW);
  digitalWrite(PIN_buzzer, LOW);
}
//*********************************************Subprogram EEP_Write_String()
void EEP_Write_String(byte part, String val) {
  int start_addw = part * 32;
  int w_len = val.length();
  EEPROM.write(start_addw, w_len + 1);
  int end_w = start_addw + w_len;
  byte len = 0;
  for (int i = start_addw + 1; i < end_w + 1; i++) {
    EEPROM.write(i, int(val[len]));
    len++;
  }
  len = 0;
}
//*********************************************Subprogram EEP_Read_String()
String EEP_Read_String(byte part) {
  int start_addr = part * 32;
  String str;
  int ch;
  int r_len = EEPROM.read(start_addr);
  int end_r = start_addr + r_len;
  for (int i = start_addr + 1; i < end_r; i++) {
    ch = EEPROM.read(i);
    str += char(ch);
  }
  return str;
}