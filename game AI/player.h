#include <string>
#include "mancala.h"

class Player
{
protected: // ensure subclasses can see these
	char name[20];
	enum player_type side;
public:
	Player(char *n, enum player_type p) : side(p) 
		{strcpy(name,n); }
	char *ReportName() { return name; }
	virtual int ChooseMove(const Board& b) = 0;
	// leave for subclasses
};

class Computer : public Player
{
public:
	// pass data to superclass' constructor
	Computer(char *n, enum player_type p) : Player(n,p) {};
	int ChooseMove(const Board& b) 
	{ 
		cout << "You fill this in\n";
		return 0;
	}
};

class Human: public Player
{
public:
	Human(char *n, enum player_type p) : Player(n,p) {};
	int ChooseMove(const Board& b);
};

int Human::ChooseMove(const Board& b)
// get move from user input
{
	int theMove = -1;
	cout << name;	
	cout << ", please enter the number of the pit you wish to sow.\n";
	if (side == TOP)
		cout << "You are on the top.\n";
	else
		cout << "You are on the bottom.\n";
	cout << "Numbering is left to right (0-5): ";
	while ((theMove < 0) || (theMove > 5))
	{
		// error check for pits out of bounds
		cin >> theMove;
		if ((theMove < 0) || (theMove > 5))
		{
			cout << "Illegal pit number. Try again: ";
		}
	}
	return theMove;
}
