/** @file
 * Interfejs klasy zawierającej wierzchołek drzewa odwróconych przekierowań i
 * funkcje wykonujące na nim podstawowe operacje
 *
 * @author Franciszek Sulima <fs438728@students.mimuw.edu.pl>
 * @copyright Franciszek Sulima
 * @date 2022
 */

#ifndef PHONE_NUMBERS_PHONE_FORWARD_REVERSE_NODE_H
#define PHONE_NUMBERS_PHONE_FORWARD_REVERSE_NODE_H

#include "phone_forward.h"
#include "phone_numbers.h"


/**
 *  System przekierowan przedstawiony jako drzewo. Kazdemu wierzcholkowi
 *  odpowiada pewien "prefiks", a jego synowi w nextDigit[c] odpowiada
 *  "prefiksc", przy czym korzen odpowiada za pusty prefiks (niebedacy slowem)
 */
typedef struct PhoneForwardRevNode {
    /**
     * Struktura zawierajaca numery telefonow przekierowane na numer
     * reprezentowany przez dany wierzcholek w drzewie
     */
    PhoneNumbers *redirected;

    /**
     * Tablica indeksow do prefiksow dluzszych o jedna cyfre
     */
    struct PhoneForwardRevNode *nextDigit[HOW_MANY_DIGITS];

    ///rodzic w drzewie
    struct PhoneForwardRevNode *parent;
}
PhoneForwardRevNode;

/**
 * tworzy nowa strukture nie zawierajaca przekierowan, wskazujaca
 * na swojego rodzica
 * @param parent - rodzic
 * @return Wkaznik na utworzona strukture lub NULL gdy nie udalo sie
 * zaalokowac pamieci
 */
PhoneForwardRevNode * phfwdRevNodeNewParent(PhoneForwardRevNode * parent);

/**
 * Funkcja usuwa cale pfRevNode i cale jego poddrzewo,
 * jeśli na wejściu otrzymamy NULL, to nic nie robi.
 * @param pfNode
 */
void phfwdRevNodeDelete(PhoneForwardRevNode *pfRevNode);

#endif