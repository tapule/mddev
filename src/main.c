#include <stdint.h>
#include "smd/pad.h"

volatile uint16_t s1;
volatile uint16_t s2;
volatile uint16_t s3;

int main()
{
    uint8_t type[2];

    pad_update();
    type[0] = pad_type(PAD_1);
    type[1] = pad_type(PAD_2);


    while (1)
    {
        uint16_t i;

        for (i = 0; i< 1000; ++i)
        {
            __asm__ volatile ("\tnop\n");
            __asm__ volatile ("\tnop\n");
            __asm__ volatile ("\tnop\n");
            __asm__ volatile ("\tnop\n");
        }
        s1 = pad_btn_state(PAD_1, PAD_BTN_LEFT);
        s2 = pad_btn_pressed(PAD_1, PAD_BTN_LEFT);
        s3 = pad_btn_released(PAD_1, PAD_BTN_LEFT);

        pad_update();
    }
}
