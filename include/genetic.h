#ifndef GENETIC_H
#define GENETIC_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include "util.h"
#include "greedy_v2.h"
#include "local_search.h"

/// @brief Sorteia um índice com base na QUALIDADE das soluções associaidas
/// @param fitnessSolutions: Vetor contendo as qualidads de soluções
/// @param tamPop: Tamanho do vetor Trip** chamado "populacao"
/// @param totalScore: Soma Total dos valores de Fitness
/// @return int
int rouletteWheel(vector<int> fitnessSolutions, int tamPop, int totalScore)
{
    double random_number = rand() / (float)RAND_MAX;

    float val_acumulado = 0;
    int i;
    for (i = 0; i < tamPop; i++)
    {
        val_acumulado += fitnessSolutions[i] / (float)totalScore;
        if (random_number <= val_acumulado)
        {
            return i;
        }
    }
    return i - 1;
}

/// @brief Baseado na clase Trip, monta um vetor com os índices dos hoteis. Então, se por exemplo, tivermos as seguintes trips: trip 0 => {0, 2}, trip 1 => {2, 2}, trip 2 => {2, 1}; o cromossomo gerado será dado por: {0, 2, 2, 1}.
/// @param data
/// @param trips
/// @return vector<int>
vector<int> findChromosome(OPHS *data, Trip **trips)
{
    vector<int> hotels;

    hotels.push_back(trips[0]->getStartHotel());
    for (int i = 0; i < data->getNumTrips(); i++)
    {

        hotels.push_back(trips[i]->getEndHotel());
    }

    return hotels;
}

/// @brief Função que aplica a mutação em uma dada solução. Primeiro, é sorteado uma trip, após isso é alterado o hotel final dessa trip. A partir disso, os hotéis das próximas trips vão sendo trocados, até ficar tudo viável. Quando se alcança uma possibilidade consistente, aplica-se lógica construtiva para adicionar os POI's.
/// @param data
/// @param indexParent: Índice da solução a ser modificada
/// @param populacao: Vetor contendo todas as soluções na população atual
/// @param cromossomos: Vetor guardando os cromossomos das soluções na população atual  
void applyMutation(OPHS *data, int indexParent, vector<Trip **> populacao, vector<vector<int>> cromossomos, mt19937 *gen)
{
    //Sorteia um índice para a trip. Importante que a última trip não possa ser sorteada, pois senão, não haveria nenhuma modificação no cromossomo!
    int idTrip = intRandom(0, data->getNumTrips() - 2, gen);

    vector<int> new_chromossome;

    // Primeira parte do cromossomo vindo do Parent
    new_chromossome.push_back(0);

    for (int i = 0; i < idTrip; i++)
    {
        new_chromossome.push_back(populacao[indexParent][i]->getEndHotel());
    }

    
    vector<int> combination;
    vector<int> hoteisPossiveis;
    // Descobre hotéis finais possíveis para alocar na trip sorteada
    for (int i = 0; i < data->getNumExtraHotels() + 2; i++)
    {
        if (data->getMatrix()[populacao[indexParent][idTrip]->getStartHotel()][i][idTrip])
        {
            hoteisPossiveis.push_back(i);
        }
    }

    //Sorteia um dos possíveis hoteis e adiciona numa combinação, que guarda os hoteis pós trip aleatória. Esse combination no fim servirá para compor o restante de new_chromossome.
    int hotelEscolhido = hoteisPossiveis[intRandom(0, hoteisPossiveis.size()-1, gen)];
    combination.push_back(hotelEscolhido);

    for (int i = idTrip + 1; i < data->getNumTrips(); i++)
    {
        if (!data->getMatrix()[i][combination.back()][cromossomos[indexParent][i + 1]])
        {     
            // Se a trip seguinte não for feasible, ou seja, deu ruim na trip seguinte.
            vector<int> hoteisPossiveis;

            // Descobre os hoteis possiveis para fim da trip
            if(i == data->getNumTrips() - 1){
                combination.push_back(1);
            }{
                for (int j = 0; j < data->getNumExtraHotels() + 2; j++)
                {
                    if (data->getMatrix()[combination.back()][j][i])
                    {
                        hoteisPossiveis.push_back(j);
                    }
                }

                // Altera o o hotel final
                int hotelEscolhido = hoteisPossiveis[intRandom(0, hoteisPossiveis.size()-1, gen)];
                combination.push_back(hotelEscolhido);
            }     
        }
        else
        {

            // Se não foi necessário trocar o hotel final da trip seguinte, só termino de preencher com os hoteis restantes que já existiam no parent. 
            for (int j = i + 1; j <= data->getNumTrips(); j++)
            {
                combination.push_back(cromossomos[indexParent][j]);
            }

            break;
        }
    }

    //Como dito acima, adiciono os valores do combination para completar o restante do novo cromossomo.
    for (int i = 0; i < combination.size(); i++)
    {
        new_chromossome.push_back(combination[i]);
    }

    // Inicializa os hoteis da classe Trip
    int cont = 0;
    while (cont < data->getNumTrips())
    {
        data->getTrip(cont)->setStartHotel(new_chromossome[cont]);
        data->getTrip(cont)->setEndHotel(new_chromossome[cont + 1]);
        cont++;
    }

    // Construtivo sendo aplicado sem nenhum POI dentro.
    Trip **sol = constructive_algorithm(data, gen, 0);
}


/// @brief Função que aplica a crossover com base em duas soluções distintas. Primeiro sorteia-se a trip pivô. Depois, altera-se os hotéis a partir do hotel final da trip pivo do p1, adicionando os hoteis do p2. Após recombinar os hoteis, parte dos POI's advém de p1 (trips 0 até tripPivot), e depois disso o restante é preenchido com lógica construtiva. É importante depois de adicionar os POI's originais de p1 na tripPivot remover alguns nós para para deixar essa trip VIÁVEL.
/// @param data
/// @param indexParent1
/// @param indexParent2
/// @param populacao: Vetor contendo todas as soluções na população atual
/// @param cromossomos: Vetor guardando os cromossomos das soluções na população atual
void applyCrossOverI(OPHS *data, int indexParent1, int indexParent2, vector<Trip **> populacao, vector<vector<int>> cromossomos, mt19937 *gen)
{

    // [a b c d] -> supondo essa combinação de hoteis
    // (Última trip de fora) permitir sortear somente as trips [a b] [b c], visto que sortear a última [c d] só faria trocar o valor de d. Mas, valor de d no nosso problema é fixo.

    // ARTIGO ORIGINAL: One of the trips (trip d) of the first parent, P1, is randomly selected. This trip is called the pivot trip

    int tripPivot = intRandom(0, data->getNumTrips() - 2, gen);

    // ARTIGO ORIGINAL: The algorithm then checks if it is feasible for this pivot trip to start from the initial hotel of the selected trip in the first parent solution and end in the final hotel of the selected trip in the second parent solution. This check, can be done efficiently by using the matrix of pairs of hotels.If the condition is false, another trip is selected randomly.

    //Certifica-se que a combinação de hoteis escolhida pra trip pivô é viável usando a matriz de viabilidade.
    int inicio = populacao[indexParent1][tripPivot]->getStartHotel();
    int fim = populacao[indexParent2][tripPivot]->getEndHotel();
    
    while (!data->getMatrix()[populacao[indexParent1][tripPivot]->getStartHotel()][populacao[indexParent2][tripPivot]->getEndHotel()][tripPivot])
    {
        tripPivot = intRandom(0, data->getNumTrips() - 2, gen);
    }


    // ARTIGO ORIGINAL: If the condition in (b) is true, the new sequence of hotels is formed by taking the hotel sequence of the first part of the first parent up to the initial hotel of the pivot trip and combining it with the hotel sequence from the final hotel of the pivot trip in the second parent, to the final depot.
    vector<int> new_chromossome;

    // Primeira parte vindo do P1
    new_chromossome.push_back(0);

    for (int i = 0; i < tripPivot; i++)
    {
        new_chromossome.push_back(populacao[indexParent1][i]->getEndHotel());
    }

    // Segunda parte vindo do P2
    for (int i = tripPivot; i < data->getNumTrips(); i++)
    {
        new_chromossome.push_back(cromossomos[indexParent2][i + 1]);
    }

    // Inicializa os hoteis da classe Trip
    int cont = 0;
    while (cont < data->getNumTrips())
    {
        data->getTrip(cont)->setStartHotel(new_chromossome[cont]);
        data->getTrip(cont)->setEndHotel(new_chromossome[cont + 1]);
        cont++;
    }

    set<int> visiteds;

    // Enchendo as trips antes da trip pivô, pois podem permanecer iguais!
    for (int j = 0; j < tripPivot; j++)
    {
        for (int i = 0; i < populacao[indexParent1][j]->getNodes().size(); i++)
        {
            data->getTrip(j)->add(populacao[indexParent1][j]->getNode(i));
            visiteds.insert(populacao[indexParent1][j]->getNode(i).id); // adiciona os nos visitados das trips antes da pivô
        }
        data->getTrip(j)->setCurrentLength(calcTripLength(data, data->getTrip(j)));
    }


    // Enchendo a trip pivô com os POI's derivados do primeiro pai (P1)
    for (int i = 0; i < populacao[indexParent1][tripPivot]->getNodes().size(); i++)
    {
        data->getTrip(tripPivot)->add(populacao[indexParent1][tripPivot]->getNode(i));
    }


    data->getTrip(tripPivot)->setCurrentLength(calcTripLength(data, data->getTrip(tripPivot)));


    // Tratando a trip pivô, para garantir que ela não fique inviável.
    vector<Node> *nos = data->getTrip(tripPivot)->getNodesPointer();
    while (!nos->empty() && data->getTrip(tripPivot)->getCurrentLength() > data->getTrip(tripPivot)->getTd())
    {
        // Pego o nó de menor score
        int index = intRandom(0, nos->size() - 1, gen);

        nos->erase(nos->begin() + index);

        // Atualizo o trip length
        data->getTrip(tripPivot)->setCurrentLength(calcTripLength(data, data->getTrip(tripPivot)));
    }



    for (int i = 0; i < data->getTrip(tripPivot)->getNodes().size(); i++)
    {
        visiteds.insert(data->getTrip(tripPivot)->getNode(i).id); // adiciona os nós visitados da trip pivo.
    }

    // Completa a parte final do tour.
    Trip **sol = constructive_algorithm_modified(data, gen, visiteds, tripPivot + 1, 0);

}


/// @brief Função que controla a evolução das gerações usando CrossOvers e Mutações.
/// @param data
/// @param tamPopInit: Parâmetro importante, pois diz o tamanho inicial da população.
/// @param percentualCrossOver: Parâmetro importante, pois ajuda a controlar qual percentual da população atual irá sofrer cross-overs.
/// @param percentualMutacao: Parâmetro importante, pois ajuda a controlar qual percentual da população atual irá sofrer mutações.
/// @param gama: Parâmetro importante, pois controla a porcentagem dos indivíduos que serão selecionados para a próxima geração.
/// @param numGenerations: Parâmetro importante, pois controla o número de gerações.
/// @return Trip**: Melhor Solução encontrada no processo
Trip** genetic_algorithm(OPHS *data, int tamPopInit, float percentualCrossOver, float percentualMutacao, float gama, int numGenerations, mt19937 *gen)
{
    //Variáveis associadas a melhor solução.
    Trip **melhor;
    int scoreMelhor;
    vector<int> cromossomoDoMelhor;

    //Variáveis associadas a população.
    vector<Trip **> populacao;
    vector<int> fitnessSolutions;
    vector<vector<int>> cromossomos;

    /*Geração da População Inicial*/

    Trip **copia = makeCopySolution(data, data->getTrips()); // Trips vazias

    int totalScore = 0;

    for (int i = 0; i < tamPopInit; i++)
    {
        greedy_randomized_adaptive_reactive_procedure(data, gen); // Gero solução
        
        int score = getScoreTour(data, data->getTrips()); // Calculo o score do tour

        if(i == 0){
            //Primeira solução gerada.

            melhor = makeCopySolution(data, data->getTrips());
            scoreMelhor = score;
            cromossomoDoMelhor = findChromosome(data, melhor);
            
        }else{
            if(score > scoreMelhor){
                //Se a solução gerada atualmente for superior a melhor

                populacao.push_back(makeCopySolution(data, melhor)); // Salvo melhor antigo na população

                fitnessSolutions.push_back(scoreMelhor); // Salvo melhor antigo score na lista de fitness

                cromossomos.push_back(cromossomoDoMelhor); // Salvo melhor antigo cromossomo na lista de cromossomos

                totalScore += scoreMelhor; //Incremento em totalScore

                //Atualizo as variáveis do melhor para comportar o novo melhor
                melhor = makeCopySolution(data, data->getTrips());
                scoreMelhor = score;
                cromossomoDoMelhor = findChromosome(data, melhor);

            }else{
                //Se a solução gerada atualmente NÃO for superior a melhor

                populacao.push_back(makeCopySolution(data, data->getTrips())); // Salvo solução na população

                fitnessSolutions.push_back(score); // Salvo score na lista de fitness

                vector<int> chromossome = findChromosome(data, data->getTrips());
                cromossomos.push_back(chromossome);

                totalScore += score; //Incremento em totalScore
            }
        }
        
        //Procedimento padrão para inicializar na próxima vez como vazio.
        data->setTrips(copia);
        copia = makeCopySolution(data, data->getTrips());
    }

    /// Loop das Gerações
    for (int k = 0; k < numGenerations; k++)
    {
        cout << "Geração: " << k << endl;
        //Guarda o tamanho que vai começar.
        int tamPopulacaoIter = populacao.size();

        /* Aplicação do CrossOver I*/
        for (int i = 1; i <= (int) 2 * percentualCrossOver * tamPopulacaoIter; i++)
        {
            //Sorteia P1 e P2
            int indexParent1 = intRandom(0, populacao.size() - 1, gen);
            int indexParent2 = intRandom(0, populacao.size() - 1, gen);
            while (indexParent1 == indexParent2)
            {
                //Caso P2 seja igual a P1.
                indexParent2 = intRandom(0, populacao.size() - 1, gen);
            }
            
            if (i % 2 != 0)
                applyCrossOverI(data, indexParent1, indexParent2, populacao, cromossomos, gen); // Aplica crossover com tripPivot no p1
            else
                applyCrossOverI(data, indexParent2, indexParent1, populacao, cromossomos, gen); // Aplica crossover com tripPivot no p2

            int score = getScoreTour(data, data->getTrips()); // Calculo o score do tour
            
            if(score > scoreMelhor){
                populacao.push_back(makeCopySolution(data, melhor)); // Salvo antigo melhor na população

                fitnessSolutions.push_back(scoreMelhor); // Salvo antigo melhor score na lista de fitness

                cromossomos.push_back(cromossomoDoMelhor); // Salvo antigo melhor cromosso na lista de cromossomos

                totalScore += scoreMelhor; //Incremento em totalScore

                //Atualizo os dados que quem é melhor.
                melhor = makeCopySolution(data, data->getTrips());
                scoreMelhor = score;
                cromossomoDoMelhor = findChromosome(data, melhor);

            }else{
                populacao.push_back(makeCopySolution(data, data->getTrips())); // Salvo solução na população

                totalScore += score; // Incremento totalScore

                fitnessSolutions.push_back(getScoreTour(data, data->getTrips())); // Salvo score na lista de fitness

                vector<int> chromossome = findChromosome(data, data->getTrips());
                cromossomos.push_back(chromossome);
            }

            data->setTrips(copia);
            copia = makeCopySolution(data, data->getTrips());
        }
        

        /* Aplicacao da Mutação */
        for (int i = 1; i <= (int) 2 * percentualMutacao * tamPopulacaoIter; i++)
        {
            // Sorteia quem irá ser mutado
            int indexParent = intRandom(0, populacao.size() - 1, gen);

            //Aplica a mutação
            applyMutation(data, indexParent, populacao, cromossomos, gen);

            int score = getScoreTour(data, data->getTrips()); // Calculo o score do tour
            
            if(score > scoreMelhor){
                populacao.push_back(makeCopySolution(data, melhor)); // Salvo melhor na população

                fitnessSolutions.push_back(scoreMelhor); // Salvo score do melhor na lista de fitness

                cromossomos.push_back(cromossomoDoMelhor); // Salvo cromossomo do melhor na lista de cromossomos

                totalScore += scoreMelhor; //Incremento em totalScore

                //Atualizo dados do melhor
                melhor = makeCopySolution(data, data->getTrips());
                scoreMelhor = score;
                cromossomoDoMelhor = findChromosome(data, melhor);

            }else{
                populacao.push_back(makeCopySolution(data, data->getTrips())); // Salvo solução na população

                totalScore += score;

                fitnessSolutions.push_back(getScoreTour(data, data->getTrips())); // Salvo score na lista de fitness

                vector<int> chromossome = findChromosome(data, data->getTrips());
                cromossomos.push_back(chromossome);
            }

            data->setTrips(copia);
            copia = makeCopySolution(data, data->getTrips());
        }

        /*Gerenciamento da População*/
        

        //Novas variáveis auxiliares de população
        vector<Trip **> novaPopulacao;
        vector<int> novosFitnessSolutions;
        vector<vector<int>> novosCromossomos;

        //Uso um vetor auxiliar para guardar de forma ordenada os índices das melhores qualidades.
        vector<int> indicesOrdenados;
        for (int j = 0; j < fitnessSolutions.size(); j++)
        {
            indicesOrdenados.push_back(j);
        }
        std::sort(indicesOrdenados.begin(), indicesOrdenados.end(),
                  [&fitnessSolutions](int a, int b)
                  {
                      return fitnessSolutions[a] > fitnessSolutions[b];
                  });

        //Set para ajudar a controlar os cromossomos repetidos (!!!).
        set<vector<int>> cromossomosSet;

        //Parte que de fato irá criar a nova população.
        int cont = 0;
        totalScore = 0; //Precisa zerar pra próxima vez.
        for (; cont <  ceil(gama * populacao.size()); cont++)
        {
            //Se o cara já não foi adicionado antes
            if ((cromossomosSet.find(cromossomos[indicesOrdenados[cont]]) == cromossomosSet.end()))
            {
                novaPopulacao.push_back(populacao[indicesOrdenados[cont]]);
                novosFitnessSolutions.push_back(fitnessSolutions[indicesOrdenados[cont]]);
                novosCromossomos.push_back(cromossomos[indicesOrdenados[cont]]);

                totalScore += fitnessSolutions[indicesOrdenados[cont]];

                cromossomosSet.insert(cromossomos[indicesOrdenados[cont]]);
            }
        }

        //Atualizo as variáveis antigas para comportar o novo pool
        populacao = novaPopulacao;
        fitnessSolutions = novosFitnessSolutions;
        cromossomos = novosCromossomos;
    }

    return melhor;
}


/*
Container pra imprimir coisas associadas a cromossomos
// cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    // // Imprime o score da população inicial (Geração 0)
    // for (int i = 0; i < tamPop-1; i++)
    // {
    //     cout << "[ ";
    //     for (int j = 0; j < cromossomos[i].size(); j++)
    //     {
    //         cout << cromossomos[i][j] << " ";
    //     }
    //     cout << "]  ";
    //     cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
    // }
    // cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
*/

/*
Container pra imprimir gerenciamento de população


        cout << "-+-+-+-+-+-+-Gerenciamento da População-+-+-+-+-+-+-+-+-+-+\n";
        // Imprime o score do pool final nessa geração.
        for (int i = 0; i < ceil(gama * populacao.size()); i++)
        {
            cout << "[ ";
            for (int j = 0; j < cromossomos[i].size(); j++)
            {
                cout << cromossomos[i][j] << " ";
            }
            cout << "]  ";
            cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
        }
        cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";

*/


#endif
