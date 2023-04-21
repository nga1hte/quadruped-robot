#include <Arduino.h>
#include "MiniKame.h"

//Serial commands mapping for movement
#define FORWARD 'f'
#define LEFT 'l'
#define STAND 's'
#define RIGHT 'r'
#define BACKWARD 'b'
#define GO 'g'
#define RIGHT_FRONT 'c'
#define RIGHT_BACK 'e'
#define LEFT_FRONT 'd'
#define LEFT_BACK 'h'
#define HELLO 'z'
#define PUSH_UP 'p'
#define MOON_WALK 'm'
#define UP_DOWN 'u'
#define DANCE 'd'

#define TIME_INTERVAL 5000
#define LED_PIN 13

//creating a MiniKame object, check minikame.cpp
MiniKame robot;

//variables definition for the robot

bool auto_mode = true;
bool state = true;
char cmd = STAND;
bool random_walk = false;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A7)); // getting seed for random

  robot.init();
  delay(2000);
}

void loop() {
  if (auto_mode) {
    if (random_walk)
      auto_gaits(0);
    else
      auto_gaits(1);
  } else {
    gaits(cmd);
  }
}

//monitors serial events and is run after every loop
void serialEvent() {
  char tmp = -1;
  boolean taken = false; // is steps already taken?

  while (Serial.available()) {
    state = !state; // inverse state
    digitalWrite(LED_PIN, 1 * state); //flashing LED indicating cmd received
    tmp = Serial.read(); //Serial.println(cmd);
    taken = gaits(tmp); // steps taken?

    if (taken) // basically repeat the previous serial command
      cmd = tmp;
  }
}

// types of walks, read cmd from serial

boolean  gaits(char cmd) {
  bool taken = true;
  switch (cmd) {
    case GO:
      robot.walk();
      break;
    case FORWARD:
      robot.walk();
      break;
    case BACKWARD:
      robot.walk(0);
      break;
    case RIGHT:
      robot.turnR(1, 550);
      break;
    case LEFT:
      robot.turnL(1, 550);
      break;
    case RIGHT_FRONT:
      robot.turnR(1, 550);
      robot.walk();
      break;
    case RIGHT_BACK:
      robot.turnR(1, 550);
      robot.walk(0);
      break;
    case LEFT_FRONT:
      robot.turnL(1, 550);
      robot.walk();
      break;
    case LEFT_BACK:
      robot.turnL(1, 550);
      robot.walk(0);
      break;
    case STAND:
      robot.home();
      break;
    case HELLO:
      robot.hello();
      break;
    case PUSH_UP:
      robot.pushUp(1, 3000);
      break;
    case MOON_WALK:
      robot.moonwalkL(1, 5000);
      break;
    default:
      taken = false;
  }
  return taken;
}

// gaits to be taken if auto is enabled and random walks
void auto_gaits(int pattern) {
  char movements[] = {FORWARD, LEFT, STAND, RIGHT, BACKWARD, GO, DANCE, MOON_WALK, PUSH_UP};
  char movements2[] = {FORWARD, LEFT, FORWARD,  RIGHT, BACKWARD, DANCE, MOON_WALK, STAND, PUSH_UP};
  static unsigned long cur_time = 0, prev_time = 0;
  static char cmd = FORWARD, prev_cmd = -1;
  static int c = 0;

  cur_time = millis();
  if (cur_time - prev_time >= TIME_INTERVAL) {
    prev_time += TIME_INTERVAL; // fixed prev_time = cur_time that can result in cummulative errors
    do {
      switch (pattern) {
        case 0: // if random_walk is 0
          c = (int)random(0, sizeof(movements));
          cmd = movements[c];
          break;
        case 1: // if random_walk is 1 
          c = c % sizeof(movements2);
          cmd = movements2[c++];
          break;
        default:
          pattern = 0;
      }
    } while (cmd == prev_cmd); // as long as no new commands is sent from serial
    prev_cmd = cmd; 
  }

  
  switch (cmd) {
    case GO:
      robot.pushUp(2, 5000);
      break;
    case FORWARD:
      robot.walk();
      break;
    case BACKWARD:
      robot.walk(0);
      break;
    case RIGHT:
      robot.turnR(1, 550);
      break;
    case LEFT:
      robot.turnL(1, 550);
      break;
    case DANCE:
      robot.dance(2, 1000);
      break;
    case MOON_WALK:
      robot.moonwalkL(4, 2000);
      break;
    case UP_DOWN:
      robot.upDown(4, 250);
      break;
    case STAND:
      robot.home();
      break;
    case PUSH_UP:
      robot.pushUp(1, 3000);
      break;
    default:
      robot.home();
  }
}
