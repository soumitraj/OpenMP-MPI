/* 
 *  Transpose a square matrix using MPI_Alltoall.
 *  In this example, the number of processes is 4 and
 *  the dimension of the matrix is 128x128.
 *  The example is adapted from 
 *  https://www.surfsara.nl/systems/shared/mpi/mpi-intro
 */

#include <stdio.h>
#include <mpi.h>

/*
 *  The following function is used to transpose a matrix 'a' of
 *  dimensions 'n'x'n'.
 */ 
void trans (double *a, int n)
{
    int i, j;
    int ij, ji, l;
    double tmp;
    ij = 0;
    l = -1;
    for (i = 0; i < n; i++)
    {
        l += n + 1;
        ji = l;
        ij += i + 1;
        for (j = i+1; j < n; j++)
	{
	    tmp = a[ij];
	    a[ij] = a[ji];
	    a[ji] = tmp;
	    ij++;
	    ji += n;
	}
    }
}

/*
 * The following is the main function.
 */
int main (int argc, char *argv[])
{
    double a[128][32];
    double b[128][32];

    int i, j, nprocs, rank;

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (nprocs != 4)
    {
        if (rank == 0)
	    printf ("Number of processes for this example must be 4\n");
        MPI_Finalize ();
        return 1;
    }

    /* Generate the part of the matrix held by the current process 
     * so that each matrix element is unique   */
    for (i = 0; i < 128; i++)
        for (j = 0; j < 32; j++)
            a[i][j] = 1000 * i + j + 32 * rank;

   /* Communication among processes: the number of matrix elements to
    * send to every other process and to receive from every other process 
    * is (128/nprocs) x 32 = 32x32, and not 128x32. */
    MPI_Alltoall (&a[0][0],	/* beginning address for data to be send  */
		32 * 32,	/* number of matrix elements to send to one process  */
		MPI_DOUBLE,	/* data type for sent data */
		&b[0][0],	/* address for receiving the data */
		32 * 32,	/* number of matrx elements to receive from a process */
		MPI_DOUBLE,	/* data type for received data */
		MPI_COMM_WORLD);

  /* Now locally transpose the 4 square matrices (32x32) that are stored in the process: */

    for (i = 0; i < 4; i++)
        trans (&b[i * 32][0], 32);

  /* Check the result */
    for (i = 0; i < 128; i++)
        for (j = 0; j < 32; j++)
        {
	    if (b[i][j] != 1000 * (j + 32 * rank) + i )
	    {
	        printf ("process %d found b[%d][%d] = %f, but %f was expected\n",
		    rank, i, j, b[i][j], (double) (1000 * (j + 32 * rank) + i));
	        MPI_Abort (MPI_COMM_WORLD,1);
	        return 1;
	    }
        }
    if (rank == 0) printf ("Transpose is complete.\n");

    MPI_Finalize ();
    return 0;
}
