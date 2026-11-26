#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <chrono>
using namespace std;

template <class M,class T>
M distancia_Euclidiana(const T &n, const T &m){
    M rpta=0;
    for(int i=0; i<n.size();i++){
        rpta+=pow(n[i]-m[i],2);
    }
    return sqrt(rpta);
}

struct Punto{
    vector<double> coords;
    int clusterId;
    Punto(vector<double> _coords) : coords(_coords), clusterId(-1){}
};

// KD tree
struct NodoKD{
    int idx;
    NodoKD *izq, *der;
    NodoKD(int _idx) : idx(_idx), izq(NULL), der(NULL){}
};

NodoKD* construirKD(vector<int> &ids, int ini, int fin, int prof, const vector<vector<double>> &cent){
    if(ini >= fin) return NULL;
    int eje = prof % cent[0].size();
    sort(ids.begin()+ini, ids.begin()+fin, [&](int a, int b){
        return cent[a][eje] < cent[b][eje];
    });
    int mid = (ini + fin) / 2;
    NodoKD* nodo = new NodoKD(ids[mid]);
    nodo->izq = construirKD(ids, ini, mid, prof+1, cent);
    nodo->der = construirKD(ids, mid+1, fin, prof+1, cent);
    return nodo;
}

void buscarKD(NodoKD* nodo, const vector<double> &q, const vector<vector<double>> &cent,
              int prof, int &mejor, double &mejorDist){
    if(nodo == NULL) return;
    double dist = distancia_Euclidiana<double, vector<double>>(q, cent[nodo->idx]);
    if(dist < mejorDist){
        mejorDist = dist;
        mejor = nodo->idx;
    }
    int eje = prof % q.size();
    double diff = q[eje] - cent[nodo->idx][eje];
    NodoKD *cerca = (diff < 0) ? nodo->izq : nodo->der;
    NodoKD *lejos = (diff < 0) ? nodo->der : nodo->izq;
    buscarKD(cerca, q, cent, prof+1, mejor, mejorDist);
    if(fabs(diff) < mejorDist)
        buscarKD(lejos, q, cent, prof+1, mejor, mejorDist);
}

void liberarKD(NodoKD* nodo){
    if(nodo == NULL) return;
    liberarKD(nodo->izq);
    liberarKD(nodo->der);
    delete nodo;
}

// KMEAN
// los centroides finales quedan en 'centroides'
int Kmeans(vector<Punto> &points, int k, bool usarKD, vector<vector<double>> &centroides){
    int n = points.size();
    if(n < k) return 0;
    int dims = points[0].coords.size();

    centroides.clear();
    for (int i = 0; i < k; ++i) {
        int indiceAleatorio = rand() % n;
        centroides.push_back(points[indiceAleatorio].coords);
    }
    bool cambios = true;
    int iteracion = 0;

    while (cambios) {
        cambios = false;
        iteracion++;

        NodoKD* raiz = NULL;
        if (usarKD) {
            vector<int> ids(k);
            for (int j = 0; j < k; ++j) ids[j] = j;
            raiz = construirKD(ids, 0, k, 0, centroides);
        }

        for (size_t i = 0; i < points.size(); ++i) {
            double minDistancia = -1;
            int mejorCluster = -1;

            if (usarKD) {
                minDistancia = 1e18;
                buscarKD(raiz, points[i].coords, centroides, 0, mejorCluster, minDistancia);
            } else {
                for (int j = 0; j < k; ++j) {
                    double dist = distancia_Euclidiana<double, vector<double>>(points[i].coords, centroides[j]);
                    if (minDistancia == -1 || dist < minDistancia) {
                        minDistancia = dist;
                        mejorCluster = j;
                    }
                }
            }
            if (points[i].clusterId != mejorCluster) {
                points[i].clusterId = mejorCluster;
                cambios = true;
            }
        }
        liberarKD(raiz);

        vector<vector<double>> nuevasSumas(k, vector<double>(dims, 0.0));
        vector<int> conteoPorCluster(k, 0);
        for (const auto& p : points) {
            conteoPorCluster[p.clusterId]++;
            for (int dim = 0; dim < dims; ++dim)
                nuevasSumas[p.clusterId][dim] += p.coords[dim];
        }
        for (int j = 0; j < k; ++j) {
            if (conteoPorCluster[j] > 0) {
                for (int dim = 0; dim < dims; ++dim)
                    centroides[j][dim] = nuevasSumas[j][dim] / conteoPorCluster[j];
            }
        }
    }
    return iteracion;
}

bool Importar_csv(string file_name, vector<Punto> &ListaPuntos){
    cout<< "Leyendo '"<<file_name<<"'..."<<endl;
    ifstream archivo(file_name);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo encontrar '"<<file_name<<"'." << endl;
        return false;
    }
    string linea;
    bool esCabecera = true;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        if (esCabecera) { esCabecera = false; continue; }
        stringstream ss(linea);
        string celda;
        vector<double> medidas;
        vector<string> tokens;
        while (getline(ss, celda, ',')) tokens.push_back(celda);
        if (tokens.empty()) continue;
        // el ultimo elemento es la clase, se ignora
        for (size_t i = 0; i < tokens.size(); ++i) {
            medidas.push_back(stod(tokens[i]));
        }
        ListaPuntos.push_back(Punto(medidas));
    }
    archivo.close();
    cout << "Se cargaron " << ListaPuntos.size() << " registros exitosamente.\n\n";
    return true;
}


// tiempo (ms) de una ejecucion
double tiempoKmeans(vector<Punto> &puntos, int k, bool usarKD, int semilla, int &iter, vector<vector<double>> &cent){
    srand(semilla);
    auto inicio = chrono::steady_clock::now();
    iter = Kmeans(puntos, k, usarKD, cent);
    auto fin = chrono::steady_clock::now();
    return chrono::duration<double, milli>(fin - inicio).count();
}

// promedio de varias repeticiones
double tiempoPromedio(const vector<Punto> &puntos, int k, bool usarKD, int reps = 5){
    double total = 0;
    int iter;
    vector<vector<double>> cent;
    for (int r = 1; r <= reps; ++r) {
        vector<Punto> copia = puntos;   // la copia no entra en el tiempo medido
        total += tiempoKmeans(copia, k, usarKD, r, iter, cent);
    }
    return total / reps;
}

// suma de distancias cuadraticas de cada punto a su centroide
double inercia(const vector<Punto> &puntos, const vector<vector<double>> &cent){
    double s = 0;
    for (const auto &p : puntos) {
        double d = distancia_Euclidiana<double, vector<double>>(p.coords, cent[p.clusterId]);
        s += d * d;
    }
    return s;
}

void guardarClusters(string archivo, const vector<Punto> &puntos){
    ofstream f(archivo);
    f << "x,y,cluster\n";
    for (const auto &p : puntos)
        f << p.coords[0] << "," << p.coords[1] << "," << p.clusterId << "\n";
}

void guardarCentroides(string archivo, const vector<vector<double>> &cent){
    ofstream f(archivo);
    f << "x,y\n";
    for (const auto &c : cent)
        f << c[0] << "," << c[1] << "\n";
}

// (b)10 ejecuciones con k = 18
void experimentoB(const vector<Punto> &todos){
    int k = 18;
    ofstream out("resultados_b.csv");
    out << "ejecucion,brute_ms,kd_ms,iter_brute,iter_kd,inercia_brute,inercia_kd,dif_max_centroides\n";

    cout << "=== (b) k = 18, 10 ejecuciones ===\n";
    for (int r = 1; r <= 10; ++r) {
        vector<Punto> pB = todos, pK = todos;
        vector<vector<double>> cB, cK;
        int itB, itK;
        double tB = tiempoKmeans(pB, k, false, r, itB, cB);
        double tK = tiempoKmeans(pK, k, true,  r, itK, cK);

        // cuanto difieren los centroides de ambas versiones
        double dif = 0;
        for (int j = 0; j < k; ++j)
            for (size_t d = 0; d < cB[j].size(); ++d)
                dif = max(dif, fabs(cB[j][d] - cK[j][d]));

        out << r << "," << tB << "," << tK << "," << itB << "," << itK << ","
            << inercia(pB, cB) << "," << inercia(pK, cK) << "," << dif << "\n";
        cout << "Ejecucion " << r << ": brute=" << tB << " ms (" << itB << " it), kd="
             << tK << " ms (" << itK << " it), dif centroides=" << dif << "\n";

        // guardar la primera ejecucion para graficarla
        if (r == 1) {
            guardarClusters("clusters_brute.csv", pB);
            guardarClusters("clusters_kd.csv", pK);
            guardarCentroides("centroides_brute.csv", cB);
            guardarCentroides("centroides_kd.csv", cK);
        }
    }
}

// (c) k fijo, n variable
void experimentoC(const vector<Punto> &todos){
    int ks[] = {5, 15, 25, 50, 75};
    int ns[] = {1000, 1150, 1300, 1450, 1600, 1750, 1900, 2050, 2200, 2400};

    ofstream out("tiempos_vs_n.csv");
    out << "k,n,brute_ms,kd_ms\n";
    cout << "\n=== (c) k fijo, n variable ===\n";
    for (int k : ks) {
        for (int n : ns) {
            if (n > (int)todos.size()) continue;
            vector<Punto> sub(todos.begin(), todos.begin() + n);
            double tB = tiempoPromedio(sub, k, false);
            double tK = tiempoPromedio(sub, k, true);
            out << k << "," << n << "," << tB << "," << tK << "\n";
            cout << "k=" << k << " n=" << n << " brute=" << tB << " kd=" << tK << "\n";
        }
    }
}

// (d) n fijo, k variable
void experimentoD(const vector<Punto> &todos){
    int ks[] = {5, 15, 25, 50, 75, 100, 125, 150, 200};
    int ns[] = {1000, 1450, 1900, 2400};

    ofstream out("tiempos_vs_k.csv");
    out << "n,k,brute_ms,kd_ms\n";
    cout << "\n=== (d) n fijo, k variable ===\n";
    for (int n : ns) {
        if (n > (int)todos.size()) continue;
        vector<Punto> sub(todos.begin(), todos.begin() + n);
        for (int k : ks) {
            double tB = tiempoPromedio(sub, k, false);
            double tK = tiempoPromedio(sub, k, true);
            out << n << "," << k << "," << tB << "," << tK << "\n";
            cout << "n=" << n << " k=" << k << " brute=" << tB << " kd=" << tK << "\n";
        }
    }
}

int main()
{
    vector<Punto> ListaPuntos;
    if (Importar_csv("data2k.csv", ListaPuntos)) {
        experimentoB(ListaPuntos);
        experimentoC(ListaPuntos);
        experimentoD(ListaPuntos);
    }
    return 0;
}
