#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <queue>
#include <ctime>
#include <cmath>
#include <stdlib.h>
#include <thread>
#include <chrono>
using namespace std;

typedef vector<vector<int>> matriu;

struct Coordenades {
	int x;
	int y;
};

//calcula la distancia en el pla entre dos punts P1 = (x1, y1) , P2 = (x2, y2) 
float calcul_dist(const double& x1, const double& y1, const double& x2, const double& y2){
    double x = x2 - x1;
    double y = y2 - y1;
    x = x*x;
    y = y*y;
    double d = sqrt((double)(x+y));
    return d;
}

//---------------------------------------------------------------------------------
// GENERADOR DE GRAFS
//---------------------------------------------------------------------------------

//Genera un graf aleatori de n nodes, radi r i dimensio dr x dr
void generar_graf(const int& n, const float& r, const double& d, matriu& matG){
	vector<Coordenades> nodes(n);
    srand(time(NULL));
	//genera els nodes del graf i els hi dona coordenades
	for(int i = 0; i < n; ++i) {
		nodes[i].x = d*r*(((double)(rand()%100))/100.0);
		nodes[i].y = d*r*(((double)(rand()%100))/100.0);
	}
	
	//itera per la meitat inferior de la matriu (per tant un cop per parell de nodes) 
    for(int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (calcul_dist(nodes[i].x, nodes[i].y, nodes[j].x, nodes[j].y) <= r){
                matG[i][j] = 1;
                matG[j][i] = 1;
            }
        }
    }
}

//genera un graf graella nxn
//n es la mida del costat de la graella, no el num de caselles (nxn)
void generar_graella(const int& n, matriu& matG) {
	int caselles = n*n;
	
	for(int i = 0; i < caselles; ++i) {
        if (i+1 < caselles && (i+1) % n != 0) matG[i][i+1] = 1; //casella i connectada amb la de la dreta 
        if (i-1 >= 0 && i % n != 0) matG[i][i-1] = 1; 			//casella i connectada amb la de l'esquerra
        if (i+n < caselles) matG[i][i+n] = 1; 					//casella i connectada amb la de sota
        if (i-n >= 0) matG[i][i-n] = 1; 						//casella i connectada amb la de sobre
	}															//en tots els casos nomes si n'hi ha
}

//---------------------------------------------------------------------------------
// 	ALGORITMES DE PERCOLACIO
//---------------------------------------------------------------------------------

//percola cada node del graf G amb probabilitat q
matriu percolacio_nodes(matriu G, float& q) {
    default_random_engine gen(time(NULL)); 	//generador de nombres pseudo-random
    bernoulli_distribution d(q); 			//crea una distribucio de bernoulli 'd' amb probabilitat de true = q

	int per_eliminar = 0; 	//per guardar els nodes a borrar
    int Gsize = G[0].size();
    vector<bool> del(Gsize, false);
	for(int i = 0; i < Gsize; ++i) {
        if(d(gen)) { 		//amb probabilitat q s'ha d'eliminar el node
            del[i] = true;
            per_eliminar++;
        }
    }
    int sol_size = Gsize - per_eliminar;
    matriu sol(sol_size, vector<int>(sol_size));	//nova matriu solucio per treure els nodes eliminats
    if (sol_size == 0) return {{-1}};
	int i_sol = 0;
	for(int i = 0; i < Gsize; ++i) { 	// iterem per la matriu originial i
		int j_sol = 0;              	// nomes passem els nodes no eliminats a la nova matriu
		if(!del[i]) {
			for(int j = 0; j < Gsize; ++j) {
				if(!del[j]) {
					sol[i_sol][j_sol] = G[i][j];
					j_sol++;				
				}
			}
		i_sol++;
		}
	}
    return sol;
}

//percola cada aresta del graf G amb probabilitat q
matriu percolacio_arestes(matriu G, float& q) {
    default_random_engine gen(time(NULL)); 	//generador de nombres pseudo-random
    bernoulli_distribution d(q);   			//crea una distribucio de bernoulli 'd' amb probabilitat de true = q

    int size = G[0].size();
    for (int i = 1; i < size; ++i){
        for (int j = 0; j < i; ++j) {
            if(G[i][j] == 1) {
				if(d(gen)) {   		//amb probabilitat q, elimina l'aresta
                    G[i][j] = 0;
                    G[j][i] = 0;
                }
            }
        }
    }
    return G;
}


//---------------------------------------------------------------------------------
// ALGORITME CONTADOR DE COMPONENTS CONNEXOS
//---------------------------------------------------------------------------------

//conta el nombre de components connexos que té el graf amb matriu d'adjacencies mat
int count_connexos(matriu& mat) {
	if(mat[0][0] == -1) return 0;
	int comps = 0;
	int size = mat[0].size();
	queue<int> nodes;
	vector<bool> visited(size, false);
	nodes.push(0);
	bool remaining = true;
	while(remaining) { 					//mentres quedin nodes sense visitar, iterem.
		while (!nodes.empty()) {  		//mentres quedin nodes per visitar al component connex actual, iterem.
			int i = nodes.front(); 		//anem al primer node de la cua i el marquem com a visitat
			nodes.pop();
			visited[i] = true;
			for (int j = 0; j < size; ++j) {
				if(mat[i][j] == 1 && !visited[j]) nodes.push(j); 	//afegim tots els nodes no
			}                                                   	//visitats que podem visitar desde l'actual a la cua
		}
		//quan la qua queda buida, ja hem visitat tots els nodes del component
		comps++;
		remaining = false;
		//mirem si queda algun node no visitat:
		//si hi ha algun node fora dels components anteriors
		for(int i = 0; i < size && !remaining; ++i) {
			//si n'hi ha, l'afegim a la cua per visitar el seu component
			if(!visited[i]) {
				remaining = true;
				nodes.push(i);
			}
		}
	}
	
	return comps;
}

//imprimeix la matriu mat per pantalla
void print_mat (const matriu& mat) {
    int size = mat[0].size();
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) cout << mat[i][j] << ' ';
        cout << endl;
    }
}


int main() {
    //es modifiquen les variables necessaries i es compila + executa, potser una mica massa rudimentari pero...
	float r;
    double d;
    float q;
    int n;
    //perquè no salti warning es poden comentar les variables no necessaries (r i d al generar graelles)
	/*
    cout << "Generacio de un graf geometric aleatori connex (q,n,r,d)" << endl;
    cout << "q: probabilitat de percolacio" << endl;
    cout << "n: nodes del graf" << endl;
    cout << "r: distancia per la que dos vertex es consideren conectats" << endl;
    cout << "d: factor de multiplicació de l'espai euclidi" << endl; 
	*/
	n = 75;
	r = 4;
	d = 2.5;
	
	for(int i = 0; i < 50; ++i) {
		cout << i+1;
		for (q = 0.00; q < 1.04; q+= 0.05) {
			//Descomentem aixo per generacio de graelles:
			//int size = n*n;
			//matriu matG(size, vector<int>(size, 0));
            //generar_graella(n, matG);

			//Descomentem aixo per generacio de grafs geometrics connexos
			matriu matG(n, vector<int>(n, 0));
			generar_graf(n, r, d, matG);
            while(count_connexos(matG) != 1) generar_graf(n, r, d, matG);
            
			//Descomentem aixo per fer percolacions d'arestes al graf generat 
            matriu matG_perar = percolacio_arestes(matG, q);
            int comp = count_connexos(matG_perar);
            
			//Descomentem aixo per fer percolacions de nodes al graf generat 
            //matriu matG_perno = percolacio_nodes(matG, q);
            //int comp = count_connexos(matG_perno);

            cout << "," << comp;
		}
		//perque el temps que s'utilitza per generar la seed sigui diferent per cada iteració 
		this_thread::sleep_for(chrono::seconds(1));
		cout << endl;
	} 
	while(1);
}




