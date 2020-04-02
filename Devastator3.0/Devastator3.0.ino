#include <Arduino.h>

#define LEFT_EN 4
#define LEFT_PWM 5
#define RIGHT_EN 7
#define RIGHT_PWM 6

uint8_t l_fwds = LOW, l_back = HIGH;
uint8_t r_fwds = LOW, r_back = HIGH;

void setMotorDirections(bool left_fwds, bool right_fwds)
{
  if (left_fwds) {
    l_fwds = !l_fwds;
    l_back = !l_back;
  }

  if (right_fwds) {
    r_fwds = !r_fwds;
    r_back = !r_back;
  }
}

void writeMotors(int16_t pwm_left, int16_t pwm_right)
{
  if (pwm_left < 0) {
    pwm_left = -pwm_left;
    digitalWrite(LEFT_EN, l_back);
  } else {
    digitalWrite(LEFT_EN, l_fwds);
  }

  if (pwm_right < 0) {
    pwm_right = -pwm_right;
    digitalWrite(RIGHT_EN, r_back);
  } else {
    digitalWrite(RIGHT_EN, r_fwds);
  }

  pwm_left = constrain(pwm_left, 0, 255);
  pwm_right = constrain(pwm_right, 0, 255);

  analogWrite(LEFT_PWM, pwm_left);
  analogWrite(RIGHT_PWM, pwm_right);
}

void setup() 
{
  bool enabled = false;
  Serial.begin(9600);
  pinMode(LEFT_EN, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);

  setMotorDirections(true, true);
  Serial.println("\nReady! Waiting for enable...");

  while (true) {
    if (Serial.available()) {
      if (Serial.read() == 'G') {
        if (Serial.read() == 'O') {
          Serial.println("Enabled!");
          break; 
        }   
      }

      if (!enabled) {
        Serial.println("Please enter 'GO':");
      }
    }
    
    delay(10);
  }
}

#define BUFF_SIZE 256
char input_buff[BUFF_SIZE];
char *buff_ptr = input_buff;
const char EOL = '\n';

void skipSpaces()
{
  //Serial.println(F("Skipping spaces..."));
  while (*buff_ptr++ == ' ');
  buff_ptr--;
}

int16_t readInt16()
{
  int16_t returned_value = -256; // error value
  int16_t value = 0;
  int16_t mult = 1;
  bool valid = false;

  //Serial.print(F("Reading int, start is '"));
  //Serial.print(*buff_ptr >= '0' && *buff_ptr <= '9' ? char(*buff_ptr) : *buff_ptr);
  //Serial.println("'");

  if (*buff_ptr == '-') {
    //Serial.println(F("Minus found!"));
    mult = -1;
    buff_ptr++;
  }

  while((buff_ptr < input_buff + BUFF_SIZE) &&
        (*buff_ptr >= '0' && *buff_ptr <= '9')) {
    value = value * 10 + int(*buff_ptr - '0');
    valid = true;
    buff_ptr++;
  }

  if (valid) {
    returned_value = mult * value;
  }

  return returned_value;
}

void clearBuffer()
{
  for (buff_ptr = input_buff;
       buff_ptr < input_buff + BUFF_SIZE && *buff_ptr != '\0';
       buff_ptr++) {
    *buff_ptr = '\0';
  }

  buff_ptr = input_buff;
}

void processSerial()
{
  int16_t pwm_left, pwm_right;
  
  skipSpaces();
  pwm_left = readInt16();
  skipSpaces();
  pwm_right = readInt16();

  //Serial.print(F("PWM left: "));
  //Serial.print(pwm_left);
  //Serial.print(F(", PWM right: "));
  //Serial.println(pwm_right);

  if (abs(pwm_left) <= 255 && abs(pwm_right) <= 255) {
    // I hate Arduino's indentation so much
    //Serial.print(F("Read both expected values of "));
    //Serial.print(pwm_left);
    //Serial.print(F(" and "));
    //Serial.println(pwm_right);
    writeMotors(pwm_left, pwm_right);
  }

  clearBuffer();
}

void loop() 
{
  char input;
  
  while (Serial.available()) {
    input = char(Serial.read());
    //Serial.print(F("Received byte "));
    //Serial.println(input);

    *buff_ptr++ = input;

    if (input == EOL) {
      //Serial.print(F("Processing Serial! Buffer is: "));
      //Serial.println(input_buff);
      Serial.println(F("Processing!"));
      buff_ptr = input_buff;
      processSerial();
    }
  }
}
