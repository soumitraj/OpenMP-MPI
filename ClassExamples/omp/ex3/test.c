/* 
 *  Compute the average of numbers with MPI_Scatter and MPI_Gather
 *  The example is adapted from 
 *  http://mpitutorial.com/mpi-scatter-gather-and-allgather/
 *  First the process with rank 0 (root process) creates a list of numbers, which it
 *  then distributes among the other processes, then each process computes an 
 *  average of the numbers it got and sends to the root process. The root process 
 *  then computes an average of the average.
 */

#include <stdio.h>
#include <omp.h>
#include <mpi.h>

/* This routine computes the average among n floating point numbers 
 * stored in a */
float compute_avg (float* a, int n)
{
    int i;
    float sum = 0;

#pragma omp parallel for reduction(+:sum)
  for (i=0; i < n; i++)
    sum = sum + a[i];

    return sum/n;
}

int main (int argc, char *argv[])
{
    float *rand_nums, *sub_rand_nums;
    float *sub_avgs;
    int i, j, nprocs, rank, to, from, k, num;
    int elements_per_proc = 10;
    float avg;

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

    if (rank == 0) {
        num = elements_per_proc * nprocs;
        rand_nums = (float*) malloc(sizeof(float) * num);
        for (i=0; i<num; ++i) {
            rand_nums[i] = rand() % 1000;
        }
    }
    /*  Create a buffer that will hold a subset of the random numbers */
    sub_rand_nums = (float*) malloc(sizeof(float) * elements_per_proc);

    /* Scatter the random numbers to all processes */
    MPI_Scatter(rand_nums, elements_per_proc, MPI_FLOAT, sub_rand_nums,
                elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);

    /* Compute the average of your subset */
    float sub_avg = compute_avg(sub_rand_nums, elements_per_proc);

    printf ("%d: %f\n", rank, sub_avg);

    /* Gather all partial averages down to the root process */
    if (rank == 0) {
        sub_avgs = (float*) malloc(sizeof(float) * nprocs);
        printf("Starting to gather\n");
    }
    MPI_Gather(&sub_avg, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, 0,
                   MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Finished gather\n");
        avg = compute_avg(sub_avgs, nprocs);
        printf ("Average: %f\n", avg);
        free (rand_nums);
        free (sub_avgs);
    }

    MPI_Finalize ();
    return 0;
}
