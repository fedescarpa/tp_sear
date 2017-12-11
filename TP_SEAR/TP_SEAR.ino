#include <SPI.h>
#include <Keypad.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>


// ==========================
// == MOTOR CONFIG
// ==========================

int const MOTOR_POLES = 4;
int const MOTOR_STEPS = 4;

int const STEP [ MOTOR_STEPS ][ MOTOR_POLES ] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

int const GEAR_RATIO = 64;
int const SHAFT_STEPS = 8;

int const SHAFT_MOTOR_STEPS_RATIO = GEAR_RATIO * MOTOR_STEPS;
int const MOTOR_TURN_DELAY = 1;

boolean turnRight, turnLeft;
int shaftPositionX = 0;
int motorStepX = 0;

const int m1 = 53;
const int m2 = 51;
const int m3 = 49;
const int m4 = 47;

boolean turnUp, turnDown;
int shaftPositionY = 0;
int motorStepY = 0;

const int w1 = 23;
const int w2 = 25;
const int w3 = 27;
const int w4 = 29;


// ==========================
// == KEYPAD CONFIG
// ==========================

const int ROWS = 4;
const int COLS = 3;

const char keys[ROWS][COLS] = {
  {'*', '0', '#'},
  {'7', '8', '9'},
  {'4', '5', '6'},
  {'1', '2', '3'},
};

byte colPins[COLS] = {40, 42, 44};
byte rowPins[ROWS] = {38, 36, 34, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char KEY_UP    = '8';
const char KEY_DOWN  = '2';
const char KEY_LEFT  = '6';
const char KEY_RIGHT = '4';


// ==========================
// == LCD CONFIG
// ==========================

const int D4 = A13;
const int D5 = A12;
const int D6 = A11;
const int D7 = A10;

const int RS = A9;
const int EN = A8;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


// ==========================
// == ETHERNET CONFIG
// ==========================

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:

byte SERVER[] = {192, 168, 3, 139};

IPAddress server(SERVER[0], SERVER[1], SERVER[2], SERVER[3]);    // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 3, 108);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

String response = "";
boolean nextIsResponse = false;

const int PORT = 3005;

String RESPONSE_UP    = "@up";
String RESPONSE_DOWN  = "@down";
String RESPONSE_LEFT  = "@left";
String RESPONSE_RIGHT = "@right";


// ==========================
// == GLOBAL VARS
// ==========================

long time = 0;

const int TEST_MODE = 1;
const int MAINT_MODE = 2;
const int NORMAL_MODE = 4;

int MODE = 0;


// ==========================
// == SETUP
// ==========================

void setup() {
  setupSerial();
  setupLCD();
  setupMotor();
  setupEthernet();
}

void setupSerial() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("...Starting...");
}

void setupMotor() {
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(w1, OUTPUT);
  pinMode(w2, OUTPUT);
  pinMode(w3, OUTPUT);
  pinMode(w4, OUTPUT);
  Serial.println("Motor Configured");
}

void setupLCD() {
  Serial.println("Setup LCD");
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Hello Moto!");
  Serial.println("LCD Started");
}

void setupEthernet() {
  Serial.println("Setup Ethernet");
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  delay(1000);
  Serial.println("Ethernet started");
}


// ==========================
// == LOOP
// ==========================

void loop() {

  if (MODE == 0) {
    MODE = print_set_mode();
  }

  if (MODE == TEST_MODE) test_mode();
  if (MODE == MAINT_MODE) maint_mode();
  if (MODE == NORMAL_MODE) normal_mode();

}

void test_mode() {
}

void maint_mode() {
  char key = keypad.getKey();
  move_from_command(key);
}

void normal_mode() {
  make_request();
}

String get_str_code_from(char key) {
  if (key == KEY_UP)    return RESPONSE_UP;
  if (key == KEY_DOWN)  return RESPONSE_DOWN;
  if (key == KEY_LEFT)  return RESPONSE_LEFT;
  if (key == KEY_RIGHT) return RESPONSE_RIGHT;
}

char get_char_code_from(String response) {
  if (response == RESPONSE_UP)    return KEY_UP;
  if (response == RESPONSE_DOWN)  return KEY_DOWN;
  if (response == RESPONSE_LEFT)  return KEY_LEFT;
  if (response == RESPONSE_RIGHT) return KEY_RIGHT;
}

void print_in_lcd(char key) {

  lcd.setCursor(8, 1);
  lcd.print(" X:");
  lcd.print(shaftPositionX);
  lcd.print(" Y:");
  lcd.print(shaftPositionY);

  Serial.print(" X:");
  Serial.print(shaftPositionX);
  Serial.print(" Y:");
  Serial.println(shaftPositionY);

}

int print_set_mode() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingrese modo");
  Serial.println("Ingrese modo");
  char key;
  do {
    key = keypad.getKey();
  } while (key != '#' && key != '0' && key != '*');

  Serial.print("Modo Seteado");
  lcd.clear();
  lcd.print("Modo Seteado");
  lcd.setCursor(0, 1);

  if (key == '#') {
    lcd.print("Test");
    return TEST_MODE;
  }
  if (key == '0') {
    lcd.print("Maint");
    return MAINT_MODE;
  }
  if (key == '*') {
    lcd.print("Normal");
    return NORMAL_MODE;
  }
}

void lcd_print_if(char key, char expected, String message) {
  if (key == expected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    Serial.println(message);
  }
}

void move_from_command(char key) {

  turnUp    = key == KEY_UP;
  turnDown  = key == KEY_DOWN;
  turnLeft  = key == KEY_LEFT;
  turnRight = key == KEY_RIGHT;

  if (turnLeft || turnRight || turnDown || turnUp) {
    move_shaft();
    print_in_lcd(key);
  }

}

void move_shaft() {
  int turnNo;
  if ((turnRight && shaftPositionX <= 7) || (shaftPositionX > 0 && turnLeft) || (turnUp && shaftPositionY <= 7) || (shaftPositionY > 0 && turnDown)) {
    for (turnNo = 0; turnNo < SHAFT_MOTOR_STEPS_RATIO; turnNo++) {
      move_motor();
      delay(2);
    }
    if (turnRight) {
      shaftPositionX++;
    } else if (turnLeft) {
      shaftPositionX--;
    } else if (turnUp) {
      shaftPositionY++;
    } else if (turnDown) {
      shaftPositionY--;
    }
  }
}

void move_motor() {
  digitalWrite( m1, STEP[motorStepX][0] );
  digitalWrite( m2, STEP[motorStepX][1] );
  digitalWrite( m3, STEP[motorStepX][2] );
  digitalWrite( m4, STEP[motorStepX][3] );
  digitalWrite( w1, STEP[motorStepY][0] );
  digitalWrite( w2, STEP[motorStepY][1] );
  digitalWrite( w3, STEP[motorStepY][2] );
  digitalWrite( w4, STEP[motorStepY][3] );

  if (turnRight) {
    motorStepX++;
  } else if (turnLeft) {
    motorStepX--;
  } else if (turnUp) {
    motorStepY++;
  } else if (turnDown) {
    motorStepY--;
  }
  motorStepX = ( motorStepX + 4 ) % 4 ;
  motorStepY = ( motorStepY + 4 ) % 4 ;
}


void make_request() {
  if (client.available()) {
    while(client.available()){
      char c = client.read();
      nextIsResponse = nextIsResponse || c == '@';
      if(nextIsResponse){
        response += c;
      }
    }

    char key = get_char_code_from(response);

    move_from_command(key);
    response = "";
    client.stop();
    nextIsResponse = false;
  }

  long elapsed_time = millis() - time;
  if (elapsed_time >= 0 * 1000) {
    time = millis();
    if(!client.connected()){
      if (client.connect(server, PORT)) {
        Serial.println("Request starting");
        client.println("GET /hola HTTP/1.1");
        client.println("Host: " + String(SERVER[0]) + "." + String(SERVER[1]) + "."  + String(SERVER[2]) + "." + String(SERVER[3]));
        client.println("Connection:close");
        client.println();
      } else{
        Serial.println("Error making the request");
      }
    }
  }
}

