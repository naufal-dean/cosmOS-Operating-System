#ifndef INTERRUPT_DEFINE
#define INTERRUPT_DEFINE

#define printString_intr(BX) interrupt(0x21, 0x00, (BX), 0, 0)
#define readString_intr(BX) interrupt(0x21, 0x01, (BX), 0, 0)
#define readSector_intr(BX, CX) interrupt(0x21, 0x02, (BX), (CX), 0)
#define writeSector_intr(BX, CX) interrupt(0x21, 0x03, (BX), (CX), 0)
#define readFile_intr(BX, CX, DX, AH) interrupt(0x21, (AH << 8) + 0x04, (BX), (CX), (DX))
#define writeFile_intr(BX, CX, DX, AH) interrupt(0x21, (AH << 8) + 0x05, (BX), (CX), (DX))
#define executeProgram_intr(BX, CX, DX, AH) interrupt(0x21, (AH << 8) + 0x06, (BX), (CX), (DX))

#endif // INTERRUPT_DEFINE