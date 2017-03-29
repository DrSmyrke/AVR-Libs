#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd5110.h"

/* delay macro function */
#define lcd_delay() int i;for(i=-32000;i<32000;i++)

/* Command type sent to the lcd */
typedef enum { LCD_CMD  = 0, LCD_DATA = 1 } LcdCmdData;

/* Function prototypes */
void lcd_base_addr(unsigned int addr);
void lcd_send(unsigned char data, LcdCmdData cd);

/* The lcd cursor position */
int lcdCacheIdx;

/* Alphabet lookup */
const unsigned char PROGMEM font5x7 [][5] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
	{ 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 }   // z
};
const unsigned char PROGMEM weekDays [][10] = {
	{0x00,0x7e,0x02,0x02,0x7e,0x00,0x78,0x10,0x78,0x00},   // Пн
	{0x00,0x7e,0x52,0x56,0x3c,0x00,0x08,0x78,0x08,0x00},   // Вт
	{0x00,0x3c,0x42,0x42,0x24,0x00,0x78,0x28,0x38,0x00},   // Ср
	{0x00,0x0e,0x10,0x10,0x7e,0x00,0x08,0x78,0x08,0x00},   // Чт
	{0x00,0x7e,0x02,0x02,0x7e,0x00,0x08,0x78,0x08,0x00},   // Пт
	{0x00,0x3c,0x42,0x42,0x24,0x00,0x7c,0x54,0x74,0x00},   // Сб
	{0x00,0x7e,0x52,0x56,0x3c,0x00,0x78,0x48,0x48,0x00}   // Вс
};
const unsigned char PROGMEM font15x16 [][15] = {
{0x00,0xc0,0xe0,0x30,0x18,0xc,0xc,0x6,0x6,0x6,0x6,0xc,0x1c,0xf8,0xf0}, //0
{0x00,0x00,0x20,0x30,0x10,0x18,0x8,0xfc,0x3e,0x6,0x00,0x00,0x00,0x00,0x00}, //1
{0x00,0x00,0x18,0x18,0xc,0xc,0x6,0x6,0x6,0x8e,0xfc,0x38,0x00,0x00,0x00}, //2
{0x00,0x00,0x18,0x18,0x18,0x98,0xc8,0xe8,0xac,0xbc,0x9c,0x8c,0xc,0x00,0x00}, //3
{0x00,0x00,0x80,0xc0,0x60,0x30,0x18,0xc,0x8e,0xfe,0x1f,0x00,0x00,0x00,0x00}, //4
{0x00,0x00,0x00,0x40,0xf0,0xf8,0x8c,0x84,0x84,0x84,0x84,0x4,0x4,0x4,0x6}, //5
{0x00,0x00,0x00,0xc0,0xe0,0x70,0x18,0xc,0x6,0x6,0x00,0x00,0x00,0x00,0x00}, //6
{0x00,0x00,0x00,0xc,0xc,0xc,0xc,0xc,0xc,0x8c,0xcc,0x6c,0x7c,0x1c,0x4}, //7
{0x00,0x00,0x00,0x60,0xf0,0x98,0x18,0xc,0xc,0x8c,0xf8,0x70,0x20,0x00,0x00}, //8
{0x00,0x00,0x00,0xe0,0xf0,0x18,0xc,0xc,0x86,0x86,0xc6,0xfe,0x3c,0x00,0x00} //9
};
const unsigned char PROGMEM font15x16bottom [][15] = {
{0x1f,0x3f,0x70,0x60,0x60,0x60,0x60,0x30,0x30,0x18,0x1c,0xe,0x7,0x3,0x1}, //0
{0x00,0x60,0x60,0x60,0x60,0x20,0x7e,0x7f,0x30,0x30,0x10,0x18,0x18,0x18,0x00}, //1
{0xc0,0xe0,0x70,0x70,0x38,0x38,0x34,0x36,0x33,0x31,0x30,0x30,0x30,0x30,0x00}, //2
{0x00,0x60,0xe0,0xc0,0xc1,0xc1,0xc0,0xc0,0x60,0x60,0x70,0x39,0x1f,0xe,0x00}, //3
{0x00,0x3,0x3,0x3,0x1,0x1,0x1,0x1,0x7f,0x7f,0x1,0x1,0x1,0x3,0x00}, //4
{0x00,0x30,0x30,0x60,0x60,0x60,0x60,0x60,0x70,0x30,0x39,0x1f,0xe,0x00,0x00}, //5
{0x00,0x00,0x1f,0x3f,0x60,0x7c,0x67,0x63,0x61,0x61,0x31,0x19,0x1f,0xf,0x00}, //6
{0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x78,0xe,0x3,0x1,0x00,0x00,0x00,0x00}, //7
{0x00,0x00,0x00,0x38,0x7c,0x4d,0x47,0x43,0x43,0x67,0x3c,0x38,0x00,0x00,0x00}, //8
{0x00,0x00,0x00,0x00,0x1,0x1,0x1,0x1,0x1,0x78,0x7f,0x3,0x00,0x00,0x00} //9
};

/* Performs IO & LCD controller initialization */
void lcd_init(void){
    // Pull-up on reset pin
    LCD_PORT |= LCD_RST_PIN;
	
	// Set output bits on lcd port
	LCD_DDR |= LCD_RST_PIN | LCD_SCE_PIN | LCD_DC_PIN | LCD_DATA_PIN | LCD_CLK_PIN;
    
	// Wait after VCC high for reset (max 30ms)
    delay(150);
    
    // Toggle display reset pin
    LCD_PORT &= ~LCD_RST_PIN;
    lcd_delay();
    LCD_PORT |= LCD_RST_PIN;

    // Disable LCD controller
    LCD_PORT |= LCD_SCE_PIN;

    lcd_send(0x21, LCD_CMD);  // LCD Extended Commands
    lcd_send(0xC8, LCD_CMD);  // Set LCD Vop(Contrast)
    lcd_send(0x06, LCD_CMD);  // Set Temp coefficent
    lcd_send(0x13, LCD_CMD);  // LCD bias mode 1:48
    lcd_send(0x20, LCD_CMD);  // Standard Commands, Horizontal addressing
    lcd_send(0x0C, LCD_CMD);  // LCD in normal mode
    
    // Clear lcd
    lcd_clear();
	
	// For using printf
	//fdevopen(lcd_chr, 0);
}

/* Set display contrast. Note: No change is visible at ambient temperature */
void lcd_contrast(unsigned char contrast){
	lcd_send(0x21, LCD_CMD);				// LCD Extended Commands
    lcd_send(0x80 | contrast, LCD_CMD);		// Set LCD Vop(Contrast)
    lcd_send(0x20, LCD_CMD);				// LCD std cmds, hori addr mode
}

/* Clears the display */
void lcd_clear(void){
	lcdCacheIdx = 0;
	
	lcd_base_addr(lcdCacheIdx);
	
    // Set the entire cache to zero and write 0s to lcd
	int i;
    for(i=0;i<LCD_CACHE_SIZE;i++) {
		lcd_send(0, LCD_DATA);
    }
	delay(2000);
}

/* Clears an area on a line */
void lcd_clear_area(unsigned char line, unsigned char startX, unsigned char endX){  
    // Start and end positions of line
    int start = (line-1)*84+(startX-1);
    int end = (line-1)*84+(endX-1);
	
	lcd_base_addr(start);
    
    // Clear all data in range from cache
	unsigned int i;
    for(i=start;i<end;i++) {
        lcd_send(0, LCD_DATA);
    }
}

/* Clears an entire text block. (rows of 8 pixels on the lcd) */
void lcd_clear_line(unsigned char line){
    lcd_clear_area(line, 1, LCD_X_RES);
}

/* Sets cursor location to xy location corresponding to basic font size */
void lcd_goto_xy(unsigned char x, unsigned char y){
    lcdCacheIdx = (x-1)*6 + (y-1)*84;
}

/* Sets cursor location to exact xy pixel location on the lcd */
void lcd_goto_xy_exact(unsigned char x, unsigned char y){
    lcdCacheIdx = (x-1) + (y-1)*84;
}

/* Displays a character at current cursor location */
void lcd_chr(char chr){
	lcd_base_addr(lcdCacheIdx);
	// 5 pixel wide characters and add space
	unsigned char i;
	for(i=0;i<5;i++){
		lcd_send(pgm_read_byte(&font5x7[chr-32][i]) << 1, LCD_DATA);
	}
	lcd_send(0, LCD_DATA);
	lcdCacheIdx += 6;
}
void lcd_chrNum(char chr,unsigned char x,unsigned char y){
	lcd_goto_xy_exact(x,y);lcd_base_addr(lcdCacheIdx);
	unsigned char i;
	//for(i=0;i<15;i++){lcd_send(pgm_read_byte(&font15x16[chr][i])<<1,LCD_DATA);}
	for(i=0;i<15;i++){lcd_send(pgm_read_byte(&font15x16[chr][i]),LCD_DATA);}
	lcd_goto_xy_exact(x,y+1);lcd_base_addr(lcdCacheIdx);
	for(i=0;i<15;i++){lcd_send(pgm_read_byte(&font15x16bottom[chr][i]),LCD_DATA);}
}
void lcd_chrWD(char chr,unsigned char x,unsigned char y){
	lcd_goto_xy_exact(x,y);lcd_base_addr(lcdCacheIdx);
	unsigned char i;
	for(i=0;i<10;i++){lcd_send(pgm_read_byte(&weekDays[chr-1][i]),LCD_DATA);}
	lcd_goto_xy_exact(x,y+1);lcd_base_addr(lcdCacheIdx);
	lcdCacheIdx+=12;
}
/* Displays string at current cursor location and increment cursor location */
void lcd_str(char *str){
    while(*str){lcd_chr(*str++);}
}

// Set the base address of the lcd
void lcd_base_addr(unsigned int addr) {
	lcd_send(0x80 |(addr % LCD_X_RES), LCD_CMD);
	lcd_send(0x40 |(addr / LCD_X_RES), LCD_CMD);
}

/* Sends data to display controller */
void lcd_send(unsigned char data, LcdCmdData cd){
	// Data/DC are outputs for the lcd (all low)
	LCD_DDR |= LCD_DATA_PIN | LCD_DC_PIN;
	
    // Enable display controller (active low)
    LCD_PORT &= ~LCD_SCE_PIN;

    // Either command or data
    if(cd == LCD_DATA) {
        LCD_PORT |= LCD_DC_PIN;
    } else {
        LCD_PORT &= ~LCD_DC_PIN;
    }
	unsigned char i;
	for(i=0;i<8;i++) {
	
		// Set the DATA pin value
		if((data>>(7-i)) & 0x01) {
			LCD_PORT |= LCD_DATA_PIN;
		} else {
			LCD_PORT &= ~LCD_DATA_PIN;
		}
		
		// Toggle the clock
		LCD_PORT |= LCD_CLK_PIN;
		LCD_PORT &= ~LCD_CLK_PIN;
	}

	// Disable display controller
    LCD_PORT |= LCD_SCE_PIN;
	
	// Data/DC can be used as button inputs when not sending to LCD (/w pullups)
	LCD_DDR &= ~(LCD_DATA_PIN | LCD_DC_PIN);
	LCD_PORT |= LCD_DATA_PIN | LCD_DC_PIN;
}

void lcd_col(char chr){
	lcd_base_addr(lcdCacheIdx);
	lcd_send(chr, LCD_DATA);
	lcdCacheIdx++;
}

/* It goes back the cursor on LCD for a single step 
   This function added by Vassilis on 01 Jan 2012 */
void lcd_pixelBack(void){
	lcdCacheIdx--;
}

/* Prints on LCD a hex based picture.
   A hex picture can be produced from the "LCDAssistant.exe" windows based software. 
   This function added by Vassilis on 01 Jan 2012 */
void printPictureOnLCD ( const unsigned char *data){
	unsigned int i;

 	lcd_goto_xy(1,1);
	for(i=0;i<LCD_TOTAL_PIXEL_COLUMNS;i++)
		lcd_col(pgm_read_byte(data++));
	delay(10000);
}
