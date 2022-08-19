#include <Regexp.h>

#include <LiquidCrystal.h>

float ans = 0;

int getCharInt(char cap[12]){
  int number = 0;
  int ten_power = 0;
  
  for (int i = 11; i >= 0; i--){
      char dig = cap[i];
      if (dig == '0' || dig == '1' || dig == '2' || dig == '3' || dig == '4' || dig == '5' || dig == '6' || dig == '7' || dig ==  '8' || dig == '9'){
          // Serial.print("Digit for iteration");
          // Serial.print(i);
          // Serial.print(" is :");
          // Serial.println(dig);
          number += (dig - '0') * pow(10, ten_power);
          ten_power += 1;
      }
  }
  return number;
}



void matchCallback  (const char * match,
                      const unsigned int length,
                      const MatchState & ms){
  char cap [12];   // must be large enough to hold captures
  
  Serial.print ("Matched: ");
  Serial.write ((byte *) match, length);
  Serial.println ();
  char func;
  int prev_num = 0; 
  
  for (byte i = 0; i < ms.level; i++){
    Serial.print ("Capture "); 
    Serial.print (i, DEC);
    Serial.print (" = ");
    ms.GetCapture (cap, i);
    Serial.println (cap); 
    if (! (i % 2)){
        func = cap[0];
        // Serial.print("Changed func to: ");
        // Serial.println(func);
    } else if (i % 2){
      if (!func){
        func = '+';
      }

      int num = getCharInt(cap);
      
      if (func == '+'){
        ans += num;
        // Serial.print("Called addition for number ");
        // Serial.print(num);
        // Serial.print(" Current sum is now ");
        // Serial.println(ans[2]);
        
      } else if (func == '-'){
        ans -= num;
        // Serial.print("Called subtraction for number ");
        // Serial.print(num);
        // Serial.print(" Current sum is now ");
        // Serial.println(ans[3]);
      } else if (func == '*'){
        ans *= num;
        // Serial.print("Called multiplication for number ");
        // Serial.print(num);
        // Serial.print(" Current sum is now ");
        // Serial.println(ans[1]);
      } else if (func == '/'){
        ans /= num;
      } else if (func == '^'){
        ans = pow(ans, num);
      }
    }
  } // end of for each capture

} 
void regex(char buf[16]) {
  
  unsigned long count;
  MatchState ms (buf);
  Serial.println (buf);
  
  count = ms.GlobalMatch ("([-+*/^]*)(%d+)([-+*/^])(%d+)", matchCallback);

  Serial.print ("Found ");
  Serial.print (count);
  Serial.println (" matches.");
}

struct Button {
    int pin;
    int state;
    char state_value[5];
    int index;
};

float time_since_start = 0;

Button numPad1 {7, LOW, {'1', '6', '+', 'l', 'C'}, 0};
Button numPad2 {13, LOW, {'2', '7', '-', 't', 'C'}, 1};
Button arithematicOpertations {8, LOW, {'3', '8', '*', 's', 'C'}, 2};
Button mathOperations {10, LOW, {'4', '9', '/', 'c', 'C'}, 3};
Button clearButton {9, LOW, {'5', '0', '=', '^', 'C'}, 4};

int calculator_state = 5;
char calc_display[16];
int cursor_pos = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(){
  Serial.begin(9600);

  pinMode(numPad1.pin, INPUT);
  pinMode(numPad2.pin, INPUT);
  pinMode(arithematicOpertations.pin, INPUT);
  pinMode(mathOperations.pin, INPUT);
  pinMode(clearButton.pin, INPUT);
  
  // contrast 102 / 51 = 2V
  analogWrite(6, 51);
  lcd.begin(16, 2);
  
}
void loop()
{
  numPad1.state = digitalRead(numPad1.pin);
  numPad2.state = digitalRead(numPad2.pin);
  arithematicOpertations.state = digitalRead(arithematicOpertations.pin);
  mathOperations.state = digitalRead(mathOperations.pin);
  clearButton.state = digitalRead(clearButton.pin);
  // Serial.println("Num 1:");
  // Serial.println(numPad1.state);
  // Serial.println("Num 2:");
  // Serial.println(numPad2.state);

  if (numPad1.state == HIGH
      || numPad2.state == HIGH
      || arithematicOpertations.state == HIGH
      || mathOperations.state == HIGH
      || clearButton.state == HIGH){
    if (calculator_state >= 0 && calculator_state < 5){
      lcd.setCursor(cursor_pos, 0);
      char resp;

      if (numPad1.state == HIGH){
        // Serial.println("Printed for 1:");
        // Serial.println(numPad1.state_value[calculator_state]);
        resp = numPad1.state_value[calculator_state];

      } else if (numPad2.state == HIGH){
        // Serial.println("Printed for 2:");
        // Serial.println(numPad2.state_value[calculator_state]);
        resp = numPad2.state_value[calculator_state];

      } else if (arithematicOpertations.state == HIGH){
        resp = arithematicOpertations.state_value[calculator_state];

      } else if (mathOperations.state == HIGH){
        resp = mathOperations.state_value[calculator_state];

      } else if (clearButton.state == HIGH){
        resp = clearButton.state_value[calculator_state];
      }
      
      lcd.print(resp);
      
      if (resp == 'C'){
        lcd.clear();
        memset(calc_display, 0, 16);
        ans = 0;
        cursor_pos = 0;
      } else if (resp == '='){
        regex(calc_display);
        lcd.clear();
        memset(calc_display, 0, 16);
        lcd.print(ans);
        ans = 0;
        cursor_pos = 5;
      } else if (resp == 's'){
        regex(calc_display);
        if (ans == 0){
          ans = getCharInt(calc_display);
        }
        lcd.clear();
        memset(calc_display, 0, 16);
        Serial.println(sin(ans * PI / 180));
        lcd.print(sin(ans * PI / 180));
        ans = 0;
        cursor_pos = 5;
      } else if (resp != 's' || resp != 'c' || resp != 't'){
        calc_display[cursor_pos] = resp;
        cursor_pos += 1;
      }

      calculator_state = 5;
      Serial.println(calc_display);
      
    } else {
        if (numPad1.state == HIGH){
            calculator_state = numPad1.index;
          } else if (numPad2.state == HIGH){
            calculator_state = numPad2.index;
          } else if (arithematicOpertations.state == HIGH){
            calculator_state = arithematicOpertations.index;
          } else if (mathOperations.state == HIGH){
            calculator_state = mathOperations.index;
          } else if (clearButton.state == HIGH){
            calculator_state = clearButton.index;
          }
    }
  } 

  lcd.setCursor(15, 1);
  lcd.rightToLeft();
  lcd.print(calculator_state);
  lcd.print(":etatS");

  lcd.setCursor(0, 1);
  lcd.leftToRight();
  lcd.print(" ");

  // Serial.println("Calculator state");
  // Serial.println(calculator_state);
  
  delay(1000);
}
