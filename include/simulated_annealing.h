#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#include "util.h"
#include <cmath>
#include <random>
#include <set>

using namespace std;

set<int> getIdsInSolution(OPHS *data, Trip **solution)
{
    set<int> idsInSolution;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        for (const Node &node : solution[i]->getNodes())
        {
            idsInSolution.insert(node.id);
        }
    }

    return idsInSolution;
}

vector<int> getIdsNotInSolution(OPHS *data, set<int> idsInSolution)
{
    vector<int> idsNotInSolution;
    for (int i = data->getNumExtraHotels() + 2; i < data->getNumExtraHotels() + data->getNumVertices(); i++)
    {
        if (idsInSolution.find(data->getVertex(i).id) == idsInSolution.end())
        {
            idsNotInSolution.push_back(data->getVertex(i).id);
        }
    }

    return idsNotInSolution;
}

bool isTripFeasible(OPHS *data, Trip *trip)
{
    float lengthTrip = calcTripLength(data, trip);
    cout << "1. Tamanho máximo da trip: " << trip->getTd();
    cout << " - Tamanho da trip: " << lengthTrip << endl;

    return lengthTrip <= trip->getTd();
}

bool isTripFeasible(OPHS *data, Trip *trip, vector<Node> nos)
{
    float lengthTrip = calcTripLength(data, trip->getStartHotel(), trip->getEndHotel(), nos);
    cout << "2. Tamanho máximo da trip: " << trip->getTd();
    cout << " - Tamanho da trip: " << lengthTrip << endl;

    return lengthTrip <= trip->getTd();
}

/*
    sortear uma trip e um index dessa trip
    verificar qual o melhor no para adicionar nessa posição
*/

void printCandidatos(set<tuple<int, int, int>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int valor1, valor2, valor3;
        std::tie(valor1, valor2, valor3) = tupla;
        std::cout << "Trip " << valor1 << " No " << valor2 << " random " << valor3 << std::endl;
    }
}

void printCandidatos(vector<tuple<int, int, int, float, float>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int indexTrip, indexNoRemover, idVertexAdd;
        float novoScore, novaDistancia;
        std::tie(indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia) = tupla;
        std::cout << "Trip " << indexTrip << " IndexNoRemover " << indexNoRemover << " idVertexAdd " << idVertexAdd << " NovoScore: " << novoScore << " NovaDistancia: " << novaDistancia << std::endl;
    }

    if (candidatosGerados.size() == 0)
        cout << "Ninguem na lista..." << endl;
}

// <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
void printCandidatosInsert(vector<tuple<int, int, int, float, float>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int indexTrip, indexNo, idVertexAdd;
        float incrementoDistancia, ratio;
        tie(indexTrip, indexNo, idVertexAdd, incrementoDistancia, ratio) = tupla;

        cout << "Trip " << indexTrip;
        cout << " indexNo " << indexNo;
        cout << " idVertexAdd " << idVertexAdd;
        cout << " incrementoDistancia: " << incrementoDistancia;
        cout << " ratio " << ratio << endl;
    }
}

void printNodes(vector<Node> nodesTrip)
{
    float totalScore = 0;
    for (int i = 0; i < nodesTrip.size(); i++)
    {
        cout << "\t" << nodesTrip[i].id << " ";
        totalScore += nodesTrip[i].score;
    }
    cout << endl;
    cout << "\tScore trip: " << totalScore << endl;
}

int calcMaxCandidatosGerados(OPHS *data, Trip **solution, int sizeNotInSol)
{
    int acc = 0;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        acc += solution[i]->getNodes().size() * sizeNotInSol;
    }

    return acc;
}

//  <indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia>
bool compare(tuple<int, int, int, float, float> a, tuple<int, int, int, float, float> b)
{
    return std::get<4>(a) > std::get<4>(b);
}

// <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
bool compareInsert(tuple<int, int, int, float, float> a, tuple<int, int, int, float, float> b)
{
    return std::get<4>(a) > std::get<4>(b);
}

bool compareExchange(tuple<int, int, int, float> a, tuple<int, int, int, float> b)
{
    return std::get<3>(a) > std::get<3>(b);
}

Trip **generateRandomNeighbor(OPHS *data, Trip **solution, mt19937 *gen)
{
    bool feasibleRandomSolution = false;

    int numVertices = data->getNumExtraHotels() + data->getNumVertices(); // Numero de vertices do problema
    set<int> idsInSolution = getIdsInSolution(data, solution);
    vector<int> idsNotInSolutionOriginal = getIdsNotInSolution(data, idsInSolution);
    set<tuple<int, int, int>> candidatosGerados; // tupla com <indexTrip, indexNoRemover, idRandomVertexAdd>

    int k = 0;
    while (!feasibleRandomSolution)
    {
        k++;
        vector<int> idsNotInSolution = idsNotInSolutionOriginal;

        // pega um vertice aleatorio, desconsiderando os index dos hoteis e que não esteja na solução
        // int idRandomVertexAdd = intRandom(data->getNumExtraHotels() + 2, numVertices - 1, gen); // id do no que vai ser colocado
        int indexRandomVertexAdd = intRandom(0, idsNotInSolution.size() - 1, gen); // index no idsNotInSolution do no que vai ser colocado
        int idRandomVertexAdd = idsNotInSolution[indexRandomVertexAdd];
        idsNotInSolution.erase(idsNotInSolution.begin() + indexRandomVertexAdd); // apago para não pegar esse id novamente

        int indexRandomTrip = intRandom(0, data->getNumTrips() - 1, gen); // index da trip a ser modificada
        vector<Node> nodesTrip = solution[indexRandomTrip]->getNodes();   // nos da trip aleatoria

        // pega um no aleatório da trip [indexRandomTrip] - nodesTrip não tem os hoteis
        int indexRandomNodeRemover = intRandom(0, nodesTrip.size() - 1, gen); // index do no que vai ser removido

        tuple<int, int, int> candidato = make_tuple(indexRandomTrip, indexRandomNodeRemover, idRandomVertexAdd);

        // fica no loop enquanto não gerar um vertex valido
        while (idsNotInSolution.size() > 0 && (candidatosGerados.find(candidato) != candidatosGerados.end()))
        {
            indexRandomVertexAdd = intRandom(0, idsNotInSolution.size() - 1, gen); // index no idsNotInSolution do no que vai ser colocado
            idRandomVertexAdd = idsNotInSolution[indexRandomVertexAdd];

            idsNotInSolution.erase(idsNotInSolution.begin() + indexRandomVertexAdd); // apago para não pegar esse id novamente
            candidato = make_tuple(indexRandomTrip, indexRandomNodeRemover, idRandomVertexAdd);
        }

        // se não achou idRandomVertexAdd valido, vai pra proxima iteração sortear outra trip e outro no pra remover
        if (idsNotInSolution.size() == 0)
        {
            continue;
        }

        auto result = candidatosGerados.insert(candidato); // candidatos validos que ja foram avaliados

        Node randomVertex = data->getVertex(idRandomVertexAdd);
        nodesTrip[indexRandomNodeRemover] = randomVertex; // Substituir por randomVertex

        float lengthTrip = calcTripLength(data, solution[indexRandomTrip]->getStartHotel(), solution[indexRandomTrip]->getEndHotel(), nodesTrip);
        // cout << "-------------vizinho------------------" << endl;
        // printIdsTrips(data, solution);
        if (lengthTrip <= solution[indexRandomTrip]->getTd())
        {
            // cout << "nós trip: " << endl;
            // for (int k = 0; k < nodesTrip.size(); k++)
            // {
            //     cout << nodesTrip[k].id << " ";
            // }
            // cout << endl;
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;

            solution[indexRandomTrip]->setNodes(nodesTrip);
            solution[indexRandomTrip]->setCurrentLength(lengthTrip);
            feasibleRandomSolution = true;

            // cout << "Gerou solução viavel" << endl;
            // cout << "index trip a ser modificada: " << indexRandomTrip << endl;
            // cout << "index no removido: " << indexRandomNodeRemover << endl;
            // cout << "id do no add: " << indexRandomVertexAdd << endl;
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;
        }

        // cout <<"indexRandomTrip: "<<indexRandomTrip<<" indexRandomVertexAdd: "<<indexRandomVertexAdd<< " indexRandomNodeRemover: "<<indexRandomNodeRemover<<endl;
    }
    return solution;
}

vector<tuple<int, int, int, float, float>> geraCandidatos(OPHS *data, Trip **solution, mt19937 *gen)
{
    set<int> idsInSolution = getIdsInSolution(data, solution);
    vector<int> idsNotInSolutionOriginal = getIdsNotInSolution(data, idsInSolution);

    // <indexTrip, indexNoRemover, idRandomVertexAdd, novoScore, novaDistancia> - testar sem calc novoScore
    vector<tuple<int, int, int, float, float>> candidatosGerados;

    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        vector<Node> nodesTrip = solution[indexTrip]->getNodes();
        for (int indexNoRemover = 0; indexNoRemover < nodesTrip.size(); indexNoRemover++)
        {
            // faço esse calculo para todos caras que NÃO estão na solução
            // removendo indexNoRemover e adicionando idVertexAdd no lugar, qual a novaDistancia e qual o beneficio
            for (int i = 0; i < idsNotInSolutionOriginal.size(); i++)
            {
                int idVertexAdd = idsNotInSolutionOriginal[i];

                // remover no e adicionar o novo
                nodesTrip[indexNoRemover] = data->getVertex(idVertexAdd);

                // calcular distancia total nova
                float novaDistancia = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTrip);
                if (novaDistancia <= solution[indexTrip]->getTd())
                {
                    tuple<int, int, int, float, float> candidato = make_tuple(indexTrip, indexNoRemover, idVertexAdd, 0, novaDistancia);
                    candidatosGerados.push_back(candidato);
                }
            }
        }
    }

    return candidatosGerados;
}

tuple<int, int, int, float, float> getMelhorCandidato(vector<tuple<int, int, int, float, float>> candidatos, mt19937 *gen)
{
    // pegar o cara que produz a menor distancia ou sortear entre os cara da lista??

    int indexCandidato = intRandom(0, candidatos.size() - 1, gen);
    // int indexCandidato = candidatos.size() - 1;
    // int indexCandidato = 0;
    tuple<int, int, int, float, float> melhorCandidato = candidatos[indexCandidato];

    // std::sort(candidatos.begin(), candidatos.end(), compare);
    // auto &melhorCandidato = candidatos.back();

    return melhorCandidato;
}

/*
   Para cada vertice em cada trip, verifico se removendo esse vertice e adicionando outro vertice que não está no tour, cria uma trip viavel.
  Se sim, adiciono numa lista de candidatos. Depois seleciono aleatoriamente um desses candidatos para adicionar na trip.
*/
Trip **generateRandomNeighbor2(OPHS *data, Trip **solution, mt19937 *gen)
{
    // cout << "---------------------------------------" << endl;
    // <indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia>
    vector<tuple<int, int, int, float, float>> candidatosGerados = geraCandidatos(data, solution, gen);

    if (!candidatosGerados.empty())
    {
        std::sort(candidatosGerados.begin(), candidatosGerados.end(), compare);
        // printCandidatos(candidatosGerados);
        tuple<int, int, int, float, float> melhorCandidato = getMelhorCandidato(candidatosGerados, gen);

        int indexTrip, indexNoRemover, idVertexAdd;
        float novoScore, novaDistancia;
        std::tie(indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia) = melhorCandidato;

        vector<Node> nodesTrip = solution[indexTrip]->getNodes();
        nodesTrip[indexNoRemover] = data->getVertex(idVertexAdd);

        solution[indexTrip]->setNodes(nodesTrip);

        float lengthTrip = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTrip);

        solution[indexTrip]->setCurrentLength(lengthTrip);

        return solution;
    }
    else
    {
        // cout <<"Não encontrou ninguem...."<<endl;
        return solution;
    }
}

/*
    Para cada nó não incluido, é encontrada a posição com menor distancia.
    Desses nós aquele que produz o maior score/novadistancia é inserido na solução
*/
Trip **insert(OPHS *data, Trip **solution, mt19937 *gen, set<int> nosExcluidos)
{
    // cout << "Aplicando insert" << endl;
    set<int> idsInSolution = getIdsInSolution(data, solution);
    for (const auto &el : nosExcluidos)
    {
        idsInSolution.insert(el);
    }

    vector<int> idsNotInSolutionOriginal = getIdsNotInSolution(data, idsInSolution);

    // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
    vector<tuple<int, int, int, float, float>> candidatosGerados;

    // achar para cada vertice não incluido a posição que faz o menor incremento na distancia
    for (int idxVertexAdd = 0; idxVertexAdd < idsNotInSolutionOriginal.size(); idxVertexAdd++)
    {
        Node noInserir = data->getVertex(idsNotInSolutionOriginal[idxVertexAdd]);

        tuple<int, int, int, float, float> melhorCandidato = make_tuple(-1, -1, noInserir.id, 0.0, 0.0); // mantem o melhor candidato para determinado idxVertexAdd

        for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
        {
            vector<Node> nodesTrip = solution[indexTrip]->getNodes();
            float antigaDistancia = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTrip);

            // tentar adicionar o no na trip em cada uma das posições
            for (int indexNo = 0; indexNo < nodesTrip.size(); indexNo++)
            {
                vector<Node> nodesTripTemp = solution[indexTrip]->getNodes();

                nodesTripTemp.insert(nodesTripTemp.begin() + indexNo, noInserir);
                float novaDistancia = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTripTemp);

                if (novaDistancia <= solution[indexTrip]->getTd()) // verifica se é viável
                {
                    float incrementoDistancia = novaDistancia - antigaDistancia;
                    float ratio = calcTripScore(nodesTripTemp);
                    if (incrementoDistancia != 0)
                        ratio = ratio / incrementoDistancia;

                    int indexTripTupla, indexNoTupla, idxVertexAddTupla;
                    float incrementoDistanciaTupla, ratioTupla;

                    std::tie(indexTripTupla, indexNoTupla, idxVertexAddTupla, incrementoDistanciaTupla, ratioTupla) = melhorCandidato;

                    if (indexTripTupla == -1)
                    { // primeiro candidato

                        // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
                        melhorCandidato = make_tuple(indexTrip, indexNo, noInserir.id, incrementoDistancia, ratio);
                    }
                    else if (incrementoDistancia < incrementoDistanciaTupla) // substitui somente se tiver um incremento menor na distancia
                    {
                        // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
                        melhorCandidato = make_tuple(indexTrip, indexNo, noInserir.id, incrementoDistancia, ratio);
                    }
                }
            }
        }
        if (get<0>(melhorCandidato) != -1)
            candidatosGerados.push_back(melhorCandidato);
    }
    std::sort(candidatosGerados.begin(), candidatosGerados.end(), compareInsert);
    // printCandidatosInsert(candidatosGerados);

    // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
    if (candidatosGerados.size() > 1)
    {
        // cout << "Inserindo ";
        // cout << " indexTrip: " << std::get<0>(candidatosGerados[0]);
        // cout << " indexNo " << std::get<1>(candidatosGerados[0]);
        // cout << " idxVertexAdd: " << std::get<2>(candidatosGerados[0]);
        // cout << " incrementoDistancia: " << std::get<3>(candidatosGerados[0]);
        // cout << " ratio " << std::get<4>(candidatosGerados[0]) << endl
        //      << endl;

        int idxAddTrip = std::get<0>(candidatosGerados[0]);
        int idxAddNo = std::get<1>(candidatosGerados[0]);
        Node noAddTrip = data->getVertex(std::get<2>(candidatosGerados[0]));

        // cout << "ANTES " << endl;
        // solution[idxAddTrip]->dadosTrip();
        // solution[idxAddTrip]->dadosNodes();

        solution[idxAddTrip]->add(noAddTrip, idxAddNo);
        float lengthTrip = calcTripLength(data, solution[idxAddTrip]->getStartHotel(), solution[idxAddTrip]->getEndHotel(), solution[idxAddTrip]->getNodes());
        solution[idxAddTrip]->setCurrentLength(lengthTrip);

        // cout << "DEPPOIS " << endl;
        // solution[idxAddTrip]->dadosTrip();
        // solution[idxAddTrip]->dadosNodes();
    }

    return solution;
}

// pode estar errada
// tenta inserir 2 caras da trip e adicionar outros com insert
Trip **extract2Insert(OPHS *data, Trip **solution, mt19937 *gen)
{
    // Para cada Trip, começo do primeiro vertice e removo 2 nos consecutivos
    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        Trip **solCopia = makeCopySolution(data, solution);
        vector<Node> nodesTrip = solCopia[indexTrip]->getNodes();

        if (solCopia[indexTrip]->getNodes().size() > 2)
        {
            for (int i = 0; i < solCopia[indexTrip]->getNodes().size() - 1; i++)
            {
                // cout << "TRIP: " << indexTrip << endl;
                solCopia = makeCopySolution(data, solution);
                float scoreAntigo = getScoreTour(data, solCopia);

                set<int> nosExcluidos;
                nodesTrip = solCopia[indexTrip]->getNodes();

                // cout << "size antes: " << nodesTrip.size() << endl;
                // printNodes(nodesTrip);

                // tenta remover i e i+1
                nosExcluidos.insert(nodesTrip[i].id);
                nosExcluidos.insert(nodesTrip[i + 1].id);

                nodesTrip.erase(nodesTrip.begin() + i);
                nodesTrip.erase(nodesTrip.begin() + i);

                // cout << "size depois: " << nodesTrip.size() << endl;
                // printNodes(nodesTrip);

                // cout << "ANTES: " << endl;

                // solCopia[indexTrip]->dadosTrip();
                // solCopia[indexTrip]->dadosNodes();
                // cout << "SCORE ANTIGO: " << scoreAntigo << endl;

                solCopia[indexTrip]->setNodes(nodesTrip);
                float lengthTrip = calcTripLength(data, solCopia[indexTrip]->getStartHotel(), solCopia[indexTrip]->getEndHotel(), nodesTrip);
                solCopia[indexTrip]->setCurrentLength(lengthTrip);

                // cout << "DEPOIS: " << endl;
                // solCopia[indexTrip]->dadosTrip();
                // solCopia[indexTrip]->dadosNodes();
                float scoreNovo = getScoreTour(data, solCopia);
                // cout << "SCORE NOVO: " << scoreNovo << endl;

                // nos removidos e solucao atualizada, agr mando pro insert para tentar melhorar
                insert(data, solCopia, gen, nosExcluidos);

                scoreNovo = getScoreTour(data, solCopia);

                if (scoreNovo > scoreAntigo)
                {
                    // cout << "Melhorou alguma coisa..." << endl;
                    solution = solCopia;
                }
                // cout << "DEPOIS ATUALIZAR SOL: " << endl;
                // solCopia[indexTrip]->dadosTrip();
                // solCopia[indexTrip]->dadosNodes();
                // cout << "SCORE NOVO APOS INSERT: " << scoreNovo << endl;
            }
        }
        // cout << " PROXIMA ITERAÇÃO" << endl;
        // exit(0);
    }

    return solution;
}

/*
    tenta trocar entre trips consecutivas
*/
Trip **exchange(OPHS *data, Trip **solution, mt19937 *gen)
{
    // indexTrip, indexNoI, indexNoJ, increseLength
    vector<tuple<int, int, int, float>> candidatosInverter; // salva apenas os que diminuir a distancia, então posso pegar o que der a menor

    for (int indexTrip = 0; indexTrip < data->getNumTrips() - 1; indexTrip++)
    {
        vector<Node> nodesTripI = solution[indexTrip]->getNodes();
        vector<Node> nodesTripJ = solution[indexTrip + 1]->getNodes();

        float lengthTripIOld = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTripI);
        float lengthTripJOld = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTripJ);

        for (int i = 0; i < nodesTripI.size(); i++)
        {
            for (int j = 0; j < nodesTripJ.size(); j++)
            {

                Node nodeAux = nodesTripI[i];
                nodesTripI[i] = nodesTripI[j];
                nodesTripJ[j] = nodeAux;

                float lengthTripI = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTripI);
                float lengthTripJ = calcTripLength(data, solution[indexTrip + 1]->getStartHotel(), solution[indexTrip + 1]->getEndHotel(), nodesTripJ);

                if (lengthTripI < solution[indexTrip]->getTd() && lengthTripJ < solution[indexTrip + 1]->getTd())
                {
                    float increseLength = (lengthTripIOld + lengthTripJOld) - (lengthTripI + lengthTripJ);

                    tuple<int, int, int, float> candidato = std::make_tuple(indexTrip, i, j, increseLength);
                    candidatosInverter.push_back(candidato);

                    // cout << " index trip: " << indexTrip << " i: " << i << " j: " << j << " increseLength: " << increseLength << endl;
                    // exit(0);
                }

                nodeAux = nodesTripI[j];
                nodesTripI[j] = nodesTripI[i];
                nodesTripI[i] = nodeAux;
            }
        }
    }

    std::sort(candidatosInverter.begin(), candidatosInverter.end(), compareExchange);

    for (int i = 0; i < candidatosInverter.size(); i++)
    {
        // indexTrip, indexNoI, indexNoJ, increseLength
        cout << "indexTrip: " << get<0>(candidatosInverter[i]);
        cout << " indexNoI: " << get<1>(candidatosInverter[i]);
        cout << " indexNoJ: " << get<2>(candidatosInverter[i]);
        cout << " increseLength: " << get<3>(candidatosInverter[i]) << endl;
    }
    if (candidatosInverter.size() > 0)
    {
        cout << " Tem candidatos...." << endl;

        int idxTripI = get<0>(candidatosInverter[0]);
        int idxNoI = get<1>(candidatosInverter[0]);
        int idxNoJ = get<2>(candidatosInverter[0]);

        vector<Node> nosTripI = solution[idxTripI]->getNodes();
        vector<Node> nosTripJ = solution[idxTripI + 1]->getNodes();

        Node nodeAux = nosTripI[idxNoI];
        nosTripI[idxNoI] = nosTripJ[idxNoJ];
        nosTripJ[idxNoJ] = nodeAux;

        solution[idxTripI]->setNodes(nosTripI);
        solution[idxTripI + 1]->setNodes(nosTripJ);

        float lengthTripI = calcTripLength(data, solution[idxTripI]->getStartHotel(), solution[idxTripI]->getEndHotel(), nosTripI);
        float lengthTripJ = calcTripLength(data, solution[idxTripI + 1]->getStartHotel(), solution[idxTripI + 1]->getEndHotel(), nosTripJ);

        solution[idxTripI]->setCurrentLength(lengthTripI);
        solution[idxTripI + 1]->setCurrentLength(lengthTripJ);

        printTrips(data, solution);

        exit(0);
    }

    return solution;
}
/*
    Tem quer ser combinado com insert caso diminua o length
    Faço uma analise para trip
    Identifico qual inversao numa trip diminui mais a distancia
    Se tiver alguma, faço essa inversao e atualizo a solução
*/
Trip **twoOpt(OPHS *data, Trip **solution, mt19937 *gen)
{
    // cout << "Aplicando twoOpt" << endl;
    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        vector<Node> nodesTrip = solution[indexTrip]->getNodes();
        float antigaDistancia = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTrip);

        // <i, j, novaDistancia>
        tuple<int, int, float> candidatoInverter; // salva apenas os que diminuir a distancia, então posso pegar o que der a menor
        candidatoInverter = make_tuple(-1, -1, -1);

        for (int i = 0; i < nodesTrip.size(); i++)
        {
            for (int j = i + 1; j < nodesTrip.size(); j++)
            {
                vector<Node> novoNodesTrip = solution[indexTrip]->getNodes();

                novoNodesTrip[i] = nodesTrip[j];
                novoNodesTrip[j] = nodesTrip[i];

                float novaDistancia = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), novoNodesTrip);

                if (novaDistancia < antigaDistancia)
                {
                    // cout << "i: " << i << " j " << j << " old " << antigaDistancia << " new " << novaDistancia << endl;
                    // printNodes(nodesTrip);
                    // printNodes(novoNodesTrip);

                    int posI, posJ;
                    float newD;
                    std::tie(posI, posJ, newD) = candidatoInverter;

                    if (posI == -1) // primeira inserção na tupla
                    {
                        candidatoInverter = make_tuple(i, j, novaDistancia);
                    }
                    else if (novaDistancia < newD) // substitui somente se a novaDistancia for menor que a atual
                    {
                        candidatoInverter = make_tuple(i, j, novaDistancia);
                    }
                }
            }
        }

        if (std::get<0>(candidatoInverter) != -1)
        {
            // cout << "Trocando ";
            // cout << "Trip " << indexTrip;
            // cout << " i: " << std::get<0>(candidatoInverter);
            // cout << " j " << std::get<1>(candidatoInverter);
            // cout << " antigoD: " << antigaDistancia;
            // cout << " melhorD " << std::get<2>(candidatoInverter) << endl
            //      << endl;

            // cout << "antes: " << endl;
            // solution[indexTrip]->dadosTrip();
            // solution[indexTrip]->dadosNodes();
            solution[indexTrip]->inverterIndexTrip(std::get<0>(candidatoInverter), std::get<1>(candidatoInverter), std::get<2>(candidatoInverter));

            // cout << "depois" << endl;
            // solution[indexTrip]->dadosTrip();
            // solution[indexTrip]->dadosNodes();
        }
    }

    return solution;
}

float updateTemperature(float T)
{
    return 0.99 * T;
}
/// @brief Gera uma solução vizinha de uma solução inicial com SA
/// @param data
/// @param initialSolution solução inicial
/// @param iterations número de iterações por temperatura
/// @param T Temperatura inicial
/// @param Tmin Temperatura mínima
/// @return Trip
Trip **simulatedAnnealing(OPHS *data, Trip **initialSolution, int iterations, float T, float Tmin, mt19937 *gen)
{
    Trip **bestSolution = initialSolution;
    set<int> vazio;

    int iteracoesT = 0;
    while (T > Tmin)
    {
        int iter = 0;
        while (iter < iterations)
        {
            Trip **copySolution = makeCopySolution(data, initialSolution);
            Trip **neighborSolution = generateRandomNeighbor2(data, copySolution, gen);
            // neighborSolution = insert(data, neighborSolution, gen, vazio);
            // neighborSolution = generateRandomNeighbor2(data, neighborSolution, gen);

            // neighborSolution = exchange(data, neighborSolution, gen);
            // neighborSolution = extract2Insert(data, neighborSolution, gen);

            float neighborSolutionScore = getScoreTour(data, neighborSolution);
            float initialSolutionScore = getScoreTour(data, initialSolution);

            float deltaE = neighborSolutionScore - initialSolutionScore;

            if (deltaE > 0)
            {
                initialSolution = neighborSolution;

                // posso gerar uma solução (neighborSolution) melhor que a solução base (initialSolution), deltaE>0,
                // mas que não é melhor que a melhor solução obtida  até agora (bestSolution)
                float bestTotalScore = getScoreTour(data, bestSolution);
                if (neighborSolutionScore > bestTotalScore)
                {
                    bestSolution = neighborSolution;
                    cout << "Achou sol melhor..."
                         << "Qualidade: " << neighborSolutionScore << endl;
                }
            }
            else
            {
                float p = exp(-deltaE / T);
                float x = doubleRandom(0.0, 1.0, gen);
                // cout << "Temperatura T: " << T << "\t|";
                // cout << " deltaE: " << deltaE << "\t|";
                // cout << " probabilidade: " << p << "\t\t|";
                // cout << " Valor aleatorio: " << x << endl;
                if (x < p)
                {
                    initialSolution = neighborSolution;
                }
            }

            // cout << "Iteração: " << iter << " Qualida da best: " << getScoreTour(data, bestSolution)<<endl;

            iter++;
        }
        iteracoesT++;
        T = updateTemperature(T);
        if (iteracoesT % 10 == 0)
            cout << "IteracaoTemperatura: " << iteracoesT << " Temperatura: " << T << " Qualida da best: " << getScoreTour(data, bestSolution) << endl;
    }
    return bestSolution;
}

#endif