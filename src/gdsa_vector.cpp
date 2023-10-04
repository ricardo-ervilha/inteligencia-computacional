#include <iostream>
#include "../include/gdsa_vector.h"

using namespace std;

template <typename STRU>
GdsaVector<STRU>::GdsaVector(int size) {
    this->len = 0;

    if (size < 1)
        size = 1;
    else 
        this->size = size;

    items = new STRU[size];
}

template <typename STRU>
GdsaVector<STRU>::~GdsaVector() {
    delete[] items;
}

template <typename STRU>
void GdsaVector<STRU>::endAdd(const STRU& stru) {
    if (len == size) {
        size *= 2;
        STRU* newItems = new STRU[size];
        for (int i = 0; i < len; i++) {
            newItems[i] = items[i];
        }
        delete[] items;
        items = newItems;
    }
    items[len++] = stru;
}

template <typename STRU>
STRU GdsaVector<STRU>::endPop() {
    return items[--len];
}

template <typename STRU>
STRU GdsaVector<STRU>::pop(int p) {
    STRU ret = items[p];
    len--;
    for (int i = p; i < len; i++) {
        items[i] = items[i + 1];
    }
    return ret;
}

template <typename STRU>
void GdsaVector<STRU>::add(int p, const STRU& stru) {
    if (len == size) {
        size *= 2;
        STRU* newItems = new STRU[size];
        for (int i = 0; i < p; i++) {
            newItems[i] = items[i];
        }
        newItems[p] = stru;
        for (int i = p; i < len; i++) {
            newItems[i + 1] = items[i];
        }
        delete[] items;
        items = newItems;
    } else {
        for (int i = len - 1; p <= i; i--) {
            items[i + 1] = items[i];
        }
        items[p] = stru;
    }
    len++;
}