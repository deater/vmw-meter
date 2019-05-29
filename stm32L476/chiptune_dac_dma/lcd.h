void LCD_Clock_Init(void);
void LCD_Pin_Init(void);
void LCD_Configure(void);
void LCD_Display_Name(void);
void LCD_Display_String(char *string);
void LCD_Clear(void);


void LCD_WriteChar(uint8_t ch, int point, int colon, uint8_t position);

uint32_t LCD_display_int(uint32_t val);
