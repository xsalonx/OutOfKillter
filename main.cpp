//https://booksc.org/book/37275399/a95a1b/?wrongHash

#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <algorithm>
#include <unordered_set>

#define BitomrukArcFirst 1
#define ScoreDiv 2

using namespace std;



int mapNodeNameToIdx(int f, int l, int n){
    if (f == -1) {
        return l;
    } else if(f==n-1) {
        return n + ((n)*(n-1))/2 + l;
    } else {
        return ((n+1)*n)/2 - ((n-f)*(n - f - 1))/2 + (l - f - 1);
    }
}

struct Edge{
    int lowBound, upBound, flow, fr, to;
    long long int cost;
};


struct Node{
    long long int potential;
    bool labelled;
    Edge* labelledThroughEdge;
    bool inOrderToIncreaseFlow;
    vector<Edge*> out;
    vector<Edge*> in;
};

class Graph{
public:
    int V;
    int BitomrukPointsOnStart;
    int BitomrukScoreToWin;
    int entrantsNumb;
    vector<Edge*> edges;
    Node** vertices;
    // zawodnicy mają NodeName (-1, 0), (-1, 1) ... (-1, n-1)
    // mecze maję NodeName (0, 1), (0, 2) ... (0, n-1), (1, 2), (1, 3) ... (1, n-1), ... ..., (n-2, n-1)

    Graph(int n, int BitomrukScoreToWin_, int dataSet[][4]){
        BitomrukPointsOnStart=0;
        entrantsNumb = n;
        BitomrukScoreToWin = BitomrukScoreToWin_;
        V = 2 + ((n-1)*n)/2 + n; // Liczba wszystkich wierzchołków: zawodnicy + mecze + ujście + źródło, ujście ma indeks V-2, a źródło V-1;
        vertices = new Node*[V];
        int tIdx = mapNodeNameToIdx(n-1, 0, n);
        int sIdx = mapNodeNameToIdx(n-1, 1, n);
        unordered_set<int> inactiveNodes;
// Konstruowanie punktów odpowiadjących zawodnikom;
        for (int i=0; i<n; i++) {
            vertices[i] = new Node;
        }
// Konstruowanie wierzchołków odpowiadjących meczom;
        for (int i=n; i<V-2; i++) {
            vertices[i] = new Node;
        }
// Konstruowanie ujścia i źródła;
        vertices[V-2] = new Node;
        vertices[V-1] = new Node;
        int idx;
// Dodawanie krawędzi między wierzchołkami meczów, a zwodników;
        int entrant1, entrant2, winner, briberyCost, looser;
        for (int i=0; i<(n*(n-1))/2; i++) {
            entrant1 = dataSet[i][0];
            entrant2 = dataSet[i][1];
            if (entrant1 > entrant2) {
                int tmp = entrant1;
                entrant1 = entrant2;
                entrant2 = tmp;
            }
            winner = dataSet[i][2];
            if (winner==entrant1){
                looser = entrant2;
            } else {
                looser = entrant1;
            }
            briberyCost = dataSet[i][3];
            idx = mapNodeNameToIdx(entrant1, entrant2, n);
            if (winner == 0/*Bitomruk wygrywa*/) {
                // Wierzchołek (ent1, ent2) będzie zbędny...
                BitomrukPointsOnStart++;
                inactiveNodes.insert(idx);
            } else /* Dodajemy krawędzie z meczu do obu zawodników */{
                Edge *eWinner = new Edge;
                eWinner->fr = idx;
                eWinner->to = mapNodeNameToIdx(-1, winner ,n);
                eWinner->upBound=1;
                eWinner->lowBound=0;
                eWinner->cost = 0;
                vertices[idx]->out.push_back(eWinner);
                vertices[winner]->in.push_back(eWinner);
                edges.push_back(eWinner);

                Edge * eLooser = new Edge;
                eLooser->fr = idx;
                eLooser->to = mapNodeNameToIdx(-1, looser, n);
                eLooser->cost = briberyCost;
                eLooser->lowBound=0;
                eLooser->upBound=1;
                vertices[idx]->out.push_back(eLooser);
                vertices[looser]->in.push_back(eLooser);
                edges.push_back(eLooser);
            }
        }
// Dodawanie krawędzi między wierzchołkami zawodników, a ujściem;
        // Od Bitomruka;
        Edge *e = new Edge;
        e->fr = 0;
        e->to = tIdx;
        e->upBound = BitomrukScoreToWin_-BitomrukPointsOnStart;
        e->lowBound = BitomrukScoreToWin_-BitomrukPointsOnStart;
        e->cost=0;
        vertices[0]->out.push_back(e);
        vertices[V-2]->in.push_back(e);
        edges.push_back(e);
        // Dla reszty zawodników;
        for (int v=1; v<n; v++) {
            e = new Edge;
            e->upBound = BitomrukScoreToWin_;
            e->lowBound=0;
            e->cost=0;
            e->fr = v;
            e->to = tIdx;
            vertices[v]->out.push_back(e);
            vertices[V-2]->in.push_back(e);
            edges.push_back(e);
        }
// Dodawanie krawędzi między źródłem, a wierzchołkami meczy;
        for (idx=n; idx<V-2; idx++) {
                if (inactiveNodes.find(idx)==inactiveNodes.end()) {
                    e = new Edge;
                    e->to = idx;
                    e->fr = sIdx;
                    e->lowBound = 1;
                    e->upBound = 1;
                    e->cost = 0;
                    vertices[V - 1]->out.push_back(e);
                    vertices[idx]->in.push_back(e);
                    edges.push_back(e);
                }

        }
// Dodanie krawędzi między ujściem, a źródłem;
        e = new Edge;
        e->lowBound = (n*(n-1))/2 - BitomrukPointsOnStart;
        e->upBound = (n*(n-1))/2 - BitomrukPointsOnStart;
        e->cost=0;
        e->fr = tIdx;
        e->to = sIdx;
        vertices[V-2]->out.push_back(e);
        vertices[V-1]->in.push_back(e);
        edges.push_back(e);


    }

    void resetGraph() {
        for (int idx=0; idx<this->V; idx++) {
            this->vertices[idx]->labelled = false;
            this->vertices[idx]->potential = 0;
        }
        for (int i=0; i<this->edges.size(); i++) {
            this->edges[i]->flow = 0;
        }
    }
    void resetLabelling() {
        for (int i=0; i<this->V; i++) {
            this->vertices[i]->labelled = false;
        }
    }
    void resetPotentials(){
        for (int i=0; i<this->V; i++) {
            this->vertices[i]->potential=0;
        }
    }
    void modifyBitomrukExpectations(int newBitomrukScoreToWin){
        this->BitomrukScoreToWin = newBitomrukScoreToWin;
        this->vertices[0]->out[0]->lowBound = newBitomrukScoreToWin - this->BitomrukPointsOnStart;
        this->vertices[0]->out[0]->upBound = newBitomrukScoreToWin - this->BitomrukPointsOnStart;
        for (int i=1; i< this->entrantsNumb; i++) {
            this->vertices[i]->out[0]->upBound = newBitomrukScoreToWin;
        }

    }
    void clearGraph() {
        for (int i=0; i<this->edges.size(); i++) {
            delete this->edges[i];
        }
        for (int i=0; i<this->V; i++) {
            delete this->vertices[i];
        }
        delete this->vertices;
    }
};


long long int calcProfit(Edge* e, Graph &G, int from, int to) {
    return (long long int)(G.vertices[to]->potential - e->cost - G.vertices[from]->potential);
}

bool isReacheableByDeacreasing(Edge* e, Graph &G, int antyToIdx, int antyFromIdx) {
    long long int profit = calcProfit(e, G, antyToIdx, antyFromIdx);
    return (profit <= 0 && e->flow > e->lowBound) or (profit > 0 && e->flow > e->upBound);
}

bool isReacheableByIncreasing(Edge* e, Graph &G, int fromIdx, int toIdx) {
    long long int profit = calcProfit(e, G, fromIdx, toIdx);
    return (profit < 0 && e->flow < e->lowBound) or (profit >= 0 && e->flow < e->upBound);
}

bool DFSLabelling(Graph &G, vector<int> &labelledNodes, int fromIdx, int alphaIdx) {

    int toIdx, antyToIdx;
    Edge* e;
    G.vertices[fromIdx]->labelled = true;
    labelledNodes.push_back(fromIdx);
    if (alphaIdx == fromIdx) return true;

// Etykietowanie wierzchołków osiągalnych z G.vertices[fromIdx];
    for (int i=0; i<G.vertices[fromIdx]->out.size(); i++) {
        e = G.vertices[fromIdx]->out[i];
        toIdx = e->to;
        if (!G.vertices[toIdx]->labelled and isReacheableByIncreasing(e, G, fromIdx, toIdx)) {
            G.vertices[toIdx]->labelledThroughEdge = e;
            G.vertices[toIdx]->inOrderToIncreaseFlow = true;
            if (DFSLabelling(G, labelledNodes, toIdx, alphaIdx)) return true;
        }
    }
// Etykietowanie wierzchołków, z których G.vertices[fromIdx] jest osiągalny;
    for (int i=0; i<G.vertices[fromIdx]->in.size(); i++) {
        e = G.vertices[fromIdx]->in[i];
        antyToIdx = e->fr;
        if (!G.vertices[antyToIdx]->labelled and isReacheableByDeacreasing(e, G, antyToIdx, fromIdx)) {
            G.vertices[antyToIdx]->labelledThroughEdge = e;
            G.vertices[antyToIdx]->inOrderToIncreaseFlow = false;
            if (DFSLabelling(G, labelledNodes, antyToIdx, alphaIdx)) return true;

        }
    }

    return false;
}
void adjustFlowThroughAugmentingPath(Graph &G, int alphaIdx) {
    int currIdx = alphaIdx;
    if (G.vertices[currIdx]->inOrderToIncreaseFlow) {
        G.vertices[currIdx]->labelledThroughEdge->flow += 1;
        currIdx = G.vertices[currIdx]->labelledThroughEdge->fr;
    } else {
        G.vertices[currIdx]->labelledThroughEdge->flow -= 1;
        currIdx = G.vertices[currIdx]->labelledThroughEdge->to;
    }

    while (currIdx != alphaIdx) {
        if (G.vertices[currIdx]->inOrderToIncreaseFlow) {
            G.vertices[currIdx]->labelledThroughEdge->flow += 1;
            currIdx = G.vertices[currIdx]->labelledThroughEdge->fr;
        } else {
            G.vertices[currIdx]->labelledThroughEdge->flow -= 1;
            currIdx = G.vertices[currIdx]->labelledThroughEdge->to;
        }
    }
}
int increasePotentials(Graph &G, vector<int> &labelledNodes) {
    long long int currProfit;
    int unlabelledNodeIdx;
    int labelledNodeIdx;
    Edge* e;
    // Obliczanie min neg
    long long int profit1 = LONG_LONG_MIN;
    for (int i=0; i<labelledNodes.size(); i++) {
        labelledNodeIdx = labelledNodes[i];
        for (int j=0; j<G.vertices[labelledNodeIdx]->out.size(); j++) {
            e = G.vertices[labelledNodeIdx]->out[j];
            unlabelledNodeIdx = e->to;
            if (!G.vertices[unlabelledNodeIdx]->labelled) {
                currProfit = calcProfit(e, G, labelledNodeIdx, unlabelledNodeIdx);
                if (currProfit < 0 && e->flow <= e->upBound) profit1 = max(currProfit, profit1);
            }
        }
    }
    // Obliczanie min pos
    long long int profit2 = LONG_LONG_MAX;
    for (int idx = 0; idx < G.V; idx++) {
        if (!G.vertices[idx]->labelled) {
            for (int j=0; j<G.vertices[idx]->out.size(); j++) {
                labelledNodeIdx = G.vertices[idx]->out[j]->to;
                currProfit = calcProfit(e, G, unlabelledNodeIdx, labelledNodeIdx);
                if (currProfit > 0 && e->flow >= e->lowBound) profit2 = min(profit2, currProfit);
            }
        }
    }
// Zwiększanie potencjałów wierzchołków niepoetykietowanych
    long long int deltaPotential;
    if (profit2 == LONG_LONG_MAX && profit1 == LONG_LONG_MIN) return -1;
    else if (profit1 == LONG_LONG_MIN) deltaPotential = profit2;
    else if (profit2 == LONG_LONG_MAX) deltaPotential = -profit1;
    else deltaPotential = min(profit2, -profit1);
    // Zwiększanie potencjałów;
    for (int idx = 0; idx < G.V; idx++) {
        G.vertices[idx]->potential += deltaPotential;
    }
    for (int i=0; i<labelledNodes.size(); i++) {
        G.vertices[labelledNodes[i]]->potential -= deltaPotential;
    }

    return 0;
}

int minCostForParticularBriberingModusOperandi(int n, int matchesNumber, Graph &G) {
// ..(B)..; Szukanie krawędzi "out-of-kilter", w tym przypadku zawsze krawędź t -> s lub (-1, 0) -> t,
// więc alphaIdx zawsze równe G.V-2 lub 0, a betaIdx zawsze równe G.V-1 lub G.V-2;

    vector<int> labelledNodes;
    int alphaIdx = G.V-2;
    int betaIdx = G.V-1;
    Edge* e;

    while (true) {

// ..(C)..; Etykietowanie wierzchołków póki można i wierzchołek alpha nie jest zaetykietowany;

        G.vertices[betaIdx]->labelledThroughEdge = G.vertices[betaIdx]->in[0];
        G.vertices[betaIdx]->inOrderToIncreaseFlow = true;
        if (betaIdx==G.V-2) G.vertices[betaIdx]->inOrderToIncreaseFlow = G.vertices[betaIdx]->labelledThroughEdge->flow < G.BitomrukScoreToWin - G.BitomrukPointsOnStart;
        else G.vertices[betaIdx]->inOrderToIncreaseFlow = G.vertices[betaIdx]->labelledThroughEdge->flow < matchesNumber - G.BitomrukPointsOnStart;

        DFSLabelling(G, labelledNodes, betaIdx, alphaIdx);

        if (G.vertices[alphaIdx]->labelled) {
// ..(E)..; Mamy przepływ
            adjustFlowThroughAugmentingPath(G, alphaIdx);
        } else {
// ..(D)..; Modyfikujemy potencjały niepoetykietowanych wierzchołków;
            if (increasePotentials(G, labelledNodes) == -1) return -1;
        }
// ..(F)..; każdy zaetykietowany wierzchołek traci etykietę;
        int idx;
        for (int i=0; i<labelledNodes.size(); i++) {
            idx = labelledNodes[i];
            G.vertices[idx]->labelled = false;
        }
        labelledNodes.clear();

        if (BitomrukArcFirst) {
            if (G.vertices[0]->out[0]->flow != G.BitomrukScoreToWin - G.BitomrukPointsOnStart) {
                alphaIdx = 0;
                betaIdx = G.V - 2;
            } else if (G.vertices[G.V - 2]->out[0]->flow != matchesNumber - G.BitomrukPointsOnStart) {
                alphaIdx = G.V - 2;
                betaIdx = G.V - 1;
            } else break;
        } else {
            if (G.vertices[G.V - 2]->out[0]->flow != matchesNumber) {
                alphaIdx = G.V - 2;
                betaIdx = G.V - 1;
            } else if (G.vertices[0]->out[0]->flow != G.BitomrukScoreToWin) {
                alphaIdx = 0;
                betaIdx = G.V - 2;
            } else break;
        }


    }
// Sumowanie kosztów;
    int costOfBriberies = 0;
    for (int i=0; i<G.edges.size(); i++) {
        e = G.edges[i];
        costOfBriberies += e->flow * (int)e->cost;
    }

    return costOfBriberies;
}

int centreScore;
bool cmp(int a, int b) {
    return abs(a-centreScore) <= abs(b-centreScore);
}
int* getArrayOfBitomrukScores(int n, Graph &G, int matchesNumber, int &size){
    int st = max(G.BitomrukPointsOnStart, matchesNumber/n + (n+1)%2);
    size = n-1 - st + 1;
    int *arr = new int[n-1 - st + 1];
    for (int i=0; i<size; i++) {
        arr[i] = n-1 - i;
    }
    centreScore = arr[(size-1)/ScoreDiv];
    sort(arr, arr+size);
    return arr;
}

bool canWin() {

// Czytanie danych;
    int B, n;
    cin >> B >> n;
    if (n<=1) return true;

    int matchesNumber = (n * (n - 1)) / 2;
    int dataSet[matchesNumber][4];
    for (int i = 0; i < matchesNumber; i++) {
        cin >> dataSet[i][0] >> dataSet[i][1] >> dataSet[i][2] >> dataSet[i][3];
        if (dataSet[i][3] < 0) exit(1);
    }
    if (n==2 && dataSet[0][3]<=B) return true;
// Konstrukcja sieci i wyznacznie cyrkulacji o minimalnym koszcie;
    Graph G(n, n-1, dataSet);
    int size=0;
    int *arr = getArrayOfBitomrukScores(n, G, matchesNumber, size);
    int cost;
    int BitomrukScoreToWin;
    G.resetGraph();

    for (int i=0; i<size; i++) {

        BitomrukScoreToWin = arr[i];
        G.modifyBitomrukExpectations(BitomrukScoreToWin);
        // cost = -1 jeżeli brak cyrkulacji;
        cost = minCostForParticularBriberingModusOperandi(n, matchesNumber, G);
        if (cost >-1 && cost <= B) {
            G.clearGraph();
            delete arr;
            return true;
        }
        G.resetLabelling();
        G.resetPotentials();
    }

    delete arr;
    G.clearGraph();
    return false;
}



int main() {

    int T;
    cin >> T;
    string ans[T];
    for (int i=0; i<T; i++) {
        if (canWin()) ans[i] = "TAK";
        else ans[i] = "NIE";
    }
    for (int i=0; i<T; i++) {
        cout << ans[i] << endl;
    }

    return 0;
}
