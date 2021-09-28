# Gens Kmod - Kdebug info (Copyright ©2007 Kaneda)

It uses the VDP control port (0xC00004) to do debugging stuff in Gens Kmod
emulator by using VDP's out of the functional range registers (0x00 - 0x17)

## Register 29 (0x1D): Gens control
As any added register, the original Megadrive/Genesis should skip them (as
written in the Dev Kit documentation)

### Gens control register
Take control of Gens throught your rom

Value is one of these control codes
0x00        pause
0x01        show debug 68k window
0x02        show debug z80 window
0x03        show debug VDP window
0x04        show debug sub68k window
0x05        show debug CDC window
0x06        show debug CD Gfx window
0x07        show debug 32X main window
0x08        show debug 32X sub window
0x09        show debug 32X VDP window
0x0A        show debug VDP register window
0x0B        show debug Sprite window
0x0C        show debug YM2612 window
0x0D        show debug PSG window
0x0E        show watchers window
0x0F        show Layers window
0x10        show message window
0x11        show debug CD register window
0x50        dump 68k ram
0x51        dump Z80 ram
0x52        dump S68k ram
0x60        take screenshot
0x62        start record gmv
0x63        stop record gmv
0x64        start gmv
0x65        stop gmv

0x00/Pause is the more useful one : pause your game at an important point and
check what you want

## Register 30 (0x1E): Register Message
As any added register, the original Megadrive/Genesis should skip them (as
written in the Dev Kit documentation)

### Message register
output any text on the Message window

Value is a printable char or 0 for output
Use a loop to input a full string but be aware Message register will automatically
ouput a 255 chars string and start a new one

## Register 31 (0x1F): Timer
! WARNING : this register doesn't work during VInt !
As any added register, the original Megadrive/Genesis should skip them (as
written in the Dev Kit documentation)

### Timer register
start/reset and output a timer (based on m68k cycles)

Value is sr000000 where
s=0                Counter output on Message window
s=1                Counter start
r=0                <undefined>
r=1                reset counter

I made it for optimizing issue : you can know how cycles a call take and
optimize it.



