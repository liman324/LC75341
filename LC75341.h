// liman324@yandex.ru rcl-radio.ru
// 15.01.2022

#ifndef LC75341_H
#define LC75341_H

#include <Arduino.h>
class LC75341
{
  public:
    LC75341(uint8_t ce, uint8_t di, uint8_t cl);

    void addr();
    void set_input(byte in);
    // input 1...4 = byte 0...3 (byte 4...7 = All switches off)
    void set_gain(byte gain); 
    // gain 0...30 dB step 2 dB = byte 0...15
    void set_volume(byte vol);
    // volume 0...-79 dB = byte 0...79
    void set_treble(int treb);
    // treble 10...-10 dB step 2 dB = int 5...-5 
    void set_bass(int bass);
    // bass 20...0 dB step 2 dB = int 10...0 
    void set_ch(byte ch);
    // Channel Selection RCH = byte 2, LCH = byte 1, Left and right together = byte 3
    void test();
    	
  private:
        uint8_t CE;
        uint8_t DI;
        uint8_t CL;
};
	
#endif //LC75341_H
