#include <WiFi.h>

const char* ssid = "Softex 2";
const char* password = "SoftexUFP3!!";

const char* host = "192.168.1.19";

WiFiClient client;

// Pinos do LED e Buzzer
const int ledRed = 13;
const int buzzer = 11;
const int btn = 10;
const int ldr = A0;

// Servo
const int servoPin = 12;
const int servoChannel = 0;
const int servoFreq = 50;      // 50 Hz (servo padrão)
const int servoResolution = 16;

const int touchPin = T0;
int limiarTouch = 30; 

// Configuração Display 7 Segmentos
#define A 8
#define B 9
#define C 4
#define D 3
#define E 2
#define F 7
#define G 6

bool seven_segments[10][7] = { 
  { 1,1,1,1,1,1,0 }, // 0
  { 0,1,1,0,0,0,0 }, // 1
  { 1,1,0,1,1,0,1 }, // 2
  { 1,1,1,1,0,0,1 }, // 3
  { 0,1,1,0,0,1,1 }, // 4
  { 1,0,1,1,0,1,1 }, // 5
  { 1,0,1,1,1,1,1 }, // 6
  { 1,1,1,0,0,0,0 }, // 7
  { 1,1,1,1,1,1,1 }, // 8
  { 1,1,1,1,0,1,1 }  // 9
};


uint32_t angleToDuty(int angle) {
  return map(angle, 0, 180, 1638, 8192);
}


void writeNumber(int num) {
  if (num < 0 || num > 9) return;
  digitalWrite(A, seven_segments[num][0]);
  digitalWrite(B, seven_segments[num][1]);
  digitalWrite(C, seven_segments[num][2]);
  digitalWrite(D, seven_segments[num][3]);
  digitalWrite(E, seven_segments[num][4]);
  digitalWrite(F, seven_segments[num][5]);
  digitalWrite(G, seven_segments[num][6]);
}

void turnDisplayOff() 
{
  digitalWrite(A, 0);
  digitalWrite(B, 0);
  digitalWrite(C, 0);
  digitalWrite(D, 0);
  digitalWrite(E, 0);
  digitalWrite(F, 0);
  digitalWrite(G, 0);
}

// Variáveis de Controle
enum State { DEACTIVATED, ARMING, ACTIVATED, WAITING_PASSWORD, TRIGGERED };
State state = DEACTIVATED;
String input = "";
int tries = 0;
int limiarLdr = 200;
unsigned long timerState = 0;
int valueClient = 0;


void setup() {
  ledcSetup(servoChannel, servoFreq, servoResolution);
  ledcAttachPin(servoPin, servoChannel);
  ledcWrite(servoChannel, angleToDuty(0));  // Trava começa aberta

  pinMode(A, OUTPUT); pinMode(B, OUTPUT); pinMode(C, OUTPUT);
  pinMode(D, OUTPUT); pinMode(E, OUTPUT); pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  pinMode(ldr, INPUT);
  
  Serial.begin(115200);
  Serial.setTimeout(50);

  WiFi.begin(ssid, password);

  Serial.println("Sistema Iniciado. Pressione o botao para armar.");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop() {
  int stateLdr = analogRead(ldr);
  int touchValue = touchRead(touchPin);

  int btnState = digitalRead(btn);
;

  switch(state) {
    case DEACTIVATED: {
      input = "";
      tries = 0;
      digitalWrite(ledRed, LOW);
      noTone(buzzer);

      ledcWrite(servoChannel, angleToDuty(0)); // posição inicial

      if (btnState == LOW) {
        state = ARMING;
        timerState = millis();
        Serial.println("Armando em 10 segundos...");
      }
      break;
    }

    case ARMING: {
      // Pisca LED durante 10 segundos
      if ((millis() - timerState) % 500 < 250) {
        digitalWrite(ledRed, HIGH);
      } else {
        digitalWrite(ledRed, LOW);
      }

      if (millis() - timerState >= 10000) {
        state = ACTIVATED;
        tries = 0;
        ledcWrite(servoChannel, angleToDuty(90)); // fecha a trava
        Serial.println("ALARME ATIVADO! Trava fechada.");
      }
      break;
    }

    case ACTIVATED: {
      digitalWrite(ledRed, LOW);

      if (stateLdr > limiarLdr || touchValue < limiarTouch) {
        tone(buzzer, 1000, 200);
        //delay(500);
        tone(buzzer, 1000, 200);

        state = WAITING_PASSWORD;
        timerState = millis();

        tries = 0;

        Serial.println("Intrusão detectada! Digite a senha e aperte ENTER (10s):");
        Serial.println(touchValue);
      }
      break;
    }

    case WAITING_PASSWORD: {
      // Contador regressivo no Display
      int elapsed = (millis() - timerState) / 1000;
      int countdown = 9 - elapsed;
      if (countdown >= 0) {
        writeNumber(countdown);
      }

      // Timeout 10 segundos
      if (millis() - timerState >= 10000) {
        Serial.println("\nTempo esgotado!");
        state = TRIGGERED;
        break;
      }
      
	  checkPassword();
      break;
    }

    case TRIGGERED: {
      unsigned long currentMillis = millis();
      
      tone(buzzer, 1000);
      if (currentMillis % 300 < 150) {
        digitalWrite(ledRed, HIGH);
      } else {
        digitalWrite(ledRed, LOW);
      }

      checkPassword(); 
      break;
    }
  }

}


void checkPassword() {

  input = Serial.readString();
  input.trim();

  if (input.length() > 0) {

    String user = sendPasswordToServer(input);

    if (user.length() > 0) {
      Serial.print("Usuário autenticado: ");
      Serial.println(user);
      Serial.println("Alarme desativado.");

      ledcWrite(servoChannel, angleToDuty(0)); // abre a trava
      turnDisplayOff();
      state = DEACTIVATED;
      noTone(buzzer);

    } else {
      tries++;
      timerState = millis();
      input = "";

      Serial.println("Senha incorreta!");
      Serial.print("Tentativas: ");
      Serial.println(tries);

      if (tries >= 2) {
        Serial.println("Número máximo de tentativas atingido!");
        state = TRIGGERED;
        turnDisplayOff();
      } else {
        Serial.println("Digite a senha novamente e aperte ENTER:");
      }
    }
  }
}


String sendPasswordToServer(String password) {

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("Falha ao conectar no servidor");
    return "";
  }

  client.print(String("GET /auth?password=") + password + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return "";
    }
  }

  String response = client.readString();
  client.stop();

  int index = response.indexOf("USER:");

  if (index >= 0) {
    return response.substring(index + 5); // retorna nome
  }

  return "";
}



