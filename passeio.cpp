#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <mpi.h>

#include <vector>
#include <utility>
#include <stack>

using namespace std;

#define QTD_MAQUINAS 2
#define NUCLEOS_POR_MAQUINA 4
#define TAMANHO_TABULEIRO 6

typedef struct{
	int idThread;
	vector<pair<int, pair<int, int>>> *caminho;
	stack<pair<int, pair<int, int>>> *arvoreExpansao;	

} param;

inline vector<vector<pair<int, int>>> procuraCaminho(void * arg); //recebe como parametro o tamanho do tabuleiro, id da task pai, qtd_maquinas e núcleos por máquina. 

inline void inicializaCaminho(int indiceAtual, vector<pair<int, pair<int, int>>> &caminho, stack<pair<int, pair<int, int>>> &arvoreExpansao);

inline void gerarMovimentosPossiveis(int indiceAtual, vector<pair<int, pair<int, int>>> &caminho, stack<pair<int, pair<int, int>>> &arvoreExpansao);

int main(int argc, char *argv[]){
	int qtd_tasks, id_task;
	int pos_inicial_x = 0;
	int pos_inicial_y = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_task);	
	MPI_Comm_size(MPI_COMM_WORLD, &qtd_tasks);	
	
	pthread_t * threads;
	param * parametros;

	threads = (pthread_t *) malloc(NUCLEOS_POR_MAQUINA * sizeof(pthread_t));
	parametros = (param *) malloc(NUCLEOS_POR_MAQUINA * sizeof(param));


	//#pragma omp parallel for num_threads(NUCLEOS_POR_MAQUINA)
	for(int i = 0; i < NUCLEOS_POR_MAQUINA; i++){
		vector<pair<int, pair<int, int>>> *caminho = new vector<pair<int, pair<int, int>>>();
		parametros[i].caminho = caminho;
		stack<pair<int, pair<int, int>>> *arvoreExpansao = new stack<pair<int, pair<int, int>>>();
		parametros[i].arvoreExpansao = arvoreExpansao;
	//	printf("%p -- %p\n", parametros[i].caminho, parametros[i].arvoreExpansao);

		parametros[i].caminho->push_back(make_pair(0, make_pair(pos_inicial_x, pos_inicial_y)));
		parametros[i].idThread = i;
		inicializaCaminho(id_task * NUCLEOS_POR_MAQUINA + i, *(parametros[i].caminho), *(parametros[i].arvoreExpansao));
	}
	
	for(int i = 0; i < NUCLEOS_POR_MAQUINA; i++){
		delete parametros[i].caminho;
		delete parametros[i].arvoreExpansao;
	}

	free(threads);
	free(parametros);
	MPI_Finalize();

}

inline vector<vector<pair<int, int>>> procuraCaminho(void * arg){
	param * p = (param *) arg;

	printf("aaa\n");

	vector<vector<pair<int, int>>> a;
	return a;
}

inline void gerarMovimentosPossiveis(int indiceAtual, vector<pair<int, pair<int, int>>> &caminho, stack<pair<int, pair<int, int>>> &arvoreExpansao){
	int movimentos[][2] = {{-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};	
	int pos_x = caminho.back().second.first;
	int pos_y = caminho.back().second.second;
	int nivel = caminho.back().first;
	for(int i = 0; i < 8; i++){
		int mov_x = pos_x + movimentos[i][0];
		int mov_y = pos_y + movimentos[i][1];
		if(mov_x >= 0 and mov_x < TAMANHO_TABULEIRO and mov_y >= 0 and mov_y < TAMANHO_TABULEIRO) arvoreExpansao.push(make_pair(nivel + 1, make_pair(mov_x, mov_y)));	
	}
}

inline void inicializaCaminho(int indiceAtual, vector<pair<int, pair<int, int>>> &caminho, stack<pair<int, pair<int, int>>> &arvoreExpansao){
	gerarMovimentosPossiveis(caminho, arvoreExpansao);
	printf("indice: %d\n | mov: %d - %d\n", indiceAtual, arvoreExpansao.top().second.first, arvoreExpansao.top().second.second);
	
	
}


