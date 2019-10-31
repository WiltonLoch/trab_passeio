#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdlib.h>

#define random() ((rand() ^ rand()) / (RAND_MAX + 1.0))

// Estrutura que armazena uma matriz
typedef struct {
      double **data;
      int rows;
      int cols;
} matrix_t;

// Estrutura que descreve um bloco da matriz
typedef struct {
      int row_start;
      int row_end;
      int col_start;
      int col_end;
} block_t;

matrix_t *matrix_create(int rows, int cols);


void matrix_destroy(matrix_t *m);

void matrix_randfill(matrix_t *m);


void matrix_fill(matrix_t *m, double val);


matrix_t *matrix_multiply(matrix_t *A, matrix_t *B);


void matrix_block_multiply(matrix_t *A, matrix_t *B, matrix_t *C,  
                           block_t* block);

matrix_t *matrix_submatrix(matrix_t *A, block_t *block);


void matrix_block_copy(matrix_t *source, matrix_t *dest, int source_row, 
                       int source_col, int dest_row, int dest_col, int rows,
                       int cols);

void matrix_print(matrix_t *m);


int matrix_equal(matrix_t*A, matrix_t *B);



block_t *partition_create(int n);

void partition_destroy(block_t *part);

void partition_print(block_t *part);

block_t *matrix_row_partition(matrix_t *m, int n);

#endif 
