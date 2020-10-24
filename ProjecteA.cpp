#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <queue>
using namespace std;

typedef vector<vector<int>> matriu;

matriu percolacio_nodes(matriu G, float q) {
    string w_seed;
    cout << "doneu una seed pel calcul de probabilitats(percolacio nodes): " << endl; 
    cin >> w_seed;
    
    //genera una seed a partir de l'input 'w_seed'
    seed_seq seed (w_seed.begin(), w_seed.end()); 
    //generador de nombres 'random' a partir de la seed generada   
    default_random_engine gen(seed); 
    //crea una distribuci√≥ de bernoulli 'd' amb probabilitat de true = q
    bernoulli_distribution d(q);

    int size = G[0].size();
    for(int i = 0; i < size; ++i) {
        if(d(gen)) {
            //amb probabilitat q elimina totes les arestes del node
            for(int j = 0; j < size; ++j) {
                G[i][j] = 0;
                G[j][i] = 0;
            }
            cout << "s'ha eliminat el node " << i << endl;
        }
    }

    return G;
}

matriu percolacio_arestes(matriu G, float q) {
    
    string w_seed;
    cout << "doneu una seed pel calcul de probabilitats: " << endl; 
    cin >> w_seed;

    //genera una seed a partir de l'input 'w_seed'
    seed_seq seed (w_seed.begin(), w_seed.end()); 
    //generador de nombres 'random' a partir de la seed generada   
    default_random_engine gen(seed); 
    //crea una distribuci√≥ de bernoulli 'd' amb probabilitat de true = q
    bernoulli_distribution d(q);

    int size = G[0].size();
    for (int i = 1; i < size; ++i){
        for (int j = 0; j < i; ++j) {
            if(G[i][j] == 1) {
                //amb probabilitat q, elimina l'aresta
                if(d(gen)) {
                    G[i][j] = 0;
                    G[j][i] = 0;
                    cout << "s'ha eliminat l'aresta " << i << ' ' << j << endl;
                }
            }
        }
    }

    return G;
}

int count_connexos (matriu mat) {
	int comps = 0;
	int size = mat[0].size();
	queue<int> nodes;
	vector<bool> visited(size, false);
	nodes.push(0);
	bool remaining = true;
	//mentres quedin nodes sense visitar, iterem
	while(remaining) {
		//mentres quedin nodes per visitar al component connex actual, iterem
		//basicament un bfs iteratiu
		while (!nodes.empty()) {
			//anem al primer node de la cua i el marquem com a visitat
			int i = nodes.front();
			nodes.pop();
			visited[i] = true;
			for (int j = 0; j < size; ++j) {
				//afegim tots els nodes no visitats que podem visitar desde l'actual a la cua
				if(mat[i][j] == 1 && !visited[j]) nodes.push(j);
			}
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

void print_mat (matriu &mat) {
    int size = mat[0].size();
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) cout << mat[i][j] << ' ';
        cout << endl;
    }
}

int main() {

    cout << "Quants nodes te el teu graf G?" << endl;
    int nodes;
    cin >> nodes;
    cout << "Enumerarem els nodes de 0 a " << nodes-1 << endl;
    cout << "A continuacio, indica les arestes originals del graf G amb parelles de nodes, separats per un espai, acabant amb una parella -1 -1" << endl;
    
    matriu matG(nodes,vector<int>(nodes));
    
    for(int i = 0; i < nodes; ++i){
        for(int j = 0; j < nodes; ++j) matG[i][j] = 0;
    }
    
    int i = 0, j = 0;
    cin >> i >> j;
    while (i != -1 && j != -1){
        if (i >= 0 && i < nodes && j >= 0 && j < nodes) {
            matG[i][j] = 1;
            matG[j][i] = 1;
        }
        else cout << "Ups, t'has equivocat en l'ultim node (" << i << ", " << j << "), torna'l a escriure" << endl;
        cin >> i >> j;
    }

    cout << "la teva matriu d'adjascencies ha quedat aixi:" << endl;
    print_mat(matG);
	
	cout << "el teu graf te " << count_connexos(matG) << " components connexos" << endl;
	
    cout << "Ara nomes falta la probabilitat de percolacio (de treure cada node/aresta, entre 0 i 1): ";
    float q;
    cin >> q;
	
    
    matriu matG_perar = percolacio_arestes(matG, q);
    matriu matG_perno = percolacio_nodes(matG, q);

    cout << "Despres de la percolacio d'arestes ha quedat aixi: " << endl;
    print_mat(matG_perar);
	
	cout << "el teu graf amb percolaciÛ d'arestes te " << count_connexos(matG_perar) << " components connexos" << endl;
	
    cout << endl << "Despres de la percolacio de nodes ha quedat aixi: " << endl;
    print_mat(matG_perno); 
    
    cout << "el teu graf amb percolaciÛ de nodes te " << count_connexos(matG_perno) << " components connexos" << endl;
    
}
