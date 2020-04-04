#include "text.h"

void print(char *string) {
	printString_intr(string);
}

void input(char *string) {
	readString_intr(string);
}

void inputEditable(char *string) {
	char histSector[SECTOR_SIZE];
	// Remove shell flag
	readSector_intr(histSector, HISTORY_SECTOR);
    stringCpy(histSector + HIST_METADATA_OFFSET, "edit");
	writeSector_intr(histSector, HISTORY_SECTOR);
    // Input command
	readString_intr(string);
    // Remove shell flag
	readSector_intr(histSector, HISTORY_SECTOR);
    stringCpy(histSector + HIST_METADATA_OFFSET, "");
	writeSector_intr(histSector, HISTORY_SECTOR);
}
