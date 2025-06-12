#include <Adafruit_CircuitPlayground.h>
#include <math.h>

//  CONFIGURATION 
const int maxDifficulty = 6;
const int minDifficulty = 1;
const int touchPins[7] = {A1, A2, A3, A4, A5, A6, A7};
const int numTouchPads = 7;

//  STATE TRACKING 
int difficulty = 1;
int roundsToWin;
bool bombs[7];
int currentRound = 0;
bool gameActive = false;
bool inRoulette = false;
String playerColor = "";
unsigned long lastTouchTime = 0;
const unsigned long touchCooldown = 1000; // 1 second cooldown

//  BUTTON STATES 
bool prevLeft = false;
bool prevRight = false;
bool started = false;

//  MIDI Frequencies 
float midi[127];

//  SAD MELODY 
int sadTrombone[4][2] = {
  {55, 300},  
  {52, 300},  
  {50, 300},  
  {43, 800}   
};

//  CELEBRATION MELODY 
int marioWin[7][2] = {
  {76, 100},  
  {76, 100},  
  {76, 100},  
  {72, 75},  
  {76, 75},  
  {79, 200},  
  {67, 200}   
};

//  SETUP 
void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  randomSeed(analogRead(A0));
  generateMIDI();
  showDifficulty();
}

//  MAIN LOOP 
void loop() {
  if (!gameActive && !inRoulette) {
    handleDifficultySelection();
  } else if (gameActive) {
    handleBombGame();
  } else if (inRoulette) {
    handleRoulette();
  }
}

//  DIFFICULTY SETUP 
void handleDifficultySelection() {
  bool left = CircuitPlayground.leftButton();
  bool right = CircuitPlayground.rightButton();

  if (left && !prevLeft) {
    difficulty = max(minDifficulty, difficulty - 1);
    showDifficulty();
  }
  if (right && !prevRight) {
    difficulty = min(maxDifficulty, difficulty + 1);
    showDifficulty();
  }

  // Both buttons to start
  if (left && right && !started) {
    started = true;
    startGame();
  }

  prevLeft = left;
  prevRight = right;
}

void showDifficulty() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < difficulty; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0); // Yellow dots for difficulty
  }
}

void startGame() {
  gameActive = true;
  currentRound = 0;
  roundsToWin = 7 - difficulty;
  randomizeBombs();
  CircuitPlayground.clearPixels();
  Serial.println("Game started!");
}

void randomizeBombs() {
  for (int i = 0; i < numTouchPads; i++) bombs[i] = false;

  int count = 0;
  while (count < difficulty) {
    int index = random(0, numTouchPads);
    if (!bombs[index]) {
      bombs[index] = true;
      count++;
    }
  }
  Serial.print("Bombs: ");
  for (int i = 0; i < numTouchPads; i++) {
    if (bombs[i]) {
      Serial.print(i + 1);
      Serial.print(" ");
    }
  }
  Serial.println();
}

//  BOMB GAME 
void handleBombGame() {
  if (millis() - lastTouchTime < touchCooldown) return;

  for (int i = 0; i < numTouchPads; i++) {
    if (CircuitPlayground.readCap(touchPins[i]) < 50) {
      lastTouchTime = millis();

      if (bombs[i]) {
        Serial.println("BOOM!");
        playSadMelody();
        resetGame();
        return;
      } else {
        currentRound++;
        Serial.print("Safe! Rounds cleared: ");
        Serial.println(currentRound);
        if (currentRound >= roundsToWin) {
          for (int g = 0; g < 10; g++) {
            CircuitPlayground.setPixelColor(g, 0, 255, 0);
          }
          delay(500);
          CircuitPlayground.clearPixels();
          gameActive = false;
          inRoulette = true;
        } else {
          for (int g = 0; g < 10; g++) {
            CircuitPlayground.setPixelColor(g, 0, 255, 0);
          }
          delay(300);
          CircuitPlayground.clearPixels();
          randomizeBombs();
        }
        return;
      }
    }
  }
}

//  ROULETTE 
void handleRoulette() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {
      CircuitPlayground.setPixelColor(i, 255, 0, 0);
    } else {
      CircuitPlayground.setPixelColor(i, 255, 255, 255);
    }
  }

  int lightVal = CircuitPlayground.lightSensor();
  int mapped = map(lightVal, 0, 1023, 0, 255);

  if (mapped < 10) playerColor = "black";
  else if (mapped > 150) playerColor = "red";
  else return;

  delay(500);

  int spins = random(20, 40);
  int finalIndex = 0;
  for (int i = 0; i < spins; i++) {
    CircuitPlayground.clearPixels();
    for (int j = 0; j < 10; j++) {
      if (j % 2 == 0) CircuitPlayground.setPixelColor(j, 255, 0, 0);
      else CircuitPlayground.setPixelColor(j, 255, 255, 255);
    }
    int idx = i % 10;
    finalIndex = idx;
    CircuitPlayground.setPixelColor(idx, 255, 255, 0);
    delay(100);
  }

  bool landedOnRed = (finalIndex % 2 == 0);
  if ((landedOnRed && playerColor == "red") || (!landedOnRed && playerColor == "black")) {
    playCelebration();
  } else {
    playSadMelody();
  }
  resetGame();
}

//  MELODIES 
void playSadMelody() {
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.playTone(midi[sadTrombone[i][0]], sadTrombone[i][1]);
    delay(50);
  }
}
//plays celebration and sets light green after
void playCelebration() {
  for (int i = 0; i < 7; i++) {
    CircuitPlayground.playTone(midi[marioWin[i][0]], marioWin[i][1]);
    delay(50);
  }
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0);
  }
  delay(500);
  CircuitPlayground.clearPixels();
}
//reset game code
void resetGame() {
  CircuitPlayground.clearPixels();
  gameActive = false;
  inRoulette = false;
  started = false;
  showDifficulty();
}
//converts the sound levels
void generateMIDI() {
  for (int x = 0; x < 127; ++x) {
    midi[x] = (440.0 / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
  }
}
