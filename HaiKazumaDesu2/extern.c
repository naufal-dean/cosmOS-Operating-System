int strToInt(char * string);
void intToStr(int number, char * buffer);
int div(int a, int b);
int mod(int a, int b);
void menu();
void interface();

int main() {
  interrupt(0x21, 0, "Welcome to Cosmic Calulator!\r\n", 0, 0);
  interface();
	while(1);
	return 0;
}

int strToInt(char * string) {
	int val = 0, i = 0, neg;

	neg = string[0] == '-';
	if (neg) i++;
	while (string[i] != 0x0) {
		val = val * 10 + (string[i] - 48);
		i++;
	}
	if (neg) val *= -1;
	return val;
}

void intToStr(int number, char * buffer) {
	int tempNum, i, j, neg;

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = 0x0;
	} else {
        neg = number < 0;
        if (neg) number *= -1;
        tempNum = number;
        
		i = 0;
		while (tempNum != 0) {
            buffer[i] = mod(tempNum, 10) + 48;
            tempNum = div(tempNum, 10);
            i++;
		}
        if (neg) buffer[i++] = '-';
        
        // swap
        for (j = 0; j < div(i, 2); ++j) {
            buffer[j] ^= buffer[i-j-1];
            buffer[i-j-1] ^= buffer[j];
            buffer[j] ^= buffer[i-j-1];
        }
        buffer[i] = 0x0;
	}
}

int div(int a, int b) {
  int l = 0, r = a, ret = 0;

  while (l <= r) {
    int mid = (l + r) >> 1;
    if (a >= mid * b) {
      l = mid + 1;
      ret = mid;
    } else {
      r = mid - 1;
    }
  }
  return ret;
}

int mod(int a, int b) {
  return a - b * div(a, b);
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

