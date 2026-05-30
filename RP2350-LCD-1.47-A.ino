/**
 * ==================================================================================
 *   WAVESHARE RP2350-LCD-1.47-A - ZERO-DEPENDENCY FUTURISTIC AI AGENT UI SKETCH
 * ==================================================================================
 * 
 * This sketch implements a fully self-contained, high-performance sci-fi animated
 * dashboard for the Waveshare RP2350-LCD-1.47-A development board (172x320 resolution).
 * 
 * KEY FEATURES:
 * - ZERO external library dependencies (no Adafruit_GFX, no Adafruit_ST7789 required!).
 * - Built-in optimized DirectST7789 hardware SPI display driver.
 * - Custom high-speed graphics drawing engine (lines, circles, rounded rectangles, text).
 * - Compact embedded 5x7 monospaced ASCII font array (1.2KB) in flash memory.
 * - Full-screen double buffering (110KB) in RP2350's 520KB SRAM for 100% flicker-free ~55 FPS.
 * 
 * Hardware Pin Mapping:
 * - TFT_DC   : GPIO 16 (Data/Command)
 * - TFT_CS   : GPIO 17 (Chip Select)
 * - TFT_CLK  : GPIO 18 (SPI Clock - SCK)
 * - TFT_MOSI : GPIO 19 (SPI TX - MOSI)
 * - TFT_RST  : GPIO 20 (Hardware Reset)
 * - TFT_BL   : GPIO 21 (Backlight Pin - active HIGH)
 * ==================================================================================
 */

#include <SPI.h>
#include <math.h>

// --- Hardware Pin Definitions ---
#define TFT_DC   16
#define TFT_CS   17
#define TFT_CLK  18
#define TFT_MOSI 19
#define TFT_RST  20
#define TFT_BL   21

// --- Display Specifications ---
#define SCREEN_WIDTH  172
#define SCREEN_HEIGHT 320
#define LCD_COL_OFFSET 34 // Offset due to 172x320 window inside 240x320 ST7789 controller

// --- Color Palette (RGB565) ---
#define COLOR_BLACK       0x0000
#define COLOR_DEEP_BLUE   0x0005 // Cosmic deep blue-black background
#define COLOR_CYAN        0x07FF // Bright neon cyan
#define COLOR_CYAN_GLOW   0x03FF // Fading cyan
#define COLOR_DARK_CYAN   0x022F // Deep slate cyan/teal
#define COLOR_MAGENTA     0xF81F // Intense magenta
#define COLOR_PURPLE      0xB01F // Glowing futuristic purple
#define COLOR_VIOLET      0x780F // Deep sci-fi violet
#define COLOR_GREEN       0x07E0 // Bright green for battery charging
#define COLOR_WHITE       0xFFFF // Pure white for icons/text
#define COLOR_GRAY        0x7BEF // Sci-fi terminal text gray
#define COLOR_DARK_GRAY   0x10A2 // Sleek panel borders

// --- Classic 5x7 ASCII Monospaced Font Bitmap Data (256 * 5 = 1280 bytes) ---
static const uint8_t font[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x5B, 0x4F, 0x5B, 0x3E, 0x3E, 0x6B,
    0x4F, 0x6B, 0x3E, 0x1C, 0x3E, 0x7C, 0x3E, 0x1C, 0x18, 0x3C, 0x7E, 0x3C,
    0x18, 0x1C, 0x57, 0x7D, 0x57, 0x1C, 0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 0x00,
    0x18, 0x3C, 0x18, 0x00, 0xFF, 0xE7, 0xC3, 0xE7, 0xFF, 0x00, 0x18, 0x24,
    0x18, 0x00, 0xFF, 0xE7, 0xDB, 0xE7, 0xFF, 0x30, 0x48, 0x3A, 0x06, 0x0E,
    0x26, 0x29, 0x79, 0x29, 0x26, 0x40, 0x7F, 0x05, 0x05, 0x07, 0x40, 0x7F,
    0x05, 0x25, 0x3F, 0x5A, 0x3C, 0xE7, 0x3C, 0x5A, 0x7F, 0x3E, 0x1C, 0x1C,
    0x08, 0x08, 0x1C, 0x1C, 0x3E, 0x7F, 0x14, 0x22, 0x7F, 0x22, 0x14, 0x5F,
    0x5F, 0x00, 0x5F, 0x5F, 0x06, 0x09, 0x7F, 0x01, 0x7F, 0x00, 0x66, 0x89,
    0x95, 0x6A, 0x60, 0x60, 0x60, 0x60, 0x60, 0x94, 0xA2, 0xFF, 0xA2, 0x94,
    0x08, 0x04, 0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E, 0x20, 0x10, 0x08, 0x08,
    0x2A, 0x1C, 0x08, 0x08, 0x1C, 0x2A, 0x08, 0x08, 0x1E, 0x10, 0x10, 0x10,
    0x10, 0x0C, 0x1E, 0x0C, 0x1E, 0x0C, 0x30, 0x38, 0x3E, 0x38, 0x30, 0x06,
    0x0E, 0x3E, 0x0E, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14,
    0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62, 0x36, 0x49,
    0x56, 0x20, 0x50, 0x00, 0x08, 0x07, 0x03, 0x00, 0x00, 0x1C, 0x22, 0x41,
    0x00, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x08,
    0x08, 0x3E, 0x08, 0x08, 0x00, 0x80, 0x70, 0x30, 0x00, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x00, 0x00, 0x60, 0x60, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02,
    0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x72, 0x49,
    0x49, 0x49, 0x46, 0x21, 0x41, 0x49, 0x4D, 0x33, 0x18, 0x14, 0x12, 0x7F,
    0x10, 0x27, 0x45, 0x45, 0x45, 0x39, 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x41,
    0x21, 0x11, 0x09, 0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x46, 0x49, 0x49,
    0x29, 0x1E, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x40, 0x34, 0x00, 0x00,
    0x00, 0x08, 0x14, 0x22, 0x41, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x41,
    0x22, 0x14, 0x08, 0x02, 0x01, 0x59, 0x09, 0x06, 0x3E, 0x41, 0x5D, 0x59,
    0x4E, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E,
    0x41, 0x41, 0x41, 0x22, 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x7F, 0x49, 0x49,
    0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x3E, 0x41, 0x41, 0x51, 0x73,
    0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x20, 0x40,
    0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40,
    0x40, 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E,
    0x41, 0x41, 0x41, 0x3E, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51,
    0x21, 0x5E, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x26, 0x49, 0x49, 0x49, 0x32,
    0x03, 0x01, 0x7F, 0x01, 0x03, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x1F, 0x20,
    0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14,
    0x63, 0x03, 0x04, 0x78, 0x04, 0x03, 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00,
    0x7F, 0x41, 0x41, 0x41, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41,
    0x41, 0x7F, 0x04, 0x02, 0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x03, 0x07, 0x08, 0x00, 0x20, 0x54, 0x54, 0x78, 0x40, 0x7F, 0x28,
    0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x28,
    0x7F, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x08, 0x7E, 0x09, 0x02, 0x18,
    0xA4, 0xA4, 0x9C, 0x78, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D,
    0x40, 0x00, 0x20, 0x40, 0x40, 0x3D, 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,
    0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04, 0x78, 0x04, 0x78, 0x7C, 0x08,
    0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38, 0xFC, 0x18, 0x24, 0x24,
    0x18, 0x18, 0x24, 0x24, 0x18, 0xFC, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x48,
    0x54, 0x54, 0x54, 0x24, 0x04, 0x04, 0x3F, 0x44, 0x24, 0x3C, 0x40, 0x40,
    0x20, 0x7C, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C,
    0x44, 0x28, 0x10, 0x28, 0x44, 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x44, 0x64,
    0x54, 0x4C, 0x44, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x77, 0x00,
    0x00, 0x00, 0x41, 0x36, 0x08, 0x00, 0x02, 0x01, 0x02, 0x04, 0x02, 0x3C,
    0x26, 0x23, 0x26, 0x3C, 0x1E, 0xA1, 0xA1, 0x61, 0x12, 0x3A, 0x40, 0x40,
    0x20, 0x7A, 0x38, 0x54, 0x54, 0x55, 0x59, 0x21, 0x55, 0x55, 0x79, 0x41,
    0x22, 0x54, 0x54, 0x78, 0x42, 0x21, 0x55, 0x54, 0x78, 0x40, 0x20, 0x54,
    0x55, 0x79, 0x40, 0x0C, 0x1E, 0x52, 0x72, 0x12, 0x39, 0x55, 0x55, 0x55,
    0x59, 0x39, 0x54, 0x54, 0x54, 0x59, 0x39, 0x55, 0x54, 0x54, 0x58, 0x00,
    0x00, 0x45, 0x7C, 0x41, 0x00, 0x02, 0x45, 0x7D, 0x42, 0x00, 0x01, 0x45,
    0x7C, 0x40, 0x7D, 0x12, 0x11, 0x12, 0x7D, 0xF0, 0x28, 0x25, 0x28, 0xF0,
    0x7C, 0x54, 0x55, 0x45, 0x00, 0x20, 0x54, 0x54, 0x7C, 0x54, 0x7C, 0x0A,
    0x09, 0x7F, 0x49, 0x32, 0x49, 0x49, 0x49, 0x32, 0x3A, 0x44, 0x44, 0x44,
    0x3A, 0x32, 0x4A, 0x48, 0x48, 0x30, 0x3A, 0x41, 0x41, 0x21, 0x7A, 0x3A,
    0x42, 0x40, 0x20, 0x78, 0x00, 0x9D, 0xA0, 0xA0, 0x7D, 0x3D, 0x42, 0x42,
    0x42, 0x3D, 0x3D, 0x40, 0x40, 0x40, 0x3D, 0x3C, 0x24, 0xFF, 0x24, 0x24,
    0x48, 0x7E, 0x49, 0x43, 0x66, 0x2B, 0x2F, 0xFC, 0x2F, 0x2B, 0xFF, 0x09,
    0x29, 0xF6, 0x20, 0xC0, 0x88, 0x7E, 0x09, 0x03, 0x20, 0x54, 0x54, 0x79,
    0x41, 0x00, 0x00, 0x44, 0x7D, 0x41, 0x30, 0x48, 0x48, 0x4A, 0x32, 0x38,
    0x40, 0x40, 0x22, 0x7A, 0x00, 0x7A, 0x0A, 0x0A, 0x72, 0x7D, 0x0D, 0x19,
    0x31, 0x7D, 0x26, 0x29, 0x29, 0x2F, 0x28, 0x26, 0x29, 0x29, 0x29, 0x26,
    0x30, 0x48, 0x4D, 0x40, 0x20, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x38, 0x2F, 0x10, 0xC8, 0xAC, 0xBA, 0x2F, 0x10, 0x28, 0x34,
    0xFA, 0x00, 0x00, 0x7B, 0x00, 0x00, 0x08, 0x14, 0x2A, 0x14, 0x22, 0x22,
    0x14, 0x2A, 0x14, 0x08, 0x55, 0x00, 0x55, 0x00, 0x55, 0xAA, 0x55, 0xAA,
    0x55, 0xAA, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
    0x10, 0x10, 0x10, 0xFF, 0x00, 0x14, 0x14, 0x14, 0xFF, 0x00, 0x10, 0x10,
    0xFF, 0x00, 0xFF, 0x10, 0x10, 0xF0, 0x10, 0xF0, 0x14, 0x14, 0x14, 0xFC,
    0x00, 0x14, 0x14, 0xF7, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x14,
    0x14, 0xF4, 0x04, 0xFC, 0x14, 0x14, 0x17, 0x10, 0x1F, 0x10, 0x10, 0x1F,
    0x10, 0x1F, 0x14, 0x14, 0x14, 0x1F, 0x00, 0x10, 0x10, 0x10, 0xF0, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x10, 0x10, 0x10,
    0x10, 0xF0, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x14, 0x00,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x1F, 0x10, 0x17, 0x00, 0x00, 0xFC,
    0x04, 0xF4, 0x14, 0x14, 0x17, 0x10, 0x17, 0x14, 0x14, 0xF4, 0x04, 0xF4,
    0x00, 0x00, 0xFF, 0x00, 0xF7, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0xF7, 0x00, 0xF7, 0x14, 0x14, 0x14, 0x17, 0x14, 0x10, 0x10, 0x1F, 0x10,
    0x1F, 0x14, 0x14, 0x14, 0xF4, 0x14, 0x10, 0x10, 0xF0, 0x10, 0xF0, 0x00,
    0x00, 0x1F, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x14, 0x00, 0x00, 0x00,
    0xFC, 0x14, 0x00, 0x00, 0xF0, 0x10, 0xF0, 0x10, 0x10, 0xFF, 0x10, 0xFF,
    0x14, 0x14, 0x14, 0xFF, 0x14, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00,
    0x00, 0xF0, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0,
    0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x38, 0x44, 0x44, 0x38, 0x44, 0xFC, 0x4A, 0x4A,
    0x4A, 0x34, 0x7E, 0x02, 0x02, 0x06, 0x06, 0x02, 0x7E, 0x02, 0x7E, 0x02,
    0x63, 0x55, 0x49, 0x41, 0x63, 0x38, 0x44, 0x44, 0x3C, 0x04, 0x40, 0x7E,
    0x20, 0x1E, 0x20, 0x06, 0x02, 0x7E, 0x02, 0x02, 0x99, 0xA5, 0xE7, 0xA5,
    0x99, 0x1C, 0x2A, 0x49, 0x2A, 0x1C, 0x4C, 0x72, 0x01, 0x72, 0x4C, 0x30,
    0x4A, 0x4D, 0x4D, 0x30, 0x30, 0x48, 0x78, 0x48, 0x30, 0xBC, 0x62, 0x5A,
    0x46, 0x3D, 0x3E, 0x49, 0x49, 0x49, 0x00, 0x7E, 0x01, 0x01, 0x01, 0x7E,
    0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x44, 0x44, 0x5F, 0x44, 0x44, 0x40, 0x51,
    0x4A, 0x44, 0x40, 0x40, 0x44, 0x4A, 0x51, 0x40, 0x00, 0x00, 0xFF, 0x01,
    0x03, 0xE0, 0x80, 0xFF, 0x00, 0x00, 0x08, 0x08, 0x6B, 0x6B, 0x08, 0x36,
    0x12, 0x36, 0x24, 0x36, 0x06, 0x0F, 0x09, 0x0F, 0x06, 0x00, 0x00, 0x18,
    0x18, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x30, 0x40, 0xFF, 0x01, 0x01,
    0x00, 0x1F, 0x01, 0x01, 0x1E, 0x00, 0x19, 0x1D, 0x17, 0x12, 0x00, 0x3C,
    0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ==========================================
// DIRECT ST7789 SCREEN SPI DRIVER CLASS
// ==========================================
class DirectST7789 {
public:
  void writeCommand(uint8_t cmd) {
    digitalWrite(TFT_DC, LOW);  // Command Mode
    digitalWrite(TFT_CS, LOW);  // Select Chip
    SPI.transfer(cmd);
    digitalWrite(TFT_CS, HIGH); // Deselect
  }

  void writeData(uint8_t data) {
    digitalWrite(TFT_DC, HIGH); // Data Mode
    digitalWrite(TFT_CS, LOW);  // Select Chip
    SPI.transfer(data);
    digitalWrite(TFT_CS, HIGH); // Deselect
  }

  void writeDataBuf(uint16_t *buf, uint32_t len) {
    digitalWrite(TFT_DC, HIGH); // Data Mode
    digitalWrite(TFT_CS, LOW);  // Select Chip
    
    // SPI.transfer uses highly-optimized FIFO SPI blocks in earlephilhower's pico core
    SPI.transfer((uint8_t*)buf, nullptr, len * 2);
    
    digitalWrite(TFT_CS, HIGH); // Deselect
  }

  void init() {
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_BL, OUTPUT);

    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_BL, HIGH); // Turn on Backlight

    // Hardware Reset Pulse
    digitalWrite(TFT_RST, HIGH);
    delay(50);
    digitalWrite(TFT_RST, LOW);
    delay(50);
    digitalWrite(TFT_RST, HIGH);
    delay(150);

    writeCommand(0x01); // Software Reset
    delay(150);

    writeCommand(0x11); // Sleep Out
    delay(150);

    writeCommand(0x3A); // Interface Pixel Format
    writeData(0x05);    // 16-bit/pixel color (RGB565)

    writeCommand(0x36); // Memory Data Access Control (MADCTL)
    writeData(0x00);    // Portrait mode, standard RGB color order

    writeCommand(0x21); // Display Inversion ON (Required for Waveshare IPS screens)
    
    writeCommand(0x29); // Display ON
    delay(100);
  }

  void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Add the 34 columns offset required for the 1.47-inch LCD panel
    x0 += LCD_COL_OFFSET;
    x1 += LCD_COL_OFFSET;

    writeCommand(0x2A); // Column Address Set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(0x2B); // Row Address Set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(0x2C); // Write RAM command
  }
};

// --- Instantiate Driver ---
DirectST7789 tft;

// --- Double Buffer Frame Buffer (172 x 320 uint16_t = 110,080 bytes) ---
uint16_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

// ==========================================
// CUSTOM VECTOR GRAPHICS DRAWING ENGINE
// ==========================================

// Draw static/dynamic pixel directly to frame buffer
inline void drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
    frameBuffer[y * SCREEN_WIDTH + x] = color;
  }
}

// Clear screen frame buffer
void fillScreen(uint16_t color) {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    frameBuffer[i] = color;
  }
}

// Fast vertical line drawing
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (x < 0 || x >= SCREEN_WIDTH) return;
  int16_t y0 = y < 0 ? 0 : y;
  int16_t y1 = (y + h) > SCREEN_HEIGHT ? SCREEN_HEIGHT : (y + h);
  for (int16_t i = y0; i < y1; i++) {
    frameBuffer[i * SCREEN_WIDTH + x] = color;
  }
}

// Fast horizontal line drawing
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if (y < 0 || y >= SCREEN_HEIGHT) return;
  int16_t x0 = x < 0 ? 0 : x;
  int16_t x1 = (x + w) > SCREEN_WIDTH ? SCREEN_WIDTH : (x + w);
  uint16_t *row = &frameBuffer[y * SCREEN_WIDTH];
  for (int16_t i = x0; i < x1; i++) {
    row[i] = color;
  }
}

// Draw filled solid rectangle
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (int16_t j = 0; j < h; j++) {
    drawFastHLine(x, y + j, w, color);
  }
}

// Bresenham's line algorithm for arbitrary lines
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int16_t err = dx + dy, e2;
  
  while (true) {
    drawPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

// Draw hollow circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  
  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);
  
  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

// Draw filled solid circle
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  for (int16_t y = -r; y <= r; y++) {
    for (int16_t x = -r; x <= r; x++) {
      if (x * x + y * y <= r * r) {
        drawPixel(x0 + x, y0 + y, color);
      }
    }
  }
}

// Draw custom rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  drawFastHLine(x + r, y, w - 2 * r, color);
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color);
  drawFastVLine(x, y + r, h - 2 * r, color);
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color);
  
  // Renders corners mathematically
  for (int16_t j = 0; j < r; j++) {
    for (int16_t i = 0; i < r; i++) {
      if ((i - r)*(i - r) + (j - r)*(j - r) <= r*r && (i - r + 1)*(i - r + 1) + (j - r)*(j - r) > r*r) {
        drawPixel(x + i, y + j, color);
        drawPixel(x + w - 1 - i, y + j, color);
        drawPixel(x + i, y + h - 1 - j, color);
        drawPixel(x + w - 1 - i, y + h - 1 - j, color);
      }
    }
  }
}

// Draw filled rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  if (r == 0) {
    for (int16_t j = 0; j < h; j++) {
      drawFastHLine(x, y + j, w, color);
    }
    return;
  }
  for (int16_t j = y; j < y + h; j++) {
    for (int16_t i = x; i < x + w; i++) {
      // Excludes outside corners of the capsule circle bounds
      if (i < x + r && j < y + r && (i - (x + r))*(i - (x + r)) + (j - (y + r))*(j - (y + r)) > r*r) continue;
      if (i > x + w - 1 - r && j < y + r && (i - (x + w - 1 - r))*(i - (x + w - 1 - r)) + (j - (y + r))*(j - (y + r)) > r*r) continue;
      if (i < x + r && j > y + h - 1 - r && (i - (x + r))*(i - (x + r)) + (j - (y + h - 1 - r))*(j - (y + h - 1 - r)) > r*r) continue;
      if (i > x + w - 1 - r && j > y + h - 1 - r && (i - (x + w - 1 - r))*(i - (x + w - 1 - r)) + (j - (y + h - 1 - r))*(j - (y + h - 1 - r)) > r*r) continue;
      drawPixel(i, j, color);
    }
  }
}

// Draw single monospaced char from font array
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if (c < 32 || c > 127) return; // Print only valid ASCII symbols
  
  int fontIndex = (c - 32) * 5;
  for (int8_t i = 0; i < 5; i++) {
    uint8_t line = font[fontIndex + i];
    for (int8_t j = 0; j < 8; j++) {
      if (line & (1 << j)) {
        if (size == 1) {
          drawPixel(x + i, y + j, color);
        } else {
          fillRoundRect(x + i * size, y + j * size, size, size, 0, color);
        }
      } else if (bg != color) {
        if (size == 1) {
          drawPixel(x + i, y + j, bg);
        } else {
          fillRoundRect(x + i * size, y + j * size, size, size, 0, bg);
        }
      }
    }
  }
}

// Draw monospaced string
void drawString(int16_t x, int16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size) {
  while (*str) {
    drawChar(x, y, *str, color, bg, size);
    x += 6 * size; // 5 px width + 1 px horizontal spacing
    str++;
  }
}

// ==========================================
// SYSTEM DATA & STATE VARIATION
// ==========================================

struct ParticleStar {
  float x;
  float y;
  float speed;
  uint16_t color;
};

#define NUM_STARS 12
ParticleStar stars[NUM_STARS];

bool isBlinking = false;
uint32_t blinkTimer = 0;
uint32_t nextBlinkTime = 3000;
const uint16_t BLINK_DURATION = 150; // ms

// --- Setup Function ---
void setup() {
  // Config BL pin as output and turn it on
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Initialize Hardware SPI0 on standard Waveshare pins
  SPI.setTX(TFT_MOSI);
  SPI.setSCK(TFT_CLK);
  SPI.begin();

  // Initialize display controller registers
  tft.init();

  // Seed the random number generator
  randomSeed(rp2040.getFreeHeap());

  // Initialize Background Starfield
  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].x = random(4, SCREEN_WIDTH - 4);
    stars[i].y = random(30, SCREEN_HEIGHT - 30);
    stars[i].speed = random(5, 16) / 10.0; // 0.5 to 1.6 pixels per frame
    stars[i].color = (random(0, 3) == 0) ? COLOR_CYAN_GLOW : COLOR_DARK_CYAN;
  }
}

// --- Dynamic Rotating Tech Ring Drawing Helper ---
void drawTechRing(int16_t cx, int16_t cy, int16_t r, float angleOffset, int segments, int gapAngleDeg, uint16_t color, int thickness) {
  float step = 0.04; // Angular resolution (radians)
  float gapRad = (gapAngleDeg * PI) / 180.0;
  
  for (int i = 0; i < segments; i++) {
    float segStart = angleOffset + (i * 2.0 * PI / segments);
    float segEnd = segStart + (2.0 * PI / segments) - gapRad;
    
    for (float a = segStart; a < segEnd; a += step) {
      float cos_a = cos(a);
      float sin_a = sin(a);
      for (int t = 0; t < thickness; t++) {
        int16_t cur_r = r - t;
        int16_t x = cx + cur_r * cos_a;
        int16_t y = cy + cur_r * sin_a;
        drawPixel(x, y, color);
      }
    }
  }
}

// --- Cute AI Mascot Eye Drawing Helper ---
void drawMascotEye(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  fillRoundRect(x - w/2, y - h/2, w, h, w/2, color);
}

// --- AI Smile Mouth Drawing Helper ---
void drawSmile(int16_t cx, int16_t cy, int16_t r, uint16_t color) {
  for (float a = 0.25 * PI; a <= 0.75 * PI; a += 0.05) {
    int16_t x = cx + r * cos(a);
    int16_t y = cy + r * sin(a);
    drawPixel(x, y, color);
    drawPixel(x, y + 1, color); // Double thickness
  }
}

// --- Main Loop ---
void loop() {
  // Clear canvas buffer to deep space background
  fillScreen(COLOR_DEEP_BLUE);

  float t = millis() / 1000.0;
  uint32_t currentMillis = millis();

  // ==========================================
  // 1. DRAW BACKGROUND COSMIC STARFIELD
  // ==========================================
  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].y += stars[i].speed;
    if (stars[i].y > SCREEN_HEIGHT - 35) {
      stars[i].y = 35;
      stars[i].x = random(4, SCREEN_WIDTH - 4);
    }
    drawPixel(stars[i].x, stars[i].y, stars[i].color);
  }

  // ==========================================
  // 2. DRAW SCI-FI TOP HEADER & STATUS BAR
  // ==========================================
  // Header Bracket Accents
  drawLine(10, 10, 20, 10, COLOR_DARK_CYAN);
  drawLine(10, 10, 10, 30, COLOR_DARK_CYAN);
  drawLine(SCREEN_WIDTH - 10, 10, SCREEN_WIDTH - 20, 10, COLOR_DARK_CYAN);
  drawLine(SCREEN_WIDTH - 10, 10, SCREEN_WIDTH - 10, 30, COLOR_DARK_CYAN);
  
  // Header Text ("AI AGENT")
  drawString(14, 13, "AI AGENT", COLOR_CYAN, COLOR_CYAN, 1);

  // Status panel circles
  fillCircle(SCREEN_WIDTH/2 - 8, 30, 2, COLOR_MAGENTA);
  fillCircle(SCREEN_WIDTH/2, 30, 2, COLOR_PURPLE);
  fillCircle(SCREEN_WIDTH/2 + 8, 30, 2, COLOR_CYAN);

  // WiFi Signal Icon
  fillCircle(114, 19, 1, COLOR_WHITE);
  drawPixel(112, 17, COLOR_WHITE); drawPixel(113, 16, COLOR_WHITE); drawPixel(114, 16, COLOR_WHITE); drawPixel(115, 16, COLOR_WHITE); drawPixel(116, 17, COLOR_WHITE);
  drawPixel(110, 14, COLOR_WHITE); drawPixel(111, 13, COLOR_WHITE); drawPixel(112, 12, COLOR_WHITE); drawPixel(113, 12, COLOR_WHITE); drawPixel(114, 12, COLOR_WHITE); drawPixel(115, 12, COLOR_WHITE); drawPixel(116, 12, COLOR_WHITE); drawPixel(117, 13, COLOR_WHITE); drawPixel(118, 14, COLOR_WHITE);

  // Bluetooth Icon
  int bx = 130; int by = 11;
  drawLine(bx, by, bx, by + 10, COLOR_WHITE);
  drawLine(bx, by, bx + 3, by + 3, COLOR_WHITE);
  drawLine(bx + 3, by + 3, bx, by + 5, COLOR_WHITE);
  drawLine(bx, by + 5, bx + 3, by + 7, COLOR_WHITE);
  drawLine(bx + 3, by + 7, bx, by + 10, COLOR_WHITE);
  drawLine(bx, by + 5, bx - 3, by + 2, COLOR_WHITE);
  drawLine(bx, by + 5, bx - 3, by + 8, COLOR_WHITE);

  // Battery Icon
  drawRoundRect(144, 11, 18, 10, 1, COLOR_CYAN);
  fillRect(162, 14, 2, 4, COLOR_CYAN);
  // Battery animation cycle
  int batteryCharge = ((int)(t * 1.5)) % 4;
  for (int b = 0; b < batteryCharge; b++) {
    fillRoundRect(146 + (b * 4), 13, 3, 6, 0, COLOR_GREEN);
  }

  // ==========================================
  // 3. DRAW CENTRAL HOLOGRAPHIC TECH-RINGS
  // ==========================================
  int16_t cx = SCREEN_WIDTH / 2; // Center X (86)
  int16_t cy = 118;              // Center Y

  // Pulse outer holographic halo glow
  float pulseHalo = abs(sin(t * 2.0));
  drawCircle(cx, cy, 56 + pulseHalo * 2, COLOR_VIOLET);
  drawCircle(cx, cy, 55 + pulseHalo * 2, COLOR_DARK_CYAN);

  // Segmented Outer Ring (CW rotation)
  drawTechRing(cx, cy, 49, t * 1.2, 3, 40, COLOR_CYAN, 2);

  // Segmented Middle Ring (CCW rotation)
  drawTechRing(cx, cy, 44, -t * 1.8, 6, 25, COLOR_PURPLE, 1);

  // Inner circular dotted tick marks
  for (int deg = 0; deg < 360; deg += 30) {
    float angle = (deg * PI) / 180.0;
    int16_t tx = cx + 37 * cos(angle);
    int16_t ty = cy + 37 * sin(angle);
    drawPixel(tx, ty, COLOR_CYAN_GLOW);
  }

  // Brackets crosshair accents
  drawLine(cx - 56, cy, cx - 52, cy, COLOR_CYAN);
  drawLine(cx + 52, cy, cx + 56, cy, COLOR_CYAN);
  drawLine(cx, cy - 56, cx, cy - 52, COLOR_CYAN);
  drawLine(cx, cy + 52, cx, cy + 56, COLOR_CYAN);

  // ==========================================
  // 4. DRAW BREATHING MASCOT CUTE AI FACE
  // ==========================================
  // State machine controlling blinking sequence
  if (currentMillis - blinkTimer >= nextBlinkTime) {
    isBlinking = true;
    blinkTimer = currentMillis;
    nextBlinkTime = random(2500, 5500); // Blinks every 2.5 to 5.5s
  }
  if (isBlinking && (currentMillis - blinkTimer >= BLINK_DURATION)) {
    isBlinking = false;
  }

  // Gentle mascot breathing scale
  float breathScale = 1.0 + 0.05 * sin(t * 3.5);
  
  if (isBlinking) {
    // Closed horizontal flat eyes during a blink
    fillRoundRect(cx - 16, cy - 1, 9, 3, 1, COLOR_CYAN);
    fillRoundRect(cx + 7, cy - 1, 9, 3, 1, COLOR_CYAN);
  } else {
    // Open vertical rounded capsule eyes
    int16_t eyeWidth = 5;
    int16_t eyeHeight = (int16_t)(13.0 * breathScale);
    
    // Glowing cyan outline, with solid clean white center
    drawMascotEye(cx - 12, cy - 2, eyeWidth + 2, eyeHeight + 2, COLOR_CYAN_GLOW);
    drawMascotEye(cx - 12, cy - 2, eyeWidth, eyeHeight, COLOR_WHITE);
    
    drawMascotEye(cx + 12, cy - 2, eyeWidth + 2, eyeHeight + 2, COLOR_CYAN_GLOW);
    drawMascotEye(cx + 12, cy - 2, eyeWidth, eyeHeight, COLOR_WHITE);
  }

  // Friendly smile mouth arc
  int16_t smileRadius = (int16_t)(7.0 * breathScale);
  drawSmile(cx, cy - 1, smileRadius, COLOR_CYAN);

  // ==========================================
  // 5. DRAW AUDIO SPECTROMETER WAVEFORM
  // ==========================================
  int16_t waveY = 205; // Center height coordinates
  
  // Spectrum bracket outlines
  drawLine(15, waveY - 14, 11, waveY - 14, COLOR_CYAN);
  drawLine(11, waveY - 14, 11, waveY + 14, COLOR_CYAN);
  drawLine(11, waveY + 14, 15, waveY + 14, COLOR_CYAN);

  drawLine(SCREEN_WIDTH - 15, waveY - 14, SCREEN_WIDTH - 11, waveY - 14, COLOR_CYAN);
  drawLine(SCREEN_WIDTH - 11, waveY - 14, SCREEN_WIDTH - 11, waveY + 14, COLOR_CYAN);
  drawLine(SCREEN_WIDTH - 11, waveY + 14, SCREEN_WIDTH - 15, waveY + 14, COLOR_CYAN);

  // Renders 23 composite spectrum bars
  const int numBars = 23;
  const int barSpacing = 5;
  int startBarX = cx - ((numBars - 1) * barSpacing) / 2;

  for (int i = 0; i < numBars; i++) {
    int barX = startBarX + (i * barSpacing);
    float dist = abs(i - (numBars - 1) / 2.0) / ((numBars - 1) / 2.0); // 0.0 (center) to 1.0 (edges)
    
    // Gaussian envelope dampening to zero at outer limits
    float envelope = exp(-3.2 * dist * dist);
    
    // Harmonic wave math synthesis
    float wave1 = sin(i * 0.45 - t * 9.5) * 12.0;
    float wave2 = cos(i * 0.95 + t * 15.0) * 7.0;
    float wave3 = sin(i * 1.6 - t * 24.0) * 3.5;
    
    int barHeight = (int)abs((wave1 + wave2 + wave3) * envelope) + 2;
    if (barHeight > 15) barHeight = 15; // Clip screen boundaries

    // Horizontal Sweeping Color Gradient (Cyan in center, Magenta at borders)
    uint8_t r = (uint8_t)(0   + (176 - 0)   * dist);
    uint8_t g = (uint8_t)(243 + (0 - 243)   * dist);
    uint8_t b = (uint8_t)(255 + (255 - 255) * dist);
    uint16_t barColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

    // Draw the vertical line inside buffer
    drawFastVLine(barX, waveY - barHeight, barHeight * 2, barColor);
    // Render top/bottom bright accent pixels
    drawPixel(barX, waveY - barHeight - 1, COLOR_WHITE);
    drawPixel(barX, waveY + barHeight, COLOR_WHITE);
  }

  // ==========================================
  // 6. DRAW TERMINAL CONSOLE & FOOTER STATS
  // ==========================================
  // Microphone Indicator Graphic
  int mx = 18; int my = 250;
  fillRoundRect(mx, my, 4, 8, 2, COLOR_CYAN);
  drawLine(mx - 2, my + 4, mx - 2, my + 6, COLOR_CYAN);
  drawLine(mx + 5, my + 4, mx + 5, my + 6, COLOR_CYAN);
  drawLine(mx - 2, my + 6, mx + 5, my + 6, COLOR_CYAN);
  drawLine(mx + 1, my + 6, mx + 1, my + 9, COLOR_CYAN);
  drawLine(mx - 1, my + 9, mx + 3, my + 9, COLOR_CYAN);

  // Trailing dot listening console animation
  char listenText[18];
  int dotCount = ((int)(t * 2.2)) % 4;
  if (dotCount == 0) strcpy(listenText, "Listening");
  else if (dotCount == 1) strcpy(listenText, "Listening.");
  else if (dotCount == 2) strcpy(listenText, "Listening..");
  else strcpy(listenText, "Listening...");

  drawString(30, 252, listenText, COLOR_WHITE, COLOR_WHITE, 1);

  // Status visualizer pulse dot
  float statusPulse = abs(sin(t * 3.5));
  fillCircle(150, 255, 3, COLOR_CYAN);
  drawCircle(150, 255, 4 + statusPulse * 3, COLOR_DARK_CYAN);

  // Console log frame borders
  drawRoundRect(10, 276, SCREEN_WIDTH - 20, 24, 4, COLOR_DARK_GRAY);

  // Braces terminal symbol
  drawString(18, 284, "{}", COLOR_MAGENTA, COLOR_MAGENTA, 1);

  // Waiting text
  drawString(36, 284, "Waiting for command", COLOR_GRAY, COLOR_GRAY, 1);

  // Decorative chevrons bottom accent slashes
  drawLine(10, 310, SCREEN_WIDTH - 10, 310, COLOR_DARK_GRAY);
  for (int c = 0; c < 5; c++) {
    int cx_line = 72 + c * 6;
    drawLine(cx_line, 316, cx_line + 3, 312, COLOR_CYAN_GLOW);
    drawLine(cx_line + 1, 316, cx_line + 4, 312, COLOR_CYAN_GLOW);
  }

  // ==========================================
  // 7. FLICKER-FREE FRAMEBUFFER FLUSH
  // ==========================================
  tft.setWindow(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  tft.writeDataBuf(frameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT);
  
  // Sync frame delay (~50 FPS regulation)
  delay(12);
}
