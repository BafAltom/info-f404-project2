#include "IA.hpp"

IA::IA()
{
	for(int i=0; i<21; ++i)
	{
		_IaKnowledge.push_back(0);
	}	
    MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);
	
}

/**
* \details	The main function of the IA, which initialize and run the game
*/
void IA::run(int numberPlayer, int typeOfIA)
{	
	srand(time(NULL));
	_numberPlayer = numberPlayer;
	_typeOfIA = typeOfIA;
	
	// The IA received the card
	receiveCards();
	
	// then the game start
	startGame();
}

/**
* \details	This function manage the IA.
*/
void IA::startGame()
{
	
	_isGameNotFinished = true;
	_isAITurn = false; //the IA never start, the GM has always the rank 0, so the human player start everytime.
	_IACanPlay = true; //If a IA make a wrong accusation, they can't make suggestion or accusation, but only disaprove a suggestion
	
	while(_isGameNotFinished)
	{
		if(_isAITurn)
		{
			if(_IACanPlay)
			{
				// We compute the number of card unknow for each type.
				int suspectsRemaining = getNumberCardsUnknow(0, 5);
				int weaponsRemaining = getNumberCardsUnknow(6, 11);
				int roomsRemaining = getNumberCardsUnknow(12, 20);
				
				if(suspectsRemaining == 1 and weaponsRemaining == 1 and roomsRemaining ==1){
					//If their is only three cards remaining, the IA can make an accusation
					IAMakeAnAccusation();
				}
				else{
					// otherwise, the IA dont know the answer and make a suggestion
					IAMakeASuggestion(suspectsRemaining, weaponsRemaining, roomsRemaining);
				}
			}
		}
		else {
			//If it's not his turn, the IA wait for a message
			int typeMessage[2];
			MPI_Recv(&typeMessage, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if(typeMessage[0] == -1)// the IA has to answer to a suggestion of one of the other players
			{
				int reply = disproveASuggestion();
				MPI_Send(&reply, 1, MPI_INT, typeMessage[1], 0, MPI_COMM_WORLD);
			}
			else if(typeMessage[0] == -2) // One of the players brodcast a suggestion or accusation
			{	
				MPI_Recv(&_currentSuggestion, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(_typeOfIA == 1){
					// The IA save this information if it's a listening IA (they use it to compute suggestions)
					if(_IaKnowledge.at(_currentSuggestion[0]) != -1){
						_IaKnowledge.at(_currentSuggestion[0]) = _IaKnowledge.at(_currentSuggestion[0]) +1;
					}
					if(_IaKnowledge.at(_currentSuggestion[1]) != -1){
						_IaKnowledge.at(_currentSuggestion[1]) = _IaKnowledge.at(_currentSuggestion[1]) +1;
					}
					if(_IaKnowledge.at(_currentSuggestion[2]) != -1){
						_IaKnowledge.at(_currentSuggestion[2]) = _IaKnowledge.at(_currentSuggestion[2]) +1;
					}
				}
			}
			else if(typeMessage[0] == -3)// The previous player finished is turns, so it's the turn of the IA
			{
				_isAITurn = true;
			}
			else if(typeMessage[0] == -4) // the game stops
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
/**
* \details	This function is call when the IA want to make a suggestion, and manage the game consequently.
*/
void IA::IAMakeASuggestion(int suspectsRemaining, int weaponsRemaining, int roomsRemaining)
{
	int suggestion[3] = {-1, -1, -1};
										
	if(_typeOfIA==0){// If the IA is a default IA
						
		// They choose randomly a card of each type in the remaining unknow cards
		suggestion[0]= getIemCardUnknow(0,rand() % suspectsRemaining);
		suggestion[1]= getIemCardUnknow(6,rand() % weaponsRemaining);
		suggestion[2]= getIemCardUnknow(12,rand() % roomsRemaining);
	}
	else {// If the IA is a default IA
		
		// They choose the card the less asked of each type in the remaining unknow cards
		suggestion[0]= getLessTriedCard(0,5);
		suggestion[1]= getLessTriedCard(6,11);
		suggestion[2]= getLessTriedCard(12,20);
	}

	// We have a suggestion, we chek if somebody can disaprove it
					
	// First the IA broadcast the suggestion to all players
	int messageType4[2] = { -2, _myRank };
	for(int i =0; i< _numberPlayer; ++i)
	{
		if (i != _myRank){
			MPI_Send(&messageType4, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&suggestion, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}

	// After that, the IA see with all player one by one if they can disapprove this suggestion (and stop if a player can)
	int playerAsking = (_myRank +1)% _numberPlayer;
	while (playerAsking != -1 )
	{

		int messageType5[2] = { -1, _myRank };
		MPI_Send(&messageType5, 2, MPI_INT, playerAsking, 0, MPI_COMM_WORLD);
					
		int reply;
		MPI_Recv(&reply, 1, MPI_INT, playerAsking, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// if reply == -6, this player can't disapprove this suggestion, we trie with the following
		if(reply ==-6){
			playerAsking = (playerAsking +1) % _numberPlayer;
			if(playerAsking == _myRank){
				playerAsking = -1;
				// Nobody can disaprove this suggestion, so we find the solution, and we save this suggestion for make an accusation during the next turn
				for(unsigned int i=0; i< _IaKnowledge.size(); ++i)
				{
					if(i!= suggestion[0] and i!= suggestion[1] and i!= suggestion[2]){
						_IaKnowledge.at(i)=-1;
					}
				}
			}
		}
		else{ // otherwise a player disaprove the suggestion.
			playerAsking = -1;
			_IaKnowledge.at(reply)=-1;
		}
	}
					
	// the IA has finished his turn.
	_isAITurn = false;
	int messageType6[2] = { -3, _myRank };
	MPI_Send(&messageType6, 2, MPI_INT, (_myRank+1)% _numberPlayer, 0, MPI_COMM_WORLD);
}

/**
* \details	This function is call when the IA want to make an accusation, and manage the game consequently.
*/
void IA::IAMakeAnAccusation()
{
	// First we compute the accusation
	int accusation[3];
	int cmp = 0;
	for(unsigned int i=0; i< _IaKnowledge.size(); ++i)
	{
		if(_IaKnowledge.at(i)!=-1){
			accusation[cmp] = i;
			cmp ++;
		}
	}
	
	// First the IA broadcast the suggestion to all players
	int messageType[2] = { -2, _myRank };
	for(int i =0; i< _numberPlayer; ++i)
	{
		if (i != _myRank){
			MPI_Send(&messageType, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&accusation, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}

	// After that, the IA see with the GM if they can disapprove this accusation		
	int messageType2[2] = { -5, _myRank };
	MPI_Send(&messageType2, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
	MPI_Send(&accusation, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
					
	int reply;
	MPI_Recv(&reply, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					
	if(reply ==-1){
		// If the Accusation is wrong
		_IACanPlay = false;
		// The game continue, but the IA can only disapprove suggestions, and not playing again.
		int messageType3[2] = { -3, _myRank };
		MPI_Send(&messageType3, 2, MPI_INT, (_myRank+1)% _numberPlayer, 0, MPI_COMM_WORLD);
	}
	// Otherwise, the GM will stop the game
	_isAITurn = false;
}

/**
* \details	this function is used by the listening IA, for define the suggestion for each type
* 			(the cards are saved on one vector, so inf et sup are the limits on this vector for the card of each type)
* \return 	the card the less tried by all players (and if there is some card egals we choose randomly one of them).
*/
int IA::getLessTriedCard(int inf, int sup)
{
	int card = -1;
	int i = rand() % ((sup-inf)+1);
	int j;

	// we choose a card unknow randomly
	// we use a random, because otherwise, all IA will start at the same position of the vector of cards
	while(card ==-1)
	{
		if(_IaKnowledge.at(inf+i) != -1){
			card = inf+i;
			j = i;
		}
		i = (i+1) % ((sup-inf)+1);
	}
	// we search if ther is a card less tried
	while(i != j)
	{
		if(_IaKnowledge.at(inf+i) != -1 and _IaKnowledge.at(inf+i) < _IaKnowledge.at(card)){
			card = inf+i;
		}
		i = (i+1) % ((sup-inf)+1);	
	}
	return card;
}
/**
* \details	this function is used by the default IA, for define the suggestion for each type
* 			(the cards are saved on one vector, so start is the begining on this vector for the card of each type, and 
* 			IemCardWanted a random number which design the card that we want)
* \return 	a random card
*/
int IA::getIemCardUnknow(int start, int IemCardWanted)
{
	// We follow the card of each type until we find the iem card unknow
	int i = start;
	int card =-1;
	while(card == -1)
	{
		if(_IaKnowledge.at(i) != -1){
			if(IemCardWanted == 0) {
				card = i;
			}
			IemCardWanted --;
		}
		i++;
	}
	return card;
}
/**
* \details	this function is used for compute the number of card that are not disapproved (for a type)
* 			(the cards are saved on one vector, so inf et sup are the limits on this vector for the card of each type)
* \return 	the number of card that are not disapproved (for the type given in parameters)
*/
int IA::getNumberCardsUnknow(int inf, int sup)
{
	int number = 0;
	for(int i = inf; i <= sup; ++i)
	{
		if(_IaKnowledge.at(i) != -1){
			number ++;
		}
	}
	return number;
}
/**
* \details	this function is used by the IA for see if she can disapprove a suggestion.
* \return 	-6 if the IA can't disaproved this suggestion, the number of the card disaproved otherwise
*/
int IA::disproveASuggestion()
{
	int reply = -6;
	vector<int> cardDisaproved;
	// First we save all card which disaproved this suggestion
	for(unsigned int i=0; i < _IaDeck.size(); ++i)
	{
		if(_currentSuggestion[0] == _IaDeck.at(i) or  _currentSuggestion[1] == _IaDeck.at(i) or _currentSuggestion[2] == _IaDeck.at(i)   ){
			cardDisaproved.push_back(_IaDeck.at(i));
		}
	}
	// Then we choose a card randomly.
	if(cardDisaproved.size() !=0)
	{
		reply = cardDisaproved.at(rand() % cardDisaproved.size());
	}
	return reply;
}
/**
* \details	this function is used by the IA for receive and store her cards.
*/
void IA::receiveCards()
{
	int card=0;
	// We dont know how much card we will receive, so we use a terminal sign
	while(card != -1)
	{
		MPI_Recv(&card, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(card != -1){
			// we store the card in one vector, and we update the knowledge of the IA about the possible cards for the solution.
			_IaKnowledge.at(card)=-1;
			_IaDeck.push_back(card);
		}
	}
}
