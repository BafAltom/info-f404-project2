#ifndef IA_HPP
#define	IA_HPP


#include <mpi.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class AI{
    
public:
	AI();
    ~AI() {}
    
    void run(int numberPlayer, int typeOfAI);
    
private:
	void receiveCards();
	void startGame();
	void AIMakeAccusation();
	void AIMakeSuggestion(int suspectsRemaining, int weaponsRemaining, int roomsRemaining);
	int disaproveASuggestion();
	int getNumberCardsUnknow(int inf, int sup);
	int getIemCardUnknow(int start, int IemCardWanted);
	int getLessTriedCard(int inf, int sup);
	

	vector<int> _AiKnowledge;	
	vector<int> _AiDeck;
	int _numberPlayer;
	int _typeOfAI;
	int _myRank;
	bool _isGameNotFinished;
	bool _isAiTurn;
	bool _AiCanPlay;
	int _currentSuggestion[3];
	
	
	
	
	
};

#endif
