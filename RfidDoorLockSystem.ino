#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h> // For I2C communication
#include <LiquidCrystal_I2C.h> // For I2C LCD

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 6
#define GREEN_LED 7
#define RED_LED 8
#define BUZZER_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;

// Initialize the LCD (I2C address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to your I2C address if different

byte authorizedUID[4] = {0x12, 0x34, 0x56, 0x78}; // Replace with your RFID tag's UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myServo.attach(SERVO_PIN);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  myServo.write(0); // Locked position
  digitalWrite(RED_LED, HIGH); // Red LED on (locked)

  // Initialize the LCD
  lcd.begin();
  lcd.backlight(); // Turn on the backlight

  // Display welcoming message
  lcd.setCursor(0, 0);
  lcd.print("  Hello Engineer");
  lcd.setCursor(0, 1);
  lcd.print("      Joe!");
  delay(3000); // Display the message for 3 seconds

  // Clear the LCD and display system ready message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Door Lock");
  lcd.setCursor(0, 1);
  lcd.print("  System Ready");
  delay(2000);
  lcd.clear();

  // Display "Please insert card" message
  lcd.setCursor(0, 0);
  lcd.print("  Please Scan");
  lcd.setCursor(0, 1);
  lcd.print(" your Card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (rfid.uid.uidByte[0] == authorizedUID[0] &&
      rfid.uid.uidByte[1] == authorizedUID[1] &&
      rfid.uid.uidByte[2] == authorizedUID[2] &&
      rfid.uid.uidByte[3] == authorizedUID[3]) {
    Serial.println("Access Granted!");
    lcd.setCursor(0, 0);
    lcd.print(" Access Granted ");
    digitalWrite(GREEN_LED, HIGH); // Green LED on
    digitalWrite(RED_LED, LOW); // Red LED off
    tone(BUZZER_PIN, 1000, 500); // Beep
    myServo.write(90); // Unlock door

    // Display "Welcome in Engineer" message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Welcome in");
    lcd.setCursor(0, 1);
    lcd.print("   Engineer!");
    delay(3000); // Display the message for 3 seconds

    delay(2000); // Keep door unlocked for additional 2 seconds
    myServo.write(0); // Lock door
    digitalWrite(GREEN_LED, LOW); // Green LED off
    digitalWrite(RED_LED, HIGH); // Red LED on
    lcd.clear();

    // Display "Please insert card" message again
    lcd.setCursor(0, 0);
    lcd.print("  Please Scan");
    lcd.setCursor(0, 1);
    lcd.print("  your Card...");
  } else {
    Serial.println("Access Denied!");
    lcd.setCursor(0, 0);
    lcd.print(" Access Denied  ");
    digitalWrite(RED_LED, HIGH); // Red LED on
    tone(BUZZER_PIN, 300, 1000); // Error beep

    // Display phone number message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Call me:");
    lcd.setCursor(0, 1);
    lcd.print(" 074-138-4144"); // Replace with your phone number
    delay(3000); // Display the message for 3 seconds

    lcd.clear();

    // Display "Please insert card" message again
    lcd.setCursor(0, 0);
    lcd.print("  Please Scan");
    lcd.setCursor(0, 1);
    lcd.print(" your Card...");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}