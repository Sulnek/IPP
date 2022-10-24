#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "phone_forward_node.h"
#include "phone_forward.h"
#include "phone_numbers.h"


 PhoneForwardNode * phfwdNodeNew(void) {
    PhoneForwardNode *pf = malloc(sizeof(PhoneForwardNode));
    if (pf == NULL) {
        return NULL;
    }

    pf->redirection = NULL;
    pf->redirectionLen = 0;
    pf->pnumRedirect = NULL;
    pf->blockOfPnumRedirect = NULL;
    pf->parent = NULL;
    for (int i = 0; i < HOW_MANY_DIGITS; i++) {
        (pf->nextDigit)[i] = NULL;
    }
    return pf;
}

void deleteDeadBranches(PhoneForwardNode * pf) {
    if (pf == NULL) {
        return;
    }
    PhoneForwardNode *dummy = pf;
    while (dummy->parent != NULL) {
        bool hasSons = false;
        if (dummy->redirection == NULL) { //nie ma przekierowan
            for (int i = 0; i < HOW_MANY_DIGITS && !hasSons; i++) {
                if ((dummy->nextDigit)[i] != NULL) {
                    hasSons = true;
                }
            }
            if (!hasSons) {
                for (int i = 0; i < HOW_MANY_DIGITS; i++) {
                    if (((dummy->parent)->nextDigit)[i] == dummy) {
                        ((dummy->parent)->nextDigit)[i] = NULL;
                    } //zacieramy wskaznik w dol
                }

                //zwalniamy pamiec i idziemy do rodzica
                PhoneForwardNode *dummyParent = dummy->parent;
                free(dummy);
                dummy = dummyParent;
            }
            else {
                return;
            }
        }
        else {
            return;
        }
    }
}

void phfwdNodeDelete(PhoneForwardNode *pfNode) {
    if (pfNode == NULL) {
        return;
    }
    PhoneForwardNode *dummy = pfNode;
    PhoneForwardNode *pfNodeParent = pfNode->parent;
    while (dummy != pfNodeParent) { //czyscimy cale poddrzewo
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
            PhoneForwardNode *dummyParent = dummy->parent;
            if (dummy->redirection != NULL) {
                free(dummy->redirection);
            }
            deleteReversedRedirection(dummy->pnumRedirect,
                                      dummy->blockOfPnumRedirect);
            free(dummy);
            dummy = dummyParent;
        }
    }
}

PhoneForwardNode * phfwdNodeNewParent(PhoneForwardNode * parent) {
    PhoneForwardNode *pf = malloc(sizeof(PhoneForwardNode));
    if (pf == NULL) {
        return NULL;
    }
    pf->redirection = NULL;
    pf->redirectionLen = 0;
    pf->pnumRedirect = NULL;
    pf->blockOfPnumRedirect = NULL;
    pf->parent = parent;
    for (int i = 0; i < HOW_MANY_DIGITS; i++) {
        (pf->nextDigit)[i] = NULL;
    }
    return pf;
}