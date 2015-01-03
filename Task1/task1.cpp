/*
 * Pagerank.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: soumitra
 */

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <cmath>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <omp.h>

using namespace std;

int main(int argc, char* argv[]){
	string inputFileName ;
char* myFileName;

std::string str1 ("-f");
cout <<"argc : "<< argc;

 for (int i = 0; i < argc; i++) {

cout <<"argv :  "<<i<<" : "<<argv[i]<<endl;
if (i + 1 != argc)
	if (str1.compare(argv[i]) == 0) {
//cout<<" file name found i : "<<i<<endl;
	inputFileName = argv[i+1];
        myFileName = argv[i+1];
	}
}

//inputFileName=argv[2];
cout << "inputFileName : "<<inputFileName <<endl;
cout<< "myFile : "<<myFileName<<endl;
	int min=0,max=0;
	cout << min;
	cout <<"starting";
	string line;
	//ifstream myfile("facebook_combined.txt");
	//ifstream myfile(inputFileName);
	ifstream myfile(myFileName);
	if(myfile.is_open())
	{
		while(getline(myfile,line))
		{
			//	cout<<line<<endl;
			std::stringstream os(line);          //a standard stringstream which parses 's'
			std::string temp;

			int tmp[2];
			int k=0;
			while (os >> temp)                //the stringstream makes temp a token
			{ if( k>1) break;
				//std::cout <<temp <<std::endl;   //and deletes that token from itself

				//the token can now be
				//outputted to console, or put into an array,
				//or whatever you choose to do ith it
				int value = atoi(temp.c_str());
				tmp[k++] =  value;
				if(value<min)
					min=value;
				if(value>max)
					max=value;
			}


		}

		myfile.close();
	}
	else
		cout << "unable to open the file"<<endl;

	cout <<"min : " << min << "Max : " << max <<"Init PG "<<1.0/(max+1)<<endl;
	int totalnodes = max-min+1;
	double *pagerankArray = new double[max+1];
	int *outlinkNumArray = new int[max+1];
	int **graph2dArray = new int*[max+1];



#pragma omp parallel for 
	for(int i=0;i<max+1;i++){
		graph2dArray[i] = new int[max+1];
		pagerankArray[i] = 1.0/(totalnodes);
	}


	//ifstream myfile2("facebook_combined.txt");
	//ifstream myfile2(inputFileName);
	ifstream myfile2(myFileName);
	if(myfile2.is_open())
	{
		while(getline(myfile2,line))
		{
			//	cout<<line<<endl;
			std::stringstream os(line);          //a standard stringstream which parses 's'
			std::string temp;

			int tmp[2];
			int k=0;
			while (os >> temp)                //the stringstream makes temp a token
			{ if( k>1) break;
				//	std::cout <<temp <<std::endl;   //and deletes that token from itself

				//the token can now be
				//outputted to console, or put into an array,
				//or whatever you choose to do ith it
				int value = atoi(temp.c_str());
				tmp[k++] =  value;
				outlinkNumArray[value] = outlinkNumArray[value] + 1;

			}

			graph2dArray[tmp[0]][tmp[1]] = 1;
			graph2dArray[tmp[1]][tmp[0]] = 1;
		}
		myfile2.close();
	}
	else
		cout << "unable to open the file"<<endl;

	/*for(int o=0;o<max+1;o++)
	  {
	  cout<<" Coutlink for node "<<o<<" is :"<<outlinkNumArray[o]<<endl;
	  }*/

	/*for(int i =0;i<max+1;i++)
	  {
	  for(int j =0;j<max+1;j++)
	  cout <<graph2dArray[i][j] << " ";
	  cout<<endl;
	  }*/

	int maxPgIndex=0,minPgIndex=0,convergenceFactor=0;
	double maxPG=1.0/(totalnodes),minPG=1.0/(totalnodes),prevPG=0;
	int cflag=0;

//#pragma omp parallel shared(pagerankArray,prevPG)
	for(int itr=0;1;itr++)
	{

		prevPG = pagerankArray[maxPgIndex];
		cout<<"interation "<<itr<<" starting .... max :" << maxPgIndex << " maxPG "<<maxPG<<" Outlinks : "<<outlinkNumArray[maxPgIndex]<<endl;

		#pragma omp for
		for(int i =0;i<max+1;i++)
		{
			double sum=0.0;

			for(int j =0;j<max+1;j++)
			{

				if(graph2dArray[i][j]==1){
					sum = sum + pagerankArray[j]/outlinkNumArray[j];
				}
			}
			pagerankArray[i] =  0.15/(totalnodes) + 0.85*sum;
			//cout <<"pj "<<i<<"="<<pagerankArray[i]<<endl;
			if(pagerankArray[i] < minPG)
			{
				minPG = pagerankArray[i];
				minPgIndex = i;
			}
			if(pagerankArray[i] > maxPG)
			{
				maxPG = pagerankArray[i];
				maxPgIndex = i;
			}

			//cout<<endl;
		}

		if(fabs(pagerankArray[maxPgIndex] - prevPG) < 0.0000000001 )//&& pagerankArray[maxPgIndex]!=(0.15/(max+1)))
		{
			cout<<"COnvergence achived iteration no. :" << itr;
			cout<<" Max Page RAnk : "<<maxPG<<" For Node " << maxPgIndex<<endl;
			cout<<" Min Page RAnk : "<<minPG<<" For Node " << minPgIndex<<endl;
			cflag=1;
                        break;

		}

	}
	ofstream outputFile;
	outputFile.open("Output_task1.txt");
double sumFinal=0.0;
	for(int i=0; i<max+1; i++){
		sumFinal+=pagerankArray[i];
		outputFile<<i<<"\t"<<pagerankArray[i]<<endl;
	}	
	outputFile.close();
	
	cout<<"Final Sum "<<sumFinal<<endl;

	cout << "Finished Reading the input graph from file"<<endl;


	return 0;
}





