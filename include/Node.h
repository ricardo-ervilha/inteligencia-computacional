#ifndef NO_H
#define NO_H

#include <cmath>

typedef unsigned int uint;

typedef struct{
    float x;
    float y;
    int score;
    bool vis;
    uint id;
} Node;

void create_distance_matrix(float **d_matrix, uint size, Node *vertex){
    //matriz é construída de forma que as linhas e colunas se organizam primeiro listando todos os os hoteis e depois todos os nós. 
    //sendo assim, de 0 até h+1 temos os índices de todos os hoteis (inicio, fim, extras) e de h+2 até v+h temos os índices dos nós
    
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(i == j)
                d_matrix[i][j] = 0.0;
            else
                d_matrix[i][j] = sqrt(pow(vertex[i].x - vertex[j].x, 2) + pow(vertex[i].y - vertex[j].y, 2));
        }
    }
}

void print_node(Node node){
    std::cout << "("<<node.x<<" , "<<node.y<<") \t\t "<<node.score<<std::endl;
}

#endif // NO_H