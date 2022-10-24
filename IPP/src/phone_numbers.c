#include "phone_numbers.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

PhoneNumbers *phnumNew () {
    PhoneNumbers *pnum = malloc(sizeof(PhoneNumbers));
    if (pnum != NULL) {
        pnum->howManyRed = 0;
        pnum->leader = NULL;
    }
    return pnum;
}

bool phnumAddNumSorted(PhoneNumbers *pnum, char const* num, size_t len) {
    DoubleDirList *smaller = NULL;
    DoubleDirList  *bigger = pnum->leader;
    char * numHelp = (char *) num;
    while (bigger != NULL && compareNums(bigger->value, numHelp) < 0) {
        smaller = bigger;
        bigger = bigger->next;
    }
    if (bigger != NULL && compareNums(bigger->value, numHelp) == 0) {
        free(bigger->value);
        bigger->value = numHelp;
        return true; // wspomniane przekierowanie juz istnialo
    }
    DoubleDirList *block = malloc(sizeof(DoubleDirList));
    if (block == NULL) {
        return false;
    }
    block->prev = smaller;
    block->next = bigger;
    if (smaller == NULL) {
        pnum->leader = block; //to najmniejszy element w liscie
    }
    else {
        smaller->next = block;
    }

    if (bigger != NULL) {
        bigger->prev = block;
    }
    block ->value = (char *) num;
    block->len = len;

    pnum->howManyRed += 1;
    return true;
}

bool phnumAddNumFront(PhoneNumbers *pnum, char const * num, size_t len) {
    DoubleDirList *block = malloc(sizeof(DoubleDirList));
    if (block == NULL) {
        return false;
    }
    block->value = (char *) num;
    block->len = len;
    block->next = pnum->leader;
    block->prev = NULL;
    if (pnum->leader != NULL) {
        (pnum->leader)->prev = block;
    }
    pnum->leader = block;
    pnum->howManyRed += 1;
    return true;
}

void deleteReversedRedirection(PhoneNumbers *pnum, DoubleDirList *block) {
    if (block == NULL || pnum == NULL) {
        return;
    }
    if (block == pnum->leader) { //gdy to potrzebne aktualizujemy leadera
        pnum->leader = block->next;
        if (pnum->leader != NULL) {
            (pnum->leader)->prev = NULL;
        }
        free(block->value);
        free(block);
    }
    else {
        (block->prev)->next = block->next; //block->prev !=NULL z 1. ifa
        if (block->next != NULL) {
            (block->next)->prev = block->prev;
        }
        free(block->value);
        free(block);
    }
    pnum->howManyRed -= 1;
}
