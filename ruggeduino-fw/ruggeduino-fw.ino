#include <Arduino.h>

// We communicate with the power board at 115200 baud.
#define SERIAL_BAUD 115200

#define FW_VER 1
#define LEFT_A 2
#define LEFT_B 4
#define RIGHT_A 3
#define RIGHT_B 5

volatile double leftDist = 100.0;
volatile double rightDist = 100.0;

// FOR INTERRUPTS
void doEncoderL(){
  if (digitalRead(LEFT_A) == digitalRead(LEFT_B)){
    leftDist += 1.0; // Work out how many ticks per rotation
  } else leftDist -= 1.0;
}
void doEncoderR(){
  if (digitalRead(RIGHT_A) == digitalRead(RIGHT_B)){
    rightDist += 1.0; // Also work out how many ticks per rotation
  } else rightDist -= 1.0;
}


void setup() {
  pinMode(LEFT_A, INPUT);
  pinMode(LEFT_B, INPUT);
  pinMode(RIGHT_A, INPUT);
  pinMode(RIGHT_B, INPUT);
  // If we get something dodgy, might be worth setting the pull-up to true
  /*  // Set pull-up resistors
   *  digitalWrite(LEFT_A, HIGH);
   *  digitalWrite(LEFT_B, HIGH);
   *  digitalWrite(RIGHT_A, HIGH);
   *  digitalWrite(RIGHT_B, HIGH);
   */
  attachInterrupt(0, doEncoderL, CHANGE);
  attachInterrupt(1, doEncoderR, CHANGE);
  
  Serial.begin(SERIAL_BAUD);
}
int read_pin() {
  while (!Serial.available());
  int pin = Serial.read();
  return (int)(pin - 'a');
}
void command_read() {
  int pin = read_pin();
  // Read from the expected pin.
  int level = digitalRead(pin);
  // Send back the result indicator.
  if (level == HIGH) {
    Serial.write('h');
  } else {
    Serial.write('l');
  }
}
void command_analogue_read() {
  int pin = read_pin();
  int value = analogRead(pin);
  Serial.print(value);
}
void command_write(int level) {
  int pin = read_pin();
  digitalWrite(pin, level);
}
void command_mode(int mode) {
  int pin = read_pin();
  pinMode(pin, mode);
}

void read_Left_Motor_Distance(){
  Serial.print(leftDist);
  //leftDist = 0.0;  
}
void read_Right_Motor_Distance(){
  Serial.print(rightDist);
  //rightDist = 0.0;
}

void loop() {
  // Fetch all commands that are in the buffer
  while (Serial.available()) {
    int selected_command = Serial.read();
    // Do something different based on what we got:
    switch (selected_command) {
      case 'b':
        read_Left_Motor_Distance();
        break;
      case 'c':
        read_Right_Motor_Distance();
        break;
      case 'a':
        command_analogue_read();
        break;
      case 'r':
        command_read();
        break;
      case 'l':
        command_write(LOW);
        break;
      case 'h':
        command_write(HIGH);
        break;
      case 'i':
        command_mode(INPUT);
        break;
      case 'o':
        command_mode(OUTPUT);
        break;
      case 'p':
        command_mode(INPUT_PULLUP);
        break;
      case 'v':
        Serial.print("SRcustom:");
        Serial.print(FW_VER);
        break;
      default:
        // A problem here: we do not know how to handle the command!
        // Just ignore this for now.
        break;
    }
    Serial.print("\n");
  }
}
