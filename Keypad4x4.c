#include <8051.h>
#include "Keypad4x4.h"

char Keypad4x4() {
    int key = -1;
    int array[4] = {0xef, 0xdf, 0xbf, 0x7f};
    int k[4] = {1, 2, 4, 8};

    for (int i = 0; i < 4; i++) {
        P0 = array[i];
        for (int j = 0; j < 4; j++) {
            if ((P0 & k[j]) == 0) {
                key = i * 4 + j;
                return key;
            }
        }
    }
    return -1;
}

