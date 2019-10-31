#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "matrix.h"

// Cria matriz de dimensao rows X cols.
matrix_t *matrix_create(int rows, int cols)
{
   int i;
   double *data;
   matrix_t *m = (matrix_t *) malloc(sizeof(matrix_t));

   m->rows = rows;
   m->cols = cols;
   m->data = (double **) malloc(sizeof(double *) * rows);
   data = (double *) malloc(sizeof(double) * rows * cols);
   for (i = 0; i < rows; i++) {
      m->data[i] = &data[cols * i];
   }
   return m;
}

// Destroi matriz.
void matrix_destroy(matrix_t *m)
{
   free((void *) m->data[0]);
   free((void *) m->data);
   free((void *) m);
}

// Preenche matriz com uma sequencia de numeros aleatorios.
void matrix_randfill(matrix_t *m)
{
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = random();
      }
   }
}

// Preenche matriz com valor val.
void matrix_fill(matrix_t *m, double val)
{
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = val;
      }
   }
}

// Retorna produto das matrizes A e B.
matrix_t *matrix_multiply(matrix_t *A, matrix_t *B)
{
   matrix_t *m;
   int i, j, k, count;
   double sum;

   assert(A->cols == B->rows);
   m = matrix_create(A->rows, B->cols);
   count = A->cols;

   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         sum = 0.0;
         for (k = 0; k < count; k++) {
            sum += A->data[i][k] * B->data[k][j];
         }
         m->data[i][j] = sum;
      }
   }
   return m;
}

// Calcula um bloco do produto das matrizes A e B.
// A matriz C deve ter sido criada previamente.
void matrix_block_multiply(matrix_t *A, matrix_t *B, matrix_t *C, block_t* block)
{
   int i, j, k;
   double sum;

   assert(A->cols == B->rows);

   for (i = block->row_start; i < block->row_end; i++) {
      for (j = block->col_start; j < block->col_end; j++) {
         sum = 0.0;
         for (k = 0; k < C->cols; k++) {
            sum += A->data[i][k] * B->data[k][j];
         }
         C->data[i][j] = sum;
      }
   }
}

// Retorna uma submatriz de A.
matrix_t *matrix_submatrix(matrix_t *A, block_t* block)
{
   matrix_t *m = matrix_create(block->row_end - block->row_start, 
                               block->col_end - block->col_start);
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = A->data[i + block->row_start][j + block->col_start];
      }
   }
   return m;
}

// Copia um bloco de uma matriz origem para uma matriz destino.
void matrix_block_copy(matrix_t *source, matrix_t *dest, int source_row, 
                       int source_col, int dest_row, int dest_col, int rows,
                       int cols)
{
   int i, j;
   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
         dest->data[i + dest_row][j + dest_col] =
            source->data[i + source_row][i + source_col];
      }
   }
}

// Imprime matriz.
void matrix_print(matrix_t *m)
{
   
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         printf("%.17f ", m->data[i][j]);
      }
      printf("\n");
   }
   fflush(stdout);
}


// Verifica se as matrizes A e B sao iguais.
int matrix_equal(matrix_t*A, matrix_t *B) 
{
   int i, j;

   if (A->rows != B->rows || A->cols != B->cols)
      return 0; 

   for (i = 0; i < A->rows; i++) {
      for (j = 0; j < A->cols; j++) {
         if (A->data[i][j] != B->data[i][j])
            return 0; 
      }
   }
   return 1;
}


// Aloca espaco para uma particao da matriz (n blocos)
block_t *partition_create(int n)
{
   block_t *part = (block_t *) malloc(sizeof(block_t) * n);
   return part;
}

// Destroi particao.
void partition_destroy(block_t *part)
{
   free((void *) part);
}

// Imprime particao.
void partition_print(block_t *part)
{
   printf("[%d:%d][%d:%d]\n", 
          part->row_start, part->row_end, 
          part->col_start, part->col_end);
}


// Particiona a matriz em n blocos, cada bloco contendo
// uma ou mais linhas inteiras.
block_t *matrix_row_partition(matrix_t *m, int n)
{
   int i;
   int nrows;
   int size = m->rows / n;
   int extra = m->rows % n;
   int base = 0;
   block_t *part = partition_create(n);

   for (i = 0; i < n; i++) {
      nrows = size + (i < extra ? 1 : 0);
      part[i].row_start = base;
      part[i].row_end = base + nrows;
      part[i].col_start = 0;
      part[i].col_end = m->cols;
      base += nrows;
   }
   return part;

}
