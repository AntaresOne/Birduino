/**
  Birduino minigame.
  Yet another Flappy Bird clone, this time on an Arduino with a button and 0.96" OLED display!

  @author Mattia "AntaresOne" D'Alleva <antaresone@antaresone.eu>
  @version 0.99 7/12/19

  Wiring diagram.
  
                    Arduino Nano
                     (Example)
                   | TX    Vin |--- Power supply (or use embedded USB port)
                   | RX    GND |--- GND (OLED), Pushbutton NC 4.7kΩ Pullup resistor
                   | RST   RST |
                   | GND   5V  |--- 5V (OLED), Pushbutton NC
                   | D2    A7  |
                   | D3    A6  |
                   | D4    A5  |--- SCL (OLED)
                   | D5    A4  |--- SDA (OLED)
                   | D6    A3  |
  Pushbutton NO ---| D7    A2  |
                   | D8    A1  |
                   | D9    A0  |
                   | D10   REF |
                   | D11   3V3 |
                   | D12   D13 |

*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// #define DEBUGGABLE

#define I2C_ADDR 0x3C // 0x3C for the 128x64p OLED.
#define OLED_RESET 4

#if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define PLAYER_SIZE_X 17  // Player bitmap's X size
#define PLAYER_SIZE_Y 12  // Player bitmap's Y size
#define PIPE_SIZE_X 26    // Pipe obstacle type bitmap's X size
#define PIPE_SIZE_Y 60    // Pipe obstacle type bitmap's Y size

Adafruit_SSD1306 display(OLED_RESET);

#pragma region Pin configuration
byte controllerPin = 7;   // Controller PIN. Defaults to DIGITAL 7.
byte ledPin = 13;         // Status LED PIN. Defaults to LED_BUILTIN (13).
#pragma endregion

#pragma region Game configuration
int axisOffset = 4;       // Axis offset to define a safe screen area.

int gameState = 0;        // 0 = Home, 1 = Game, 2 = Game over
int playerScore = 0;      // Not yet implemented!

int playerPositionX = 10;                           // Fixed player position on X axis.
int playerPositionY = (64 / 2);                     // Initial player position on Y axis. (display.height() / 2)
int playerSpeedYRising = 7;                         // Player rising speed on Y axis.
int playerSpeedYFalling = (playerSpeedYRising * 2); // Player falling speed on Y axis. (rising speed * 2)

int obstacleInitialPositionX = (128 - axisOffset);  // Initial obstacle position on X axis. (display.width() - axisOffset)
int obstaclePositionX = obstacleInitialPositionX;   // Obstacle position on X axis. Defaults to initial position.
int obstacleSpeedX = 4;                             // Obstacle moving speed on X axis.

int displayHeight = -1;                             // Dinamically updated by display.height() in setup callback.
#pragma endregion

// Player's bitmap image.
static const unsigned char PROGMEM player_bmp[] =
{
  0x1, 0xf0, 0x0, 0xc, 0x8, 0x0, 0x10, 0x84, 0x0,
  0x5a, 0x8a, 0x0, 0x84, 0x88, 0x0, 0x82, 0x42, 0x0,
  0x82, 0x2f, 0x0, 0x45, 0x50, 0x80, 0x3a, 0xbf, 0x0,
  0x29, 0x55, 0x0, 0x1d, 0x5a, 0x0, 0x3, 0x40, 0x0
};

// Pipe obstacle type's bitmap image.
static const unsigned char PROGMEM pipe_bmp[] =
{
  0x31, 0x14, 0xd3, 0x0, 0x24, 0x40, 0x1d, 0x0, 0x31,
  0x14, 0xa7, 0x0, 0x24, 0xa1, 0x15, 0x0, 0x30, 0x28,
  0x97, 0x0, 0xff, 0xff, 0xff, 0xc0, 0xdf, 0xbe, 0xdb,
  0xc0, 0x80, 0x0, 0x4, 0x40, 0xa9, 0x25, 0x53, 0xc0,
  0x84, 0x90, 0xc, 0xc0, 0xd2, 0x4b, 0x45, 0xc0, 0x84,
  0x0, 0x95, 0x40, 0xd1, 0x48, 0x45, 0xc0, 0x94, 0x24,
  0x14, 0xc0, 0x81, 0x42, 0xa1, 0x40, 0xff, 0xff, 0xff,
  0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0xff, 0xff, 0xff, 0xc0, 0xa0, 0x20, 0x44, 0xc0, 0x95,
  0x15, 0x25, 0xc0, 0xc4, 0x84, 0x14, 0xc0, 0x92, 0x52,
  0x45, 0x40, 0xc0, 0x11, 0x25, 0xc0, 0xaa, 0x88, 0x95,
  0x40, 0x89, 0x22, 0x5, 0xc0, 0xa4, 0x44, 0xa4, 0xc0,
  0xb7, 0x7b, 0x7f, 0x40, 0xff, 0xff, 0xff, 0xc0, 0x20,
  0x0, 0x3, 0x0, 0x35, 0x55, 0x57, 0x0, 0x21, 0x4,
  0x53, 0x0, 0x28, 0xa2, 0x35, 0x0
};

void setup() {
  pinMode(controllerPin, INPUT_PULLUP);

#ifdef DEBUGGABLE
  Serial.begin(9600);
#endif
  
  display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR);
  display.clearDisplay();
  displayHeight = display.height();
  
  digitalWrite(ledPin, LOW);

  printMainMenu();
}

void loop() {
  doGameLogic();
}

/*
 * Returns "true" if controller button is pressed.
 */
bool isButtonPressed() {
  return (digitalRead(controllerPin) == HIGH);
}

/* 
 * Checks collisions between objects.
 */
void checkCollisions() {
#ifdef DEBUGGABLE  
  Serial.println("PlayerX: ");
  Serial.print(playerPositionX);
  Serial.println("PlayerY: ");
  Serial.print(playerPositionY);
  Serial.println();
  Serial.println("OstacoloX: ");
  Serial.print(obstaclePositionX);
#endif

  if (obstaclePositionX <= PLAYER_SIZE_X) {
    if (playerPositionY < (PIPE_SIZE_Y - PLAYER_SIZE_Y + axisOffset)) {
#ifdef DEBUGGABLE
        Serial.println("Player hit the top obstacle.");
#endif
        gameState = 2;
    } else if ((playerPositionY > (displayHeight / 2)) && (playerPositionY <= (displayHeight - axisOffset))) {
#ifdef DEBUGGABLE
        Serial.println("Player hit the bottom obstacle.");
#endif
        gameState = 2;
    }
  }

  if (playerPositionY >= (displayHeight - axisOffset)) {
#ifdef DEBUGGABLE
      Serial.println("Player hit the ground.");
#endif
      gameState = 2;
  }
}

/*
 * Runs the game loop.
 */
void doGameLogic() {
#ifdef DEBUGGABLE
  digitalWrite(ledPin, HIGH);
#endif

  if (gameState == 0) {
    if (isButtonPressed()) {
      gameState = 1;
    }
  } else if (gameState == 1) {
    if (isButtonPressed() && (playerPositionY > axisOffset)) {
      // Player rising.
      playerPositionY -= playerSpeedYRising;
    } else {
      // Player falling.
      playerPositionY += playerSpeedYFalling;
    }

    drawObjects();
    checkCollisions();

    // Move obstacle to the left.
    obstaclePositionX -= obstacleSpeedX;

    if ((obstaclePositionX + PIPE_SIZE_X) <= 0) {
      // Obstacle is going off-screen, reset its position.
      obstaclePositionX = obstacleInitialPositionX;
    }
  } else if (gameState == 2) {
    if (isButtonPressed()) {
        resetGame();
    }
    
    printGameOver();
  }
  
#ifdef DEBUGGABLE
  digitalWrite(ledPin, LOW);
#endif  
}

/*
 * Draws player and the obstacle on the OLED display.
 */
void drawObjects() {
    display.clearDisplay();
    
    display.drawBitmap(obstaclePositionX, 0, pipe_bmp, PIPE_SIZE_X, PIPE_SIZE_Y, 1);

    display.drawBitmap(playerPositionX, playerPositionY, player_bmp, PLAYER_SIZE_X, PLAYER_SIZE_Y, 1);

    display.display();
}

/*
 * Draws Game over screen.
 */
void printGameOver() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(6, 18);
  display.println("Game over!");
  display.setTextSize(1);
  display.setCursor(26, 38);
  display.println("Press button");
  display.setCursor(32, 48);
  display.println("to restart");
  display.display();
}

/*
 * Draws main menu screen.
 */
void printMainMenu() {
    display.clearDisplay();
    display.setTextColor(WHITE); 
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("AntaresOne");
    display.setTextSize(1);
    display.setCursor(0, 18);
    display.println("FlappyBirdy");
    display.setCursor(32, 38);
    display.println("Press start");
    display.setCursor(42, 48);
    display.println("to play");
    display.display();
}

/*
 * Resets the game to be played again.
 */
void resetGame() {
  obstaclePositionX = obstacleInitialPositionX;
  playerPositionY = (displayHeight / 2);
  gameState = 1;
}
