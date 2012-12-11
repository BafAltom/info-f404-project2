#include <mpi.h>
#include <iostream>
#include "gameMaster.hpp"
#include "IA.hpp"

using namespace std;

/*void Cleudo::intialiseNumberOfPlayer()
{

	_numberPlayer=0;
	while(_numberPlayer<2 or _numberPlayer>5)
	{
		cout<<"How much IA do you want ? (between 2 and 5)"<<endl;
		cin>>_numberPlayer;
	}
	_numberPlayer++; // on ajoute le joueur humain
		
		/*
		//
		//	Création des groupes, plein de bug, j'ai laissé tomber, y reviendrai si j'ai le temps, car ca ferait classe
		//
		//
		//MPI_Group orig_group, groupPlayer;
		//MPI_Comm commGame;
		int players[numberIAWanted];
		for(int i=0; i<=numberIAWanted; ++i)
		{
			players[i]=i;
		}
		
		// Extract the original group handle 
		MPI_Comm_group(MPI_COMM_WORLD, &orig_group);


		// Divide tasks into two distinct groups 
		MPI_Group_incl(orig_group, numberIAWanted, players, &groupPlayer);


		// Create new communicator and then perform collective communications 
		MPI_Comm_create(MPI_COMM_WORLD, groupPlayer, &commGame);
		//MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);

		int numtasks;
		MPI_Comm_size(commGame,&numtasks);
		cout<<"créer groupe de : "<<numtasks<<endl;
		
		

	MPI_Bcast(&_numberPlayer, 1, MPI_INT, 0, MPI_COMM_WORLD );
}*/


int main(int argc, char *argv[]) {
	
	MPI_Init(&argc, &argv);
	
	cout<<"kikou je suis cleudo le clodo"<<endl;
	
	int myRank;	
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	int numberPlayer=0;
    if(myRank ==0)
    {
		while(numberPlayer<2 or numberPlayer>5)
		{
			cout<<"How much IA do you want ? (between 2 and 5)"<<endl;
			cin>>numberPlayer;
		}
		numberPlayer++;
	}
    MPI_Bcast(&numberPlayer, 1, MPI_INT, 0, MPI_COMM_WORLD );
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    int typeOfIA=-1;
    if(myRank ==0)
    {
		cout<<"Choose the type of IA that you want :"<<endl<<" 0 = Normal IA"<<endl;
		cin>>typeOfIA;
	}
    MPI_Bcast(&typeOfIA, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(myRank == 0)
    {
		GameMaster gameMaster;
		gameMaster.run(numberPlayer);
	}
	else if(myRank < numberPlayer)
	{
		IA ia;
		ia.run(numberPlayer,typeOfIA);
	}
	
	MPI_Finalize();
    
    return 0;
}




