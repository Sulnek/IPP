/** @file
 * Interfejs klasy zawierającej przydatne funkcje operujące numerami telefonów
 *
 * @author Franciszek Sulima <fs438728@students.mimuw.edu.pl>
 * @copyright Franciszek Sulima
 * @date 2022
 */

#ifndef HANDY_FUNCTIONS_H
#define HANDY_FUNCTIONS_H

#include <stdbool.h>
#include <stddef.h>
#include "phone_forward.h"

/**
 * @brief sprawdza czy slowo jest numerem
 *
 * @param num wskaznik na sprawdzane slowo
 * @return Dlugosc numeru, jesli jest poprawny, 0 w przeciwnym przypadku
 */
size_t checkIfNumber(char const *num);

///Funkcja zwraca bool czy znak jest cyfra
bool isDigit(char c);

///Funkcja zwraca wartość cyfry zapisanej jako znaku
int valueOfDigit(char c);

/**
 * Funkcja porownuje dwa numery telefonow pod wzgledem leksykograficznym
 * @param num1
 * @param num2
 * @return 1 jesli num1 > num2 , 0 jesli num1 == num2, -1 jesli num1 < num2
 */
int compareNums(char *num1, char *num2);

#endif
