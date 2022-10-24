/** @file
 * Interfejs klasy zawierającej przydatne funkcje operujące na
 * strukturze Phone Numbers
 *
 * @author Franciszek Sulima <fs438728@students.mimuw.edu.pl>
 * @copyright Franciszek Sulima
 * @date 2022
 */

#ifndef PHONE_NUMBERS_PHONE_NUMBERS_H
#define PHONE_NUMBERS_PHONE_NUMBERS_H

#include "phone_forward.h"
#include "DoubleDirList.h"
#include "handy_functions.h"

/**
 * Liczba znaków, które uznajemy za cyfrę numeru telefonu.
 */
#define HOW_MANY_DIGITS 12

/**
 * @brief Struktura przechowująca dwustronna liste numerów telefonów
 */
typedef struct PhoneNumbers {
    /**
     * Wskaznik na pierwszy element listy
     */
    DoubleDirList *leader;

    /**
     * Ile przekierowan w strukturze
     */
    size_t howManyRed;

}
PhoneNumbers;

/**
 * @brief funkcja tworzy nowa strukture PhoneNumbers
 *
 * @return PhoneNumbers * lub NULL gdy nie uda sie alokacja pamieci
 */
PhoneNumbers *phnumNew();

/**
 * @brief funkcja dodaje numer do struktury PhoneNumbers, jeśli jeszcze on w
 * niej nie wystepuje. Numer wstawiany jest w taki sposob zeby nie zaburzac
 * uporzadkowania numerow (rosnacego).
 *
 * @param pnum niebedacy NULLem
 * @param num
 * @return true gdy udało się wstawić, false, gdy zabrakło pamięci
 */
bool phnumAddNumSorted(PhoneNumbers *pnum, char const* num, size_t len);

/**
 * funkcja dodaje numer na poczatek struktury PhoneNumbers
 * @param pnum niebedacy NULLem
 * @param num
 * @return true gdy udało się wstawić, false, gdy zabrakło pamięci
 */
bool phnumAddNumFront(PhoneNumbers *pnum, char const * num, size_t len);

/**
 * Funkcja usuwa przekierowanie ktorego odpowiednikiem jest block w liscie
 * odwroconych przekierowan - pnum (prawdopodobnie bedacej w drzewie
 * odwroconych przekierowan
 * @param pnum
 * @param block
 */
void deleteReversedRedirection(PhoneNumbers *pnum, DoubleDirList *block);

#endif