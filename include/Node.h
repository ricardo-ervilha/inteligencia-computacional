#ifdef NODE_H
#define NODE_H

typedef unsigned int uint;

typedef struct{
    uint id;
    float x;
    float y; 
    float score;
    bool vis;
} node;

#endif