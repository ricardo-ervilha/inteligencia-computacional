#ifndef GDSA_VECTOR_H
#define GDSA_VECTOR_H

//Generic Data Struct Adaptive Vector
//Ideia é reconstruir o vector para utilizarmos no projeto

template <typename STRU>
class GdsaVector {

public:
    GdsaVector(int size);
    ~GdsaVector();
    void endAdd(const STRU& stru);
    STRU endPop();
    STRU pop(int p);
    void add(int p, const STRU& stru);
    STRU get(int p) {return items[p];};
    STRU set(int p, STRU stru) {items[p] = stru;};
    int length() {return this->len;};

private:
    int len;
    int size;
    STRU* items;
};

#endif
