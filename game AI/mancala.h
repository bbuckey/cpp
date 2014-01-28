#include <iostream>
#include <string>
using namespace std;

enum player_type {TOP, BOTTOM}; // who the players are

enum player_type swapPlayer(enum player_type p)
// change from one player to another
{
	if (p == TOP)
		return BOTTOM;
	else
		return TOP;
}

enum move_result {FAIL, PASS, DONE};
// a move could not work (FAIL), end in a kalaha so the opponent
// loses a turn (PASS), or be nothing special (DONE)

class Board {
private:
	int pits[2][6]; // the playing pits 
	int topKalaha; // for scoring top's beads
	int bottomKalaha; // for scoring bottom's beads

public:
	Board(); // constructor
	Board(Board& rhs) : topKalaha(rhs.topKalaha), bottomKalaha(rhs.bottomKalaha)
		// copy constructor
	{ 
		for (int i=0; i<6; i++)
		{
			pits[TOP][i] = rhs.pits[TOP][i];
			pits[BOTTOM][i] = rhs.pits[BOTTOM][i];
		}
	}
	enum move_result sow(enum player_type pl, int pit); // make a move
	void display() const; // show the board
	bool isGameOver(); // check for game over (scoring beads if necessary)
	int topScore() const { return topKalaha; }
	int bottomScore() const { return bottomKalaha; }
};

Board::Board()
// constructor for class
{
	int i;
	for (i=0; i<6; i++)
	{
		pits[0][i] = pits[1][i] = 3;
	}
	topKalaha = 0;
	bottomKalaha = 0;
}

enum move_result Board::sow(enum player_type pl, int pit)
{
	int direction; // which way to go
	int beads; // the beads you are sowing
	int whereYouAre;

	// if illegal pit or the chosen pit is empty, move doesn't
	// work
	if ((pit < 0) || (pit >= 6) || (pits[pl][pit] == 0))
		return FAIL;
	// if it's top's turn, sow to lower pits
	if (pl == TOP)
		direction = -1; // top sows right to left to start
	else
		direction = 1; // bottom sows left to right

	beads = pits[pl][pit];
	pits[pl][pit]=0; // clear this pit
	whereYouAre = pit;
	while (beads > 0)
	{
		whereYouAre = whereYouAre + direction;
		if (whereYouAre == -1) // at top's Kalaha
		{
			if (pl == TOP) // if it's top's turn
			{
				topKalaha++; // add bead to top Kalaha
				beads--;
			}
			direction = 1; // go the other way
		}
		else if (whereYouAre == 6) // at bottom's Kalaha
		{
			if (pl == BOTTOM) // make sure top skips it
			{
				bottomKalaha++; // add bead to bottom Kalaha
				beads--;
			}
			direction = -1;
		}
		else
		{
			if (direction == 1)
				pits[BOTTOM][whereYouAre]++;
			else
				pits[TOP][whereYouAre]++;
			beads--;
		}
	}
    if (((pl == TOP) && (whereYouAre == -1)) ||
        ((pl == BOTTOM) && (whereYouAre == 6)))
            return PASS; // place a bead in your own Kalaha
                        // get a free turn
    else
	{ // capturing opponent's beads
            if ((pl == TOP) && (direction == -1) && 
                (whereYouAre != -1) && (whereYouAre != 6) && 
                (pits[TOP][whereYouAre] == 1))
            { // check to see if you're on the correct side
				// and that you placed in an empty pit
                topKalaha++; // score the last bead
                pits[TOP][whereYouAre] = 0;
                topKalaha += pits[BOTTOM][whereYouAre];
				// score beads in the opponent's pit
                pits[BOTTOM][whereYouAre] = 0;
            }
            else
                if ((pl == BOTTOM) && (direction == 1) && 
                (whereYouAre != -1) && (whereYouAre != 6) && 
                    (pits[BOTTOM][whereYouAre] == 1))
                {
                    bottomKalaha++;
                    pits[BOTTOM][whereYouAre] = 0;
                    bottomKalaha += pits[TOP][whereYouAre];
                    pits[TOP][whereYouAre] = 0;
                }

            return DONE;
        }
}

void Board::display() const
// printing the board
{
	cout << '\t';
	for (int i=0; i<6; i++) // gives column numbers
		cout << i << '\t';
	cout << "\n\t";
	for ( i=0; i<6; i++) // top's pits
		cout << pits[0][i] << '\t';
	cout << endl;
	cout << topKalaha << "\t\t\t\t\t\t\t" << bottomKalaha << endl;
	cout << '\t';
	for (i=0; i<6; i++) // bottom's pits
		cout << pits[1][i] << '\t';
	cout << endl;
}

bool Board::isGameOver()
// determines if game is over because one side is clear
// allows other player to clear remaining beads
{
	int topBeads = 0, bottomBeads = 0;
	enum player_type cleaner;

	for (int i=0; i<6; i++) // count beads left on both sides
	{
		topBeads += pits[TOP][i];
		bottomBeads += pits[BOTTOM][i];
	}
	if (topBeads == 0) // if top pits are empty, then give remaining
						// beads to bottom
	{
		cleaner = BOTTOM;
		bottomKalaha += bottomBeads;
	}
	else
		if (bottomBeads == 0) // if bottom is done, top gets beads
		{
			cleaner = TOP;
			topKalaha += topBeads;
		}
	else
		return false; // not done yet
	for (i=0; i<6; i++) // clean out all the pits if we are done
		pits[cleaner][i] = 0;
	return true;
}


