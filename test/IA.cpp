#include "IA.hpp"

IA::IA()
{
	for(int i=0; i<21; ++i)
	{
		_IaKnowledge.push_back(true);
	}	
    MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);
	
}

void IA::run(int numberPlayer, int typeOfIA)
{	
	srand(time(NULL));
	_numberPlayer = numberPlayer; // recoit le nombre de joueurs
	_typeOfIA = typeOfIA; // recoit le type d'IA
	
	receiveCards();
	
	startGame();
}
void IA::startGame()
{
	
	_isGameNotFinished = true;
	_isAITurn = false; //les IAs ne commencent jamais.
	
	while(_isGameNotFinished)
	{
		if(_isAITurn)
		{
			cout<<"l'IA "<<_myRank<<" réfléchit à une suggestion ou accusation"<<endl;
			
		}
		else // l'IA attend un message
		{
			cout<<"IA "<<_myRank<< "attend un message"<<endl;
			int typeMessage[2];
			MPI_Recv(&typeMessage, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			cout<<"IA "<<_myRank<<" a recu : "<<typeMessage[0]<<" , "<<typeMessage[1]<<endl;
			if(typeMessage[0] == -1)
			{
				cout<<"IA "<<_myRank<<" a recu une suggestion et doit y répondre"<<endl;
				int reply = disproveASuggestion();
				MPI_Send(&reply, 1, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD);
			}
			else if(typeMessage[0] == -2)
			{
				cout<<"IA"<<_myRank<<" receive a suggestion from "<<typeMessage[1]<<endl;
				MPI_Recv(&_currentSuggestion, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, 
MPI_STATUS_IGNORE);
				cout<<typeMessage[1]<<" a fait une accusation/suggestion pour "<<_currentSuggestion[0]<<" , "<<_currentSuggestion[1]<<" , "<<_currentSuggestion[2]<<" (cout from "<<_myRank<<")"<<endl;
				// Pour le moment on ne fait rien de cette information, mais peut être utile pour une IA.
			}
			else if(typeMessage[0] == -3)
			{
				cout<<"c'est au tour de l'IA "<<_myRank<<endl;
			}
			else if(typeMessage[0] == -4) // fin de la partie
			{
				if(typeMessage[1]==-1) {
					cout<<"IA "<<_myRank<<" est triste d'avoir perdue contre un vulgaire humain inférieur"<<endl<<"EX-TER-MI-NA-TE, EX-TER-MI-NA-TE, EX-TER-MI-NA-TE !!!!!!!!!!!!!!!!!!!!!"<<endl;
				}
				else {
					cout<<"IA "<<_myRank<<" says : YOU SUCK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
				}
				_isGameNotFinished = false;
			}
			
		}
		
	}
	
}

int IA::disproveASuggestion()
{
	int reply = -1;
	vector<int> cardDisaproved;
	for(unsigned int i=0; i < _IaDeck.size(); ++i)
	{
		if(_currentSuggestion[0] == _IaDeck.at(i) or  _currentSuggestion[1] == _IaDeck.at(i) or _currentSuggestion[2] == _IaDeck.at(i)   ){
			cardDisaproved.push_back(_IaDeck.at(i));
		}
	}
	if(cardDisaproved.size() !=0)
	{
		reply = cardDisaproved.at(rand() % cardDisaproved.size());
	}
	return reply;
}

void IA::receiveCards()
{
	int card=0;
	// On ne sait pas combien de cartes on va recevoir, donc utilise une borne à -1
	while(card != -1)
	{
		MPI_Recv(&card, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(card != -1){
			_IaKnowledge.at(card)=false;
			_IaDeck.push_back(card);
		}
		
	}
	cout<<"IA "<<_myRank<<" a fini de recevoir ses cartes, "<<"deck IA "<<_myRank<< " :";
			for(unsigned int j=0; j< _IaDeck.size(); ++j)
			{
				cout<<_IaDeck.at(j)<<", ";
			}
			cout<<endl;
	
}
