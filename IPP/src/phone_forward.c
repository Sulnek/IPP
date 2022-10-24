#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "phone_forward.h"
#include "phone_forward_node.h"
#include "phone_numbers.h"
#include "phone_forward_reverse_node.h"
#include "handy_functions.h"
#include "DoubleDirList.h"

/**
 * Struktura odpowiadająca za przekierowania telefonow. root jest korzeniem
 * drzewa odpowiadajacego za przekierowania zapisane w taki sposob,
 * zeby optymalnie bylo wykonac na nim funkcje phfwdGet() (wezly typu
 * PhoneForwardNode), a revRoot jest korzeniem drzewa odpowiadającego za
 * przekierownia zapisane w taki sposob, zeby optymalnie bylo wykonac na nim
 * funkcje phfwdReverse().
 */
typedef struct PhoneForward {
    PhoneForwardNode * root;
    PhoneForwardRevNode * revRoot;
}
PhoneForward;

PhoneForward * phfwdNew(void) {
    PhoneForward *pfTree = malloc(sizeof(PhoneForward));
    if (pfTree == NULL) {
        return NULL;
    }
    pfTree->root = phfwdNodeNew();
    if (pfTree->root == NULL) {
        free(pfTree);
        return NULL;
    }
    pfTree->revRoot = phfwdRevNodeNewParent(NULL);
    if (pfTree->revRoot == NULL) {
        free(pfTree->root);
        free(pfTree);
        return NULL;
    }
    return pfTree;
}

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }
    PhoneForwardNode *pfNode = pf->root;
    phfwdNodeDelete(pfNode);
    PhoneForwardRevNode *pfRevNode = pf->revRoot;
    phfwdRevNodeDelete(pfRevNode);
    free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL || num1 == NULL || num2 == NULL) {
        return false; //Blad wejscia - NULL
    }
    PhoneForwardNode *pfNode = pf->root;
    PhoneForwardRevNode *pfRevNode = pf->revRoot;

    size_t len1 = checkIfNumber(num1);
    size_t len2 = checkIfNumber(num2);
    if (len1 == 0 || len2 == 0) {
        return false; //Podane ciagi znakow to nie numery
    }

    if (compareNums((char *) num1, (char *) num2) == 0) {
        return false; // Ciagi nie roznia sie
    }

    //idziemy do poddrzewa odpowiadajacego za prefiks num1
    PhoneForwardNode *dummy = pfNode;
    for (size_t i = 0; i < len1; i++) {
        int digit = valueOfDigit(num1[i]);
        if ((dummy->nextDigit)[digit] == NULL) {
            (dummy->nextDigit)[digit] = phfwdNodeNewParent(dummy);
        }
        if ((dummy->nextDigit)[digit] == NULL) {
            deleteDeadBranches(dummy);
            return false; //blad alokacji pamieci
        }
        dummy = (dummy->nextDigit)[digit];
    }

    if (dummy->redirection != NULL &&
        compareNums(dummy->redirection, (char *) num2) == 0) {
        return true; //przekierowanie nie zmienia się
    }

    /*idziemy do poddrzewa w drzewie odwróconych przekierowań
     * odpowiadajacego za prefiks num2*/
    PhoneForwardRevNode *revDummy = pfRevNode;
    for (size_t i = 0; i < len2; i++) {
        int digit = valueOfDigit(num2[i]);
        if ((revDummy->nextDigit)[digit] == NULL) {
            (revDummy->nextDigit)[digit] = phfwdRevNodeNewParent(revDummy);
        }
        revDummy = (revDummy->nextDigit)[digit];
        if (revDummy == NULL) {
            return false; //blad alokacji pamieci
        }
    }

    //kopiujemy ciagi znakow do stringow
    char *redirHelp1 = malloc((len1 + 1) * sizeof(char));
    if (redirHelp1 ==NULL) {
        return false;
    }
    strcpy(redirHelp1, num1);
    char * redirHelp2 = malloc((len2 + 1) * sizeof(char));
    if (redirHelp2 == NULL) {
        free(redirHelp1);
        return false;
    }
    strcpy(redirHelp2, num2);

    if (!phnumAddNumFront(revDummy->redirected, redirHelp1, len1)) {
        free(redirHelp1);
        free(redirHelp2);
        return false;
    }
    free(dummy->redirection);
    dummy->redirection = redirHelp2;
    dummy->redirectionLen = len2;
    dummy->pnumRedirect = revDummy->redirected;
    dummy->blockOfPnumRedirect = (revDummy->redirected)->leader;

    return true;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL || num == NULL) {
        return; //blad wejscia - NULL
    }
    
    size_t len = checkIfNumber(num);
    if (len == 0) {
        return; //num nie reprezentuje numeru
    }

    //idziemy do poddrzewa odpowiadajacego za prefiks num
    PhoneForwardNode *pfNode = pf->root;
    PhoneForwardNode *dummy = pfNode;
    int digit = 0;
    for (size_t i = 0; i < len; i++) {
        digit = valueOfDigit(num[i]);
        dummy = (dummy->nextDigit)[digit];
        if (dummy == NULL) {
            return;
        }
    }

    PhoneForwardNode *dummyParent = dummy->parent;
    //zacieramy do niego wskaznik i usuwamy poddrzewo
    if (dummy->parent != NULL) {
        ((dummy->parent)->nextDigit)[digit] = NULL;
    }
    phfwdNodeDelete(dummy);
    deleteDeadBranches(dummyParent);
}

/**
 * @brief funkcja zwraca pierwszy numer, na który num zostanie przekierowany
 * i zapisuje pod lenH jego dlugosc
 * 
 * @param[in] pf - struktura z danymi przekierowan
 * @param[in] num - numer do przekierowania
 * @param[in] len - dlugosc num
 * @param[in] size_t *lenH - dlugosc numeru po przekierowaniu
 * @return char const* pierwszy numer na ktory num zostanie przekierowany,
 * NULL- gdy nie udalo sie zaalokowac pamieci.
 */
static inline char * switchPrefix(PhoneForward const *pf, char const *num,
                                  size_t len, size_t *lenH) {
    char *redNum = NULL; //numer przekierowania
    char *newPref = NULL; //nowy prefiks dla num
    size_t maxPrefLen = 0; //dlugosc maksymalnego przekierowywalnego prefiksu
    size_t newPrefLen = 0; //dlugosc nowego prefiksu
    PhoneForwardNode *pfNode = pf->root;
    PhoneForwardNode *dummy = pfNode;
    size_t i = 0;

    /**
     * idziemy do poddrzewa odpowiadajacego za prefiks num,
     * jesli takie istnieje. Aktualizujemy najwiekszy przekierowywalny prefiks
     * i prefiks na ktory jest on przekierowywany wraz z ich dlugosciami
     */
    while (dummy != NULL && i < len) {
        int digit = valueOfDigit(num[i]);
        dummy = (dummy->nextDigit)[digit];
        i++;
        if (dummy != NULL) {
            if (dummy->redirection != NULL) {
                newPref = dummy->redirection;
                maxPrefLen = i;
                newPrefLen = dummy->redirectionLen;
            }
        }
    }

    //tworzymy numer na ktory zostanie przekierowany num
    if (newPref == NULL) { //nie znaleziono zarejestrowanego prefiksu w num
        redNum = malloc((len + 1) * sizeof(char));
        if (redNum == NULL) {
            return NULL;
        }
        strcpy(redNum, num);
        (*lenH) = len;
        return redNum;
    }
    size_t redNumLen = newPrefLen + len - maxPrefLen;
    redNum = malloc((redNumLen + 1) * sizeof(char));
    if (redNum == NULL) {
        return NULL; //blad alokacji pamieci
    }
    for (i = 0; i < newPrefLen; i++) {
        redNum[i] = newPref[i];
    }
    for (i = newPrefLen; i < redNumLen; i++) {
        redNum[i] = num[i - newPrefLen + maxPrefLen];
    }
    redNum[redNumLen] = '\0';
    (*lenH) = redNumLen;
    return redNum;  
}

PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    PhoneNumbers *pnum = phnumNew();
    if (pnum == NULL) {
        return NULL;
    }
    char * newNum = NULL; //numer na ktory num zostanie przekierowany
    size_t len = checkIfNumber(num);
    size_t lenH = 0; 
    if (len == 0) {
        return pnum; //num nie reprezentuje numeru
    }
    newNum = switchPrefix(pf, num, len, &lenH);
    if (newNum != NULL) {
        if (phnumAddNumFront(pnum, newNum, lenH) == false) {
            return NULL; //blad alokacji pamieci
        }
    }
    else {
        return NULL; //blad alokacji pamieci
    }
    return pnum;
}

PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    size_t len = checkIfNumber(num);
    if (len == 0) {
        return phnumNew(); //przy bledzie alokacji pamieci - NULL
    }
    PhoneNumbers *pnum = phnumNew();
    if (pnum == NULL) {
        return NULL;
    }
    char * number = malloc((len+1) * sizeof(char));
    if (number == NULL) {
        phnumDelete(pnum);
        return NULL;
    }
    strcpy(number, num);
    if (!phnumAddNumSorted(pnum, number, len)) {
        phnumDelete(pnum);
        free(number);
        return NULL;
    }
    PhoneForwardRevNode *pfRevNode = pf->revRoot;
    PhoneForwardRevNode *dummy = pfRevNode;
    size_t i = 0;
    while (dummy != NULL && i < len) {
        int digit = valueOfDigit(num[i]);
        dummy = (dummy->nextDigit)[digit];
        if (dummy != NULL) {
            DoubleDirList *block = (dummy->redirected)->leader;
            while (block != NULL) {
                size_t numlen = len - i + block->len;
                number = malloc( (numlen + 1) * sizeof(char));
                if (number == NULL) {
                    phnumDelete(pnum);
                    return NULL;
                }
                for (size_t j = 0; j < block->len; j++) {
                    number[j] = (block->value)[j];
                }
                for (size_t j = 0; j < numlen - block->len + 1; j++) {
                    number[j + block->len] = num[i + j + 1];
                }
                if (!phnumAddNumSorted(pnum, number, numlen)) {
                    phnumDelete(pnum);
                    free(number);
                    return NULL;
                }
                block = block->next;
            }
        }
        i++;
    }
    return pnum;
}

char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL) {
        return NULL;
    }
    if (idx < pnum->howManyRed) {
        DoubleDirList *dummy = pnum->leader;
        for (size_t i = 0; i < idx; i++) {
            dummy = dummy->next;
        }
        return dummy->value;
    }
    else {
        return NULL;
    }
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum == NULL) {
        return;
    }
    DoubleDirList *dummy = pnum->leader;
    while (dummy != NULL) {
        DoubleDirList *dummyNext = dummy->next;
        free(dummy->value);
        free(dummy);
        dummy = dummyNext;
    }
    free(pnum);
}

PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    size_t len = checkIfNumber(num);
    if (len == 0) {
        return phnumNew(); //przy bledzie alokacji pamieci - NULL
    }
    PhoneNumbers *pnum = phnumNew();
    if (pnum == NULL) {
        return NULL;
    }
    char * number = malloc((len+1) * sizeof(char));
    if (number == NULL) {
        phnumDelete(pnum);
        return NULL;
    }
    strcpy(number, num);

    PhoneNumbers *pnumDummy = phfwdGet(pf, number);
    if (pnumDummy == NULL) {
        phnumDelete(pnum);
        free(number);
        return NULL;
    }
    char *numTransferred = (char *) phnumGet(pnumDummy, 0);
    if (compareNums(numTransferred, (char *) num) == 0) {
        if (!phnumAddNumSorted(pnum, number, len)) {
            phnumDelete(pnum);
            free(number);
            phnumDelete(pnumDummy);
            return NULL;
        }
    }
    else {
        free(number);
    }
    phnumDelete(pnumDummy);

    PhoneForwardRevNode *pfRevNode = pf->revRoot;
    PhoneForwardRevNode *dummy = pfRevNode;
    size_t i = 0;
    while (dummy != NULL && i < len) {
        int digit = valueOfDigit(num[i]);
        dummy = (dummy->nextDigit)[digit];
        if (dummy != NULL) {
            DoubleDirList *block = (dummy->redirected)->leader;
            while (block != NULL) {
                size_t numlen = len - i + block->len;
                number = malloc( (numlen + 1) * sizeof(char));
                if (number == NULL) {
                    phnumDelete(pnum);
                    return NULL;
                }
                for (size_t j = 0; j < block->len; j++) {
                    number[j] = (block->value)[j];
                }
                for (size_t j = 0; j < numlen - block->len + 1; j++) {
                    number[j + block->len] = num[i + j + 1];
                }

                pnumDummy = phfwdGet(pf, number);
                if (pnumDummy == NULL) {
                    phnumDelete(pnum);
                    free(number);
                    return NULL;
                }
                numTransferred = (char *) phnumGet(pnumDummy, 0);
                if (compareNums(numTransferred, (char *) num) == 0) {
                    if (!phnumAddNumSorted(pnum, number, numlen)) {
                        phnumDelete(pnum);
                        phnumDelete(pnumDummy);
                        free(number);
                        return NULL;
                    }
                }
                else {
                    free(number);
                }
                phnumDelete(pnumDummy);

                block = block->next;
            }
        }
        i++;
    }
    return pnum;
}