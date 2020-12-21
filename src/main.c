#include "smd/z80.h"

static int arr[] = { 1, 10, 4, 5, 6, 7 };
static int sum;
static const int n = sizeof(arr) / sizeof(arr[0]);

int main()
{
    z80_bus_request();
    
    int i;
    while (1)
    {
        for (i = 0; i < n; ++i)
        {
            sum += arr[i];
        }
    }
}
