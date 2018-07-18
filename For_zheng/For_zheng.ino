#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int timeWidth = 0;
int dataOut = 0;
int lostData = 0;
int flag = 0;
int temp = 0;
int P0 = 0;
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;
int BPChour = 0;
int BPCminuate = 0;
int BPCsecond = 0;
int BPCweek = 0;
int BPCday = 0;
int BPCmonth = 0;
int BPCyear = 0;
int BPCArray[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
int led = 53;
int ledbling = 0;
int ledshine = 0;
void runtime(int TT, int hourT) {
  BPCsecond = (millis() / 1000 + 1) % TT;
  //Serial.print(BPCsecond);
  if (BPCsecond == 0) {
    BPCminuate++;
  }
  if (BPCminuate == TT) {
    BPChour++;
    BPCminuate == 0;
  }
  if (BPChour == hourT) {
    BPCday++;
    BPChour = 0;
  }
  //没写完自己加


}

void receivebpc() {
  int value = analogRead(A0);
  //高低电平你自己判断
  if (value < 100) {
    timeWidth++;
    lostData = 0;
  }
  else {
    if (timeWidth != 0) {
    dataOut = timeWidth - 1;
    BPCArray[flag] = dataOut;
    flag++;
    timeWidth = 0;
    }
    //BPCArray[18] = dataOut;
    lostData++;
  }
}

void decoding() {
  P1 = BPCArray[0];
  P2 = BPCArray[1];
  P3 = BPCArray[9];
  P4 = BPCArray[18];

  if (BPCArray[0] * 20 + 20 == 60) {
    //BPCsecond = 0;
    BPCminuate = BPCArray[4] * 16 + BPCArray[5] * 4 + BPCArray[6] + 1;
  }
  else {
    //BPCsecond = BPCArray[0] * 20 + 20;
    BPCminuate = BPCArray[4] * 16 + BPCArray[5] * 4 + BPCArray[6];
  }
  if (BPCminuate == 60) {
    BPChour = BPCArray[2] * 4 + BPCArray[3] + 1;
    BPCminuate = 0;
  }
  else {
    BPChour = BPCArray[2] * 4 + BPCArray[3];
  }
  if (BPChour == 0) { //noon 12
    BPChour = 12;
  }
  BPCweek = BPCArray[7] * 4 + BPCArray[8];
  BPCday = BPCArray[10] * 16 + BPCArray[11] * 4 + BPCArray[12];
  BPCmonth = BPCArray[13] * 4 + BPCArray[14];
  BPCyear = 2000 + BPCArray[15] * 16 + BPCArray[16] * 4 + BPCArray[17];
}

void printTime() {
  //打印函数自己加
  lcd.clear();
  lcd.print(BPCyear);
  lcd.print("-");


  lcd.print(BPCmonth);
  lcd.print("-");


  lcd.print(BPCday);
  lcd.print("-");

  switch (BPCweek) {
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wed"); break;
    case 4: lcd.print("Thu"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sta"); break;
    case 7: lcd.print("Sun"); break;
    default: lcd.print("Mon");
  }

  lcd.setCursor(0, 1);
  lcd.print(BPChour);
  lcd.print(":");

  lcd.print(BPCminuate);

  lcd.print(":");

  lcd.print(BPCsecond);

  if (P3 > 1) {
    lcd.print("PM");
  }
  else {
    lcd.print("AM");
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i, j;

  for (i = 0; i < 10; i++) {
    receivebpc();
    if (lostData > 12) {
      //建议不解码second
      ledshine++;
      ledbling = 1;
      if(ledshine<=2){
        decoding();
      }      
      flag = 0;
      lostData = 0;
    }//if(lostData>12)
    //每秒打印
    if (ledbling == 1) {
      if (ledshine<2){
      digitalWrite(led, HIGH);
      delay(50);
      digitalWrite(led, LOW);
      delay(50);
      }
      else{
        digitalWrite(led, HIGH);
        delay(50);
      }      
    }
    else {
      digitalWrite(led, LOW);
      delay(100);
    }
  }//for (i=0;i<10;i++)
  runtime(60, 24);
  printTime();
}//loop()
