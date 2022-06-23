#include <Arduino.h>
#include <M5Core2.h>
#undef min
AXP192 Axp;

// define pin-name
#define GPIO *(volatile uint32_t *)0x3FF44004 // Assign DAC register address
#define Pos 25
#define Neg 26
#define Dmp 27
#define En_MosD 14
#define En_Reg 19
#define En_3V 0
#define En_POS 2
#define En_NEG 13
#define trigger 36

bool scan = true;
bool isCharging = false;

void IRAM_ATTR toggleGPIO() // Toggle GPIO pins to generate square waves
{
  taskDISABLE_INTERRUPTS();
  for (int i = 0; i < 16; i++) // Cleate 2MHz-16wvs logic signals.
  {
    GPIO = BIT(trigger) | !BIT(Neg) | !BIT(Dmp) | BIT(En_3V) | BIT(En_POS) | BIT(En_NEG) | BIT(En_Reg) | BIT(En_MosD);
    for (int j = 0; j < 9; j++)
      __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    GPIO = !BIT(trigger) | BIT(Neg) | !BIT(Dmp) | BIT(En_3V) | BIT(En_POS) | BIT(En_NEG) | BIT(En_Reg) | BIT(En_MosD);
    for (int j = 0; j < 9; j++)
      __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
  }
  // Switch on/off the dump-line MOSFET gate
  GPIO = !BIT(trigger) | !BIT(Neg) | BIT(Dmp) | BIT(En_3V) | BIT(En_POS) | BIT(En_NEG) | BIT(En_Reg) | BIT(En_MosD);

  for (int i = 0; i < 160; i++)
    __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  GPIO = !BIT(trigger) | !BIT(Neg) | !BIT(Dmp) | BIT(En_3V) | BIT(En_POS) | BIT(En_NEG) | BIT(En_Reg) | BIT(En_MosD);
  taskENABLE_INTERRUPTS();
}

void setup()
{
  // Initialize M5Core2, Don't use M5.begin() cuz it conflicts with BLE initialization.
  Axp.begin();
  M5.Lcd.begin();
  M5.Lcd.setTextSize(3);
  M5.Rtc.begin();
  Serial.begin(115200);

  // Initialize the GPIO mode
  pinMode(trigger, OUTPUT);
  pinMode(Neg, OUTPUT);
  pinMode(Dmp, OUTPUT);
  pinMode(En_MosD, OUTPUT);
  pinMode(En_Reg, OUTPUT);
  pinMode(En_3V, OUTPUT);
  pinMode(En_POS, OUTPUT);
  pinMode(En_NEG, OUTPUT);
  pinMode(trigger, INPUT);
  attachInterrupt(trigger, toggleGPIO, RISING);
  // Initialize GPIO outputs
  digitalWrite(trigger, LOW);
  digitalWrite(Neg, LOW);
  digitalWrite(Dmp, LOW);
  digitalWrite(En_3V, HIGH);
  digitalWrite(En_POS, HIGH);
  digitalWrite(En_NEG, HIGH);
  delay(2);
  digitalWrite(En_Reg, HIGH);
  delay(6);
  digitalWrite(En_MosD, HIGH); // Activate MOSFET
  delayMicroseconds(6);        // Waiting for stable electric output
}

void loop()
{
  M5.update();
  isCharging = Axp.isCharging();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Battery info:\n");
  M5.Lcd.printf("-Vol. = %3.4f V\n", Axp.GetBatVoltage());
  M5.Lcd.printf("-Per. = %3.1f %%\n", Axp.GetBatteryLevel());
  if (isCharging) {
    M5.Lcd.printf("\nNow Charging...\n");
  } else {
    M5.Lcd.printf("\nDischarge...   \n");
  }

  delay(1000);
}
