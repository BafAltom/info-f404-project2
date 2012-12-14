#include <mpi.h>
#include <iostream>
#include "GameMaster.hpp"
#include "AI.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	
	MPI_Init(&argc, &argv);
	
	
	int myRank;	
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	int numberPlayer=0;
    if(myRank ==0){	
		cout<<"How much IA do you want ? (between 2 and 5)"<<endl;
		cin>>numberPlayer;
		numberPlayer++;
	}
    MPI_Bcast(&numberPlayer, 1, MPI_INT, 0, MPI_COMM_WORLD );
    
    
    int typeOfAI=-1;
    if(myRank ==0)
    {
		cout<<"Choose the type of AI that you want :"<<endl<<" 0 = Default AI		1 = Listening AI		2 = Mix AI"<<endl;
		cin>>typeOfAI;
	}
    MPI_Bcast(&typeOfAI, 1, MPI_INT, 0, MPI_COMM_WORLD );
    
    if(myRank == 0)
    {
		GameMaster gameMaster;
		gameMaster.run(numberPlayer);
	}
	else if(myRank < numberPlayer)
	{
		AI ai;
		if(typeOfAI == 2){
			//for the mix IA :  odds AI's use the listening AI, the others use the default AI
			ai.run(numberPlayer,myRank%2);
		}
		else{
			ai.run(numberPlayer,typeOfAI);
		}
	}
	
	MPI_Finalize();
    
    return 0;
}




