// Регулятор громкости и тембра - https://rcl-radio.ru/?p=112468

#define IR2 0x33B820DF // button encoder
#define IR3 0x33B8946B // mute
#define IR4 0x33B810EF // >>>
#define IR5 0x33B8E01F // <<<
#define IR6 0x33B844BB // INPUT

#include <LC75341.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // http://forum.rcl-radio.ru/misc.php?action=pan_download&item=45&download=1
#include <Encoder.h>           // https://rcl-radio.ru/wp-content/uploads/2019/05/Encoder.zip       
#include <EEPROM.h>
#include <MsTimer2.h>          // https://rcl-radio.ru/wp-content/uploads/2018/11/MsTimer2.zip 
#include <boarddefs.h>         // входит в состав библиотеки IRremote
#include <IRremote.h>          // https://rcl-radio.ru/wp-content/uploads/2019/06/IRremote.zip

LC75341 lc(3,4,2);   // CE,DI,CL 
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
Encoder myEnc(9, 8);// DT, CLK
IRrecv irrecv(12); // указываем вывод модуля IR приемника
decode_results ir; 

byte v1[8] = {0b00111,0b00111,0b00111,0b00111,0b00111,0b00111,0b00111,0b00111};
byte v2[8] = {0b00111,0b00111,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000};      
byte v3[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111};
byte v4[8] = {0b11111,0b11111,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111};
byte v5[8] = {0b11100,0b11100,0b00000,0b00000,0b00000,0b00000,0b11100,0b11100};
byte v6[8] = {0b11100,0b11100,0b11100,0b11100,0b11100,0b11100,0b11100,0b11100};
byte v7[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00111,0b00111};
byte v8[8] = {0b11111,0b11111,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000}; 
unsigned long times,oldPosition  = -999,newPosition;
byte a[6],d1,d2,d3,d4,d5,d6,e1,e2,e3,w,w2,x,gr1,gr2,www;
int menu,vol_reg,in_reg,mute_reg,treb_reg,menu0=100,treb_print,bass_reg,bass_print,gain0_print;
int chl,chr,i,gain0,gain1,gain2,gain3,gain4,gain5,ball,fun,vol_old;



void setup(){
  irrecv.enableIRIn();lcd.init();lcd.backlight();Wire.begin();
  MsTimer2::set(1, to_Timer);MsTimer2::start();
  lcd.createChar(1, v1);lcd.createChar(2, v2);lcd.createChar(3, v3);lcd.createChar(4, v4);
  lcd.createChar(5, v5);lcd.createChar(6, v6);lcd.createChar(7, v7);lcd.createChar(8, v8);
  if(EEPROM.read(100)!=0){for(int i=0;i<101;i++){EEPROM.update(i,0);}}// очистка памяти при первом включении  
  pinMode(10,INPUT);        // encoder SW
  pinMode(11,INPUT_PULLUP); // input button
  pinMode(7,INPUT_PULLUP);  // mute button
  delay(100);
  vol_reg = EEPROM.read(0);treb_reg = EEPROM.read(1)-5;bass_reg = EEPROM.read(2);gain1 = EEPROM.read(4);
  gain2 = EEPROM.read(5);gain3 = EEPROM.read(6);gain4 = EEPROM.read(7);gain5 = EEPROM.read(8);
  in_reg = EEPROM.read(9);ball = EEPROM.read(10)-4;
  switch(in_reg){
     case 0: gain0 = gain1;break;
     case 1: gain0 = gain2;break;
     case 2: gain0 = gain3;break;
     case 3: gain0 = gain4;break;
     }
  audio_L();
  audio_R();
  }

void loop(){
/// IR ////////////////////////////////////////
   if ( irrecv.decode( &ir )) {Serial.print("0x");Serial.println( ir.value,HEX);irrecv.resume();times=millis();w=1;w2=1;}// IR приемник - чтение, в мониторе порта отображаются коды кнопок
   if(ir.value==0){gr1=0;gr2=0;}// запрет нажатий не активных кнопок пульта  
/// BUTTON ///////////////////////////////////
   if(mute_reg==0){   
     if(digitalRead(10)==LOW||ir.value==IR2){menu++;gr1=0;gr2=0;cl();times=millis();w=1;w2=1;if(menu>3){menu=0;}}
     if((digitalRead(11)==LOW||ir.value==IR6)){in_reg++;menu=4;cl();times=millis();w=1;w2=1;www=1;if(in_reg>3){in_reg=0;}}
     }
     if((digitalRead(7)==LOW||ir.value==IR3)&&mute_reg==0){mute_reg=1;menu=100;cl();vol_old=vol_reg;vol_reg=79;audio_R();audio_L();lcd.setCursor(6,0);lcd.print("MUTE");}
     if((digitalRead(7)==LOW||ir.value==IR3)&&mute_reg==1){mute_reg=0;menu=0;cl();vol_reg=vol_old;audio_R();audio_L();}
 
////////////// VOLUME ///////////////////////////////////////////////////////////////////
 if(menu==0){
   if(ir.value==IR5){vol_reg++;gr1=1;gr2=0;cl1();times=millis();w=1;w2=1;vol_func();audio_R();audio_L();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){vol_reg++;gr2=0;cl1();times=millis();w=1;w2=1;vol_func();audio_R();audio_L();}// кнопка >>>>>>
   if(ir.value==IR4){vol_reg--;gr1=0;gr2=1;cl1();times=millis();w=1;w2=1;vol_func();audio_R();audio_L();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){vol_reg--;gr1=0;cl1();times=millis();w=1;w2=1;vol_func();audio_R();audio_L();}// кнопка <<<<<<
 
   if (newPosition != oldPosition){oldPosition = newPosition;vol_reg=vol_reg+newPosition;myEnc.write(0);newPosition=0;times=millis();w=1;w2=1;vol_func();audio_R();audio_L();}
   a[0]= (79-vol_reg)/10;a[1]=(79-vol_reg)%10;
   for(x=0;x<2;x++){switch(x){case 0: e1=10,e2=11,e3=12;break;case 1: e1=13,e2=14,e3=15;break;}digit();}
   if(mute_reg==0){lcd.setCursor(0,0);lcd.print("VOLUME");}else{lcd.setCursor(0,0);lcd.print("MUTE");}
   lcd.setCursor(0,1);lcd.print("INPUT ");lcd.print(in_reg+1);
  }
////////////// TREBLE ///////////////////////////////////////////////////////////////////
 if(menu==1){
   if(ir.value==IR4){treb_reg++;gr1=1;gr2=0;cl1();delay(200);times=millis();w=1;w2=1;treb_func();audio_R();audio_L();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){treb_reg++;gr2=0;cl1();delay(200);times=millis();w=1;w2=1;treb_func();audio_R();audio_L();}// кнопка >>>>>>
   if(ir.value==IR5){treb_reg--;gr1=0;gr2=1;cl1();delay(200);times=millis();w=1;w2=1;treb_func();audio_R();audio_L();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){treb_reg--;gr1=0;cl1();delay(200);times=millis();w=1;w2=1;treb_func();audio_R();audio_L();}// кнопка <<<<<<
 
   if (newPosition != oldPosition){oldPosition = newPosition;treb_reg=treb_reg-newPosition;myEnc.write(0);newPosition=0;times=millis();w=1;w2=1;treb_func();
   audio_R();audio_L();}
   if(treb_reg<0){treb_print = abs(treb_reg)*2;lcd.setCursor(7,0);lcd.write((uint8_t)3);}else{treb_print = treb_reg*2;lcd.setCursor(7,0);lcd.print(" ");}
   a[0]= treb_print/10;a[1]=treb_print%10;
   for(x=0;x<2;x++){switch(x){case 0: e1=8,e2=9,e3=10;break;case 1: e1=11,e2=12,e3=13;break;}digit();}
   lcd.setCursor(0,0);lcd.print("TREBLE");lcd.setCursor(0,1);lcd.print("CONTROL");lcd.setCursor(14,0);lcd.print("dB");
   } 
////////////// BASS ///////////////////////////////////////////////////////////////////
 if(menu==2){
   if(ir.value==IR4){bass_reg++;gr1=1;gr2=0;cl1();delay(200);times=millis();w=1;w2=1;bass_func();audio_R();audio_L();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){bass_reg++;gr2=0;cl1();delay(200);times=millis();w=1;w2=1;bass_func();audio_R();audio_L();}// кнопка >>>>>>
   if(ir.value==IR5){bass_reg--;gr1=0;gr2=1;cl1();delay(200);times=millis();w=1;w2=1;bass_func();audio_R();audio_L();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){bass_reg--;gr1=0;cl1();delay(200);times=millis();w=1;w2=1;bass_func();audio_R();audio_L();}// кнопка <<<<<<
 
   if (newPosition != oldPosition){oldPosition = newPosition;bass_reg=bass_reg-newPosition;myEnc.write(0);newPosition=0;times=millis();w=1;w2=1;bass_func();
   audio_R();audio_L();}
   bass_print = abs(bass_reg)*2;
   a[0]= bass_print/10;a[1]=bass_print%10;
   for(x=0;x<2;x++){switch(x){case 0: e1=8,e2=9,e3=10;break;case 1: e1=11,e2=12,e3=13;break;}digit();}
   lcd.setCursor(0,0);lcd.print("BASS");lcd.setCursor(0,1);lcd.print("CONTROL");lcd.setCursor(14,0);lcd.print("dB");
   }    

//////// BALANCE /////////////////////////////////////////////////////////////// 
 if(menu==3){ 
   if(ir.value==IR4){ball++;gr1=1;gr2=0;cl1();times=millis();w=1;w2=1;ball_fun();audio_R();audio_L();}// кнопка > 
   if(ir.value==IR5){ball--;gr1=0;gr2=1;cl1();times=millis();w=1;w2=1;ball_fun();audio_R();audio_L();}// кнопка <
 
   if (newPosition != oldPosition){oldPosition = newPosition;
   ball=ball-newPosition;myEnc.write(0);newPosition=0;times=millis();w=1;w2=1;ball_fun();audio_R();audio_L();}
   lcd.setCursor(4,0);lcd.print(F("   <>   "));lcd.setCursor(4,1);lcd.print(F("CHL  CHR"));
   chl=(4+ball)-4;chr=(4-ball)-4;
   if(chl<0){lcd.setCursor(12,0);chl=abs(chl);lcd.write((uint8_t)3);}else{lcd.setCursor(12,0);lcd.print(" ");}
   if(chr<0){lcd.setCursor(0,0);chr=abs(chr);lcd.write((uint8_t)3);}else{lcd.setCursor(0,0);lcd.print(" ");}
   if(w2==1){w2=0;a[0]=chl;a[1]=chr;
   for(i=0;i<2;i++){
      switch(i){
        case 0: e1=1,e2=2,e3=3;break;
        case 1: e1=13,e2=14,e3=15;break;
        }
      switch(a[i]){
        case 0: d1=1,d2=8,d3=6,d4=1,d5=3,d6=6;break;
        case 1: d1=32,d2=2,d3=6,d4=32,d5=32,d6=6;break;
        case 2: d1=2,d2=8,d3=6,d4=1,d5=4,d6=5;break;
        case 3: d1=2,d2=4,d3=6,d4=7,d5=3,d6=6;break;
        case 4: d1=1,d2=3,d3=6,d4=32,d5=32,d6=6;break;
        case 5: d1=1,d2=4,d3=5,d4=7,d5=3,d6=6;break;
        case 6: d1=1,d2=4,d3=5,d4=1,d5=3,d6=6;break;
        case 7: d1=1,d2=8,d3=6,d4=32,d5=32,d6=6;break;
        case 8: d1=1,d2=4,d3=6,d4=1,d5=3,d6=6;break;
        case 9: d1=1,d2=4,d3=6,d4=7,d5=3,d6=6;break;
    }
      lcd.setCursor(e1,0);lcd.write((uint8_t)d1);lcd.setCursor(e2,0);lcd.write((uint8_t)d2);lcd.setCursor(e3,0);lcd.write((uint8_t)d3);
      lcd.setCursor(e1,1);lcd.write((uint8_t)d4);lcd.setCursor(e2,1);lcd.write((uint8_t)d5);lcd.setCursor(e3,1);lcd.write((uint8_t)d6);
 }}}  
 ////////////// INPUT GAIN ///////////////////////////////////////////////////////////////////
 if(menu==4){
  switch(in_reg){
     case 0: gain0 = gain1;break;
     case 1: gain0 = gain2;break;
     case 2: gain0 = gain3;break;
     case 3: gain0 = gain4;break;
     }
   if(ir.value==IR4){gain0++;gr1=1;gr2=0;cl1();times=millis();w=1;w2=1;gain_func();audio_R();audio_L();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain0++;gr2=0;cl1();times=millis();w=1;w2=1;gain_func();audio_R();audio_L();}// кнопка >>>>>>
   if(ir.value==IR5){gain0--;gr1=0;gr2=1;cl1();times=millis();w=1;w2=1;gain_func();audio_R();audio_L();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain0--;gr1=0;cl1();times=millis();w=1;w2=1;gain_func();audio_R();audio_L();}// кнопка <<<<<<
 
  if (newPosition != oldPosition){oldPosition = newPosition;gain0=gain0-newPosition;myEnc.write(0);newPosition=0;times=millis();www=1;w=1;w2=1;gain_func();}
  switch(in_reg){
     case 0: gain1 = gain0;break;
     case 1: gain2 = gain0;break;
     case 2: gain3 = gain0;break;
     case 3: gain4 = gain0;break;
     }  
   gain0_print = gain0*2;
   a[0]= gain0_print/10;a[1]=gain0_print%10;
   for(x=0;x<2;x++){switch(x){case 0: e1=8,e2=9,e3=10;break;case 1: e1=11,e2=12,e3=13;break;}digit();}
   if(www==1){audio_R();audio_L();www=0;}
   lcd.setCursor(0,0);lcd.print("IN GAIN");lcd.setCursor(14,0);lcd.print("dB");
   lcd.setCursor(0,1);lcd.print("INPUT ");lcd.print(in_reg+1);}    
 
////////////////////////////////////////////////////////////////////////////////////////////   
////////////////// EEPROM //////////////////////////////////////////////////////////////
 if(millis()-times>10000 && w==1 && mute_reg==0){
     EEPROM.update(0,vol_reg);EEPROM.update(1,treb_reg+5);EEPROM.update(2,bass_reg);EEPROM.update(4,gain1);
     EEPROM.update(5,gain2);EEPROM.update(6,gain3);EEPROM.update(7,gain4);EEPROM.update(8,gain5);
     EEPROM.update(9,in_reg);EEPROM.update(10,ball+4);
     if(menu!=0){lcd.clear();menu=0;w=0;}}        
} // loop end


void gain_func(){{if(gain0<0){gain0=0;}if(gain0>15){gain0=15;}}}
void ball_fun(){if(ball>4){ball=4;}if(ball<-4){ball=-4;}}
void bass_func(){if(bass_reg<0){bass_reg=0;}if(bass_reg>10){bass_reg=10;}}
void cl(){ir.value=0;delay(300);lcd.clear();}
void cl1(){ir.value=0;delay(300);}
void treb_func(){if(treb_reg<-5){treb_reg=-5;}if(treb_reg>5){treb_reg=5;}}
void vol_func(){if(vol_reg<4){vol_reg=4;}if(vol_reg>79){vol_reg=79;}}
void to_Timer(){newPosition = myEnc.read()/4;} 

void audio_L(){
  lc.addr();
  lc.set_input(in_reg);   // input 1...4 = byte 0...3 (byte 4...7 = All switches off)
  lc.set_gain(gain0);    // gain 0...30 dB step 2 dB = byte 0...15
  lc.set_volume(vol_reg-ball);  // volume 0...-79 dB = byte 0...79
  lc.set_treble(treb_reg);  // treble -10...10 dB step 2 dB = int -5...5 
  lc.set_bass(bass_reg);    // bass 0...20 dB step 2 dB = int 0...10
  lc.set_ch(1);      // Channel Selection RCH = byte 2, LCH = byte 1, Left and right together = byte 3 
  lc.test();
  }

void audio_R(){
  lc.addr();
  lc.set_input(in_reg);   // input 1...4 = byte 0...3 (byte 4...7 = All switches off)
  lc.set_gain(gain0);    // gain 0...30 dB step 2 dB = byte 0...15
  lc.set_volume(vol_reg+ball);  // volume 0...-79 dB = byte 0...79
  lc.set_treble(treb_reg);  // treble -10...10 dB step 2 dB = int -5...5 
  lc.set_bass(bass_reg);    // bass 0...20 dB step 2 dB = int 0...10
  lc.set_ch(2);      // Channel Selection RCH = byte 2, LCH = byte 1, Left and right together = byte 3 
  lc.test();
  }  

void digit(){switch(a[x]){
case 0: d1=1,d2=8,d3=6,d4=1,d5=3,d6=6;break;case 1: d1=32,d2=2,d3=6,d4=32,d5=32,d6=6;break;
case 2: d1=2,d2=8,d3=6,d4=1,d5=4,d6=5;break;case 3: d1=2,d2=4,d3=6,d4=7,d5=3,d6=6;break;
case 4: d1=1,d2=3,d3=6,d4=32,d5=32,d6=6;break;case 5: d1=1,d2=4,d3=5,d4=7,d5=3,d6=6;break;
case 6: d1=1,d2=4,d3=5,d4=1,d5=3,d6=6;break;case 7: d1=1,d2=8,d3=6,d4=32,d5=32,d6=6;break;
case 8: d1=1,d2=4,d3=6,d4=1,d5=3,d6=6;break;case 9: d1=1,d2=4,d3=6,d4=7,d5=3,d6=6;break;}
lcd.setCursor(e1,0);lcd.write((uint8_t)d1);lcd.setCursor(e2,0);lcd.write((uint8_t)d2);lcd.setCursor(e3,0);lcd.write((uint8_t)d3);
lcd.setCursor(e1,1);lcd.write((uint8_t)d4);lcd.setCursor(e2,1);lcd.write((uint8_t)d5);lcd.setCursor(e3,1);lcd.write((uint8_t)d6);}
   
