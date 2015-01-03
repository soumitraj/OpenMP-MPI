#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<fstream>
#include<map>
#include<mpi.h>
#include<climits>
using namespace std;

int main(int argc, char* argv[]){
	//string inputFile="test.txt";
	string inputFile="input.csv";
	
	int N=0;
	int rc, nprocs;
	int *localKey;
		
	//Reading values from the file
	string line;
	int i,j, rank;
	int key_value_pairs=0;
	//Calculating value of N
	int min=INT_MAX;
	int max=INT_MIN;

	
    
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

	ifstream myfile1(inputFile.c_str());
	if (myfile1.is_open())
	{
		while (getline (myfile1,line) )
   		{
			char* input;
			input=strtok((char*)line.c_str(),",");
			int v1=atoi(input);
				if(v1<min)
					min=v1;
				if(v1>max)
					max=v1;
				key_value_pairs++;
			input=strtok(NULL," ");
			int v2=atoi(input);
		}
	}
    myfile1.close();
    cout<<"Max is "<<max<<" and min is "<<min<<endl;
    N=max-min+1;
    
    cout<<" Number of processes "<<nprocs<<endl;
    int keysPerProcessor=key_value_pairs/nprocs;
    int count=0;
    cout<<"Keys per processor "<<keysPerProcessor<<endl;

    int *dataKey= (int *) calloc(keysPerProcessor, sizeof(int));
    int *dataValue=(int *) calloc(keysPerProcessor, sizeof(int));
    MPI_Status status;

       
    
    int *keyArray=(int *)calloc(key_value_pairs, sizeof(int));
    int *valueArray=(int *)calloc(key_value_pairs, sizeof(int));
    int index=0;

    cout<<"Space allocated to key and value "<<endl;
    //Passing tasks to processors

if(rank==0)
{
    ifstream myfile(inputFile.c_str());
	if (myfile.is_open())
	{
		while (getline (myfile,line) )
   		{
			char* input;
			input=strtok((char*)line.c_str(),",");
			int key=atoi(input);
				*(keyArray+index)=key;
			input=strtok(NULL," ");
			int value=atoi(input);
				*(valueArray+index)=value;
			index++;
		}		
	}
    myfile1.close();
	cout<<"Index value "<<index<<endl;    

   for(int i=0; i<nprocs; i++)
	cout<<i<<"   "<<keyArray[i*keysPerProcessor]<<endl;
    cout<<endl;
  int input;

    for(int i=1; i<nprocs; i++){
	cout<<"Starting send for processor "<<i<<endl;
    	MPI_Send((keyArray+(i*keysPerProcessor)), keysPerProcessor, MPI_INT, i, 0, MPI_COMM_WORLD);
	//cin>>input;
	MPI_Recv(dataKey, keysPerProcessor, MPI_INT,0,0,MPI_COMM_WORLD, &status);
    	MPI_Send((valueArray+(i*keysPerProcessor)), keysPerProcessor, MPI_INT, i, 0, MPI_COMM_WORLD);
	MPI_Recv(dataValue, keysPerProcessor, MPI_INT,0,0,MPI_COMM_WORLD, &status);
	cout<<"Send complete to processor "<<i<<endl;
    }
	//MPI_Send((keyArray+(i*keysPerProcessor)), keysPerProcessor, MPI_INT, 1, 0, MPI_COMM_WORLD);
    cout<<"Tasks have been passed to processor"<<endl;
}

   if(rank!=0){
    	//MPI_Recv(dataKey, keysPerProcessor, MPI_INT,0,0,MPI_COMM_WORLD, &status);
	//MPI_Recv(dataValue, keysPerProcessor, MPI_INT,0,0,MPI_COMM_WORLD, &status);
    }
    cout<<"Data received by all prcoessors"<<endl;
    std::map<int,int> map_temp;
	std::map<int,int> ::iterator it ;
	std::map<int,int> map_result;
    
    for(int i=0;i<keysPerProcessor;i++)
	{
		it=map_temp.find(dataKey[i]);
		if(it==map_temp.end())
			map_temp.insert(std ::pair<int,int>(dataKey[i],dataValue[i]));
		else
			it->second=(it->second)+dataKey[i];	
	}
    	
    cout<<"Local key value sum complete "<<endl;   
   if(rank!=0)
	   MPI_Finalize ();
}
