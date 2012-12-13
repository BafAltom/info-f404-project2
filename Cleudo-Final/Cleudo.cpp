#include <mpi.h>
#include <iostream>
#include "gameMaster.hpp"
#include "IA.hpp"

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
    
    
    int typeOfIA=-1;
    if(myRank ==0)
    {
		cout<<"Choose the type of IA that you want :"<<endl<<" 0 = Normal IA		1 = Listening IA		2 = Mix IA"<<endl;
		cin>>typeOfIA;
	}
    MPI_Bcast(&typeOfIA, 1, MPI_INT, 0, MPI_COMM_WORLD );
    
    if(myRank == 0)
    {
		GameMaster gameMaster;
		gameMaster.run(numberPlayer);
	}
	else if(myRank < numberPlayer)
	{
		IA ia;
		if(typeOfIA == 2){
			//for the mix IA :  odds IA's use the listening IA, the others use the default IA
			ia.run(numberPlayer,myRank%2);
		}
		else{
			ia.run(numberPlayer,typeOfIA);
		}
	}
	
	MPI_Finalize();
    
    return 0;
}




