#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


char buffer [50];

void oledPrint(String val1){
  u8g2.clearBuffer();          // clear the internal memory
//  u8g2.setFont(u8g2_font_cupcakemetoyourleader_tr);  // choose a suitable font
// Pas mal  u8g2.setFont(u8g2_font_logisoso24_tn);  // choose a suitable font
  u8g2.setFont(u8g2_font_fub17_tf);  // choose a suitable font
  u8g2.drawStr(0,25, val1.c_str());  // write something to the internal memory
//  u8g2.drawStr(90,12, ltoa(val1, buffer, 10));  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
}
