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



inline vector<vector<pair<int, int>>> procuraCaminho(); //recebe como parametro o tamanho do tabuleiro, id da task pai, qtd_maquinas e núcleos por máquina. 

int main(int argc, char *argv[]){
	int qtd_tasks, id_task;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_task);	
	MPI_Comm_size(MPI_COMM_WORLD, &qtd_tasks);	
	
	pthread * threads;
	

	procuraCaminho();

	MPI_Finalize();

}

vector<vector<pair<int, int>>> procuraCaminho(){

	int movimentos[][2] = {{-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};	

	stack<pair<int, int>> arvore_expansao;	

	for();

	vector<vector<pair<int, int>>> a;
	return a;
}
