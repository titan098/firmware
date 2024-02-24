// define i2c pins otherwise we hang on boot
#define I2C_SDA 8
#define I2C_SCL 9

#define HAS_SCREEN 0
#define HAS_BATTERY 0

#define BUTTON_PIN 0

#define LED_PIN LED_BUILTIN // LED
#define LED_INVERTED 1

#define HAS_GPS 0
#define GPS_RX_PIN 20
#define GPS_TX_PIN 21

#undef LORA_SCK
#undef LORA_MISO
#undef LORA_MOSI
#undef LORA_CS
#undef LORA_DIO1
#undef LORA_DIO2

#define USE_SX1262
#define LORA_SCK 4
#define LORA_MISO 5
#define LORA_MOSI 6
#define LORA_CS 7
#define LORA_RESET 2
#define LORA_DIO1 1
#define LORA_DIO2 10
#define LORA_BUSY 3

// Clashes with the UART, not needed
// #define LORA_TXEN 20
// #define LORA_RXEN 21

#define SX126X_CS LORA_CS
#define SX126X_DIO1 LORA_DIO1
#define SX126X_DIO2 LORA_DIO2
#define SX126X_BUSY LORA_BUSY
#define SX126X_RESET LORA_RESET
// #define SX126X_TXEN LORA_TXEN
// #define SX126X_RXEN LORA_RXEN

#define SX126X_DIO2_AS_RF_SWITCH

// Core126X uses this, RA-01SH does not
// #define SX126X_DIO3_TCXO_VOLTAGE 1.8