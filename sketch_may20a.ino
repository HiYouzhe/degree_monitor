#include <Stepper.h>

// --- 步進馬達設定 ---
const int stepsPerRevolution = 2000; 
Stepper myStepper(200, 52, 50, 51, 53);

// --- 矩陣鍵盤掃描設定 ---
const byte colPins[] = {30, 31, 32, 33}; 
const byte rowPins[] = {34, 35, 36, 37}; 
char keys[4][4] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'}, 
  {'C','D','E','F'}
};

// --- 七段顯示器設定 ---
byte segmentPins[] = {38, 39, 40, 41, 42, 43, 44, 45}; 
byte digitPins[] = {46, 47, 48, 49}; 

byte numData[] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F 
};

String inputAngle = ""; 

void setup() {
  myStepper.setSpeed(60);
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }

  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);
}

void loop() {
  char key = scanKeypad(); 

  if (key != '\0') {
    if (key >= '0' && key <= '9') {
      if (inputAngle.length() < 4) inputAngle += key;
    } 
    else if (key == 'B') {
      if (inputAngle.length() > 0) inputAngle.remove(inputAngle.length() - 1);
    } 
    else if (key == 'E') {
      if (inputAngle.length() > 0) {
        int angle = inputAngle.toInt();
        long targetSteps = (long)angle * stepsPerRevolution / 360;
        myStepper.step(targetSteps);
        inputAngle = ""; 
      }
    }
    delay(250); 
  }

  // 刷新顯示
  if (inputAngle.length() > 0) {
    displayNumber(inputAngle.toInt());
  } else {
    displayNumber(0);
  }
}

char scanKeypad() {
  for (int c = 0; c < 4; c++) {
    digitalWrite(colPins[c], LOW);
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rowPins[r]) == LOW) {
        delay(20); 
        while(digitalRead(rowPins[r]) == LOW); 
        digitalWrite(colPins[c], HIGH);
        return keys[r][c];
      }
    }
    digitalWrite(colPins[c], HIGH);
  }
  return '\0';
}

void displayNumber(int n) {
  int temp = n;
  for (int i = 0; i < 4; i++) {
    int digit = temp % 10;
    showDigit(i, digit);
    delay(5); 
    temp /= 10;
    if (temp == 0) break; 
  }
}

// 修正後的核心顯示函式
void showDigit(int pos, int val) {
  // 1. 先關閉所有位數 (COM 腳位)
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], HIGH); 
  }

  // 2. 取得段碼
  byte segments = numData[val];

  // 3. 設定 a-g 段碼 (針對 1 顯示成 E 的邏輯反轉修正)
  for (int i = 0; i < 7; i++) {
    if ((segments >> i) & 0x01) {
      digitalWrite(segmentPins[i], LOW);  // 亮起
    } else {
      digitalWrite(segmentPins[i], HIGH); // 熄滅
    }
  }

  // 4. 開啟當前位數 (COM)
  digitalWrite(digitPins[3 - pos], LOW); 
}
