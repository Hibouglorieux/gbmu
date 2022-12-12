/* TIMER*/

#define M_DIV	mem[0xFF04]
#define M_TIMA	mem[0xFF05]
#define M_TMA	mem[0xFF06]
#define M_TAC	mem[0xFF07]

 /* Interrupts */

#define IT_VBLANK 0x40
#define IT_LCD_STAT 0x48
#define IT_TIMER 0x50
#define IT_SERIAL 0x58
#define IT_JOYPAD 0x60

/* Memory */

#define MEM_SIZE (0xFFFF + 1)

/*Screen */
#define QUAD_COLOR 0b11

