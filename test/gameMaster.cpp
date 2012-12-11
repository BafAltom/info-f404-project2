#include "gameMaster.hpp"


void GameMaster::run(int numberPlayer)
{	
	// demande du nombre de joueurs IA
	_numberPlayer = numberPlayer;
	
	// distribue les cartes.
	distributeCard();
	
	// start game
	startGame();
}

void GameMaster::startGame()
{
	
	_isGameNotFinished = true;
	_isHumanTurn = true; //MJ a le rank 0, le joueur humain commence donc toujours.
	
	while(_isGameNotFinished)
	{
		if(_isHumanTurn)
		{
			//int messageType[2] = { -1, 0 };
			int suggestion[3];
			int suggestionType;
			cout<<"Do you want to make an accusation (type 0) or a suggestion (type 1)  ?"<<endl;
			cin>>suggestionType;
			//messageType[0] = messageType[0] + suggestionType;
			cout<<"Liste of the cards"<<endl;
			cout<<"Suspects					Weapons						Rooms"<<endl
				<<"--------------------------------------------------------------------------"<<endl
				<<"0 : Miss scarlet		6 : Candlestick				12 : Kitchen"<<endl
				<<"1 : Colonel Mustard		7 : Knife				13 : Dining room"<<endl
				<<"2 : Mrs. White			8 : Lead pipe				14 : Ballroom"<<endl
				<<"3 : Reverend Green		9 : Revolver				15 : Conservatory"<<endl
				<<"4 : Mrs. Peacock		10 : Hangman's knot			16 : Billiard room"<<endl
				<<"5 : Professor Plum		11 : Spanner				17 : Library"<<endl
				<<"									18 : Study"<<endl
				<<"									19 : Hall"<<endl
				<<"									20 : Lounge"<<endl
				<<"--------------------------------------------------------------------------"<<endl;
			cout<<"Choose the suspect"<<endl;
			cin>>suggestion[0];
			cout<<"Choose the weapon"<<endl;
			cin>>suggestion[1];
			cout<<"Choose the room"<<endl;
			cin>>suggestion[2];
			
			
			if(suggestionType == 1) // le joueur veut faire une suggestion
			{
				//cout<<"le joueur envoie le message :"<<messageType[0]<<" from "<<messageType[1]<<endl;
				cout<<"suggestion :"<<suggestion[0]<<" , "<<suggestion[1]<<" , "<<suggestion[2]<<endl;
				// On avertit toutes les IA qu'on fait une suggestion.
				int messageType[2] = { -2, 0 };
				for(int i =0; i< _numberPlayer; ++i)
				{
					cout<<"send message type"<<messageType[0]<<","<<messageType[1]<<" a "<<i<<endl;
					MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
					cout<<"send suggestion a "<<i<<endl;
					MPI_Send(&suggestion, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
				}
				// On voit avec chaque IA si elle peut réfuter cette suggestion (jusqu'à ce qu'une d'entre elle le fasse)
				cout<<"on a broadcaster la suggestion a toutes les IA"<<endl;
				int playerAsking = 1;
				while (playerAsking !=-1)
				{
					cout<<"on demande a "<<playerAsking<<endl;
					int messageType[2] = { -1, 0 };
					MPI_Send(&messageType, 2, MPI_INT, playerAsking, 0, MPI_COMM_WORLD);
					MPI_Send(&suggestion, 3, MPI_INT, playerAsking, 0, MPI_COMM_WORLD);
					
					cout<<"playerAsking va répondre"<<endl;
					int reply;
					MPI_Recv(&reply, 1, MPI_INT, playerAsking, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					// if reply == -1 => il n'a pas put répondre, passe au suivant
					cout<<"playerAsking a répondu "<<reply<<endl;
					if(reply ==-1){
						playerAsking ++;
						if(playerAsking >= _numberPlayer){
							playerAsking = -1;
							cout<<"Nobody can refute your suggestion"<<endl;
						}
					}
					else{ // quelqu'un a réfuter cette accusation
						cout<<"The IA "<<playerAsking<<" refute your suggestion for the card "<<valueOfCard(reply)<<endl;	
						playerAsking = -1;
					}
					
				}
				
				
			}
			else // le joueur veut faire une accusation
			{
				bool resultOfAccusation = isAccusationTrue(suggestion);
				int messageType[2] = { -4, 0 };
				if(resultOfAccusation)
				{
					cout<<"Congratulation, you win this game"<<endl;
					messageType[1] = -1;
				}
				else
				{
					cout<<"Your solution is wrong, looser"<<endl;
				}
				_isGameNotFinished=false; // si humain fait une mauvaise accusation, la partie s'arrete
				
				for(int i =0; i< _numberPlayer; ++i)
				{
					MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
				}
				
				
			}
			
		}
		else // le MJ attend un message
		{
			int typeMessage[2];
			MPI_Recv(&typeMessage, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if(typeMessage[0] == -1)
			{
				cout<<"MJ a recu une suggestion et doit y répondre"<<endl;
			}
			else if(typeMessage[0] == -2)
			{
				cout<<"quelqu'un a fait une accusation ou une suggestion, MJ est prévenu pour info et ne doit pas répondre"<<endl;
				int suggestion[3];
				MPI_Recv(&suggestion, 3, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				cout<<typeMessage[1]<<" a fait une accusation/suggestion pour "<<suggestion[0]<<" , "<<suggestion[1]<<" , "<<suggestion[2]<<endl;
			}
			else if(typeMessage[0] == -3)
			{
				cout<<"c'est au tour du joueur humain"<<endl;
			}
			else if(typeMessage[0] == -5)
			{
				cout<<"MJ a recu une accusation et doit y répondre"<<endl;
			}
			
		}
		
	}
	
}

bool GameMaster::isAccusationTrue(int suggestion[3])
{
	bool isTrue = true;
	
	if(suggestion[0] != _guilty){
		isTrue = false;
	}
	else if(suggestion[1] != _crimeWeapon){
		isTrue = false;
	}
	else if(suggestion[2] != _crimeRoom){
		isTrue = false;
	}
	
	return isTrue;
}

string GameMaster::valueOfCard(int reply)
{
	const char* list[21] = {"Miss scarlet", "Colonel Mustard", "Mrs. White", "Reverend Green", "Mrs. Peacock", "Professor Plum", "Candlestick", "Knife", "Lead pipe", 
	"Revolver", "Hangman's knot", "Spanner", "Kitchen", "Dining room", "Ballroom", "Conservatory", "Billiard room", "Library", "Study", "Hall", "Lounge"};

	return list[reply];

	
}

void GameMaster::distributeCard()
{
	
	srand(time(NULL));

	// First we select the three card of the solution

	_guilty = rand() % 6;
	_crimeWeapon = 6+ (rand() % 6);
	_crimeRoom = 12+ (rand() % 9);
	cout<<_guilty<<" killed with "<<_crimeWeapon<<" in the "<<_crimeRoom<<endl;
	
	
	// Then we distribute the other card to the other students
	vector<int> deck;
	for(int i=0; i < 21; ++i)
	{
		deck.push_back(i);
	}

	// we delete the three card selected
	deck.erase(deck.begin()+(_crimeRoom));
	deck.erase(deck.begin()+(_crimeWeapon));
	deck.erase(deck.begin()+_guilty);


	
	int player =0;
	while(deck.size() !=0)
	{
		// On ne peut envoyer directement un rand(), car le deck est incomplet(on a virer 3 cartes), les numéros et les valeurs du deck sont donc !=
		int cardNumber = rand() % deck.size();
		int card = deck.at(cardNumber);
		deck.erase(deck.begin()+cardNumber);
		
		if(player == 0){
			_humanDeck.push_back(card);
		}
		else{
			MPI_Send(&card, 1, MPI_INT, player, 0, MPI_COMM_WORLD);
		}
		
		player = (player+1) % _numberPlayer ;
		
	}
	int stop =-1;
	for(int i =0; i< _numberPlayer; ++i)
	{
		MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	cout<<"deck human :";
	for(unsigned int j=0; j< _humanDeck.size(); ++j)
	{
		cout<<_humanDeck.at(j)<<", ";
	}
	cout<<endl;
	
}












