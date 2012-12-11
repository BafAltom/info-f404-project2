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
	int disproveASuggestion();
	
	// contient les connaissances de l'IA, si true=> la carte correspondante est une solution possible
	// if false, c'est que l'IA a eut une preuve que ce ne pouvait pas être cette carte
	vector<bool> _IaKnowledge;
	
	vector<int> _IaDeck;
	int _numberPlayer;
	int _typeOfIA;
	int _myRank;
	bool _isGameNotFinished;
	bool _isAITurn;
	int _currentSuggestion[3];
	
	
	
	
	
};

#endif
