#include <avr/wdt.h>

const int dataPin = 2;
const int clockPin = 3;
uint16_t ledState = 0;

int led0_1 = 0;
int led0_2 = 1;
int led0_3 = 2;

int led1 = 3;
int led2 = 4;
int led3 = 5;
int led4 = 6;
int led5 = 7;
int led6 = 8;
int led7 = 9;
int led8 = 10;
int led9 = 11;
int led10 = 12;
int led11 = 13;
int led12 = 14;
int led13 = 15;

int leds[16] = { led0_1, led0_2, led0_3, led1, led2, led3, led4, led5, led6, led7, led8, led9, led10, led11, led12, led13 };

int inputBtn = 50;
int buzzer = 51;

int gamemode = 0;
int state = 0;
unsigned long start = 0;

int listen = 0;
unsigned long execStart = 0;

int letterParts[] = { -1, -1, -1, -1, -1, -1 };
int letterPointer = 0;

String letters[39] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "+", "-" };

int morseCodes[40][6] = {
  { 0, 1, -1, -1, -1, -1 },   // A
  { 1, 0, 0, 0, -1, -1 },     // B
  { 1, 0, 1, 0, -1, -1 },     // C
  { 1, 0, 0, -1, -1, -1 },    // D
  { 0, -1, -1, -1, -1, -1 },  // E
  { 0, 0, 1, 0, -1, -1 },     // F
  { 1, 1, 0, -1, -1, -1 },    // G
  { 0, 0, 0, 0, -1, -1 },     // H
  { 0, 0, -1, -1, -1, -1 },   // I
  { 0, 1, 1, 1, -1, -1 },     // J
  { 1, 0, 1, -1, -1, -1 },    // K
  { 0, 1, 0, 0, -1, -1 },     // L
  { 1, 1, -1, -1, -1, -1 },   // M
  { 1, 0, -1, -1, -1, -1 },   // N
  { 1, 1, 1, -1, -1, -1 },    // O
  { 0, 1, 1, 0, -1, -1 },     // P
  { 1, 1, 0, 1, -1, -1 },     // Q
  { 0, 1, 0, -1, -1, -1 },    // R
  { 0, 0, 0, -1, -1, -1 },    // S
  { 1, -1, -1, -1, -1, -1 },  // T
  { 0, 0, 1, -1, -1, -1 },    // U
  { 0, 0, 0, 1, -1, -1 },     // V
  { 0, 1, 1, -1, -1, -1 },    // W
  { 1, 0, 0, 1, -1, -1 },     // X
  { 1, 0, 1, 1, -1, -1 },     // Y
  { 1, 1, 0, 0, -1, -1 },     // Z
  { 1, 1, 1, 1, 1, -1 },      // 0
  { 0, 1, 1, 1, 1, -1 },      // 1
  { 0, 0, 1, 1, 1, -1 },      // 2
  { 0, 0, 0, 1, 1, -1 },      // 3
  { 0, 0, 0, 0, 1, -1 },      // 4
  { 0, 0, 0, 0, 0, -1 },      // 5
  { 1, 0, 0, 0, 0, -1 },      // 6
  { 1, 1, 0, 0, 0, -1 },      // 7
  { 1, 1, 1, 0, 0, -1 },      // 8
  { 1, 1, 1, 1, 0, -1 },      // 9
  { 0, 1, 0, 1, 0, -1 },      // +
  { 1, 0, 0, 0, 0, 1 },       // -
  { 1, 1, 1, 1, 1, 1 },       // END
};

String predefined[26] = { "ALFA", "BRAVO", "CHARLIE", "DELTA", "ECO", "FOXTROT", "GOLF", "HOTEL", "INDIA", "JULIET", "KILO", "LIMA", "MIKE", "NOVEMBER", "OSCAR", "PAPA", "QUEBEC", "ROMEO", "SIERRA", "TANGO", "UNIFORM", "VICTOR", "WHISKEY", "XRAY", "YANKEE", "ZULU" };

String selectedWord = "";
String playerAWord = "";
String playerBWord = "";
String globalWord = "";

int playerAScore = 0;
int playerBScore = 0;

void updateLeds() {
  shiftOut(dataPin, clockPin, MSBFIRST, highByte(ledState));
  shiftOut(dataPin, clockPin, MSBFIRST, lowByte(ledState));
}

void turnOn(int ledIndex) {
  bitSet(ledState, ledIndex);
  updateLeds();
}

void turnOff(int ledIndex) {
  bitClear(ledState, ledIndex);
  updateLeds();
}

void clearDisplay() {
  ledState = 0;
  updateLeds();
}

void preCheck() {
  for (int x = 0; x <= 15; x++) {
    turnOn(x);
    delay(10);
  }
  delay(200);
  for (int x = 0; x <= 15; x++) {
    turnOff(x);
    delay(10);
  }
  delay(100);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  for (int x = 0; x <= 15; x++) {
    turnOn(x);
  }

  delay(200);
  digitalWrite(buzzer, LOW);
  clearDisplay();
}

void reset() {
  wdt_enable(WDTO_15MS);
  while (1) {}
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  preCheck();
  Serial.begin(9600);
  pinMode(inputBtn, INPUT_PULLUP);
  randomSeed(analogRead(0));

  wdt_disable();
}

void displayLetter(String letter, int sound = 0) {
  letter.toUpperCase();

  String row1 = "ABCDEFGHIJKLM";
  String row2 = "NOPQRSTUVWXYZ";
  String row3 = "0123456789+-";

  if (letter == "") {
    turnOn(led0_3);
    turnOn(led13);
  } else {
    if (row1.indexOf(letter) >= 0) {
      turnOn(led0_3);
      if (letter == "A") { turnOn(led1); }
      if (letter == "B") { turnOn(led2); }
      if (letter == "C") { turnOn(led3); }
      if (letter == "D") { turnOn(led4); }
      if (letter == "E") { turnOn(led5); }
      if (letter == "F") { turnOn(led6); }
      if (letter == "G") { turnOn(led7); }
      if (letter == "H") { turnOn(led8); }
      if (letter == "I") { turnOn(led9); }
      if (letter == "J") { turnOn(led10); }
      if (letter == "K") { turnOn(led11); }
      if (letter == "L") { turnOn(led12); }
      if (letter == "M") { turnOn(led13); }
    } else if (row2.indexOf(letter) >= 0) {
      turnOn(led0_2);
      if (letter == "N") { turnOn(led1); }
      if (letter == "O") { turnOn(led2); }
      if (letter == "P") { turnOn(led3); }
      if (letter == "Q") { turnOn(led4); }
      if (letter == "R") { turnOn(led5); }
      if (letter == "S") { turnOn(led6); }
      if (letter == "T") { turnOn(led7); }
      if (letter == "U") { turnOn(led8); }
      if (letter == "V") { turnOn(led9); }
      if (letter == "W") { turnOn(led10); }
      if (letter == "X") { turnOn(led11); }
      if (letter == "Y") { turnOn(led12); }
      if (letter == "Z") { turnOn(led13); }

    } else if (row3.indexOf(letter) >= 0) {
      turnOn(led0_1);
      if (letter == "0") { turnOn(led1); }
      if (letter == "1") { turnOn(led2); }
      if (letter == "2") { turnOn(led3); }
      if (letter == "3") { turnOn(led4); }
      if (letter == "4") { turnOn(led5); }
      if (letter == "5") { turnOn(led6); }
      if (letter == "6") { turnOn(led7); }
      if (letter == "7") { turnOn(led8); }
      if (letter == "8") { turnOn(led9); }
      if (letter == "9") { turnOn(led10); }
      if (letter == "+") { turnOn(led11); }
      if (letter == "-") { turnOn(led12); }
    }
  }

  if (sound == 1) {
    int soundInd = 0;
    for (int x = 0; x < 39; x++) {
      if (letters[x] == letter) {
        soundInd = x;
        break;
      }
    }

    for (int i = 0; i < 6; i++) {
      if (morseCodes[soundInd][i] == 0) {
        digitalWrite(buzzer, HIGH);
        delay(90);
        digitalWrite(buzzer, LOW);
        delay(90);
      } else if (morseCodes[soundInd][i] == 1) {
        digitalWrite(buzzer, HIGH);
        delay(400);
        digitalWrite(buzzer, LOW);
        delay(400);
      }
    }
    delay(500);
  }
}

void displayWord(String word) {
  for (int i = 0; i < word.length(); i++) {
    String letter = String(word[i]);
    clearDisplay();
    displayLetter(letter, 1);
    delay(1500);
  }
  clearDisplay();
}

int morseIndex(int code[6]) {
  for (int i = 0; i < 40; i++) {
    bool match = true;
    for (int j = 0; j < 6; j++) {
      if (morseCodes[i][j] != code[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return i;
    }
  }
  return -1;
}

void saveLetter() {
  letterPointer = 0;
  execStart = 0;
  ledState = 0;
  clearDisplay();
  int ind = morseIndex(letterParts);

  String resultLetter = "";
  if (ind >= 0) {
    resultLetter = letters[ind];
    displayLetter(resultLetter);
    globalWord += resultLetter;
  } else {
    clearDisplay();
  }

  if (ind == 39) {
    if(gamemode == 24){
      playerBWord = globalWord;
      gamemode = 25;
    }
    else if (gamemode == 28){
      playerAWord = globalWord;
      gamemode = 29;
    }
    globalWord = "";
    clearDisplay();
  }

  Serial.print("> ");
  Serial.println(globalWord);
  for (int x = 0; x <= 5; x++) {
    letterParts[x] = -1;
  }
}

void typeWrite(String word, int speed) {
  for (int x = 0; x < word.length(); x++) {
    digitalWrite(buzzer, HIGH);
    Serial.print(word[x]);
    delay(speed);
    digitalWrite(buzzer, LOW);
    delay(speed);
  }
  Serial.println("");
}

String input() {
  Serial.print("> ");
  String result = "";
  while (result == "") {
    if (Serial.available() > 0) {
      result = Serial.readStringUntil('\n');
    }
  }
  Serial.println(result);
  return result;
}

void loop() {
  if (gamemode == 0) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Serial.println("================================");
    typeWrite("--- BIENVENIDO A StrangerTEC ---", 50);
    Serial.println("================================");
    delay(1500);
    Serial.println("");
    gamemode = 1;
  }

  if (gamemode == 1) {
    typeWrite("Por favor, selecciona un modo de juego escribiendo su número.", 25);
    Serial.println("1. Modo transmisión simple.");
    Serial.println("2. Modo transmisión y escucha.");

    int option = input().toInt();
    if (option == 1) {
      gamemode = 10;
    } else if (option == 2) {
      gamemode = 20;
    } else {
      Serial.println("Error: Esa no es una opción válida.");
      gamemode = 1;
      delay(1000);
    }
    Serial.println("");
  }

  if (gamemode == 20) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    typeWrite("--- Modo transmisión y escucha ---", 25);
    delay(1000);
    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();
    if (option == "SI" || option == "Si" || option == "si") {
      typeWrite("Instrucciones:", 15);
      delay(500);
      typeWrite("1. Se escogerá una palabra al azar.", 20);
      delay(250);
      typeWrite("2. Se mostrará la palabra en el panel.", 20);
      delay(250);
      typeWrite("3. El jugador A debe escribir la palabra en la consola.", 20);
      delay(300);
      typeWrite("4. El jugador B ahora escribe, pero con el botón.", 20);
      delay(300);
      typeWrite("5. Se mostrará el puntaje de acuerdo a los aciertos por letra.", 20);
      delay(1000);
      Serial.println("");
      Serial.println("Escribe cualquier cosa cuando estés listo.");
      input();
    }
    Serial.println();
    gamemode = 21;
  }

  if (gamemode == 21) {
    selectedWord = predefined[random(0, 26 + 1)];
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Serial.println("====================");
    typeWrite("--- Primer ronda ---", 40);
    Serial.println("====================");
    Serial.println("");
    delay(1000);
    typeWrite("Se ha escogido una palabra, la cual será mostrada a continuación...", 25);
    delay(1500);
    displayWord(selectedWord);
    delay(1000);

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Serial.println("===========================");
    typeWrite("--- Turno del jugador A ---", 40);
    Serial.println("===========================");
    delay(1000);

    gamemode = 22;
  }

  if (gamemode == 22) {
    typeWrite("Escribe aquí la palabra mostrada.", 20);
    playerAWord = input();
    playerAWord.toUpperCase();
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    gamemode = 23;
  }

  if (gamemode == 23) {
    Serial.println("===========================");
    typeWrite("--- Turno del jugador B ---", 40);
    Serial.println("===========================");
    delay(1000);

    typeWrite("Escribe la palabra en código morse, usando el botón.", 30);

    delay(300);
    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();
    if (option == "SI" || option == "Si" || option == "si") {
      typeWrite("Instrucciones:", 15);
      delay(250);
      typeWrite("Para escribir letras, tienes que hacer pulsaciones cortas o largas.", 30);
      delay(300);
      typeWrite("Por cada letra, debes enviar una serie de pulsos y esperar a que se encienda esa letra.", 30);
      delay(300);
      typeWrite("Las letras y palabras que vayas escribiendo se mostrarán aquí.", 30);
      delay(1000);
      Serial.println("");
      Serial.println("Escribe cualquier cosa cuando estés listo.");
      input();
    }
    Serial.println();
    delay(1000);
    typeWrite("Recuerda: cuando hayas terminado, envía 6 puntos (.)", 30);
    delay(500);
    typeWrite("Ya puedes comenzar a escribir.", 20);
    Serial.println("");
    gamemode = 24;
  }

  if (gamemode == 25) {
    for (int x = 0; x < selectedWord.length(); x++) {
      if (playerAWord[x] == selectedWord[x]) {
        playerAScore++;
      }
      if (playerBWord[x] == selectedWord[x]) {
        playerBScore++;
      }
    }
    playerAWord="";
    playerBWord="";

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Serial.println("====================");
    typeWrite("--- Segunda ronda ---", 40);
    Serial.println("====================");
    Serial.println("");
    delay(1000);
    typeWrite("Se utilizará la misma palabra ya escogida...", 25);
    delay(1500);
    gamemode = 26;
  }

  if (gamemode == 26) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Serial.println("===========================");
    typeWrite("--- Turno del jugador B ---", 40);
    Serial.println("===========================");
    delay(1000);
    typeWrite("Escribe aquí la palabra mostrada.", 20);
    playerBWord = input();
    playerBWord.toUpperCase();
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    gamemode = 27;
  }

  if (gamemode == 27) {
    Serial.println("===========================");
    typeWrite("--- Turno del jugador A ---", 40);
    Serial.println("===========================");
    delay(1000);

    typeWrite("Escribe la palabra en código morse, usando el botón.", 30);

    delay(300);
    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();
    if (option == "SI" || option == "Si" || option == "si") {
      typeWrite("Instrucciones:", 15);
      delay(250);
      typeWrite("Para escribir letras, tienes que hacer pulsaciones cortas o largas.", 30);
      delay(300);
      typeWrite("Por cada letra, debes enviar una serie de pulsos y esperar a que se encienda esa letra.", 30);
      delay(300);
      typeWrite("Las letras y palabras que vayas escribiendo se mostrarán aquí.", 30);
      delay(1000);
      Serial.println("");
      Serial.println("Escribe cualquier cosa cuando estés listo.");
      input();
    }
    Serial.println();
    delay(1000);
    typeWrite("Recuerda: cuando hayas terminado, envía 6 puntos (.)", 30);
    delay(500);
    typeWrite("Ya puedes comenzar a escribir.", 20);
    Serial.println("");
    gamemode = 28;
  }

  if (gamemode == 29) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    delay(1000);
    Serial.println("===================");
    typeWrite("--- Resultados ---", 100);
    Serial.println("===================");
    delay(1500);

    Serial.println("");
    for (int x = 0; x < selectedWord.length(); x++) {
      if (playerAWord[x] == selectedWord[x]) {
        playerAScore++;
      }
      if (playerBWord[x] == selectedWord[x]) {
        playerBScore++;
      }
    }
    playerAWord="";
    playerBWord="";

    delay(750);

    typeWrite("Palabra original: " + selectedWord, 75);
    delay(750);

    Serial.println("");
    String p1Txt = String(playerAScore);
    String p2Txt = String(playerBScore);

    typeWrite("Puntaje del jugador A: " + p1Txt + " aciertos.", 50);
    delay(500);
    typeWrite("Puntaje del jugador B: " + p2Txt + " aciertos.", 50);
    delay(1000);
    Serial.println("");

    if (playerAScore == playerBScore) {
      typeWrite("Tenemos un empate.", 25);
    } else if (playerAScore > playerBScore) {
      typeWrite("El jugador A gana.", 25);
    } else if (playerBScore > playerAScore) {
      typeWrite("El jugador B gana.", 25);
    }

    delay(1500);
    Serial.println("");
    typeWrite("Escribe cualquier cosa para comenzar otra vez.", 50);
    input();
    reset();
  }

  if (gamemode == 24 || gamemode == 28) {
    bool val = digitalRead(inputBtn) ? 0 : 1;

    if (val == 1) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }

    if (state == 0 && val == 1) {
      clearDisplay();
      start = millis();
      state = 1;

      execStart = millis();
      listen = 1;
    }

    if (state == 1 && val == 0) {
      unsigned long end = millis();
      unsigned long time = end - start;
      state = 0;

      if (time > 65) {
        clearDisplay();

        letterParts[letterPointer] = time < 200 ? 0 : 1;
        letterPointer += 1;

        if (letterPointer == 6) {
          saveLetter();
          listen = 0;
          return;
        }

        execStart = millis();

        Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        Serial.print("> ");
        Serial.print(letterParts[0] == 0 ? "." : letterParts[0] == 1 ? "-"
                                                                     : "");
        Serial.print(" ");
        Serial.print(letterParts[1] == 0 ? "." : letterParts[1] == 1 ? "-"
                                                                     : "");
        Serial.print(" ");
        Serial.print(letterParts[2] == 0 ? "." : letterParts[2] == 1 ? "-"
                                                                     : "");
        Serial.print(" ");
        Serial.print(letterParts[3] == 0 ? "." : letterParts[3] == 1 ? "-"
                                                                     : "");
        Serial.print(" ");
        Serial.print(letterParts[4] == 0 ? "." : letterParts[4] == 1 ? "-"
                                                                     : "");
        Serial.print(" ");
        Serial.print(letterParts[5] == 0 ? "." : letterParts[5] == 1 ? "-"
                                                                     : "");
        Serial.println(" ");
      }
    }

    if (listen == 1) {
      uint16_t tempLedState = 0;

      bitSet(tempLedState, led0_1);
      bitSet(tempLedState, led0_2);
      bitSet(tempLedState, led0_3);

      for (int i = 0; i < letterPointer; i++) {
        bitSet(tempLedState, leds[i + 3]);
      }

      if (tempLedState != ledState) {
        ledState = tempLedState;
        updateLeds();
      }

      unsigned long execEnd = millis();
      unsigned long execTime = execEnd - execStart;

      if (execTime > 1000) {
        saveLetter();
        listen = 0;
        return;
      }
    }
  }
}