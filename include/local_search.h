#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "util.h"

// <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
void printCandidatosInsert(vector<tuple<int, int, int, float, float>> candidatosGerados)
{
    cout << "Candidatos Inser..." << endl;
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

void printCandidatosMoveBest(vector<tuple<int, int, int, int, int, float>> candidatosGerados)
{
    cout << "Candidatos Move Best..." << endl;
    for (const auto &tupla : candidatosGerados)
    {
        int indexTripRemover, indexNoRemover, indexTripAdd, indexAddNo, idxVertxAdd;
        float descreaseDistanceTour;

        // desempacotando os dados
        std::tie(indexTripRemover, indexNoRemover, indexTripAdd, indexAddNo, idxVertxAdd, descreaseDistanceTour) = tupla;

        cout << "indexTripRemover " << indexTripRemover << endl;
        cout << "indexNoRemover " << indexNoRemover << endl;
        cout << "indexTripAdd " << indexTripAdd << endl;
        cout << "indexAddNo " << indexAddNo << endl;
        cout << "idxVertxAdd " << idxVertxAdd << endl;
        cout << "descreaseDistanceTour: " << descreaseDistanceTour << endl;
        cout << "-----------------------" << endl;
    }
}

//  <indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia>
bool compare(tuple<int, int, int, float, float> a, tuple<int, int, int, float, float> b)
{
    return std::get<4>(a) > std::get<4>(b);
}

// <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>

// Among the vertices for which the insertion does not make the trip
// infeasible, the vertex with maximum ratio of score over this
// increase is inserted.
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
    set<int> idsInSolution = getIdsInSolution(data, solution);
    vector<int> idsNotInSolution = getIdsNotInSolution(data, idsInSolution);
    float initialScore = getScoreTour(data, solution);

    int k = 0;
    while (k < 1000)
    {
        int indexRandomTrip = intRandom(0, data->getNumTrips() - 1, gen); // index da trip a ser modificada
        vector<Node> nodesTrip = solution[indexRandomTrip]->getNodes();   // nos da trip aleatoria

        // pega um no aleatório da trip [indexRandomTrip] - nodesTrip não tem os hoteis
        if (nodesTrip.size() == 0)
            continue;

        int indexRandomNodeRemover = intRandom(0, nodesTrip.size() - 1, gen); // index do no que vai ser removido

        for (int i = 0; i < idsNotInSolution.size(); i++)
        {
            int idRandomVertexAdd = idsNotInSolution[i];

            Node randomVertex = data->getVertex(idRandomVertexAdd);
            nodesTrip[indexRandomNodeRemover] = randomVertex; // Substituir por randomVertex

            float lengthTrip = calcTripLength(data, solution[indexRandomTrip]->getStartHotel(), solution[indexRandomTrip]->getEndHotel(), nodesTrip);
            float newScore = getScoreTour(data, solution);

            // if (initialScore > newScore)
            if (lengthTrip < solution[indexRandomTrip]->getTd())
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

                // cout << "Gerou solução viavel" << endl;
                // cout << "index trip a ser modificada: " << indexRandomTrip << endl;
                // cout << "index no removido: " << indexRandomNodeRemover << endl;
                // cout << "id do no add: " << indexRandomVertexAdd << endl;
                // printTrips(data, solution);
                // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;
                return solution;
            }
            // cout <<"indexRandomTrip: "<<indexRandomTrip<<" indexRandomVertexAdd: "<<indexRandomVertexAdd<< " indexRandomNodeRemover: "<<indexRandomNodeRemover<<endl;
        }
        k++;
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

        float lengthTrip = calcTripLength(data, solution[indexTrip]->getStartHotel(), solution[indexTrip]->getEndHotel(), nodesTrip);

        if (lengthTrip <= solution[indexTrip]->getTd())
        {
            solution[indexTrip]->setNodes(nodesTrip);
            solution[indexTrip]->setCurrentLength(lengthTrip);
        }
        
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
                    float ratio = getTripScore(nodesTripTemp);
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
    if (candidatosGerados.size() > 0)
    {
        // cout << "Inserindo ";
        // cout << " indexTrip: " << std::get<0>(candidatosGerados[0]);
        // cout << " indexNo " << std::get<1>(candidatosGerados[0]);
        // cout << " idVertexAdd: " << std::get<2>(candidatosGerados[0]);
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

        // solution[idxAddTrip]->dadosTrip();
        // solution[idxAddTrip]->dadosNodes();
        return solution;
    }
    else
    {
        return nullptr;
    }
}


/*
    Para cada nó não incluido, é encontrada a posição com menor distancia.
    Desses nós aquele que produz o maior score/novadistancia é inserido na solução
    Faz isso a partir de uma trip especifica
*/
Trip **insertGenetic(OPHS *data, Trip **solution, mt19937 *gen, set<int> nosExcluidos)
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

        for (int indexTrip = data->getNumTrips() - 1; indexTrip >= 0; indexTrip--)
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
                    float ratio = getTripScore(nodesTripTemp);
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
    if (candidatosGerados.size() > 0)
    {
        // cout << "Inserindo ";
        // cout << " indexTrip: " << std::get<0>(candidatosGerados[0]);
        // cout << " indexNo " << std::get<1>(candidatosGerados[0]);
        // cout << " idVertexAdd: " << std::get<2>(candidatosGerados[0]);
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

        // solution[idxAddTrip]->dadosTrip();
        // solution[idxAddTrip]->dadosNodes();
        return solution;
    }
    else
    {
        return nullptr;
    }
}

Trip **insertPlus(OPHS *data, Trip **solution, mt19937 *gen, set<int> nosExcluidos)
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
                    float ratio = getTripScore(nodesTripTemp);
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
                    else if (incrementoDistancia <= incrementoDistanciaTupla) // substitui somente se tiver um incremento menor na distancia
                    {
                        // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
                        melhorCandidato = make_tuple(indexTrip, indexNo, noInserir.id, incrementoDistancia, ratio);
                    }
                }
            }
        }
        // encontrou pelo menos 1 candidato
        if (get<0>(melhorCandidato) != -1)
            candidatosGerados.push_back(melhorCandidato);
    }
    std::sort(candidatosGerados.begin(), candidatosGerados.end(), compareInsert);
    // printCandidatosInsert(candidatosGerados);

    // <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
    if (candidatosGerados.size() > 0)
    {
        for (int ci = 0; ci < candidatosGerados.size(); ci++)
        {
            int idxAddTrip = std::get<0>(candidatosGerados[ci]);
            int idxAddNo = std::get<1>(candidatosGerados[ci]);
            Node noAddTrip = data->getVertex(std::get<2>(candidatosGerados[ci]));

            vector<Node> nodesTripTemp = solution[idxAddTrip]->getNodes();
            float antigaDistancia = calcTripLength(data, solution[idxAddTrip]->getStartHotel(), solution[idxAddTrip]->getEndHotel(), nodesTripTemp);

            nodesTripTemp.insert(nodesTripTemp.begin() + idxAddNo, noAddTrip);
            float novaDistancia = calcTripLength(data, solution[idxAddTrip]->getStartHotel(), solution[idxAddTrip]->getEndHotel(), nodesTripTemp);

            if (novaDistancia <= solution[idxAddTrip]->getTd())
            {
                solution[idxAddTrip]->setNodes(nodesTripTemp);
                solution[idxAddTrip]->setCurrentLength(novaDistancia);
            }
        }
        return solution;
    }
    else
    {
        return nullptr;
    }
}

// tenta inserir 2 caras da trip e adicionar outros com insert
Trip **extract2Insert(OPHS *data, Trip **solution, mt19937 *gen)
{
    // Para cada Trip, começo do primeiro vertice e removo 2 nos consecutivos
    set<int> nosExcluidos;
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

                nodesTrip = solCopia[indexTrip]->getNodes();

                // cout << "Trip: " << indexTrip << " Tamanho" << nodesTrip.size() << endl;
                // printNodes(nodesTrip);

                // tenta remover i e i+1
                // cout << "Extraindo: " << nodesTrip[i].id << " e " << nodesTrip[i + 1].id << endl;
                nosExcluidos.insert(nodesTrip[i].id);
                nosExcluidos.insert(nodesTrip[i + 1].id);

                if (i + 1 < nodesTrip.size())
                { // Verifique se há dois elementos consecutivos para remover
                    nodesTrip.erase(nodesTrip.begin() + i, nodesTrip.begin() + i + 2);
                }
                // nodesTrip.erase(nodesTrip.begin() + i);
                // nodesTrip.erase(nodesTrip.begin() + i);

                // cout << "Trip: " << indexTrip << " i: " << i << " Tamanho: " << nodesTrip.size() << endl;
                // printNodes(nodesTrip);

                // cout << "ANTES: " << endl;

                // solCopia[indexTrip]->dadosTrip();
                // solCopia[indexTrip]->dadosNodes();
                // cout << "SCORE ANTIGO: " << scoreAntigo << endl;

                float lengthTrip = calcTripLength(data, solCopia[indexTrip]->getStartHotel(), solCopia[indexTrip]->getEndHotel(), nodesTrip);

                if (lengthTrip <= solCopia[indexTrip]->getTd())
                {
                    solCopia[indexTrip]->setNodes(nodesTrip);
                    solCopia[indexTrip]->setCurrentLength(lengthTrip);

                    // cout << "DEPOIS: " << endl;
                    // solCopia[indexTrip]->dadosTrip();
                    // solCopia[indexTrip]->dadosNodes();
                    float scoreNovo = getScoreTour(data, solCopia);
                    // cout << "SCORE NOVO: " << scoreNovo << endl;

                    // nos removidos e solucao atualizada, agr mando pro insert para tentar melhorar
                    // cout << "Trip: " << indexTrip << " ANTES INSERT EXTRACT2..." << lengthTrip << endl;

                    while (insert(data, solCopia, gen, nosExcluidos) != nullptr)
                        ;

                    // cout << "Trip: " << indexTrip << " DEPOIS INSERT EXTRACT2..." << calcTripLength(data, solCopia[indexTrip]->getStartHotel(), solCopia[indexTrip]->getEndHotel(), solCopia[indexTrip]->getNodes()) << endl;

                    scoreNovo = getScoreTour(data, solCopia);

                    if (scoreNovo > scoreAntigo)
                    {
                        // cout << "Melhorou alguma coisa...Size Trip " << indexTrip << " " << lengthTrip << endl;
                        solution = solCopia;
                        i = 0;
                    }

                    // cout << "DEPOIS ATUALIZAR SOL: " << endl;
                    // solCopia[indexTrip]->dadosTrip();
                    // solCopia[indexTrip]->dadosNodes();
                    // cout << "SCORE NOVO APOS INSERT: " << scoreNovo << endl;
                }
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

    // for (int i = 0; i < candidatosInverter.size(); i++)
    // {
    //     // indexTrip, indexNoI, indexNoJ, increseLength
    //     cout << "indexTrip: " << get<0>(candidatosInverter[i]);
    //     cout << " indexNoI: " << get<1>(candidatosInverter[i]);
    //     cout << " indexNoJ: " << get<2>(candidatosInverter[i]);
    //     cout << " increseLength: " << get<3>(candidatosInverter[i]) << endl;
    // }
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

        // printTrips(data, solution);
        set<int> vazio;
        return insert(data, solution, gen, vazio);
    }
    else
    {
        return solution;
    }
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
    bool inverteu = false;
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
            inverteu = true;
        }
    }
    if (inverteu == false)
        return solution;
    else
        return solution;
}

/*
    Calcula a distancia total desconsiderando a trip pivo
*/
float calcDTour(OPHS *data, Trip **solutionCopy, int indexTripPivot)
{
    float total = 0;
    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        if (indexTrip != indexTripPivot)
        {
            total += solutionCopy[indexTrip]->getCurrentLength();
        }
    }
    return total;
}

/*
    Encontrar  a melhor posição para o nó pivot
    solutionCopy não tem o no pivot
    no pior caso, a melhor posição é a que ele já estava

    Quero saber se adicionando o pivot na solutionCopy, a nova distancia fica menor que tourDistance
*/
tuple<int, int, int, int, int, float> findBestPosition(OPHS *data, Trip **solutionCopy, mt19937 *gen, Node pivot, float tourDistance, int indexTripRemover, int indexNoRemover)
{
    /*
        indexTripRemover - index da trip de onde o no vai ser removido
        indexNoRemover - index na trip do no que vai ser removido
        indexTripAdd - index da trip onde o no vai ser adicioando
        indexAddNo - index na trip ondeo o no vai ser adicionado
        idxVertxAdd - id do no que vai ser adicionado
        descreaseDistanceTour - decremento da distancia da tour
    */

    // mantém o melhor candidato para No pivot
    tuple<int, int, int, int, int, float> melhorCandidato = make_tuple(-1, -1, -1, -1, pivot.id, 0.0);

    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        vector<Node> nodesTrip = solutionCopy[indexTrip]->getNodes();

        for (int indexNo = 0; indexNo <= nodesTrip.size(); indexNo++)
        {
            vector<Node> nodesTripTemp = solutionCopy[indexTrip]->getNodes();

            // insiro o no pivo na posição indexNo e calculo a nova distancia da trip
            nodesTripTemp.insert(nodesTripTemp.begin() + indexNo, pivot);
            // printNodes(nodesTripTemp);

            float novaDistancia = calcTripLength(data, solutionCopy[indexTrip]->getStartHotel(), solutionCopy[indexTrip]->getEndHotel(), nodesTripTemp);
            float newTourDistance = calcDTour(data, solutionCopy, indexTrip) + novaDistancia;

            if (novaDistancia <= solutionCopy[indexTrip]->getTd() && newTourDistance < tourDistance)
            {
                // cout << "OLD: " << tourDistance << " NEW: " << newTourDistance << endl;
                float newDecreaseDistance = tourDistance - newTourDistance;

                if (get<0>(melhorCandidato) == -1)
                { // primeiro candidato
                    melhorCandidato = make_tuple(indexTripRemover, indexNoRemover, indexTrip, indexNo, pivot.id, newDecreaseDistance);
                }
                else if (newDecreaseDistance > get<5>(melhorCandidato)) // substitui somente se tiver um decremnto maior da distancia da tour
                {
                    melhorCandidato = make_tuple(indexTripRemover, indexNoRemover, indexTrip, indexNo, pivot.id, newDecreaseDistance);
                }
            }
        }
    }

    return melhorCandidato;
}

Trip **moveBest(OPHS *data, Trip **solution, mt19937 *gen)
{
    // pegar os nós que estao na solução
    // set<int> idsInSolution = getIdsInSolution(data, solution);

    float tourDistance = calcTourLength(data, solution);

    /*
        indexTripRemover - index da trip de onde o no vai ser removido
        indexNoRemover - index na trip do no que vai ser removido
        indexTripAdd - index da trip onde o no vai ser adicioando
        indexAddNo - index na trip ondeo o no vai ser adicionado
        idxVertxAdd - id do no que vai ser adicionado
        descreaseDistanceTour - decremento da distancia da tour
    */
    vector<tuple<int, int, int, int, int, float>> candidatosGerados;
    tuple<int, int, int, int, int, float> melhorCandidato;

    for (int indexTrip = 0; indexTrip < data->getNumTrips(); indexTrip++)
    {
        vector<Node> nodesTrip = solution[indexTrip]->getNodes();

        for (int indexNo = 0; indexNo < nodesTrip.size(); indexNo++)
        {
            vector<Node> nodesTripTemp = solution[indexTrip]->getNodes();
            Node pivot = nodesTripTemp[indexNo];

            // cout << "Antes..." << endl;
            // printNodes(nodesTripTemp);

            // apago o no pivo
            nodesTripTemp.erase(nodesTripTemp.begin() + indexNo);

            Trip **solutionCopy = makeCopySolution(data, solution);
            solutionCopy[indexTrip]->setNodes(nodesTripTemp);
            solutionCopy[indexTrip]->setCurrentLength(calcTripLength(data, solutionCopy[indexTrip]));
            // cout << "Avaliando o no " << pivot.id << endl;

            melhorCandidato = findBestPosition(data, solutionCopy, gen, pivot, tourDistance, indexTrip, indexNo);

            if (get<0>(melhorCandidato) != -1)
            {
                // cout << "Achou candidato.." << endl;
                candidatosGerados.push_back(melhorCandidato);
            }

            // cout << "Depois..." << endl;
            // printNodes(nodesTripTemp);
        }
    }

    // ordernar de forma decrescente pelo decreaseDistance
    std::sort(candidatosGerados.begin(), candidatosGerados.end(),
              [](const auto &a, const auto &b)
              {
                  return std::get<5>(a) > std::get<5>(b);
              });

    if (candidatosGerados.size() > 0)
    {
        // Tentar addicionar os candidatos na solution
        // Verificar a viabilidade a cada candidato inserido
        // Se violar, não adicionar e tentar o proximo

        /*
            indexTripRemover - index da trip de onde o no vai ser removido
            indexNoRemover - index na trip do no que vai ser removido
            indexTripAdd - index da trip onde o no vai ser adicioando
            indexAddNo - index na trip ondeo o no vai ser adicionado
            idxVertxAdd - id do no que vai ser adicionado
            descreaseDistanceTour - decremento da distancia da tour
        */
        int indexTripRemover = std::get<0>(candidatosGerados[0]);
        int indexNoRemover = std::get<1>(candidatosGerados[0]);
        int indexTripAdd = std::get<2>(candidatosGerados[0]);
        int indexAddNo = std::get<3>(candidatosGerados[0]);
        int idxVertxAdd = std::get<4>(candidatosGerados[0]);
        float descreaseDistanceTour = std::get<5>(candidatosGerados[0]);
        Node noAddTrip = data->getVertex(idxVertxAdd);

        // cout << "Trocando o no " << idxVertxAdd 
        //         << " - Remover da Trip " << indexTripRemover << " index " << indexNoRemover << 
        //         " - Add na Trip " << indexTripAdd << " index " << indexAddNo <<" DescreaseDistanceTour: "<<descreaseDistanceTour<< endl;
        // cout << "ANTES..." << endl;
        // printIdsTrips(data, solution);

        // Remover o no da trip e atualizar a solução
        vector<Node> nodesTripRemover = solution[indexTripRemover]->getNodes();

        nodesTripRemover.erase(nodesTripRemover.begin() + indexNoRemover);

        solution[indexTripRemover]->setNodes(nodesTripRemover);
        float lengthTrip = calcTripLength(data, solution[indexTripRemover]->getStartHotel(), solution[indexTripRemover]->getEndHotel(), nodesTripRemover);
        solution[indexTripRemover]->setCurrentLength(lengthTrip);

        // Add o no na trip e atualizar a solução caso seja viável
        vector<Node> nodesTripAdd = solution[indexTripAdd]->getNodes();
        nodesTripAdd.insert(nodesTripAdd.begin() + indexAddNo, noAddTrip);
        lengthTrip = calcTripLength(data, solution[indexTripAdd]->getStartHotel(), solution[indexTripAdd]->getEndHotel(), nodesTripAdd);
        if (lengthTrip <= solution[indexTripAdd]->getTd())
        {
            solution[indexTripAdd]->setNodes(nodesTripAdd);
            solution[indexTripAdd]->setCurrentLength(lengthTrip);
            // cout << "DEPOIS..." << endl;
            // printIdsTrips(data, solution);
        }
        else
        {
            cout << "Inviavel..." << endl;
        }
    }

    return solution;
}

#endif