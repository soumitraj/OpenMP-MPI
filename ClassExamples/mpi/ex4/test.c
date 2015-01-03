/* 
 *  Compute the average of numbers using MPI_reduce
 *  The example is adapted from 
 *  http://mpitutorial.com/mpi-reduce-and-allreduce/
 */

#include <stdio.h>
#include <time.h>
#include <mpi.h>

/* This routine computes the average among n floating point numbers 
 * stored in a */
float compute_avg (float* a, int n)
{
    int i;
    float sum = 0;
    for (i=0; i<n; ++i) {
        sum += a[i];
    }
    return sum/n;
}

int main (int argc, char *argv[])
{
printf("Main");
    float *rand_nums, local_sum, global_sum;
    int i, j, nprocs, rank, k;
    int elements_per_proc = 10;
printf("Initializing .. ");
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (nprocs < 1)
    {
        if (rank == 0)
	    printf ("Number of processes for this example must be more than 1\n");
        MPI_Finalize ();
        return 1;
    }

    srand((unsigned)time(NULL) + rank*nprocs);
    rand_nums = (float*) malloc(sizeof(float) * elements_per_proc);
    for (i=0; i<elements_per_proc; ++i) {
        rand_nums[i] = rand() % 1000;
    }

    /* Sum the numbers locally */
    local_sum = 0;
    for (i = 0; i < elements_per_proc; i++) {
        local_sum += rand_nums[i];
    }

printf(" HEllo ! :P ");
    /* Print the local sums on each process */
    printf ("Local sum for process %d - %f, avg = %f\n",
         rank, local_sum, local_sum / elements_per_proc);

    /* Reduce all of the local sums into the global sum */
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0,
             MPI_COMM_WORLD);

    /* Print the result */
    if (rank == 0) {
        printf("Total sum = %f, average = %f\n", global_sum, global_sum / (nprocs * elements_per_proc));
    }
    MPI_Finalize ();
    return 0;
}
