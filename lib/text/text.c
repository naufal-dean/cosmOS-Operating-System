#include "text.h"

void print(char *string) {
	interrupt(0x21, 0x00, string, 0, 0);
}

void input(char *string) {
	interrupt(0x21, 0x01, string, 0, 0);
}