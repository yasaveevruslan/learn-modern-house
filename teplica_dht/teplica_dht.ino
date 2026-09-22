#include <DHT.h>
#include <Servo.h>

#define DHT_PIN 2
#define PUMP_PIN 5
#define FAN_PIN 3
#define LED_PIN 13
#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 11
#define LIGHT_SENSOR A3
#define SOIL1 A0
#define SOIL2 A1
#define SOIL3 A2

#define DHT_TYPE DHT11
#define SOIL_MIN 400    
#define SOIL_MAX 550    
#define TEMP_TARGET 26  
#define TEMP_HYSTERESIS 2  

DHT dht(DHT_PIN, DHT_TYPE);

Servo servo1;
Servo servo2;
Servo servo3;

bool servosOpen = false;
bool fansOn = false;


// ЗНАЧЕНИЕ ДЛЯ ДАТЧИКОВ

// температура
int temperatureValue = 26;
// пороговое значение температуры
int cofTemperature = 2;
// значение для света
int lightValue = 300;
// значение воды максимальное
int waterValueMax = 550;
// значение воды минимальное
int waterValueMin = 450;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  closeServos();
  
  Serial.println("Умная теплица запущена");
}

void loop() {
  int soil1 = analogRead(SOIL1);
  int soil2 = analogRead(SOIL2);
  int soil3 = analogRead(SOIL3);
  int avgSoil = soil1;
  
  float temp = dht.readTemperature();
  int light = analogRead(LIGHT_SENSOR);
  
  if (isnan(temp)) {
    Serial.println("Ошибка DHT! Использую 25°C по умолчанию");
    temp = 25;
  }
  
  printDebug(soil1, soil2, soil3, avgSoil, temp, light);
  
  controlPump(avgSoil);

  controlCooling(temp);

  controlLight(light);
  
  delay(2000);
}

// КОНТРОЛЬ ПОЛИВА
void controlPump(int avgSoil) {
  if (avgSoil > waterValueMax) {
    digitalWrite(PUMP_PIN, HIGH);
    Serial.println("ПОЛИВ ВКЛЮЧЕН (влажно)");
  } else if (avgSoil < waterValueMin) {
    digitalWrite(PUMP_PIN, LOW);
    Serial.println("ПОЛИВ ВЫКЛЮЧЕН (сухо)");
  } else {
    Serial.println("ПОЛИВ БЕЗ ИЗМЕНЕНИЙ");
  }
}

// КОНТРОЛЬ ТЕМПЕРАТУРЫ
void controlCooling(float temp) {
  bool needCooling = (temp > temperatureValue + cofTemperature);
  
  if (needCooling) {
    if (!servosOpen) {
      openServos();
      servosOpen = true;
    }
    if (!fansOn) {
      digitalWrite(FAN_PIN, HIGH);
      fansOn = true;
      Serial.println("КУЛЕРЫ ВКЛЮЧЕНЫ");
    }
  } else {
    if (servosOpen) {
      closeServos();
      servosOpen = false;
    }
    if (fansOn) {
      digitalWrite(FAN_PIN, LOW);
      fansOn = false;
      Serial.println("КУЛЕРЫ ВЫКЛЮЧЕНЫ");
    }
  }
}

// КОНТРОЛЬ СВЕТА
void controlLight(int light) {
  if (light < lightValue) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("СВЕТ ВКЛЮЧЕН (темно)");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("СВЕТ ВЫКЛЮЧЕН (светло)");
  }
}

// ОТКРЫТЬ ФОРТОЧКИ
void openServos() {
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
}

// ЗАКРЫТЬ ФОРТОЧКИ
void closeServos() {
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
}

// ВЫВОД ИНФОРМАЦИИ
void printDebug(int s1, int s2, int s3, int avg, float temp, int light) {
  Serial.println("ДАННЫЕ");
  Serial.print("Влажность почвы: ");
  Serial.print(s1); Serial.print(" | ");
  Serial.print(s2); Serial.print(" | ");
  Serial.print(s3); Serial.print(" | Сред: ");
  Serial.println(avg);
  
  Serial.print("Температура: ");
  Serial.print(temp);
  Serial.println(" °C");
  
  Serial.print("Освещенность: ");
  Serial.println(light);
  
  Serial.print("Полив: ");
  Serial.println(digitalRead(PUMP_PIN) ? "ВКЛ" : "ВЫКЛ");
  
  Serial.print("Кулеры: ");
  Serial.println(digitalRead(FAN_PIN) ? "ВКЛ" : "ВЫКЛ");
  
  Serial.print("Свет: ");
  Serial.println(digitalRead(LED_PIN) ? "ВКЛ" : "ВЫКЛ");

  Serial.print("Положение серво1: ");
  Serial.println(servo1.read());
  Serial.print("Положение серво2: ");
  Serial.println(servo2.read());
  Serial.print("Положение серво3: ");
  Serial.println(servo3.read());
  
  Serial.println();
}
