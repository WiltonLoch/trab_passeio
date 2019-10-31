
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include "matrix.h"

#define MASTER 0 
#define MSG_TAG 0

void mpi_bcast_s_matrix(matrix_t *m)
{
	MPI_Bcast(&m->rows, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&m->data[0][0], m->rows * m->cols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
}

matrix_t *mpi_bcast_r_matrix(){
	matrix_t *m;
	int rows;

	MPI_Bcast(&rows, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	m = matrix_create(rows, rows);
	MPI_Bcast(&m->data[0][0], rows * rows, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	return m;
}

matrix_t * master_job(matrix_t *A, matrix_t *B, int ntasks){
	mpi_bcast_s_matrix(B);	
	int extra = A->rows % ntasks;
	int partition_size = A->rows / ntasks;
	matrix_t * parted_A = matrix_create(partition_size, A->cols);
	matrix_t * C = matrix_create(A->rows, A->rows);

	MPI_Scatter(A->data[0], partition_size * A->cols, MPI_DOUBLE, parted_A->data[0], partition_size * A->cols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	
	int i, j, k;
	double sum;

	for (i = 0; i < parted_A->rows; i++) {
		for (j = 0; j < parted_A->cols; j++) {
			sum = 0.0;
			for (k = 0; k < parted_A->cols; k++) {
				sum += parted_A->data[i][k] * B->data[k][j];
			}
			C->data[i][j] = sum;
		}
	}

	for (i = ntasks * partition_size; i < A->rows; i++) {
		for (j = 0; j < A->cols; j++) {
			sum = 0.0;
			for (k = 0; k < A->cols; k++) {
				sum += A->data[i][k] * B->data[k][j];
			}
			C->data[i][j] = sum;
		}
	}


	MPI_Gather(C->data[0], partition_size * A->cols, MPI_DOUBLE, C->data[0], partition_size * A->cols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

	return C;

} 

void slave_job(int n, int taskid, int ntasks){
	matrix_t * B = mpi_bcast_r_matrix();
	int extra = n % ntasks;
	int partition_size = n / ntasks;
	matrix_t * A = matrix_create(n, n);
	matrix_t * parted_A = matrix_create(partition_size, n);
	MPI_Scatter(A->data, partition_size * n, MPI_DOUBLE, parted_A->data[0], partition_size * n, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	int i, j, k;
	double sum;
	matrix_t * C = matrix_create(partition_size, n);

	for (i = 0; i < parted_A->rows; i++) {
		for (j = 0; j < parted_A->cols; j++) {
			sum = 0.0;
			for (k = 0; k < parted_A->cols; k++) {
				sum += parted_A->data[i][k] * B->data[k][j];
			}
			C->data[i][j] = sum;
		}
	}

	MPI_Gather(C->data[0], partition_size * n, MPI_DOUBLE, NULL, partition_size * n, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);


}

int main(int argc, char *argv[]){
	int n;
	int rc;
	int taskid, ntasks;
	double start_time, end_time;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	if ((argc != 2)) {
		printf("Uso: mpirun -np <nprocs> %s <ordem da matriz quadrada>\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(1);
	}
	n = atoi(argv[1]);

	if(taskid == MASTER){
		matrix_t *A = matrix_create(n, n);
		matrix_randfill(A);
		matrix_t *B = matrix_create(n, n);
		matrix_fill(B, 1.);

		start_time = MPI_Wtime();
		matrix_t * C = master_job(A, B, ntasks);
		end_time = MPI_Wtime();

		/* for(int i = 0; i < C->rows; i++){ */
		/* 	for(int j = 0; j < C->cols; j++){ */
		/* 		printf("%lf ", C->data[i][j]); */	
		/* 	} */
		/* 	printf("\n"); */	
		/* } */

		printf("%d %f\n", ntasks, end_time - start_time);
		fflush(stdout);

		matrix_destroy(A);
		matrix_destroy(B);
		matrix_destroy(C);
	}else{
		slave_job(n, taskid, ntasks);
	}

	MPI_Finalize();
}

