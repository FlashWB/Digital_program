#include <LiquidCrystal.h>
#include <stdio.h>
#include <Wire.h>
#include <RTClib.h>
/////////////////////////////
//          RTC初始化      //
RTC_DS1307 rtc;

////////////////////////////
//        端口LCD         //
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int timeWidth = 0;
int dataOut = 0;
int lostData = 0;
int n = 0;
///////////////////////////////////
//        解码后时间变量         //
int P0 = 0;
int P1 = 0;
int P2 = 0;
int P3 = 2;
int P4 = 0;
int hour_ = 0;
int minute_ = 0;
int second_ = 0;
int week_ = 0;
int day_ = 0;
int month_ = 0;
int year_ = 0;

//////////////////////////////////////
//        显示和setup()时间变量    //
//    p0~4  没有用除p3
int P0R = 0;
int P1R = 0;
int P2R = 0;
int P3R = 0;
int P4R = 0;
int hour_R = 9;
int minute_R = 10;
int second_R = 0;
int week_R = 5;
int day_R = 15;
int month_R = 6;
int year_R = 2018;
int timewidth[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int bpcarray[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int bpcarrayT[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //校验后正确的存在这里
int getrightbpc = 0;
bool p3ture1, p3ture2, p4ture1, p4ture2;
int led = 53;
int validbpc = 0;
int changed = 0;
int head = 0;

////////////////////////////////////
//            打印LCD上          //
void printrtc() {
  DateTime now = rtc.now();
  lcd.clear();
  ////从第1行开始
  lcd.setCursor(0, 0);
  lcd.print(now.year(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  lcd.print("/");
  //  lcd.print("week");
  switch (week_R) {
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wed"); break;
    case 4: lcd.print("Thu"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sta"); break;
    case 7: lcd.print("Sun"); break;
    default: lcd.print("Mon");
  }
  // lcd.print(week);
  //lcd.print("/");
  // lcd.print((millis()/1000)%10);

  ////从第2行开始
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.print(now.second(), DEC);
  if (P3R > 1) {
    lcd.print("/PM");
  }
  else {
    lcd.print("/AM");
  }

}

/////////////////////////////////
//             接收BPC         //
void receivebpc() {
  int value = analogRead(A0);
  if (value < 100) {
    timeWidth++;
    lostData = 0;
  }
  else {
    if (timeWidth != 0) {
      timewidth[n] = timeWidth;
      dataOut = timeWidth - 1;
      bpcarray[n] = dataOut;
      n++;
    }
    timeWidth = 0;
    //bpcarray[18] = dataOut;
    lostData++;
  }
}

////////////////////////////////
//           解码             //
void decoding(int bpc[]) {
  P1 = bpc[0];
  P2 = bpc[1];
  P3 = bpc[9]; //P3>1 PM
  P4 = bpc[18];
  //秒 分钟
  if (bpc[0] * 20 + 20 == 60) {
    second_ = 0;
    minute_ = bpc[4] * 16 + bpc[5] * 4 + bpc[6] + 1;
  }
  else {
    second_ = bpc[0] * 20 + 20;
    minute_ = bpc[4] * 16 + bpc[5] * 4 + bpc[6];
  }

  //小时
  if (minute_ == 60) {
    hour_ = bpc[2] * 4 + bpc[3];
    minute_ = 0;
  }
  else {
    hour_ = bpc[2] * 4 + bpc[3];
  }
  if (hour_ == 0) {
    hour_ = 12;
  }
  week_ = bpc[7] * 4 + bpc[8]; //星期
  day_ = bpc[10] * 16 + bpc[11] * 4 + bpc[12]; //天
  month_ = bpc[13] * 4 + bpc[14]; //月
  year_ = 2000 + bpc[15] * 16 + bpc[16] * 4 + bpc[17]; //年
}

///////////////////////////////////
//             校验             //
int varify() {
  int p3ji = 0;
  int p4ji = 0;
  int index;

  if (bpcarray[1] == 0) {
    // “P1”、“P2”、“时”、“分”、“星期    奇偶统计
    for (index = 0; index < 9; index++) {
      if (bpcarray[index] == 1 || bpcarray[index] == 2) {
        p3ji++;
      }
    }

    //“日”、“月”、“年”   奇偶统计
    for (index = 10; index < 18; index++) {
      if (bpcarray[index] == 1 || bpcarray[index] == 2) {
        p4ji++;
      }
    }

    p3ture1 = ((p3ji % 2 == 0) && ((bpcarray[9] == 0) || (bpcarray[9] == 2)));
    p3ture2 = ((p3ji % 2 != 0) && ((bpcarray[9] == 1) || (bpcarray[9] == 3)));
    p4ture1 = ((p4ji % 2 == 0) && ((bpcarray[18] == 0) || (bpcarray[18] == 2)));
    p4ture2 = ((p4ji % 2 != 0) && ((bpcarray[18] == 0) || (bpcarray[18] == 2)));
    if ((p3ture1 || p3ture2) && (p4ture1 || p4ture2)) {
      return 1;
    }
    else {
      return 0;
    }

  }
  else {
    //bpcarray[1](P0)!=0
    // lcd.clear();
    //    Serial.println("p2!=0");
  }
}

///////////////////////////////////
//        传递解码后参数         //
void transferT(int yearT, int monthT, int dayT, int weekT, int hourT, int minuteT, int secondT, int P0T, int P1T, int P2T, int P3T, int P4T) {
  year_R = yearT;
  month_R = monthT;
  day_R = dayT;
  week_R = weekT;
  hour_R = hourT;
  minute_R = minuteT;
  second_R = secondT;
  P0R = P0T;
  P1R = P1T;
  P2R = P2T;
  P3R = P3T;
  P4R = P4T;
}


////////////////////////////////
//        setup()            //
void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  lcd.begin(16, 2); //设置LCD数目
  //  if (! rtc.begin()) {
  //    Serial.println("Couldn't find RTC");
  //    while (1);
  //  }
  //  else {
  //    Serial.println("Find RTC!");
  //  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(year_R, month_R, day_R, hour_R, minute_R, second_R));
  }
  else {
    /////////////////////////////////////////////////////////////////////
    //                           RTC写入                               //
    /////////////////////////////////////////////////////////////////////

    rtc.adjust(DateTime(year_R, month_R, day_R, hour_R, minute_R, second_R));
    /////////////////////////////////////////////////////////////////////
    Serial.println("写入成功");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("初始settime");
  int i, j, k, l, m, h, timewidthsum = 0; //k为5分钟
  for (i = 0; i < 5; i++) {
    // i控制分钟
    for (j = 0; j < 60; j++) {
      // j控制1秒
      for (k = 0; k < 10; k++) {
        // k控制0.1秒
        receivebpc();
        if (lostData > 13) {
          //testprintbpc();
          //head++;
          //          for (m = 0; m < 19; m++) {
          //            timewidthsum += timewidth[m];
          //          }
          //          if (72 > timewidthsum >= 18) {
          //            validbpc = 1;
          //            timewidthsum = 0;
          //            for (h = 0; h < 19; h++) {
          //              timewidth[m] = 0;
          //            }
          //          }
          //          else {
          //            validbpc = 0;
          //            timewidthsum = 0;
          //            for (h = 0; h < 19; h++) {
          //              timewidth[m] = 0;
          //            }
          //          }
          validbpc = 1;
          if (validbpc == 1 ) {
            if (varify()) {
              getrightbpc = 1; //校验标志位1
              // 传输保存bpc
              for (l = 0; l < 19; l++) {
                bpcarrayT[1] = bpcarray[1];
              }
            }
            else {
              getrightbpc = 0;
            }
          }
          n = 0;  //数组坐标初始化
          lostData = 0;  //高电平初始化
        }// if(lostData>12)
        if (validbpc == 1) {
          // Serial.println("bpc帧头");
          // blue有效bpc
          if (getrightbpc == 1) {
            digitalWrite(led, HIGH);
            delay(100);
          }// if (getrightbpc==0)
          else {
            digitalWrite(led, HIGH);
            delay(50);
            digitalWrite(led, LOW);
            delay(50);
          }
        }//if(validbpc==1)
        else {
          // validbpc==0
          digitalWrite(led, LOW);
          delay(100);
        }//if(blue==1) else

      }//for (k=0;k<10;k++)  1s

      // 获取rtc时钟模块 时间并打印
      printrtc();
      if (changed == 0) {
        if (getrightbpc == 1) {
          Serial.println("校验通过并打印");
          decoding(bpcarrayT);
          transferT(year_, month_, day_, week_, hour_, minute_, second_, P0, P1, P2, P3, P4);
          setup();
          changed = 1;
        }
      }
    }//for (j=0;j<60;j++)  1s
  }//for (i=0;i<5;i++)  1min
  getrightbpc = 0;
  changed = 0;
}
