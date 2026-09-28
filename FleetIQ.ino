#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int potPin = A0;
const int greenLED = 8;
const int redLED = 9;
const int buzzer = 10;

// System parameters
const float maxCapacity = 1000.0;   // kg
const float threshold = 500.0;      // kg

// Variables
float weight = 0;
int value = 0;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);

  // Startup screen
  lcd.setCursor(0,0);
  lcd.print("Bus Monitor");
  lcd.setCursor(0,1);
  lcd.print("Max:");
  lcd.print(maxCapacity);
  lcd.print("kg");
  delay(2000);
  lcd.clear();
}

// Function for stable reading (average)
int readStableAnalog() {
  long sum = 0;
  for(int i = 0; i < 10; i++) {
    sum += analogRead(potPin);
    delay(5);
  }
  return sum / 10;
}

void loop() {

  // Read stable value
  value = readStableAnalog();

  // Convert to weight
  weight = (value / 1023.0) * maxCapacity;

  // Calculate percentage
  float percent = (weight / maxCapacity) * 100.0;

  // ===== LCD DISPLAY =====
  lcd.setCursor(0,0);
  lcd.print("Wt:");
  lcd.print(weight,1);
  lcd.print("kg ");

  lcd.setCursor(0,1);
  lcd.print(percent,0);
  lcd.print("% ");

  // ===== STATUS LOGIC =====
  if(weight < threshold) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);

    lcd.print("Normal   ");
    
    Serial.println("Status: Normal Load");
  }
  else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);

    // Buzzer beep pattern
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);

    lcd.print("OVERLOAD");

    Serial.println("Status: OVERWEIGHT!!!");
  }

  // ===== SERIAL OUTPUT =====
  Serial.print("ADC: ");
  Serial.print(value);
  Serial.print(" | Weight: ");
  Serial.print(weight,2);
  Serial.print(" kg | Load: ");
  Serial.print(percent,1);
  Serial.println(" %");

  Serial.println("-------------------------");

  delay(400);
}
