#ifndef _ULTRA64_OPCODES_H
#define _ULTRA64_OPCODES_H

#define _SPECIAL 0x00
#define _REGIMM  0x01
#define BEQ     0x04
#define BNE     0x05
#define ADDI    0x08
#define ADDIU   0x09
#define ANDI    0x0C
#define ORI     0x0D
#define LUI     0x0F
#define _CP0     0x10
#define BEQL    0x14
#define BNEL    0x15
#define DADDI   0x18
#define LB      0x20
#define LW      0x23
#define SW      0x2B


/* SPECIAL */
#define SLL     0x00
#define SRL     0x02
#define SLLV    0x04
#define SRLV    0x06
#define JR      0x08
#define MFHI    0x10
#define MFLO    0x12
#define MULTU   0x19
#define ADDU    0x21
#define SUBU    0x23
#define AND     0x24
#define OR      0x25
#define XOR     0x26
#define SLTU    0x2B
#define DSLL32  0x3C


/* REGIMM */
#define BLTZ    0x00
#define BGEZAL  0x11


/* CP0 */
#define MFC0    0x00
#define MTC0    0x04

#endif
