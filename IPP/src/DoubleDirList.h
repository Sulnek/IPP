/** @file
 * Interfejs struktury DoubleDirList
 *
 * @author Franciszek Sulima <fs438728@students.mimuw.edu.pl>
 * @copyright Franciszek Sulima
 * @date 2022
 */

#ifndef PHONE_NUMBERS_DOUBLEDIRLIST_H
#define PHONE_NUMBERS_DOUBLEDIRLIST_H

#include <stddef.h>

/**
 * Lista dwukierunkowa, której każdy element trzyma stringa i jego długość
 */
typedef struct DoubleDirList {
    struct DoubleDirList *prev;
    struct  DoubleDirList *next;
    size_t len;
    char *value;
}
DoubleDirList;

#endif
