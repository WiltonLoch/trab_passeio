/*
 *  Exemplo de programa para multiplicacao de matrizes em paralelo, usando POSIX threads.
 *  Obs.: Somente para matrizes quadradas.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "matrix.h"

/*
 * Estrutura que contem parametros para as threads
 */
typedef struct {
      int tid;
      int ntasks;
      matrix_t *A;
      matrix_t *B;
      matrix_t *C;
} param_t;


/*
 * Calcula linhas da matriz resultante C
 */
void matrix_mult(int tid, int ntasks, matrix_t* A, matrix_t* B, matrix_t* C) 
{
   int i, j, k;
   double sum;
   int n = A->rows;
   i = tid;
   // Calcula algumas linhas da matriz resultante
   while (i < n) {
      for (j = 0; j < n; j++) {
         sum = 0.0;
         for (k = 0; k < n; k++) {
            sum += A->data[i][k] * B->data[k][j];
         }
         C->data[i][j] = sum;
      }
      i += ntasks;
   }
}

/*
 * Funcao executada por uma thread
 */
void *matrix_mult_worker(void *arg)
{
   param_t *p = (param_t *) arg;
   matrix_mult(p->tid, p->ntasks, p->A, p->B, p->C);
}

/* 
 * Calcula C = A * B, distribuindo o trabalho entre ntasks threads
 */
void matrix_mult_threads(matrix_t *A, matrix_t *B, matrix_t* C, int ntasks)
{
   int i;
   pthread_t *threads;
   param_t *args;
   
   threads = (pthread_t *) malloc(ntasks * sizeof(pthread_t));
   args = (param_t *) malloc(ntasks * sizeof(param_t));

   for (i = 0; i < ntasks; i++)
   {
      args[i].tid = i;
      args[i].ntasks = ntasks;
      args[i].A = A;
      args[i].B = B;
      args[i].C = C;
      pthread_create(&threads[i], NULL, matrix_mult_worker, (void *) (args+i));
   }
   for (i = 0; i < ntasks; i++)
   {
      pthread_join(threads[i], NULL);
   }
   free(args);
   free(threads);
}

/*
 * Tempo (wallclock) em segundos
 */ 
double wtime()
{
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec + (double) t.tv_usec / 1000000;
}

int main(int argc, char **argv)
{
   int n;
   int rc;
   int taskid, ntasks;
   double start_time, end_time;

   if ((argc != 3)) {
      printf("Uso: %s <nthreads> <ordem da matriz quadrada>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   ntasks = atoi(argv[1]); // ntasks = nthreads
   n = atoi(argv[2]);      // ordem da matriz

   // Cria matrizes
   matrix_t *A = matrix_create(n, n);
   matrix_randfill(A);
   matrix_t *B = matrix_create(n, n);
   matrix_fill(B, 1.);
   matrix_t *C = matrix_create(n, n);

   // Calcula C = A * B em ntasks, medindo o tempo
   start_time = wtime();
   matrix_mult_threads(A, B, C, ntasks);
   end_time = wtime();

   // Mostra estatisticas da execucao
   printf("%d %f\n", ntasks, end_time - start_time);
   fflush(stdout);

   matrix_destroy(A);
   matrix_destroy(B);
   matrix_destroy(C);

   return EXIT_SUCCESS;
}
