#include "../lib/math/math.h"

void menu();
void interface();

int main() {
  // var
  int * result;
  // main
  interrupt(0x21, 0, "Welcome to Cosmic Calulator!\r\n", 0, 0);
  interface();
  // back to kernel
  interrupt(0x21, (0x01 << 8) + 0x06, "shell", 0x2000, result);
	return 0;
}

void menu(){
  interrupt(0x21, 0, "Select the type of operation:\r\n", 0, 0);
  interrupt(0x21, 0, "1. Addition\r\n", 0, 0);
  interrupt(0x21, 0, "2. Substraction\r\n", 0, 0);
  interrupt(0x21, 0, "3. Multiplication\r\n", 0, 0);
  interrupt(0x21, 0, "0. EXIT\r\n", 0, 0 );
}
void interface(){
  char ch[10], buf1[10], buf2[10], buf3[20];
  int hold;
  menu();
  interrupt(0x21, 0, "Your choice: ", 0, 0);
  interrupt(0x21, 0x1, ch, 0, 0);
  
  while(strToInt(ch) != 0){
    if(strToInt(ch) > 3 || strToInt(ch) < 0){
      interrupt(0x21, 0, "Invalid number. Try again!\r\n");
      interrupt(0x21, 0x1, ch, 0, 0);
    }
   
    interrupt(0x21, 0, "Input the first number: ", 0, 0);
    interrupt(0x21, 0x1, buf1, 0, 0 );
    interrupt(0x21, 0, "Input the second number: ", 0, 0);
    interrupt(0x21, 0x1, buf2, 0, 0);

    switch(strToInt(ch)){
      case 1:
          interrupt(0x21, 0, "Result of addition: ", 0, 0);
          hold = strToInt(buf1) + strToInt(buf2);
        break;
      case 2:
          interrupt(0x21, 0, "Result of substraction: ", 0, 0);
          hold = strToInt(buf1) - strToInt(buf2);
        break;
      case 3:
          interrupt(0x21, 0, "Result of multiplication: ", 0, 0); 
          hold = strToInt(buf1) * strToInt(buf2);
        break;
    }
    intToStr(hold, buf3);
    interrupt(0x21, 0, buf3, 0, 0);
    interrupt(0x21, 0, "\r\n\r\n");
    
    interrupt(0x21, 0, "Is there anything you want to do?\r\n");
    menu();
    interrupt(0x21, 0, "Your choice: ", 0, 0);
    interrupt(0x21, 0x1, ch, 0, 0);
  }
  
  interrupt(0x21, 0, "\r\n", 0, 0);
  interrupt(0x21, 0, "Thank you for using the Cosmic Calculator!\r\n");
}

