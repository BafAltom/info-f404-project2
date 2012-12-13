#ifndef GAMEMASTER_HPP
#define	GAMEMASTER_HPP


#include <mpi.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class GameMaster{
    
public:
	GameMaster() {}
    ~GameMaster() {}
    
    void run(int numberPlayer);
 
    
private:
	
	void distributeCard();
	void startGame();
	int disaproveASuggestion();
	void HumanMakeSuggestion(int suggestion[3]);
	void HumanMakeAccusation(int accusation[3]);
	void MJcheckAccusation(int accusation[3], int sender);
	
	bool isAccusationTrue(int accusation[3]);
	string valueOfCard(int reply);


	int _numberPlayer;
	int _guilty;
	int _crimeWeapon;
	int _crimeRoom;
	vector<int> _humanDeck;
	bool _isGameNotFinished;
	bool _isHumanTurn;
	int _currentSuggestion[3];
	
	
	
	
	
};

#endif
