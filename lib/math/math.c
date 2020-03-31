#include "math.h"

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