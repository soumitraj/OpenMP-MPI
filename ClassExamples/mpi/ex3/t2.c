#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<stdio.h>


int main(int argc, char* argv[]){
	
	int N=0;
	int rc, nprocs;
	int *localKey;
		
	//Reading values from the file
	int i,j, rank;
	int key_value_pairs=0;
	//Calculating value of N
	int min=0;
	int max=0;


	
int n;
   long elapsed_seconds;
   char line[80];
  

int maxkeynum =0;
int maxKey=0;

    int *dataKey;
    int *dataValue;
    MPI_Status status;

	//Initialization
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



//printf("\n opening the file");
	FILE *fr = fopen("input.csv","rt");
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


//printf("\ntotal key value pairs is : %d",key_value_pairs); 
    

//printf("\nMax is %d and min is %d",max,min);
    N=max-min+1;
    
//    printf("\n Number of processes %d",nprocs);
    int keysPerProcessor=key_value_pairs/nprocs;
    int count=0;
  //  printf("\n Keys per processor %d",keysPerProcessor);

    int *keyArray=(int *)calloc(key_value_pairs, sizeof(int));
    int *valueArray=(int *)calloc(key_value_pairs, sizeof(int));


int *keyCountArray=(int *)calloc(max+1, sizeof(int));
    
int **array = (int **)calloc(max+1, sizeof(int *));
int **rarray = (int **)calloc(max+1, sizeof(int *));

   int index=0;

//    printf("Space allocated to key and value \n");   

//printf("opening again for rank 0 ");
fr = fopen("input.csv","rt");
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
				*(keyArray+index)=key;
keyCountArray[key] = keyCountArray[key] + 1;
			input=strtok(NULL," ");
			int value=atoi(input);
				*(valueArray+index)=value;
//printf("(%d:%d) ,",*(keyArray+index),*(valueArray+index));
			index++;
		}

//printf("\n Index value : %d",index);
for(int i=0;i<max+1;i++)
{  //if(array[i]!=NULL){
	//printf(" value %d = %d",i,*array[i]);
//}
array[i] = calloc(keyCountArray[i]+1 ,sizeof(int));
rarray[i] = calloc(keyCountArray[i]+1 ,sizeof(int));
array[i][0] = i;
//if(keyCountArray[i] !=0)
//printf("\nKey count for key %d is %d",i,keyCountArray[i]);
}



 //  for(int i=0; i<nprocs; i++)
//	printf("\n %d : %d ",i,keyArray[i*keysPerProcessor]);

	
    fclose(fr);     
    

//if(rank ==0 )
//{




fr = fopen("input.csv","rt");
int *keyTmpCountArray=(int *)calloc(max+1, sizeof(int));
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
keyTmpCountArray[key] = keyTmpCountArray[key] +1;				

			input=strtok(NULL," ");
			int value=atoi(input);
array[key][keyTmpCountArray[key]] = value;				

			index++;
		}
/*
for(int i=0;i<max+1;i++){
for(int j =0;j<keyCountArray[i];j++){
{printf("a[%d][%d]=%d ",i,j,array[i][j]);
}
}
printf("\n");
}
*/
    fclose(fr);

int input;


for(int i=0; i<max+1; i++){

if(maxkeynum <=keyCountArray[i]){
	maxkeynum = keyCountArray[i];
maxKey = i;
}

}

//printf("\n Max key %d has  %d values",maxKey,maxkeynum);
    for(int i=0; i<max+1; i++){
if(i%nprocs == rank && keyCountArray[i]!=0){


//	printf("\n Starting send %d to processor %d",array[i][0], rank);
//    	MPI_Send(&(array[i][0]), keyCountArray[i]+1, MPI_INT, i%nprocs, 0, MPI_COMM_WORLD);


MPI_Send(&(array[i][0]), keyCountArray[i]+1, MPI_INT, rank, 0, MPI_COMM_WORLD);
//	printf("\nSend complete to processor %d",i%nprocs);

//	int *buff = (int *)calloc(maxkeynum+1, sizeof(int));
int *buff = (int *)calloc(maxkeynum+1, sizeof(int));
//   	MPI_Recv((buff), maxkeynum+1, MPI_INT,i%nprocs,0,MPI_COMM_WORLD, &status);
MPI_Recv((buff), maxkeynum+1, MPI_INT,rank,0,MPI_COMM_WORLD, &status);
	
int sum=0;
for(int k=1;k<=keyCountArray[buff[0]];k++){
sum = sum +buff[k];
}
printf("\nRecieve complete to processor %d for key :%d sum : %d",rank,buff[0],sum);

    }}
	//MPI_Send((keyArray+(i*keysPerProcessor)), keysPerProcessor, MPI_INT, 1, 0, MPI_COMM_WORLD);
//    printf("Tasks have been passed to processor");

//}
 /*   //Passing tasks to processors
if(rank!=0){

for(int i=0; i<max+1; i++){

if(maxkeynum <=keyCountArray[i]){

printf("\nkeyNote Arry : %d : %d",i,keyCountArray[i]);
	maxkeynum = keyCountArray[i];
maxKey = i;
}

}
int *buff = (int *)calloc(maxkeynum+1, sizeof(int));
int index;

printf(" Rank rec %d maxkeycount : %d ,max %d",rank,maxkeynum,max);
    	MPI_Recv((buff), maxkeynum+1, MPI_INT,0,0,MPI_COMM_WORLD, &status);
	
int sum=0;
for(int k=1;k<keyCountArray[buff[0]];k++){
sum = sum +buff[k];
}
printf("\nRecieve complete to processor %d for key :%d sum : %d",0,buff[0],sum);

    } */
	//MPI_Recv(dataValue, keysPerProcessor, MPI_INT,0,0,MPI_COMM_WORLD, &status);
    
 //   printf("\nData received by all prcoessors");

    	
//    printf("\nLocal key value sum complete "); 

	   MPI_Finalize ();
}
