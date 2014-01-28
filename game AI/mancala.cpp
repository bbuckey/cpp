#include "player.h"

int main()
{

	Board b;
	int topType, bottomType; // Type refers to human or computer
	Player *plyrs[2]; // 2 players
	char pName[20]; // dummy variable to hold name entered
	int pit; // pit selected for a move
	enum move_result mRes; // result of move (fail,pass,done)

	cout << "Welcome to Mancala!\n";
	// determine name and type of top
	cout << "Do you want the top player to be (1) human or (2) computer? ";
	cin >> topType;
	while ((topType != 1) && (topType != 2))
	{
		cout << "Please enter 1 or 2.\n";
		cin >> topType;
	}
	cout << "What is the player's name? ";
	cin >> pName;
	if (topType == 1)
		plyrs[0] = new Human(pName, TOP);
	else
		plyrs[0] = new Computer(pName, TOP);
	// determine name and type of bottom
	cout << "Do you want the bottom player to be (1) human or (2) computer? ";
	cin >> bottomType;
	while ((bottomType != 1) && (bottomType != 2))
	{
		cout << "Please enter 1 or 2.\n";
		cin >> bottomType;
	}
	cout << "What is the player's name? ";
	cin >> pName;
	if (bottomType == 1)
		plyrs[1] = new Human(pName, BOTTOM);
	else
		plyrs[1] = new Computer(pName, BOTTOM);

	enum player_type whoseTurn = TOP; // top starts

	while (!b.isGameOver()) // repeat until end of game
	{
		cout << "**************************\n";
		b.display(); // print the board
		pit = plyrs[whoseTurn]->ChooseMove(b); // determine move
		while ((mRes=b.sow(whoseTurn,pit)) == FAIL)
		{
			// move didn't work, try another move
			cout << "That pit is empty.\n";
			pit = plyrs[whoseTurn]->ChooseMove(b);
		}
		if (mRes == DONE) 
			whoseTurn = swapPlayer(whoseTurn);
		// change players if turn was DONE
		// keep same player if it was a PASS
		// (it can't be FAIL or we wouldn't escape the loop)
	}
	// report scores
	cout << plyrs[0]->ReportName() << "'s score is " << 
		b.topScore() << endl;
	cout << plyrs[1]->ReportName() << "'s score is " << 
		b.bottomScore() << endl;
	// find out who won
	if (b.topScore() > b.bottomScore())
		cout << "Congratulations, " << plyrs[0]->ReportName() 
			<< "! You win!" << endl;
	else if (b.topScore() < b.bottomScore())
		cout << "Congratulations, " << plyrs[1]->ReportName() 
			<< "! You win!" << endl;
	else
		cout << "It's a draw!\n";

	return 0;
}
