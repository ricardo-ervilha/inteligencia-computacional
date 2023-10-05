#ifndef GDSA_VECTOR_H
#define GDSA_VECTOR_H

//Generic Data Struct Adaptive Vector

/*
    
    recuperar o tamanho -> O(1)
    acessar um índice -> O(1)
    alterar um elemento -> O(1)
    push_back -> O(1)
    pop_back -> O(1)
    insert -> O(n)
    erase -> O(n)
    front, back -> O(1)
    
*/

#include <stdexcept>
#include <ostream>

template <typename STRU>
class GdsaVector {

public:
    GdsaVector(int size);
    ~GdsaVector();
    void pushBack(const STRU& stru);
    STRU popBack();
    STRU erase(int p);
    void insert(int p, const STRU& stru);
    int length() {return this->len;};
    STRU front();
    STRU back();

    STRU& operator[](int index) {
    if (index < 0 || index >= len) {
        throw std::out_of_range("Index out of bounds");
    }
    return items[index];
}

private:
    int len;
    int size;
    STRU* items;
};

#endif
