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
#include <map>
/* #include <tr1/unordered_map> */

using namespace std;

#define NUCLEOS_POR_MAQUINA 4
#define TAMANHO_TABULEIRO 6

typedef struct{
	int indiceAtual;
	int qtd_tasks;
	int nucleos_por_maquina;
	int tamanho_tabuleiro;
	int pos_inicial_x;
	int pos_inicial_y;
} param;

inline void * procuraCaminhos(void * args);
inline void gerarMovimentosPossiveis(int tamanho_tabuleiro, vector<pair<int, pair<int, int>>> &caminho, map<pair<int, int>, bool> &posicoes_visitadas, stack<pair<int, pair<int, int>>> &arvoreExpansao);
inline void imprimeCaminho(vector<pair<int, pair<int, int>>> &caminho);
inline void empilhaPosOrdem(int elemento_inferior, int elemento_superior, int nivel_base, stack<pair<int, pair<int, int>>> &arvoreExpansao, vector<vector<pair<int, pair<int, int>>>> &arvoreCompleta);
inline void imprimeArvore(stack<pair<int, pair<int, int>>> arvoreExpansao);
inline void imprimeCaminhoHorizontal(int tamanho_tabuleiro, vector<pair<int, pair<int, int>>> &caminho);
inline void imprimeCaminhoCoordenado(vector<pair<int, pair<int, int>>> &caminho);

int main(int argc, char *argv[]){
	int qtd_tasks, id_task;
	int pos_inicial_x = 2;
	int pos_inicial_y = 2;

	if(argc < 3){
		printf("Chamada incorreta!\n Uso: passeio <tamanho tabuleiro> <threads>\n");
		return 0;
	}

	int tamanho_tabuleiro = stoi(argv[1]);
	int nucleos_por_maquina = stoi(argv[2]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_task);	
	MPI_Comm_size(MPI_COMM_WORLD, &qtd_tasks);	
	
	pthread_t * threads;
	param * parametros;

	threads = (pthread_t *) malloc(nucleos_por_maquina * sizeof(pthread_t));
	parametros = (param *) malloc(nucleos_por_maquina * sizeof(param));

	printf("qtd: %d\n", qtd_tasks);

	for(int i = 0; i < nucleos_por_maquina; i++){
		parametros[i].indiceAtual = id_task * nucleos_por_maquina + i;
		parametros[i].qtd_tasks = qtd_tasks;
		parametros[i].nucleos_por_maquina = nucleos_por_maquina;
		parametros[i].tamanho_tabuleiro = tamanho_tabuleiro;
		parametros[i].pos_inicial_x = pos_inicial_x;
		parametros[i].pos_inicial_y = pos_inicial_y;

		pthread_create(threads + i, NULL, procuraCaminhos, (void *) (parametros + i));
	}
	
	for(int i = 0; i < nucleos_por_maquina; i++){
		pthread_join(threads[i], NULL);
	}
	

	free(threads);
	free(parametros);
	MPI_Finalize();

}


inline void * procuraCaminhos(void * args){
	int movimentos[][2] = {{-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};	

	param * p = (param *) args;

	int indiceAtual = p->indiceAtual;
	int qtd_maquinas = p->qtd_tasks;
	int tamanho_tabuleiro = p->tamanho_tabuleiro;
	int nucleos_por_maquina = p->nucleos_por_maquina;
	int pos_inicial_x = p->pos_inicial_x;
	int pos_inicial_y = p->pos_inicial_y;

	vector<vector<pair<int, pair<int, int>>>> arvoreCompleta;
	vector<pair<int, pair<int, int>>> maiorNivel;
	vector<pair<int, pair<int, int>>> caminho;

	stack<pair<int, pair<int, int>>> arvoreExpansao;
	map<pair<int, int>, bool> posicoes_visitadas;

	maiorNivel.push_back(make_pair(-1, make_pair(pos_inicial_x, pos_inicial_y)));
	while(maiorNivel.size() < nucleos_por_maquina * qtd_maquinas * 3){
		arvoreCompleta.push_back(maiorNivel);
		maiorNivel.clear();
		for(int i = 0; i < arvoreCompleta.back().size(); i++){
			int pos_x = arvoreCompleta.back()[i].second.first;
			int pos_y = arvoreCompleta.back()[i].second.second;
			for(int j = 0; j < 8; j++){
				int mov_x = pos_x + movimentos[j][0];
				int mov_y = pos_y + movimentos[j][1];
				int ancestral_avaliado = arvoreCompleta.back()[i].first;
				int nivel_avaliado = arvoreCompleta.size() - 2;
				if(mov_x >= 0 and mov_x < tamanho_tabuleiro and mov_y >= 0 and mov_y < tamanho_tabuleiro){ 
					while(ancestral_avaliado != -1){
						if(!(mov_x == arvoreCompleta[nivel_avaliado][ancestral_avaliado].second.first and mov_y == arvoreCompleta[nivel_avaliado][ancestral_avaliado].second.second)){
							ancestral_avaliado = arvoreCompleta[nivel_avaliado][ancestral_avaliado].first;
							nivel_avaliado--;
						}else{
							break;
						}	
					}
					if(ancestral_avaliado == -1) maiorNivel.push_back(make_pair(i, make_pair(mov_x, mov_y)));	
				}
			}
		}
	}

	arvoreCompleta.push_back(maiorNivel);

	/* printf("tamanho nivel 3: %d\n", arvoreCompleta[3].size()); */
	/* printf("indice: %d\n", indiceAtual); */
	/* for(int i = 0; i < arvoreCompleta.size(); i++) */
	/* 	for(int j = 0; j < arvoreCompleta[i].size(); j++) */
	/* 		printf("\tnivel = %d, pai = %d, pos: %d - %d\n", i, arvoreCompleta[i][j].first, arvoreCompleta[i][j].second.first, arvoreCompleta[i][j].second.second); */

	int nivel_base = arvoreCompleta.size() - 1; 

	int nucleos_totais = qtd_maquinas * nucleos_por_maquina;

	int sobra = arvoreCompleta[nivel_base].size() % nucleos_totais; 
	int corte = arvoreCompleta[nivel_base].size()/nucleos_totais;
	/* printf("sobra: %d\n", sobra); */
	/* printf("corte: %d\n", corte); */
	int elemento_inferior = indiceAtual * corte + (indiceAtual < sobra ? indiceAtual : sobra);
	int elemento_superior = (indiceAtual + 1) * corte + (indiceAtual < sobra ? indiceAtual + 1 : sobra) - 1;

	/* printf("li: %d, ls: %d\n", elemento_inferior, elemento_superior); */

	int primeiro_pai = arvoreCompleta[nivel_base][elemento_inferior].first, ultimo_pai = arvoreCompleta[nivel_base][elemento_superior].first;


	/* encontra qual é o pai comum aos caminhos da thread, de modo a saber qual o menor nível(nivel_base) que não deve ser retirado do caminho */
	while(primeiro_pai != ultimo_pai and nivel_base > 0){
		/* printf("pp: %d, up: %d, nivel_base: %d\n", primeiro_pai, ultimo_pai, nivel_base); */
		primeiro_pai = arvoreCompleta[--nivel_base][primeiro_pai].first;
	       	ultimo_pai = arvoreCompleta[nivel_base][ultimo_pai].first;
	}
	nivel_base--;

	vector<pair<int, pair<int, int>>>::iterator i = caminho.begin();
	int nivel_atual = nivel_base;
	int indice = primeiro_pai;

	/* adiciona o primeiro pedaço de caminho explorado */ 
	while(nivel_atual >= 0){
		caminho.insert(i, make_pair(nivel_atual, arvoreCompleta[nivel_atual][indice].second));
		posicoes_visitadas[arvoreCompleta[nivel_atual][indice].second] = true;
		indice = arvoreCompleta[nivel_atual][indice].first;
		i = caminho.begin();
		nivel_atual--;
	}	

	empilhaPosOrdem(elemento_inferior, elemento_superior, nivel_base, arvoreExpansao, arvoreCompleta);

	/* imprimeCaminho(caminho); */

	/* printf("indiceAtual: %d\n", indiceAtual); */
	/* printf("tamanho: %d\n", arvoreExpansao.size()); */

	/* imprimeArvore(arvoreExpansao); */

	/* printf("\n\n"); */

	vector<vector<pair<int, pair<int, int>>>> solucoes;

	while(!arvoreExpansao.empty()){
		while(arvoreExpansao.top().first > caminho.back().first){
			caminho.push_back(arvoreExpansao.top());
			posicoes_visitadas[caminho.back().second] = true;
			arvoreExpansao.pop();
			if(arvoreExpansao.top().first <= caminho.back().first){
			       	gerarMovimentosPossiveis(tamanho_tabuleiro, caminho, posicoes_visitadas, arvoreExpansao);
				break;
			}
		}
		if(caminho.size() == tamanho_tabuleiro * tamanho_tabuleiro){
			printf("aaaa\n");
			solucoes.push_back(caminho);
			posicoes_visitadas[caminho.back().second] = false;
			caminho.pop_back();
		}
		if(arvoreExpansao.top().first <= caminho.back().first){
			posicoes_visitadas[caminho.back().second] = false;
			caminho.pop_back();
			/* printf("cs: %d\n", caminho.size()); */
		}

		if(caminho.size() > 34){
			printf("t: %d, s: %d c: %d | ", indiceAtual, solucoes.size(), caminho.size());
			imprimeCaminhoHorizontal(tamanho_tabuleiro, caminho);
		}

	}
	printf("solucoes: %d\n", solucoes.size());



}

inline void gerarMovimentosPossiveis(int tamanho_tabuleiro, vector<pair<int, pair<int, int>>> &caminho, map<pair<int, int>, bool> &posicoes_visitadas, stack<pair<int, pair<int, int>>> &arvoreExpansao){
	int movimentos[][2] = {{-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};	
	int pos_x = caminho.back().second.first;
	int pos_y = caminho.back().second.second;
	int nivel = caminho.back().first;
	for(int i = 0; i < 8; i++){
		int mov_x = pos_x + movimentos[i][0];
		int mov_y = pos_y + movimentos[i][1];
		if(mov_x >= 0 and mov_x < tamanho_tabuleiro and mov_y >= 0 and mov_y < tamanho_tabuleiro) if(!posicoes_visitadas[make_pair(mov_x, mov_y)] or posicoes_visitadas[make_pair(mov_x, mov_y)] == false) arvoreExpansao.push(make_pair(nivel + 1, make_pair(mov_x, mov_y)));	
	}
}

inline void imprimeArvore(stack<pair<int, pair<int, int>>> arvoreExpansao){
	while(!arvoreExpansao.empty()){
		pair<int, pair<int, int>> elemento = arvoreExpansao.top();
		printf("%d -> %d, %d\n", elemento.first, elemento.second.first, elemento.second.second);
		arvoreExpansao.pop();
	}
}

inline void imprimeCaminhoHorizontal(int tamanho_tabuleiro, vector<pair<int, pair<int, int>>> &caminho){
	for(auto& i : caminho)
		printf("%d -- ", i.second.first * tamanho_tabuleiro + i.second.second);
	printf("\n");
}


inline void imprimeCaminhoCoordenado(vector<pair<int, pair<int, int>>> &caminho){
	for(auto& i : caminho)
		printf("%d, %d -- ", i.second.first, i.second.first);
	printf("\n");
}

inline void imprimeCaminho(vector<pair<int, pair<int, int>>> &caminho){
	for(auto& i : caminho)
		printf("%d -> %d, %d\n", i.first, i.second.first, i.second.second);
}

inline void empilhaPosOrdem(int elemento_inferior, int elemento_superior, int nivel_base, stack<pair<int, pair<int, int>>> &arvoreExpansao, vector<vector<pair<int, pair<int, int>>>> &arvoreCompleta){

	for(int i = elemento_inferior; i <= elemento_superior; i++){
		int indice = i;
		int superior_atual = elemento_superior;
		for(int nivel_avaliado = arvoreCompleta.size() - 1; nivel_avaliado > nivel_base; nivel_avaliado--){
			arvoreExpansao.push(make_pair(nivel_avaliado, arvoreCompleta[nivel_avaliado][indice].second));
			if(arvoreCompleta[nivel_avaliado][indice].first == arvoreCompleta[nivel_avaliado][(indice + 1) % (superior_atual + 1)].first) break;
			indice = arvoreCompleta[nivel_avaliado][indice].first;
			superior_atual = arvoreCompleta[nivel_avaliado][superior_atual].first;
 
		}
			
	}

	// Coloquei um comentario pra ele ver q tinha mudanças e aceitar o commit, poxa github pq me matastes?!
	// Ia fazer surpresa qnd vc abrisse o código e visse tudo la ja, o readme bonitin q fiz e tals
	// te amo

	/* for(int i = 1; i <= elemento_superior; i++) */
	/* 	primeiro_pai = arvoreCompleta[nivel_atual][i].first; */
	/* 	if(primeiro_pai == ultimo_pai){ */
	/* 		arvoreExpansao.push(make_pair(nivel_atual, arvoreCompleta[nivelAtual][i].second)); */
	/* 	}else{ */
	/* 		while(nivel_atual >= nivel_base){ */
	/* 			/1* arvoreCompleta[nivel_atual][i].second; *1/ */
	/* 		} */
	/* 	} */
		
}
