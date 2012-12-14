#include "GameMaster.hpp"

/**
* \details	The main function of the game master, which initialize and run the game
*/
void GameMaster::run(int numberPlayer)
{	
	_numberPlayer = numberPlayer;
	srand(time(NULL));
	
	// The GM distribute the cards
	distributeCard();
	
	// the game start
	startGame();
}

/**
* \details	This function manage the game and the human player.
*/
void GameMaster::startGame()
{
	
	_isGameNotFinished = true;
	_isHumanTurn = true; //GM have the rank 0, so they start allways (and so the human player too)
	
	while(_isGameNotFinished)
	{
		if(_isHumanTurn)
		{
			int suggestion[3];
			int suggestionType;
			cout<<"Do you want to make an accusation (type 0) or a suggestion (type 1)  ?"<<endl;
			cin>>suggestionType;
			cout<<"Your cards :";
			for(unsigned int j=0; j< _humanDeck.size(); ++j)
			{
				cout<<valueOfCard(_humanDeck.at(j))<<" ("<<_humanDeck.at(j)<<")  ";
			}
			cout<<endl;
			cout<<"Liste of the cards"<<endl;
			cout<<"Suspects			Weapons					Rooms"<<endl
				<<"-------------------------------------------------------------------------------------"<<endl
				<<"0 : Miss scarlet		6 : Candlestick				12 : Kitchen"<<endl
				<<"1 : Colonel Mustard		7 : Knife				13 : Dining room"<<endl
				<<"2 : Mrs. White			8 : Lead pipe				14 : Ballroom"<<endl
				<<"3 : Reverend Green		9 : Revolver				15 : Conservatory"<<endl
				<<"4 : Mrs. Peacock		10 : Hangman's knot			16 : Billiard room"<<endl
				<<"5 : Professor Plum		11 : Spanner				17 : Library"<<endl
				<<"									18 : Study"<<endl
				<<"									19 : Hall"<<endl
				<<"									20 : Lounge"<<endl
				<<"-------------------------------------------------------------------------------------"<<endl;
			cout<<"Choose the suspect"<<endl;
			cin>>suggestion[0];
			cout<<"Choose the weapon"<<endl;
			cin>>suggestion[1];
			cout<<"Choose the room"<<endl;
			cin>>suggestion[2];
			
			if(suggestionType == 1) // The player want to make a suggestion
			{
				HumanMakeSuggestion(suggestion);
			}
			else // The player want to make an accusation
			{
				HumanMakeAccusation(suggestion);
			}
			// The player finish his turn, it's the turn of the AI with rank 1.
			_isHumanTurn = false;
			int messageType[2] = { -3, 0 };
			MPI_Send(&messageType, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
			
		}
		else // It's the turn of the AI's, so the GM (and human player) wait for a message of one of the AI's
		{
			int typeMessage[2];
			MPI_Recv(&typeMessage, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(typeMessage[0] == -1) // the human player has to answer to a suggestion of one of the AI's
			{
				int reply = disaproveASuggestion();
				MPI_Send(&reply, 1, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD);
			}
			else if(typeMessage[0] == -2)// One of the AI's brodcast a suggestion or accusation
			{
				MPI_Recv(&_currentSuggestion, 3, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				cout<<"AI "<<typeMessage[1]<<" make a suggestion or accusation for "<<valueOfCard(_currentSuggestion[0])<<" ("<<_currentSuggestion[0]<<") , "<<valueOfCard(_currentSuggestion[1])<<" ("<<_currentSuggestion[1]<<") , "<<valueOfCard(_currentSuggestion[2])<<" ("<<_currentSuggestion[2]<<")"<<endl;
			}
			else if(typeMessage[0] == -3) // The AI's finished their turns, so it's the turn of the human player
			{
				_isHumanTurn = true;
			}
			else if(typeMessage[0] == -5)// The GM has to answer to an accusation
			{
				int accusation[3];
				MPI_Recv(&accusation, 3, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				GMCheckAccusation(accusation, typeMessage[1]);
			}	
		}	
	}
}
/**
* \details	The GM check if the accusation is correct and manages the game consequently.
*/
void GameMaster::GMCheckAccusation(int accusation[3], int sender)
{
	if(isAccusationTrue(accusation)){
		//the accusation is correct, the AI win and the game stop.
		cout<<"AI "<<sender<<" make a correct accusation and win the game"<<endl;
		cout<<"the solution was :"<<valueOfCard(accusation[0])<<" ("<<accusation[0]<<") "<<valueOfCard(accusation[1])<<" ("<<accusation[1]<<") "<<valueOfCard(accusation[2])<<" ("<<accusation[2]<<")"<<endl;
		int reply = 0;
		MPI_Send(&reply, 1, MPI_INT, sender, 0, MPI_COMM_WORLD);
		
		// We say an all the AI's that the game is over.
		int messageType[2] = { -4, 0 };
		for(int i =1; i< _numberPlayer; ++i)
		{
			MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		_isGameNotFinished=false;
	}
	else{
		// the accusation is wrong, the GM says to the AI that they cannot play again.
		cout<<"AI "<<sender<<" make a wrong accusation and cannot play again"<<endl;
		int reply = -1;
		MPI_Send(&reply, 1, MPI_INT, sender, 0, MPI_COMM_WORLD);
	}
}
/**
* \details	This function is call when the human player maked a suggestion, and manage the game consequently.
*/
void GameMaster::HumanMakeSuggestion(int suggestion[3])
{
	// First the GM broadcast the suggestion to all AI's
	int messageType[2] = { -2, 0 };
	for(int i =1; i< _numberPlayer; ++i)
	{
		MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
		MPI_Send(suggestion, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	
	// After that, the GM see with all AI one by one if they can disapprove this suggestion (and stop if a AI can)
	int playerAsking = 1;
	while (playerAsking !=-1)
	{
		int messageType[2] = { -1, 0 };
		MPI_Send(&messageType, 2, MPI_INT, playerAsking, 0, MPI_COMM_WORLD);
					
		int reply;
		MPI_Recv(&reply, 1, MPI_INT, playerAsking, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// if reply == -6, this AI can't disapprove this suggestion, we trie with the following
		if(reply ==-6){
			playerAsking ++;
			if(playerAsking >= _numberPlayer){
				playerAsking = -1;
				cout<<"Nobody can refute your suggestion"<<endl;
			}
		}
		else{ // otherwise an AI disaprove the suggestion, the human player has finished his turn.
			cout<<"AI "<<playerAsking<<" refute your suggestion for the card "<<valueOfCard(reply)<<" ("<<reply<<")"<<endl;	
			playerAsking = -1;
		}
	}
}
/**
* \details	This function is call when the human player maked an accusation, and manage the game consequently.
*/
void GameMaster::HumanMakeAccusation(int accusation[3])
{
	int messageType[2] = { -4, 0 };
	if(isAccusationTrue(accusation)){
		// If the accusation is correct, they win
		cout<<"Congratulation, you win this game"<<endl;
		messageType[1] = -1;
	}
	else{
		// Otherwise the game stops
		cout<<"Your solution is wrong, looser"<<endl;
		cout<<"the solution was :"<<valueOfCard(_guilty)<<" ("<<_guilty<<") "<<valueOfCard(_crimeWeapon)<<" ("<<_crimeWeapon<<") "<<valueOfCard(_crimeRoom)<<" ("<<_crimeRoom<<") "<<endl;
	}
	_isGameNotFinished=false;
	// the Gm says to all AI's that the game stops.
	for(int i =1; i< _numberPlayer; ++i){
		MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
}

/**
* \details	this function is used by the Gm and the human player to answer when an AI ask for a suggestion
* \return 	-6 if the human player can't disaproved this suggestion, the number of the card disaproved otherwise
*/
int GameMaster::disaproveASuggestion()
{
	int reply = -6;
	vector<int> cardDisaproved;
	// First we save all card which disaproved this suggestion
	for(unsigned int i=0; i < _humanDeck.size(); ++i)
	{
		if(_currentSuggestion[0] == _humanDeck.at(i) or  _currentSuggestion[1] == _humanDeck.at(i) or _currentSuggestion[2] == _humanDeck.at(i)   ){
			cardDisaproved.push_back(_humanDeck.at(i));
		}
	}
	// Then, if their is more then one card, we ask to the human player which card they want to use.
	if(cardDisaproved.size() > 1){
		cout<<"you can disaprove this suggestion with some of your card, which one would you use ?"<<endl;
		for(unsigned int i=0; i < cardDisaproved.size(); ++i){
			cout<<valueOfCard(cardDisaproved.at(i))<<"("<<cardDisaproved.at(i)<<")  ";
		}
		cout<<"?"<<endl;
		cin>>reply;
	}
	// If their is only one card, we used this card without asking to the human (but we warns him)
	else if(cardDisaproved.size() == 1){
		cout<<"this suggestion is disaproved with one of your card : "<<valueOfCard(cardDisaproved.at(0))<<" ("<<cardDisaproved.at(0)<<") "<<endl;
		reply = cardDisaproved.at(0);
	}
	// If their is no card which disaproved this suggestion, reply is still at -6
	return reply;
}

/**
* \details	check if an accusation is true
* \return 	true if the accusation is true, false otherwise
*/
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

/**
* \details	convert the number of a card, on her name.
* \return 	the "name" of the card given in parameter
*/
string GameMaster::valueOfCard(int card)
{
	const char* list[21] = {"Miss scarlet", "Colonel Mustard", "Mrs. White", "Reverend Green", "Mrs. Peacock", "Professor Plum", "Candlestick", "Knife", "Lead pipe", 
	"Revolver", "Hangman's knot", "Spanner", "Kitchen", "Dining room", "Ballroom", "Conservatory", "Billiard room", "Library", "Study", "Hall", "Lounge"};

	return list[card];

	
}
/**
* \details	this function save the three card of the solution and then distribute the other cards to the players
*/
void GameMaster::distributeCard()
{
	// First we select the three card of the solution

	_guilty = rand() % 6;
	_crimeWeapon = 6+ (rand() % 6);
	_crimeRoom = 12+ (rand() % 9);	
	
	// Then we distribute the other card to the other students
	
	// We create the deck of 21 cards
	vector<int> deck;
	for(int i=0; i < 21; ++i)
	{
		deck.push_back(i);
	}

	// we delete the three card selected
	deck.erase(deck.begin()+(_crimeRoom));
	deck.erase(deck.begin()+(_crimeWeapon));
	deck.erase(deck.begin()+_guilty);

	
	// we distribute the cards one by one
	int player =0;
	while(deck.size() !=0)
	{
		// we select a card randomly, and send it to a player
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
	// when we distributed all cards, we says to the AI that all card are distributed.
	// we should do this, because the number of card distributed change with the number of AI, and so it's simpler with a terminal sign
	int stop =-1;
	for(int i =1; i< _numberPlayer; ++i)
	{
		MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	cout<<"You receive the cards :";
	for(unsigned int j=0; j< _humanDeck.size(); ++j)
	{
		cout<<valueOfCard(_humanDeck.at(j))<<" ("<<_humanDeck.at(j)<<")  ";
	}
	cout<<endl;
}

