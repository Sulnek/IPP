#include <stdbool.h>
#include <stddef.h>
#include "handy_functions.h"


size_t checkIfNumber(char const *num) {
    if (num == NULL) {
        return 0;
    }
    size_t i = 0;
    while (isDigit(num[i])) {
        i++;
    }
    if (num[i] == '\0') {
        return i;
    }
    return 0;
}


bool isDigit(char c) {
    int val = (int) c;
    if (val >= (int) '0' && val <= (int) '9') {
        return true;
    }
    if (val == (int) '*') {
        return true;
    }
    if (val == (int) '#') {
        return true;
    }
    return false;
}

int valueOfDigit(char c) {
    int val = (int)  c;
    if (val >= (int) '0' && val <= (int) '9') {
        return val - (int) '0';
    }
    if (val == (int) '*') {
        return 10;
    }
    if (val == (int) '#') {
        return 11;
    }
    return -1;
}

int compareNums(char *num1, char *num2) {
    int i = 0;
    while (isDigit(num1[i]) && isDigit(num2[i])) {
        if (valueOfDigit(num1[i]) < valueOfDigit(num2[i])) {
            return -1;
        }
        if (valueOfDigit(num1[i]) > valueOfDigit(num2[i])) {
            return 1;
        }
        i++;
    }
    if (isDigit(num1[i]) && !isDigit(num2[i])) {
        return 1;
    }
    if (!isDigit(num1[i]) && isDigit(num2[i])) {
        return -1;
    }
    return 0;
}