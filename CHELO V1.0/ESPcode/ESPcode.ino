#include <DFRobotDFPlayerMini.h>
#include <Bonezegei_LCD1602_I2C.h>

// Initialize the LCD
Bonezegei_LCD1602_I2C lcd(0x27);

//DEFINICIONES DE PINES
 
int boton[] = {33, 32, 35, 34};  

int led[] = {14, 27, 26, 25}; 
int cat[] ={1, 2, 3, 4};

const int Sonido_selector = 5;

// Using Hardware Serial for DFPlayer
DFRobotDFPlayerMini myDFPlayer;

//Encoder pins and variables
const int PinA = 5;  // CLK pin
const int PinB = 18; // DT pin
int lastEncoded = 0;
int encoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;

//VARIABLES GLOBALES

//Variables para el funcionamiento del dfplayer
int tiempo = 100;
int tiempo2 = 1150; //1450
int categoria = 0;
int categoria_actual = 0;

// Variables for message display management
unsigned long lastMessageTime = 0;
unsigned long messageDisplayDuration = 5000; // 5 seconds to display a message
bool messageActive = false;

//FUNCIONES

void Leds(int led_0, int led_1, int led_2, int led_3){
  digitalWrite(led[0], led_0);
  digitalWrite(led[1], led_1);
  digitalWrite(led[2], led_2);
  digitalWrite(led[3], led_3);
}

void Presionar_Sonar(int boton_num, int categoria_arreglo[], int sel_cat, int folder, int tiempo_sonidos){
  int categoria_index = sel_cat - 1;
  Serial.print(F("Presionado boton "));
  Serial.print(boton_num);
  Serial.print(F(" - Categoria "));
  Serial.println(categoria_arreglo[categoria_index]);
  myDFPlayer.playFolder(folder, boton_num);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
  delay(tiempo_sonidos);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // Hardware Serial on pins 16(RX) and 17(TX)

  for(int i=0; i < 4; i++){
    pinMode(led[i], OUTPUT);
  }

  for(int i=0; i < 4; i++){
    pinMode(boton[i], INPUT_PULLUP);
  }

  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);

  lcd.begin();
  lcd.print("CHELO V1.0");
  lcd.setPosition(0, 1);      //param1 = X   param2 = Y
  lcd.print("Yaya Tech");
  delay(2000);
  lcd.clear();
  
  Serial.println();
  Serial.println(F("CHELO con DFPlayer Mini")); // El F es para que no ocupe memoria RAM
  Serial.println(F("Iniciando CHELO... (Puede tomar entre 3-5 segundos)"));
  delay(2000);

  if (!myDFPlayer.begin(Serial2)) {
    Serial.println(F("No se puede iniciar"));
    Serial.println(F("1. Chequea la conexion"));
    Serial.println(F("2. Inserta la tarjeta SD"));
    while (true);
  }

  Serial.println(F("CHELO online"));
  delay(1000);

  myDFPlayer.setTimeOut(500); // Supuestamente deja la comunicacion descansar 500 (supongo que medio segundo)

  // Esto es para poner el volumen que va de 0-30
  myDFPlayer.volume(30);
  // myDFPlayer.volumeUp();
  // myDFPlayer.volumeDown();

  // Configurando ecualizacion
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); // existe normal, pop, rock, jazz, classic, bass

  // Seleccionando dispositivo de musicas
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD); // Se puede usb tambien o hasta disco xd

  Serial.println("---Welcome to CHELO---");
  myDFPlayer.playFolder(13, 1);
  delay(6000);
}

void loop() {
  // With pull-up resistors, buttons are LOW when pressed
  int boton1 = !digitalRead(boton[0]);
  int boton2 = !digitalRead(boton[1]);
  int boton3 = !digitalRead(boton[2]);
  int boton4 = !digitalRead(boton[3]);

  // Read encoder
  int MSB = digitalRead(PinA); // Most significant bit
  int LSB = digitalRead(PinB); // Least significant bit
  
  int encoded = (MSB << 1) | LSB; // Converting the 2 pin values to a single number
  int sum = (lastEncoded << 2) | encoded; // Adding it to the previous encoded value
  
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;
  
  lastEncoded = encoded; // Store this value for next time
  
  // Map encoder value to category (1-4)
  // We use modulo to wrap around and handle negative values correctly
  int mappedValue = encoderValue / 2; // Dividing by 2 to reduce sensitivity
  if (mappedValue < 0) {
    // Handle negative values correctly with modulo
    mappedValue = ((mappedValue % 4) + 4) % 4;
  } else {
    mappedValue = mappedValue % 4;
  }
  
  categoria = mappedValue + 1; // Convert from 0-3 to 1-4

  // Check if message display time has expired
  if(messageActive && (millis() - lastMessageTime >= messageDisplayDuration)) {
    messageActive = false; // Reset the message active flag
  }

  // Only process category changes if no message is being displayed
  if(!messageActive) {
    // Handle categories and buttons
    if(categoria == 1){
      categoria_actual = 1;
      Leds(1, 0, 0, 0);
      Serial.print(F("Categoria "));
      Serial.println(cat[categoria_actual-1]);
      lcd.clear();
      lcd.setPosition(0, 0);      //param1 = X   param2 = Y
      lcd.print("Basic Needs");
      delay(tiempo);  

      if(boton1 == 1){
        Presionar_Sonar(1, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Basic Needs");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Bathroom");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton2 == 1){
        Presionar_Sonar(2, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Basic Needs");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Water");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton3 == 1){
        Presionar_Sonar(3, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Basic Needs");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Food");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton4 == 1){
        Presionar_Sonar(4, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Basic Needs");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Doctor");
        lastMessageTime = millis();
        messageActive = true;
      }
    } else if(categoria == 2){
      categoria_actual = 2;
      Leds(0, 1, 0, 0);
      Serial.print(F("Categoria "));
      Serial.println(cat[categoria_actual-1]);
      lcd.clear();
      lcd.setPosition(0, 0);      //param1 = X   param2 = Y
      lcd.print("People and Relationships");
      delay(tiempo);

      if(boton1 == 1){
        Presionar_Sonar(1, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("People and Relationships");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Son");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton2 == 1){
        Presionar_Sonar(2, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("People and Relationships");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Spouse");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton3 == 1){
        Presionar_Sonar(3, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("People and Relationships");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Missed you");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton4 == 1){
        Presionar_Sonar(4, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("People and Relationships");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("I love you");
        lastMessageTime = millis();
        messageActive = true;
      }
    } else if(categoria == 3){
      categoria_actual = 3;
      Leds(0, 0, 1, 0);
      Serial.print(F("Categoria "));
      Serial.println(cat[categoria_actual-1]);
      lcd.clear();
      lcd.setPosition(0, 0);      //param1 = X   param2 = Y
      lcd.print("Daily Activities");
      delay(tiempo);
      
      if(boton1 == 1){
        Presionar_Sonar(1, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Daily Activities");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Book");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton2 == 1){
        Presionar_Sonar(2, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Daily Activities");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("TV");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton3 == 1){
        Presionar_Sonar(3, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Daily Activities");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Movie");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton4 == 1){
        Presionar_Sonar(4, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Daily Activities");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Park");
        lastMessageTime = millis();
        messageActive = true;
      }
    } else if(categoria == 4){
      categoria_actual = 4;
      Leds(0, 0, 0, 1);
      Serial.print(F("Categoria "));
      Serial.println(cat[categoria_actual-1]);
      lcd.clear();
      lcd.setPosition(0, 0);      //param1 = X   param2 = Y
      lcd.print("Communication");
      delay(tiempo);

      if(boton1 == 1){
        Presionar_Sonar(1, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Communication");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Yes");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton2 == 1){
        Presionar_Sonar(2, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Communication");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("No");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton3 == 1){
        Presionar_Sonar(3, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Communication");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Please");
        lastMessageTime = millis();
        messageActive = true;
      }
      if(boton4 == 1){
        Presionar_Sonar(4, cat, categoria_actual, categoria_actual, tiempo2);
        lcd.clear();
        lcd.setPosition(0, 0);      //param1 = X   param2 = Y
        lcd.print("Communication");
        lcd.setPosition(0, 1);      //param1 = X   param2 = Y
        lcd.print("Thank you");
        lastMessageTime = millis();
        messageActive = true;
      }
    }

    // Check if we've changed categories and play the category selection sound if so
    if(categoria != categoria_actual){
      Serial.print(F("Cambiando a categoria: "));
      Serial.println(categoria);
      
      // Play the selection sound for the new category
      myDFPlayer.playFolder(categoria, Sonido_selector);
      
      // Update the current category
      categoria_actual = categoria;
    }
  }
}