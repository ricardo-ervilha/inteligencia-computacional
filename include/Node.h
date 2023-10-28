#ifndef NO_H
#define NO_H

#include <cmath>
#include <iostream>

typedef unsigned int uint;

typedef struct{
    uint id; //Id do nó.
    float x;
    float y;
    int score;
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

void print_distance_matrix(float **d_matrix, uint size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            std::cout << d_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void print_node(Node node){
    std::cout << "ID: "<<node.id<< "\t(" <<node.x<<" , "<<node.y<<")\tScore: "<<node.score<<std::endl;
}

#endif // NO_H