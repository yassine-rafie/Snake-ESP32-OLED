#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define JOY_X 34
#define JOY_Y 35
#define JOY_SW 25
#define CELL_SIZE 4
#define COLS 32
#define ROWS 14
#define START_LENGTH 4

int snakeX[COLS * ROWS], snakeY[COLS * ROWS];
int snakeLength, foodX, foodY;

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction direction, nextDirection;

bool gameStarted = false, gameOver = false;
int score = 0, gameSpeed = 150;
unsigned long lastMove = 0;
bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;

void createFood() {
  bool validPosition = false;
  while (!validPosition) {
    foodX = random(0, COLS);
    foodY = random(0, ROWS);
    validPosition = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) { validPosition = false; break; }
    }
  }
}

void startGame() {
  snakeLength = START_LENGTH;
  snakeX[0]=16; snakeY[0]=7;
  snakeX[1]=15; snakeY[1]=7;
  snakeX[2]=14; snakeY[2]=7;
  snakeX[3]=13; snakeY[3]=7;
  direction = RIGHT; nextDirection = RIGHT;
  score = 0; gameSpeed = 150;
  gameOver = false; gameStarted = true;
  createFood(); lastMove = millis();
}

void readJoystick() {
  int x = analogRead(JOY_X), y = analogRead(JOY_Y);
  if (x < 1200) { if (direction != RIGHT) nextDirection = LEFT; }
  else if (x > 2800) { if (direction != LEFT) nextDirection = RIGHT; }
  else if (y < 1200) { if (direction != DOWN) nextDirection = UP; }
  else if (y > 2800) { if (direction != UP) nextDirection = DOWN; }
}

void readButton() {
  bool buttonState = digitalRead(JOY_SW);
  if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long now = millis();
    if (now - lastButtonTime > 250) {
      lastButtonTime = now;
      if (!gameStarted || gameOver) startGame();
    }
  }
  lastButtonState = buttonState;
}

void moveSnake() {
  direction = nextDirection;
  int newX = snakeX[0], newY = snakeY[0];
  if (direction == UP) newY--;
  else if (direction == DOWN) newY++;
  else if (direction == LEFT) newX--;
  else if (direction == RIGHT) newX++;

  if (newX < 0 || newX >= COLS || newY < 0 || newY >= ROWS) { gameOver = true; return; }
  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] == newX && snakeY[i] == newY) { gameOver = true; return; }
  }

  bool ateFood = (newX == foodX && newY == foodY);
  int newLength = snakeLength;
  if (ateFood) {
    newLength++; score++;
    if (gameSpeed > 60) gameSpeed -= 5;
  }
  for (int i = newLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1]; snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] = newX; snakeY[0] = newY; snakeLength = newLength;
  if (ateFood) createFood();
}

void drawGame() {
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0); display.print("SCORE: "); display.print(score);
  display.drawLine(0,7,127,7,SSD1306_WHITE);
  display.fillRect(foodX*CELL_SIZE, 8+foodY*CELL_SIZE, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);
  for (int i=0; i<snakeLength; i++)
    display.fillRect(snakeX[i]*CELL_SIZE, 8+snakeY[i]*CELL_SIZE, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);
  display.display();
}

void drawStartScreen() {
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); display.setCursor(34,10); display.println("SNAKE");
  display.setTextSize(1); display.setCursor(25,38); display.println("Pulsa joystick");
  display.setCursor(31,50); display.println("para empezar");
  display.display();
}

void drawGameOver() {
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); display.setCursor(22,8); display.println("GAME");
  display.setCursor(28,28); display.println("OVER");
  display.setTextSize(1); display.setCursor(43,48); display.print("Score: "); display.print(score);
  display.display();
}

void setup() {
  Serial.begin(115200); pinMode(JOY_SW, INPUT_PULLUP); Wire.begin(21,22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED no encontrada"); while (true) delay(100);
  }
  randomSeed(analogRead(32) + micros()); drawStartScreen();
}

void loop() {
  readJoystick(); readButton();
  if (!gameStarted) { drawStartScreen(); delay(30); return; }
  if (gameOver) { drawGameOver(); delay(30); return; }
  unsigned long now = millis();
  if (now - lastMove >= gameSpeed) {
    lastMove = now; moveSnake(); if (!gameOver) drawGame();
  }
}
