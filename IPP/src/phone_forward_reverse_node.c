#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "phone_forward_reverse_node.h"
#include "phone_numbers.h"
#include "phone_forward.h"

PhoneForwardRevNode * phfwdRevNodeNewParent(PhoneForwardRevNode * parent) {
    PhoneForwardRevNode *pfRevNode = malloc(sizeof(PhoneForwardRevNode));
    if (pfRevNode == NULL) {
        return NULL;
    }
    pfRevNode->parent = parent;
    pfRevNode->redirected = phnumNew();
    if (pfRevNode->redirected == NULL) {
        free(pfRevNode);
        return NULL;
    }
    for (int i = 0; i < HOW_MANY_DIGITS; i++) {
        (pfRevNode->nextDigit)[i] = NULL;
    }
    return pfRevNode;
}

void phfwdRevNodeDelete(PhoneForwardRevNode *pfRevNode) {
    if (pfRevNode == NULL) {
        return;
    }
    PhoneForwardRevNode *dummy = pfRevNode;
    PhoneForwardRevNode *pfNodeRevParent = pfRevNode->parent;
    while (dummy != pfNodeRevParent) { //czyscimy cale poddrzewo
        bool hasSons = false;
        int sonNr = 0;
        while (sonNr < HOW_MANY_DIGITS && !hasSons) {
            if ((dummy->nextDigit)[sonNr] != NULL) {
                hasSons = true;
            }
            else {
                sonNr++;
            }
        }
        if (hasSons) {
            //idziemy do liscia, zacierajac wskazniki w dol
            dummy = (dummy->nextDigit)[sonNr];
            ((dummy->parent)->nextDigit)[sonNr] = NULL;
        }
        else {
            //zwalniamy pamiec i idziemy do rodzica
            PhoneForwardRevNode *dummyParent = dummy->parent;
            phnumDelete(dummy->redirected);
            free(dummy);
            dummy = dummyParent;
        }
    }
}