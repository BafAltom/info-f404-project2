#ifndef IA_HPP
#define	IA_HPP


#include <mpi.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class IA{
    
public:
	IA();
    ~IA() {}
    
    void run(int numberPlayer, int typeOfIA);
    
private:
	void receiveCards();
	void startGame();
	void IAMakeAnAccusation();
	void IAMakeASuggestion(int suspectsRemaining, int weaponsRemaining, int roomsRemaining);
	int disproveASuggestion();
	int getNumberCardsUnknow(int inf, int sup);
	int getIemCardUnknow(int start, int IemCardWanted);
	int getLessTriedCard(int inf, int sup);
	

	vector<int> _IaKnowledge;	
	vector<int> _IaDeck;
	int _numberPlayer;
	int _typeOfIA;
	int _myRank;
	bool _isGameNotFinished;
	bool _isAITurn;
	bool _IACanPlay;
	int _currentSuggestion[3];
	
	
	
	
	
};

#endif
