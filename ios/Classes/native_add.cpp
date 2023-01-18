//
// Created by SIMON on 3/20/2022.
//

#include <stdint.h>
#include <stdlib.h>

extern "C" __attribute__((visibility("default"))) __attribute__((used))
int32_t native_add(int32_t num1, int32_t num2) {
 return num1 + num2;
}

