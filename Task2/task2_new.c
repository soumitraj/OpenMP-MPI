#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<stdio.h>


int main(int argc, char* argv[]){
//	string inputFileName ;
char* myFileName;

//string str1 ="-f";
printf("argc : %d", argc);

 for (int i = 0; i < argc; i++) {

printf("argv :  %d:%s",i,argv[i]);
if (i + 1 != argc)
	if (strcmp(argv[i],"-f")==0) {
//cout<<" file name found i : "<<i<<endl;
	//inputFileName = argv[i+1];
        myFileName = argv[i+1];
	}
}

printf("InputFileName : %s",myFileName);




	int rc, nprocs;

	int i,j, rank;
	int key_value_pairs=0;

	int min=0;
	int max=0;

FILE * Output;
    Output = fopen("data.txt", "a");

	char line[80];


	int maxkeynum =0;
	int maxKey=0;

	MPI_Status status;

	rc=MPI_Init (&argc, &argv);
	if (rc != MPI_SUCCESS) {
		printf ("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}	
	MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if (nprocs < 1)
	{
		if (rank == 0)
			printf ("Number of processes for this example must be more than 1\n");
		MPI_Finalize ();
		return 1;
	}



//	FILE *fr = fopen("input.csv","rt");
	FILE *fr = fopen(myFileName,"rt");
	int flag =0;
	while(fgets(line, 80, fr) != NULL)
	{
		if(flag==0)
		{
			flag=1;
			continue;
		}
		char* input;
		input=strtok(line,",");
		int v1=atoi(input);
		if(v1<min)
			min=v1;
		if(v1>max)
			max=v1;
		key_value_pairs++;
		input=strtok(NULL," ");
		int v2=atoi(input);
	}

	fclose(fr); 


int keysPerProcessor=key_value_pairs/nprocs;

if(rank == 0)
printf("\ntotal key value pairs : %d, Max value : %d",key_value_pairs,max);

int *localKeySumArray = (int *)calloc(max+1,sizeof(int));
int *globalKeySumArray = (int *)calloc(max+1,sizeof(int));
int *localKeyValuePairSubPartition = (int *)calloc(keysPerProcessor,sizeof(int));
int *localKVPairArrayCountIndex = (int *)calloc(nprocs,sizeof(int));
int *localValueArray=(int *)calloc(key_value_pairs, sizeof(int));

int **localArray = (int **)calloc(keysPerProcessor, sizeof(int *));

int *rowMajorKeyArray = (int *)calloc(2*(max+1),sizeof(int));


int kv_pair_index=0;
//fr = fopen("input.csv","rt");
	fr = fopen(myFileName,"rt");
	flag =0;
	while(fgets(line, 80, fr) != NULL)
	{
		if(flag==0)
		{
			flag=1;
			continue;
		}
		char* input;
		input=strtok((char*)line,",");
		int key=atoi(input);
		//keyCountArray[key] = keyCountArray[key] + 1;
		input=strtok(NULL," ");
		int value=atoi(input);
		
		int procNum = kv_pair_index%nprocs;
//if(rank==3)
//	printf("procNum : %d",procNum);

		if(procNum==rank)
			{
				 localKeyValuePairSubPartition[localKVPairArrayCountIndex[rank]] = key;
				 localValueArray[localKVPairArrayCountIndex[rank]] = value;
				 localArray[localKVPairArrayCountIndex[rank]] = calloc(2 ,sizeof(int));
				 localArray[localKVPairArrayCountIndex[rank]][0] = key; 
				 localArray[localKVPairArrayCountIndex[rank]][1] = value;
				 localKVPairArrayCountIndex[rank] = localKVPairArrayCountIndex[rank] +1;
			}
kv_pair_index++;
	}

fclose(fr);

	printf("\n Stage1. Processor number %d KV Count : %d\n",rank,localKVPairArrayCountIndex[rank]);

for(int i=0;i<localKVPairArrayCountIndex[rank];i++){
localKeySumArray[localArray[i][0]] = localKeySumArray[localArray[i][0]] + localArray[i][1];
//*(rowMajorKeyArray+2*i) = 
}

int k=0;
for(int i=0;i<max+1;i++){

/* Reduce all of the local sums into the global sum */
    MPI_Reduce((localKeySumArray+i), (globalKeySumArray+i), 1, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD);

if( localKeySumArray[i]!=0)
 k++;
}

if(rank==0){
printf("\nGlobal sum :\n");
FILE * Output;
    Output = fopen("Output_task2.txt", "a");
for(int i=0;i<max+1;i++){

/* Reduce all of the local sums into the global sum */
//    MPI_Reduce((localKeySumArray+i), (globalKeySumArray+i), 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);

fprintf(Output,"%d,%d\n",i,globalKeySumArray[i]);
//if( localKeySumArray[i]!=0)
 //k++;
}

}

printf("\nTotal unique keys in processor %d is %d\n",rank,k);
	//for(int i=0;i<=nprocs;i++)
	//{  //if(array[i]!=NULL){
		//printf(" value %d = %d",i,*array[i]);
		//}
		//printf("\nProcessor number %d indexCount : %d for index :%d\n",rank,localKVPairArrayCountIndex[i],i);
		//if(keyCountArray[i] !=0)
		//printf("\nKey count for key %d is %d",i,keyCountArray[i]);
	//}

	
MPI_Finalize ();
}
