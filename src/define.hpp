#include "Mem.hpp"

/* CPU */
#define M_EI (mem[0xFFFF])
#define M_IF (mem[0xFF0F])
#define IT_VBLANK 0x40
#define IT_LCD_STAT 0x48
#define IT_TIMER 0x50
#define IT_SERIAL 0x58
#define IT_JOYPAD 0x60

/* JOYPAD */

#define A_BUTTON SDLK_s
#define B_BUTTON SDLK_a
#define SELECT_BUTTON SDLK_LSHIFT
#define START_BUTTON SDLK_RETURN
#define JOYPAD_MSK_START	0x80
#define JOYPAD_MSK_SELECT 	0x40
#define JOYPAD_MSK_B 		0x20
#define JOYPAD_MSK_A 		0x10
#define JOYPAD_MSK_DOWN	    0x08
#define JOYPAD_MSK_UP 		0x04
#define JOYPAD_MSK_LEFT 	0x02
#define JOYPAD_MSK_RIGHT 	0x01

/* TIMER*/
#define DIV	0xFF04
#define TIMA 0xFF05
#define TMA	0xFF06
#define TAC	0xFF07
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
#define VBLANK_INT_BIT 1
#define STAT_INT_BIT 1 << 1
#define TIMER_INT_BIT 1 << 2
#define SERIAL_INT_BIT 1 << 3
#define JOYPAD_INT_BIT 1 << 4

/* Memory */

#define MEM_SIZE (0xFFFF + 1)
#define RAM_BANK_SIZE (8 * 1024)
#define ROM_BANK_SIZE (16 * 1024)
#define BIT(val, bit) ((val & (1 << bit)) >> bit)
#define SET(val, bit) (val |= (1 << bit))
#define RES(val, bit) (val &= ~(1 << bit))

/*Screen */
#define QUAD_COLOR 0b11

/* Loop */

#define GBSTATE_MSK 0x03
#define GBSTATE_H_BLANK 0
#define GBSTATE_V_BLANK 1
#define GBSTATE_OAM_SEARCH 2
#define GBSTATE_PX_TRANSFERT 3


/* SPRITES */

#define OBP0 (0xFF48) // Object/Sprite palette 0
#define OBP1 (0xFF49) // Object/Sprite palette 1

/* PPU */

//VRAM:
//starts at 8000 to 97FF
//0x8000 is always sprites, background can be either at 0x8000 or 0x8800

// FF40 is LCDC (LCD Control Regiser)
// 7: LCD display enable
// 6: Window Tile Map Address : 0 is 0x9800-0x9BFF 1 is 0x9C00-0x9FFF // NOTE verify
// 5: Window Enable
// 4: BG & Window Tile data 0 is 0x8800-0x97FF 1 is 0x8000-0x8FFF
// (noted   BG Characters in offial doc)
// 3: BG Tile Map Address 0 is 0x9800-0x9BFF 1 is 0x9C00-0x9FFF // NOTE verify
// 2 : OBJ size (0 is 8x8 | 1 is 8x16) (obj == sprite)
// 1: OBJ enable (0 off, 1 on)
// 0: Background Enable (0 off, 1 on) (always true in CGB)
#define LCDC (0xFF40)


// OAM Attributes (FE00 to FEA0 (40 x 4 bytes)):
//	byte 0 is posY
//	byte 1 is posX
//	byte 2 is tileIndex
//	byte 3 are flags:
//
// 	Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
//  Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
//  Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
//  Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
//  Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
//  Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)


// FF41 is LCDC status
// 6: LYC = LY interrupt
// 5: Mode 2 OAM Interrupt
// 4: Mode 1 V-Blank Interrupt
// 3: Mode 0 H-Blank Interrupt
// 2: LYC = LY Flag
// 1: Mode 00 => ram access, 10 OBJ search, 01 V-Blank, 11 LCD transfer
#define LCDC_STATUS (0xFF41)
#define SCY (0xFF42) // scroll Y // LCD viewport offset
#define SCX (0xFF43) // scroll X // LCD viewport offset
#define LY (0xFF44) // LCDC Y coordinate // (which line is currently rendered)
#define LYC (0xFF45) //  LY compare register (compare to this and can set FF41 byte 2 to launch interrupt if needed)
#define DMA (0xFF46) // DMA transfer and start
#define WY (0xFF4A) // Window Y pos, 0 is the top // (where should the window be placed virtually on the background)
#define WX (0xFF4B) // Window X pos, 7 should be the start https://hacktix.github.io/GBEDG/ppu/ // XXX to check
#define WX_OFFSET (7)
#define LCD_Y (0xFB00) //Top edge when == 10
#define VBK (0xFF4F) // CGB only, VRAM bank specification, 0 means bank0, 1 bank1

#define PIXEL_PER_LINE 160

#define M_LCDC (mem[LCDC])
#define M_LCDC_STATUS (mem[LCDC_STATUS])
#define M_SCY (mem[SCY])
#define M_SCX (mem[SCX])
#define M_LY (mem[LY])
#define M_LYC (mem[LYC])
#define M_DMA (mem[DMA])
#define M_OBP0 (mem[OBP0])
#define M_OBP1 (mem[OBP1])
#define M_WY (mem[WY])
#define M_WX (mem[WX])
#define M_LCD_Y (mem[LCD_Y])
#define M_VBK (mem[VBK])

#define MAX_SPRITES 40


// SOUND

#define SOUND_ON_OFF 0xFF26
#define SOUND_PANNING 0xFF25
#define SOUND_MASTER_VOLUME 0xFF24

// Sound Channel 1 — Pulse with wavelength sweep
#define CHANNEL_1_ON_BIT 0

#define NR10 0xFF10 // FF10 — NR10: Channel 1 sweep
#define NR11 0xFF11 // FF11 — NR11: Channel 1 length timer & duty cycle
#define NR12 0xFF12 // FF12 — NR12: Channel 1 volume & envelope
#define NR13 0xFF13 // FF13 — NR13: Channel 1 wavelength low [write-only]
#define NR14 0xFF14 // FF14 — NR14: Channel 1 wavelength high & control

// Sound Channel 2 — Pulse
#define CHANNEL_2_ON_BIT 1

#define NR21 0xFF16 // FF16 — NR21: Channel 1 length timer & duty cycle
#define NR22 0xFF17 // FF17 — NR22: Channel 1 volume & envelope
#define NR23 0xFF18 // FF18 — NR23: Channel 1 wavelength low [write-only]
#define NR24 0xFF19 // FF19 — NR24: Channel 1 wavelength high & control

// Sound Channel 3 — Wave output
#define CHANNEL_3_ON_BIT 2

#define NR30 0xFF1A // FF1A — NR30: Channel 3 DAC enable
#define NR31 0xFF1B // FF1B — NR31: Channel 3 length timer [write-only]
#define NR32 0xFF1C // FF1C — NR32: Channel 3 output level
#define NR33 0xFF1D // FF1D — NR33: Channel 3 wavelength low [write-only]
#define NR34 0xFF1E // FF1E — NR34: Channel 3 wavelength high & control
#define CHANNEL_3_WAVE_PATTERN 0xFF30 // FF30–FF3F — Wave pattern RAM

// Sound Channel 4 — Noise
#define CHANNEL_4_ON_BIT 3

#define NR41 0xFF20 // FF20 — NR41: Channel 4 length timer [write-only]
#define NR42 0xFF21 // FF21 — NR42: Channel 4 volume & envelope
#define NR43 0xFF22 // FF22 — NR43: Channel 4 frequency & randomness
#define NR44 0xFF23 // FF23 — NR44: Channel 4 control

#define BUFFER_SIZE 1024
#define SAMPLING_RATE 44100
#define MAX_VOLUME 0x0fff