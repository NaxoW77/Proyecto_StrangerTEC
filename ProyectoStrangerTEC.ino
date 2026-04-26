#include <avr/wdt.h>

// --- Definición de pines y configuración ---

// Leds de fila
int led0_1 = 0;
int led0_2 = 1;
int led0_3 = 2;

// Leds de columna
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

// Pines y variable de control para el circuito integrado
const int dataPin = 2;
const int clockPin = 3;
uint16_t ledState = 0;  // Esta variable se utiliza para indicar el estado y bits al circuito integrado

// Configuración y posiciones de cada led
int leds[16] = { led0_1, led0_2, led0_3, led1, led2, led3, led4, led5, led6, led7, led8, led9, led10, led11, led12, led13 };

// Pines para el botón y el buzzer
int inputBtn = 50;
int buzzer = 51;



// --- Variables globales ---

// Variable para llevar el progreso de cada minijuego
int gamemode = 0;

// Variables de control para las ejecuciones
int listen = 0;
unsigned long execStart = 0;

// Variables de control para las pulsaciones dentro de una ejecución
int state = 0;
unsigned long start = 0;

// Lista de campos para cada pulsación
// Donde:
// → -1 = Nada
// → 0 = Pulso corto (.)
// → 1 = Pulso largo (-)
int letterParts[] = { -1, -1, -1, -1, -1, -1 };

// Cursor o variable de control para recorrer las partes
int letterPointer = 0;



// --- Diccionarios ---

// Lista de letras (Abecedario)
String letters[39] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "+", "-" };

// Lista de códigos morse traducibles a letras, a partir de una lista de partes (llámese letterParts)
// Donde:
// → Cada índice de letras coincide con un código en morse, para comparar rápidamente
// → Mas existe un elemento extra en la lista de morse, que equivale a un [Enter]

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

// Lista de palabras predefinidas para utilizar en el programa
String predefined[26] = { "ALFA", "BRAVO", "CHARLIE", "DELTA", "ECO", "FOXTROT", "GOLF", "HOTEL", "INDIA", "JULIET", "KILO", "LIMA", "MIKE", "NOVEMBER", "OSCAR", "PAPA", "QUEBEC", "ROMEO", "SIERRA", "TANGO", "UNIFORM", "VICTOR", "WHISKEY", "XRAY", "YANKEE", "ZULU" };



// --- Variables de ejecución

// Variable para guardar la palabra escrita en el primer minijuego
String playerWord = "";

// Variable para guardar la frase globalmente para fácil acceso (Palabra caché)
String globalWord = "";

// Variable para guardar la frase seleccionada para el segundo minujuego
String selectedWord = "";

// Variable para guardar las palabras de ambos jugadores en el segundo minijuego
String playerAWord = "";
String playerBWord = "";

// Variables para guardar los puntajes de ambos jugadores en el segundo minijuego
int playerAScore = 0;
int playerBScore = 0;



/// --- Funciones auxiliares ---
// Estas son funciones pequeñas que se utilizan para realizar una serie de acciones repetitivas rápidamente

// Función para mostrar el estado actual de las leds dentro del
void updateLeds() {
  // Se optó por utilizar shiftOut() para mover los bits de forma automática y segura
  // Referencia: https://www.instructables.com/The-74HC164-Shift-Register-and-your-Arduino/
  shiftOut(dataPin, clockPin, MSBFIRST, highByte(ledState));
  shiftOut(dataPin, clockPin, MSBFIRST, lowByte(ledState));
}

// Función para encender una led específica
void turnOn(int ledIndex) {
  bitSet(ledState, ledIndex);  // Se coloca el bit que corresponde a esa led
  updateLeds();                // Se muestra el estado actual
}

// Función para apagar una led específica
void turnOff(int ledIndex) {
  bitClear(ledState, ledIndex);  // Se apaga el bit que corresponde a esa led
  updateLeds();                  // Se muestra el estado actual
}

// Función para apagar todas las leds rápidamente
void clearDisplay() {
  ledState = 0;
  updateLeds();
}

// Función auxiliar para comprobar que el circuito integrado, el cableado, las leds y buzzer funcionan adecuadamente
void preCheck() {

  // Se encienden las leds una a una en orden
  for (int x = 0; x <= 15; x++) {
    turnOn(x);
    delay(10);
  }

  delay(200);

  // Se apagan las leds en orden
  for (int x = 0; x <= 15; x++) {
    turnOff(x);
    delay(10);
  }

  delay(100);

  // Se prueba el buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);

  // Una prueba de encendido total rápido de leds
  for (int x = 0; x <= 15; x++) {
    turnOn(x);
  }

  delay(200);

  digitalWrite(buzzer, LOW);

  clearDisplay();
}

// Función auxiliar para reiniciar el arduino rápidamente
// Se utilizó el método de reinicio por watchdog
// Referencia: https://www.instructables.com/The-Arduino-Hang-Guardian-Arduino-Watchdog-Timer-T/
void reset() {
  wdt_enable(WDTO_15MS);
  while (1) {}
}

// Función auxiliar para escribir palabras lentamente en la terminal
void typeWrite(String word, int speed) {
  for (int x = 0; x < word.length(); x++) {  // Recorremos letra por letra

    digitalWrite(buzzer, HIGH);  // Feedback de sonido para acompañar las letras
    Serial.print(word[x]);
    delay(speed);  // Dada una velocidad, esperamos entre letras

    digitalWrite(buzzer, LOW);
    delay(speed);
  }

  Serial.println("");
}

// Función auxiliar para recibir entradas del usuario de forma segura
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



// --- Funciones principales del programa
// Estas son funciones que manejan la lógica principal del código

// Función para mostrar una letra específica en el panel
void displayLetter(String letter, int sound = 0) {

  // Se asegura que la letra esté en mayúsculas para evitar falsas coincidencias
  letter.toUpperCase();

  // Se definen virtualmente las posiciones del tablero
  String row1 = "ABCDEFGHIJKLM";
  String row2 = "NOPQRSTUVWXYZ";
  String row3 = "0123456789+-";

  // Si no se provee una letra, muestra la última posición (Error)
  if (letter == "") {
    turnOn(led0_3);
    turnOn(led13);
  } else {

    if (row1.indexOf(letter) >= 0) {  // Se busca la posición de la letra en base a la primera fila

      // Se enciende el indicador de la primer fila
      turnOn(led0_3);

      // Se enciende la led de la columna correspondiente al índice de la letra en la fila
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

    } else if (row2.indexOf(letter) >= 0) {  // Si no se encontró la letra en la primer fila, se busca su posición en la segunda fila

      // Se enciende el indicador de la segunda fila
      turnOn(led0_2);

      // Se enciende la led de la columna correspondiente al índice de la letra en la fila
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

    } else if (row3.indexOf(letter) >= 0) {  // Si no se encontró la letra en la segunda fila, se busca su posición en la tercer fila

      // Se enciende el indicador de la tercer fila
      turnOn(led0_1);

      // Se enciende la led de la columna correspondiente al índice de la letra en la fila
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

  // Reproductor de sonido
  if (sound == 1) {  // Si en el parámetro se pasa "1", se pide que se reproduzca el sonido

    // Variable para buscar una combinación de morse que equivale a la letra
    int soundInd = 0;

    // Se busca la combinación morse
    for (int x = 0; x < 39; x++) {
      if (letters[x] == letter) {
        soundInd = x;
        break;
      }
    }

    // Por cada morse, se reproduce un sonido dependiendo de su combinación
    for (int i = 0; i < 6; i++) {

      if (morseCodes[soundInd][i] == 0) {  // Si el código es 0 (.), se reproduce un pitido corto
        digitalWrite(buzzer, HIGH);
        delay(90);
        digitalWrite(buzzer, LOW);
        delay(90);

      } else if (morseCodes[soundInd][i] == 1) {  // Si el código es 1 (-), se reproduce un pitido largo
        digitalWrite(buzzer, HIGH);
        delay(400);
        digitalWrite(buzzer, LOW);
        delay(400);
      }
    }

    delay(500);  // Pausa entre caracteres
  }
}

// Función para mostrar cada letra de una palabra, llamando a la función de las letras
void displayWord(String word) {

  // Se recorre la cadena de caracteres
  for (int i = 0; i < word.length(); i++) {

    String letter = String(word[i]);
    clearDisplay();            // Se limpia el panel para evitar sobreescritura
    displayLetter(letter, 1);  // Se muestra la letra correspondiente
    delay(1500);               // Tiempo entre letras
  }

  clearDisplay();  // Se limpia el panel tras la sucesión
}


// Función para guardar una letra en una palabra global
void saveLetter() {

  // Reinicio de variables para evitar problemas
  letterPointer = 0;
  execStart = 0;
  ledState = 0;
  String resultLetter = "";

  clearDisplay();

  int ind = morseIndex(letterParts);  // Buscamos la posición de esta combinación de pulsos, si existe

  if (ind >= 0) {
    resultLetter = letters[ind];  // Guardamos la letra en base a su índice compartido con el índice de la combinación
    displayLetter(resultLetter);  // Mostramos la letra resultante
    globalWord += resultLetter;   // Construimos la palabra final global
  } else {
    clearDisplay();  // Se evita escribir por si el resultado era inválido
  }

  if (ind == 39) {  // Si la letra ingresada es [ENTER]

    if (gamemode == 12) {       // Si estamos en el primer minijuego
      playerWord = globalWord;  // Guardamos la palabra creada temporalmente antes de borrarla
      gamemode = 13;            // Avanzamos de fase en el primer minijuego

    } else if (gamemode == 24) {  // Si estamos en el segundo minijuego, primer ronda
      playerBWord = globalWord;   // Guardamos la palabra creada temporalmente en el jugador B
      gamemode = 25;              // Avanzamos de fase en el segundo minijuego

    } else if (gamemode == 27) {  // Si estamos en el segundo minijuego, segunda ronda
      playerAWord = globalWord;   // Guardamos la palabra creada temporalmente en el jugador A
      gamemode = 28;              // Avanzamos de fase en el segundo minijuego
    }

    globalWord = "";  // Reiniciamos la palabra para futuras rondas

    clearDisplay();  // Apagamos el panel
  }

  Serial.print("> ");
  Serial.println(globalWord);  // Mostramos la palabra ingresada como feedback

  for (int x = 0; x <= 5; x++) {  // Vaciamos todos los espacios de pulsos para futuras letras
    letterParts[x] = -1;          // Asignamos -1, o sea un espacio en blanco
  }
}

// Función para encontrar la posición de una letra en base a su combinación
int morseIndex(int code[6]) {

  for (int i = 0; i < 40; i++) {  // Se analiza cada elemento en la combinación
    bool match = true;
    for (int j = 0; j < 6; j++) {
      if (morseCodes[i][j] != code[j]) {  // Se compara cada elemento con una posible combinación de letras
        match = false;
        break;
      }
    }

    if (match) {  // Si se encontró qué combinación es, se devuelve su índice
      return i;
    }
  }
  return -1;  // Se devuelve "-1" si no se encontró una combinación adecuada
}



// --- Función principal para iniciar el arduino ---

void setup() {

  // Se definen los pines
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(inputBtn, INPUT_PULLUP);

  // Se inicia la comunicación con la computadora
  Serial.begin(9600);

  // Se coloca una "semilla" para que en la función random() no se repitan secuencias
  // Se utilizó el método de leer un pin análogo sin conectar, lo que produce ruido por defecto
  // Referencia: https://docs.arduino.cc/language-reference/en/functions/random-numbers/randomSeed/
  randomSeed(analogRead(A0));

  // Se evita que reset() se active accidentalmente
  wdt_disable();

  // Se realiza la comprobación inicial de los componentes
  preCheck();
}



// --- Función de ejecución de código principal ---

void loop() {

  if (gamemode == 0) {  // Primer cuadro de diálogo - Bienvenida

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("================================");

    typeWrite("--- BIENVENIDO A StrangerTEC ---", 50);  // Utilizamos la escritura letra a letra para dar un efecto

    Serial.println("================================");

    delay(1500);  // Esperamos un momento

    Serial.println("");

    gamemode = 1;  // Cambiamos al menú principal
  }


  if (gamemode == 1) {  // Menú principal

    typeWrite("Selecciona un modo de juego escribiendo su número.", 25);

    Serial.println("1. Modo transmisión simple.");
    Serial.println("2. Modo transmisión y escucha.");

    int option = input().toInt();  // Se llama a input y la respuesta se convierte en entero
    if (option == 1) {             // Si se escribió "1"
      gamemode = 10;               // Cambiamos a minijuego 1 (Ver código más abajo)

    } else if (option == 2) {  // Si se escribió "2"
      gamemode = 20;           // Cambiamos a minijuego 2 (Ver código más abajo)

    } else {                                                  // Si se recibió un valor distinto
      Serial.println("Error: Esa no es una opción válida.");  // Mensaje de error
      gamemode = 1;                                           // Volvemos al mismo modo para realizar la misma pregunta
      delay(1000);
    }
  }

  if (gamemode == 10) {  // Cuadro de diálogo - Introducción al primer minijuego

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");  // Se deja espacio para no dejar la terminal llena

    Serial.println("===============================");

    typeWrite("--- Modo transmisión simple ---", 25);

    Serial.println("===============================");

    delay(1000);

    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();                                                                     // Se llama a input para pedir una opción en texto
    if (option == "SI" || option == "Si" || option == "si" || option == "S" || option == "s") {  // Se analizan las posibles respuestas del usuario, si no, se ignora

      typeWrite("Instrucciones:", 15);
      delay(500);

      typeWrite("1. Puedes escribir palabras en morse y compararlas con una lista predefinida.", 20);
      delay(250);

      typeWrite("2. Para escribir en morse debes pulsar el botón en lapsos cortos (.) o largos (-).", 20);
      delay(250);

      typeWrite("3. Dada una combinación de pulsos, debes esperar, y se traducirá a una letra.", 20);
      delay(300);

      typeWrite("4. Combina letras para formar dicha palabra.", 20);
      delay(300);

      typeWrite("5. Cuando hayas terminado de escribir, envía 6 pulsos cortos (.).", 20);
      delay(300);

      typeWrite("6. Se evaluará lo que hayas escrito y se te indicará tu puntaje.", 20);
      delay(1000);

      Serial.println("");

      Serial.println("Escribe cualquier cosa cuando estés listo.");

      input();  // Función vacía y sin retorno para detener el código temporalmente
    }

    Serial.println("");

    gamemode = 11;  // Cambiamos al inicio del primer minijuego
  }


  if (gamemode == 11) {  // Inicio del primer minijuego
    typeWrite("Recuerda: cuando hayas terminado, envía 6 puntos (.)", 30);
    delay(1000);

    typeWrite("Ya puedes comenzar a escribir.", 20);

    gamemode = 12;  // Cambiamos a un estado donde el escritor de morse está activo
  }


  if (gamemode == 13) {  // Cuadro de diálogo - Resultados primer minijuego
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    delay(1000);

    Serial.println("===================");

    typeWrite("--- Resultados ---", 100);

    Serial.println("===================");

    delay(1000);

    Serial.println("");

    // Definimos nuevas variables para los resultados
    int playerScore = 0;
    String similar = "";

    for (int i = 0; i < 26; i++) {              // Analizamos en toda la lista de palabras predeterminadas
      if (playerWord[0] == predefined[i][0]) {  // Si existe una palabra que comience con la primer letra de la palabra ingresada
        similar = predefined[i];                // Si existe, la guardamos para comparar
      }
    }

    for (int x = 0; x < similar.length(); x++) {  // Recorremos cada letra de la palabra predefinida similar
      if (playerWord[x] == similar[x]) {          // Si la letra actual coincide con la letra actual de la palabra ingresada
        playerScore++;                            // Sumamos un punto
      }
    }

    String pScoreTxt = String(playerScore);  // Pasamos el entero del puntaje a texto para evitar problemas de concatenación
    String pStatTxt = "";                    // Se prepara un mensaje de texto para indicar el veredicto del puntaje

    if (playerScore == 0) {  // Si no se obtuvo ningún punto
      pStatTxt = "No lograste acertar ninguna letra.";

    } else if (playerScore > 0 && playerScore < similar.length()) {  // Si se obtuvo al menos un puntaje pero no todo
      pStatTxt = "Estuviste cerca. Sigue intentándolo.";

    } else if (playerScore == similar.length()) {  // Si se obtuvo todo el puntaje (coincidencia perfecta)
      pStatTxt = "Excelente. Acertaste la palabra.";
    }

    // Se muestran los mensajes de texto con los resultados
    typeWrite("Palabra escrita: " + playerWord, 50);
    delay(750);

    typeWrite("Palabra similar: " + similar, 75);
    delay(750);

    Serial.println("");

    typeWrite("Letras correctas: " + pScoreTxt, 50);
    delay(500);

    Serial.println("");

    typeWrite(pStatTxt, 40);
    delay(2500);

    Serial.println("");

    playerWord = "";  // Se reinicia la palabra temporal para futuras rondas
    playerScore = 0;  // Se reinicia el puntaje

    gamemode = 14;  // Cambiamos a un modo temporal de pregunta
  }


  if (gamemode == 14) {  // Modo temporal de pregunta
    typeWrite("¿Qué deseas hacer ahora?.", 25);
    Serial.println("1. Jugar otra partida.");
    Serial.println("2. Volver al menú principal.");

    int option = input().toInt();  // Se llama a ingresar un valor entero
    if (option == 1) {             // Si se ingresa "1"
      gamemode = 11;               // Repetimos el videjuego, donde no habrá problema pues las variables ya fueron reiniciadas

    } else if (option == 2) {  // Si se ingresa "2"
      reset();                 // Reiniciamos todo para volver al menú principal

    } else {                                                  // Si la opción es inválida
      Serial.println("Error: Esa no es una opción válida.");  // Mensaje de error
      gamemode = 14;                                          // Volvemos al mismo estado para repetir la pregunta
      delay(1000);
    }
  }


  if (gamemode == 20) {  // Cuadro de diálogo - Introducción del segundo videojuego

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("===================================");

    typeWrite("     --- Modo transmisión y escucha ---", 25);

    Serial.println("===================================");

    delay(1000);

    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();  // Se llama a input para pedir una opción en texto

    if (option == "SI" || option == "Si" || option == "si" || option == "S" || option == "s") {  // Se analizan las posibles respuestas del usuario, si no, se ignora

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

      input();  // Función vacía y sin retorno para detener la ejecución temporalmente
    }

    gamemode = 21;  // Cambiamos al inicio del segundo minijuego
  }


  if (gamemode == 21) {  // Inicio del primer videojuego

    // Se escoge una palabra aleatoria de entre las opciones disponibles
    selectedWord = predefined[random(0, 26 + 1)];

    // Cuadro de diálogo de la primer ronda del segundo minijuego
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("====================");
    typeWrite("--- Primer ronda ---", 40);
    Serial.println("====================");

    delay(1000);

    Serial.println("");

    typeWrite("Se ha escogido una palabra, la cual será mostrada a continuación...", 25);
    delay(1500);

    displayWord(selectedWord);  // Se muestra la palabra en la maqueta
    delay(1000);

    gamemode = 22;  // Cambiamos al turno del jugador A, primer ronda
  }


  if (gamemode == 22) {  // Entrada de texto del jugador A, primer ronda

    // Cuadro de diálogo del turno del jugador A
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("===========================");
    typeWrite("--- Turno del jugador A ---", 40);
    Serial.println("===========================");

    delay(1000);

    Serial.println("");

    typeWrite("Escribe aquí la palabra mostrada.", 20);

    playerAWord = input();      // Se llama a ingresar un texto
    playerAWord.toUpperCase();  // Se le da formato a la palabra para evitar problemas

    gamemode = 23;  // Cambiamos al turno del jugador B, primer ronda
  }


  if (gamemode == 23) {  // Turno del jugador B, primer ronda

    // Cuadro de diálogo del turno del jugador B
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("===========================");
    typeWrite("--- Turno del jugador B ---", 40);
    Serial.println("===========================");

    delay(1000);

    Serial.println("");

    typeWrite("Escribe la palabra en código morse, usando el botón.", 30);
    delay(300);

    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();                                                                     // Se llama a input para pedir una opción en texto
    if (option == "SI" || option == "Si" || option == "si" || option == "S" || option == "s") {  // Se analizan las posibles respuestas del usuario, si no, se ignora

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

      input();  // Función vacía y sin retorno para detener el programa temporalmente
    }

    Serial.println("");

    delay(1000);

    typeWrite("Recuerda: cuando hayas terminado, envía 6 puntos (.)", 30);
    delay(500);

    typeWrite("Ya puedes comenzar a escribir.", 20);

    gamemode = 24;  // Cambiamos a un estado donde el escritor de morse está activo
  }


  if (gamemode == 25) {  // Inicio de la segunda ronda

    // Se calcula el puntaje de cada jugador según la palabra escrita respecto a la palabra seleccionada
    for (int x = 0; x < selectedWord.length(); x++) {  // Se recorre la palabra seleccionada

      if (playerAWord[x] == selectedWord[x]) {  // Por cada coincidencia en la palabra A,
        playerAScore++;                         // Se suma un punto al jugador A
      }

      if (playerBWord[x] == selectedWord[x]) {  // Por cada coincidencia en la palabra B,
        playerBScore++;                         // Se suma un punto al jugador B
      }
    }

    // Se reinician las palabras para la segunda ronda
    playerAWord = "";
    playerBWord = "";

    // Cuadro de diálogo de la segunda ronda
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("====================");
    typeWrite("--- Segunda ronda ---", 40);
    Serial.println("====================");

    delay(1000);

    Serial.println("");

    typeWrite("Se mostrará de nuevo la palabra ya escogida...", 25);
    delay(1500);

    displayWord(selectedWord);  // Se muestra de nuevo la palabra
    delay(1000);

    gamemode = 26;  // Se cambia al turno del jugador A, segunda ronda
  }


  if (gamemode == 26) {  // Turno del jugador A, segunda ronda

    // Cuadro de diálogo del turno del jugador A
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("===========================");
    typeWrite("--- Turno del jugador A ---", 40);
    Serial.println("===========================");

    delay(1000);

    Serial.println("");

    typeWrite("Escribe la palabra en código morse, usando el botón.", 30);
    delay(300);

    typeWrite("¿Quieres ver las instrucciones? (Escribe SI/NO)", 15);

    String option = input();                                                                     // Se llama a input para pedir una opción en texto
    if (option == "SI" || option == "Si" || option == "si" || option == "S" || option == "s") {  // Se analizan las posibles respuestas del usuario, si no, se ignora

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

      input();  // Función vacía y sin retorno para detener el programa temporalmente
    }

    Serial.println("");

    delay(1000);
    typeWrite("Recuerda: cuando hayas terminado, envía 6 puntos (.)", 30);
    delay(500);

    typeWrite("Ya puedes comenzar a escribir.", 20);

    gamemode = 27;  // Cambiamos a un estado donde el escritor de morse está activo
  }


  if (gamemode == 28) {  // Turno del jugador B, segunda ronda

    // Cuadro de diálogo del jugador B
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    Serial.println("===========================");
    typeWrite("--- Turno del jugador B ---", 40);
    Serial.println("===========================");

    delay(1000);

    Serial.println("");

    typeWrite("Escribe aquí la palabra mostrada.", 20);

    playerBWord = input();      // Se llama a ingresar un texto
    playerBWord.toUpperCase();  // Se le da formato a la palabra para evitar problemas

    gamemode = 29;  // Cambiamos a mostrar los resultados del segundo minijuego
  }


  if (gamemode == 29) {  // Resultados del segundo minjuego
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    delay(1000);

    // Cuadro de diálogo de los resultados
    Serial.println("===================");
    typeWrite("--- Resultados ---", 100);
    Serial.println("===================");

    delay(1500);

    Serial.println("");

    // Se calcula el puntaje de cada jugador según la palabra escrita respecto a la palabra seleccionada
    for (int x = 0; x < selectedWord.length(); x++) {  // Se recorre la palabra seleccionada
      if (playerAWord[x] == selectedWord[x]) {         // Por cada coincidencia en la palabra A,
        playerAScore++;                                // Se suma un punto al jugador A
      }
      if (playerBWord[x] == selectedWord[x]) {  // Por cada coincidencia en la palabra B,
        playerBScore++;                         // Se suma un punto al jugador B
      }
    }

    // Se le da formato a los puntajes
    String p1Txt = String(playerAScore);
    String p2Txt = String(playerBScore);

    // Se prepara el veredicto en base a los puntajes de cada jugador
    if (playerAScore == playerBScore) {
      typeWrite("Tenemos un empate.", 25);

    } else if (playerAScore > playerBScore) {
      typeWrite("El jugador A gana.", 25);

    } else if (playerBScore > playerAScore) {
      typeWrite("El jugador B gana.", 25);
    }

    // Se reinician las palabras para futuras rondas
    playerAWord = "";
    playerBWord = "";

    // Se reinician los puntajes
    playerAScore = 0;
    playerBScore = 0;

    delay(750);

    // Se muestran los resultados del minijuego
    typeWrite("Palabra original: " + selectedWord, 75);
    delay(750);

    Serial.println("");

    typeWrite("Puntaje del jugador A: " + p1Txt + " aciertos.", 50);
    delay(500);

    typeWrite("Puntaje del jugador B: " + p2Txt + " aciertos.", 50);
    delay(1000);

    Serial.println("");

    delay(1500);

    gamemode = 30;  // Pasamos a un modo temporal de pregunta
  }


  if (gamemode == 30) {  // Modo temporal de pregunta

    typeWrite("¿Qué deseas hacer ahora?.", 25);

    Serial.println("1. Jugar otra partida.");
    Serial.println("2. Volver al menú principal.");

    int option = input().toInt();  // Llamamos a ingresar un valor entero

    if (option == 1) {
      gamemode = 21;


    } else if (option == 2) {
      reset();


    } else {                                                  // Si se ingresa un valor inválido
      Serial.println("Error: Esa no es una opción válida.");  // Mensaje de error
      gamemode = 30;                                          // Volvemos al mismo estado para repetir la pregunta

      delay(1000);
    }


    Serial.println("");
  }



  // --- Lógica principal del escritor morse ---
  if (gamemode == 12 || gamemode == 24 || gamemode == 27) {  // Si nos encontramos en un estado que nos habilita escribir

    bool val = digitalRead(inputBtn) ? 0 : 1;  // Asignamos un valor al estado del botón pulsador

    if (val == 1) {                // Si está pulsado
      digitalWrite(buzzer, HIGH);  // Hacemos sonar el buzzer (feedback sonoro)
    } else {                       // Si no está pulsado
      digitalWrite(buzzer, LOW);   // Apagamos el buzzer
    }


    if (state == 0 && val == 1) {  // Si estabamos en un estado inactivo, pero se pulsa el botón
      clearDisplay();              // Apagamos todas las luces del panel
      start = millis();            // Iniciamos un contador durante la pulsación del botón
      state = 1;                   // Pasamos a un estado donde ya pulsamos el botón, y esperamos a que se suelte

      execStart = millis();  // Iniciamos un contador durante el estado de escucha
      listen = 1;            // Pasamos a un estado de escucha
    }


    if (state == 1 && val == 0) {  // Si ya habíamos pulsado el botón, pero lo soltamos

      unsigned long end = millis();      // Tomamos el tiempo actual
      unsigned long time = end - start;  // Calculamos el tiempo desde que se pulsó hasta que se soltó el botón

      state = 0;  // Volvemos al estado donde no se está pulsando el botón

      if (time > 60) {   // Si se pulsó el botón demasiado rápido (interferencia), lo ignoramos, si no, seguimos
        clearDisplay();  // Apagamos todas las luces del panel

        letterParts[letterPointer] = time < 300 ? 0 : 1;  // Asignamos un valor corto (0) o largo (1) a la posición actual en la secuencia actual
        letterPointer += 1;                               // Subimos la posición actual en la secuencia

        if (letterPointer == 6) {  // Si nos excedimos de la secuencia
          saveLetter();            // Guardamos la letra actual
          listen = 0;              // Detenemos el estado de escucha
          return;
        }


        execStart = millis();  // Reiniciamos el tiempo de ejecución para dar más tiempo a seguir escribiendo

        // Mostramos la combinación ingresada actualmente
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

    if (listen == 1) {            // Si estamos en estado de escucha
      uint16_t tempLedState = 0;  // Creamos un estado temporal de leds para evitar problemas

      // Encendemos las leds de fila para indicar que estamos escribiendo
      bitSet(tempLedState, led0_1);
      bitSet(tempLedState, led0_2);
      bitSet(tempLedState, led0_3);

      // Encendemos una led para indicar cuantos pulsos hemos ingresado
      for (int i = 0; i < letterPointer; i++) {
        bitSet(tempLedState, leds[i + 3]);  // Encendemos leds a partir de la segunda posición pues empezamos por columnas
      }

      // Actualizamos el estado actual para evitar repeticiones
      if (tempLedState != ledState) {
        ledState = tempLedState;
        updateLeds();  // Actualizamos el panel de leds una sola vez
      }

      unsigned long execEnd = millis();              // Guardamos el tiempo actual de ejecución
      unsigned long execTime = execEnd - execStart;  // Calculamos el tiempo total de ejecución

      if (execTime > 1000) {  // Si tardamos un segundo sin ingresar nada en ejecución, guardamos la letra automáticamente
        saveLetter();         // Guardamos la letra
        listen = 0;           // Desactivamos el estado de escucha
        return;
      }
    }
  }
}
