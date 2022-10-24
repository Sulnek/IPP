/** @file
 * Interfejs klasy zawierającej wierzchołek drzewa przekierowań i
 * funkcje wykonujące na nim podstawowe operacje
 *
 * @author Franciszek Sulima <fs438728@students.mimuw.edu.pl>
 * @copyright Franciszek Sulima
 * @date 2022
 */

#ifndef PHONE_NUMBERS_PHONE_FORWARD_NODE_H
#define PHONE_NUMBERS_PHONE_FORWARD_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include "phone_forward.h"
#include "DoubleDirList.h"
#include "phone_numbers.h"

/**
 *  System przekierowan przedstawiony jako drzewo. Kazdemu wierzcholkowi
 *  odpowiada pewien "prefiks", a jego synowi w nextDigit[c] odpowiada
 *  "prefiksc", przy czym korzen odpowiada za pusty prefiks (niebedacy slowem)
 */
typedef struct PhoneForwardNode {
    ///Na co zmieni sie obecny prefiks
    char * redirection;

    ///Dlugosc prefiksu na ktory zajdzie zmiana
    size_t redirectionLen;

    /**
     * Tablica indeksow do prefiksow dluzszych o jedna cyfre
     */
    struct PhoneForwardNode *nextDigit[HOW_MANY_DIGITS];

    ///rodzic w drzewie
    struct PhoneForwardNode *parent;

    /**
     * struktura gdzie przechowany jest odpowiednik tego przekierowania w
     * drzewie odwroconych przekierowan
     */
     struct PhoneNumbers *pnumRedirect;

     /**
      * Wskaznik na element w liscie pnumRedirect odpowiadajacy temu
      * przekierowaniu w drzewie odworconych przekierowan.
      */
      struct DoubleDirList *blockOfPnumRedirect;
}
PhoneForwardNode;

/**
 * Funkcja tworzy nowy wierzcholek phfwdNode
 * @return Wskaznik na stworzony phfwdNode lub NULL gdy nie powiodla sie
 * alokacja pamieci.
 */
PhoneForwardNode * phfwdNodeNew(void);

/**
 * Funkcja usuwa cale pfNode i cale jego poddrzewo,
 * jeśli na wejściu otrzymamy NULL, to nic nie robi.
 * @param pfNode
 */
void phfwdNodeDelete(PhoneForwardNode *pfNode);


/**
 * tworzy nowa strukture nie zawierajaca przekierowan, wskazujaca
 * na swojego rodzica
 * @param parent - rodzic
 * @return Wkaznik na utworzona strukture lub NULL gdy nie udalo sie
 * zaalokowac pamieci
 */
PhoneForwardNode * phfwdNodeNewParent(PhoneForwardNode * parent);

/**
 * Funkcja dla danego wierzcholka usuwa najdluzsza galaz do ktorej nalezy,
 * taka ze zaden z wezlow w tej galezi nie zawiera przekierowan.
 * @param pf
 */
void deleteDeadBranches(PhoneForwardNode * pf);

#endif