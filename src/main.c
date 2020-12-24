#include "smd/z80.h"

static int arr[] = { 1, 10, 4, 5, 6, 7 };
static int sum;
static const int n = sizeof(arr) / sizeof(arr[0]);

int main()
{
    z80_init();
    z80_program_load((uint8_t*) 0x000000, 256);
    
    int i;
    while (1)
    {
        for (i = 0; i < n; ++i)
        {
            sum += arr[i];
        }
    }
}
