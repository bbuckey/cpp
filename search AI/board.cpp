#include "board.h"
//#include <stack>
//#include <deque>


Board::Board() : numBots(0), movesMade(0)
// default constructor for empty board
{
	int i;
	for (i=0; i<SIZE; i++)
		for (int j=0; j<SIZE; j++)
			grid[i][j] = '_';
}

Board::Board(Board &b) : numBots(b.numBots), movesMade(b.movesMade)
{
	// copy constructor
	int i;
	for (i=0; i<SIZE; i++)
		for (int j=0; j<SIZE; j++)
			grid[i][j] = b.grid[i][j];
}

void Board::SetSpot(Spot s, char robot)
{
	// put a robot in a spot
//	cout << s.GetRow()<< " " <<s.GetCol() << " " << robot << endl;
	grid[s.GetRow()][s.GetCol()] = robot; 
}

void Board::Init(ifstream& str)
{
	// read board from file
	str >> numBots;
	for (int i=0; i<numBots; i++) // for each bot
		{
			int r, c;
			char bot;
			str >> bot >> r >> c; // read its name and spot
			Spot s;
			s.SetCol(c);
			s.SetRow(r);
			SetSpot(s,bot); // put it there
		}
}

Spot Board::GetSpot(char robot) const
// finds where a given robot is
{
	int r,c;
	Spot s;
	for (r=0; r<SIZE; r++)
		for (c=0; c<SIZE; c++)
			if (grid[r][c] == robot)
				{
					s.SetCol(c);
					s.SetRow(r);
					return s;
				}
	return s;
}

bool Board::CanMove(char robot, char dir) const
// checks if a given move is legal
{
	Spot s = GetSpot(robot); // find the robot
	Spot start = s;
	if (s.BadSpot()) // if there is no such robot, the move
		return false; // clearly doesn't work
	switch (tolower(dir))
		{
		case 'u': 
			// going up in grid is going to smaller rows
			s.UpOne();
			while (!Taken(s)) // move up grid until you're stopped
				s.UpOne();
			start.UpOne(); // change start so we can check if we've
			// moved at least one space
			break;
		case 'd':
			// going down in grid is going to larger rows
			s.DownOne();
			while (!Taken(s))
				s.DownOne();
			start.DownOne();
			break;
		case 'l':
			// going left in grid is going to smaller cols
			s.LeftOne();
			while (!Taken(s))
				s.LeftOne();
			start.LeftOne();
			break;
		case 'r':
			// going right in grid is going to larger cols
			s.RightOne();
			while (!Taken(s))
				s.RightOne();
			start.RightOne();
			break;

		}
	return (!s.BadSpot() && (start != s));
	// true if space is legal, and we've actually moved
}

void Board::MakeMove(char robot, char dir)
// makes a move on the board
{
	if (!CanMove(robot,dir)) // check if we can do it
		return;

	Spot block = GetSpot(robot);
	Spot newspot = block;
	Spot startspot = block;
	if (block.BadSpot())
		return;
	switch (tolower(dir))
		{
		case 'u':
			// going up in grid is going to smaller rows
			block.UpOne();
			while (!Taken(block))
				{
					block.UpOne();
					newspot.UpOne();
				}
			break;
		case 'd':
			// going down in grid is going to larger rows
			block.DownOne();
			while (!Taken(block))
				{
				block.DownOne();
				newspot.DownOne();
				}
			break;
		case 'l':
			// going left in grid is going to smaller cols
			block.LeftOne();
			while (!Taken(block))
				{
				block.LeftOne();
				newspot.LeftOne();
				}
			break;
		case 'r':
			// going right in grid is going to larger cols
			block.RightOne();
			while (!Taken(block))
				{
				block.RightOne();
				newspot.RightOne();
				}
			break;

		}
	SetSpot(startspot,'_'); // clear old spot in grid
	SetSpot(newspot,robot); // put robot in new spot
	movesMade++; // for keeping track of the game length
}	

void Board::Display() const
// prints the grid
{
	int r, c;
	for (r=0; r<SIZE; r++)
		{
			for (c=0; c<SIZE; c++)
				if (r==(SIZE-1)/2 && c==(SIZE-1)/2 &&
						grid[r][c] == '_')
					cout << "* "; // put a * on the station
				else 
					cout << grid[r][c] << ' ';
			cout << endl;
		}
}

bool Board::operator==(const Board& otherBoard)
{
	if (numBots != otherBoard.numBots)
		return false;
	for (int x=0; x<SIZE; x++)
		for (int y=0; y<SIZE; y++)
			if (grid[x][y] != otherBoard.grid[x][y])
				return false;
			
	return true;
}




void ListBoard::bestf()
{
	Queue openlist;
	Node2 temp;
	temp.SetnBoard(eboard);
	temp.Setheur(heuristic(eboard));
	Board newone = eboard;
	while(true)
	{
		if(newone.Solved())
		{
			newone.Display();
			cout << "You won the Game" << "\nit took you " << newone.GameLength() << " to win" << endl;
			temp.printmove();
			return;
		}
		else
		{
	char a[9] = {'X','A','B','C','D','E','F','G'};
	char b[5] = {'L','D','R','U'};

	for(int i = 0; i < newone.getnumbot(); i++) //get num bots for pos moves
	{
		for(int j = 0; j < 4; j++) // makes move with bots
		{
			if(newone.CanMove(a[i],b[j]))
			{
				Node2 _temp;
				_temp = temp;
				Board tem =_temp.GetnBoard();
				tem.MakeMove(a[i],b[j]);
				_temp.SetnBoard(tem);
				_temp.Setheur(heuristic(tem)+_temp.Getheur());//A* heuristic g+f
				_temp.insertp(_temp.GetnBoard(),a[i],b[j]);
				if(!alrdyuse(tem)) // checks to see if board was alrdy made
				openlist.push(_temp);
				openlist.sortq();
			}
		}
	}

		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(newone); // inserts the board you just checked
		temp = openlist.front();
		openlist.pop();
		newone = temp.GetnBoard();
		newone.Display();
//		cout << temp.Getheur() << "  " << endl;
	}
}
/*void ListBoard::breadth() // uncomment STACK and QUEUE CLASS with BOARD object
{
	Queue openlist;
	Board temp = eboard;
	while(true)
	{
		if(temp.Solved())
		{
			temp.Display();
			cout << "You won the Game" << endl;
			return;
		}
		else
		{
	char a[9] = {'X','A','B','C','D','E','F','G'};
	char b[5] = {'U','D','L','R'};

	for(int i = 0; i < eboard.getnumbot(); i++) //get num bots for pos moves
	{
		for(int j = 0; j < 4; j++) // makes move with bots
		{
			if(eboard.CanMove(a[i],b[j]))
			{
				Board _temp;
				_temp = temp;
				_temp.MakeMove(a[i],b[j]);
				if(!alrdyuse(_temp)) // checks to see if board was alrdy made
				openlist.push(_temp);
			}
		}
	}

		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(temp); // inserts the board you just checked
		temp = openlist.front();
		openlist.pop();
		temp.Display();
	}
}
void ListBoard::depthf()
{
	Stack openlist;
	Board temp = eboard;
	while(true)
	{
		if(temp.Solved())
		{
			temp.Display();
			cout << "You won the Game" << endl;
			return;
		}
		else
		{
	char a[9] = {'X','A','B','C','D','E','F','G'};
	char b[5] = {'U','D','L','R'};

	for(int i = 0; i < eboard.getnumbot(); i++) //get num bots for pos moves
	{
		for(int j = 0; j < 4; j++) // makes move with bots
		{
			if(eboard.CanMove(a[i],b[j]))
			{
				Board _temp;
				_temp = temp;
				_temp.MakeMove(a[i],b[j]);
				if(!alrdyuse(_temp)) // checks to see if board was alrdy made
				openlist.push(_temp);
			}
		}
	}

		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(temp); // inserts the board you just checked
		temp = openlist.top();
		openlist.pop();
		temp.Display();
	}
}



/*void ListBoard::GenBoard()
{
	char a[9] = {'X','A','B','C','D','E','F','G'};
	char b[5] = {'U','D','L','R'}

	for(int i = 0; i < eboard.getnumbot(); i++) //get num bots for pos moves
	{
		for(int j = 0; j < 4; j++) // makes move with bots
		{
			if(eboard.CanMove(a[i],b[j]))
			{
				Board temp(eboard);
				temp.MakeMove(a[i],b[j]);
				if(!alrdyuse(temp)) // checks to see if board was alrdy made
				openlist<Board>.push(temp);
			}
		}
	}
}*/


/*void ListBoard::breadth()
{
	cout << "You got here" << endl;
	queue<Node *> openlist;
	Board temp = eboard;
	Node * ntemp = NULL;
	while(true)
	{
		if(temp.Solved())
		{
			temp.Display();
			cout << "You won the Game" << "\nit took you " << eboard.GameLength() << " to win" << endl;;
			return;
		}
		else
		{
//		cout << "You got here2" << endl;
		char a[9] = {'X','A','B','C','D','E','F','G'};
		char b[5] = {'U','D','L','R'};

		for(int i = 0; i < temp.getnumbot(); i++) //get num bots for pos moves
		{
			for(int j = 0; j < 4; j++) // makes move with bots
			{
				if(temp.CanMove(a[i],b[j]))
				{
					Board _temp(temp);
					_temp.MakeMove(a[i],b[j]);
					if(!alrdyuse(_temp)) // checks to see if board was alrdy made
					{
				//	cout << "You got here3 " << endl;
						insertb(_temp);
						openlist.push(pathlist);
					}
				}
			}
		}

		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(temp); // inserts the board you just checked
		ntemp = openlist.front();
		openlist.pop();
		temp = ntemp->GetnBoard();
		temp.Display();
		insertboard(temp);
	}
}

void ListBoard::depthf()
{
	stack<Node *> openlist;
	Board temp = eboard;
	Node * ntemp = NULL;
	while(true)
	{
		if(temp.Solved())
		{
			temp.Display();
			cout << "You won the Game" << "\nit took you " << eboard.GameLength() << " to win" << endl;;
			return;
		}
		else
		{
//		cout << "You got here2" << endl;
		char a[9] = {'X','A','B','C','D','E','F','G'};
		char b[5] = {'U','D','L','R'};

		for(int i = 0; i < temp.getnumbot(); i++) //get num bots for pos moves
		{
			for(int j = 0; j < 4; j++) // makes move with bots
			{
				if(temp.CanMove(a[i],b[j]))
				{
					Board _temp(temp);
					_temp.MakeMove(a[i],b[j]);
					if(!alrdyuse(_temp)) // checks to see if board was alrdy made
					{
//					cout << "You got here3 " << endl;
						insertb(_temp);
						openlist.push(pathlist);
					}
				}
			}
		}

		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(temp); // inserts the board you just checked
		ntemp = openlist.top();
		openlist.pop();
		temp = ntemp->GetnBoard();
		temp.Display();
		insertboard(temp);
	}
}*/

/*void ListBoard::bestf()
{
	queue<Node *> openlist;
	Board temp = eboard;
	Node * ntemp = NULL;
	int i = 0;
	int y = 0;
	while(true)
	{
		if(temp.Solved())
		{
			temp.Display();
			cout << "You won the Game" << "\nit took you " << eboard.GameLength() << " to win" << endl;;
			return;
		}
		else
		{
//		cout << "You got here2" << endl;
		char a[9] = {'X','A','B','C','D','E','F','G'};
		char b[5] = {'U','D','L','R'};
		++i;++y;
		for(int i = 0; i < temp.getnumbot(); i++) //get num bots for pos moves
		{
			for(int j = 0; j < 4; j++) // makes move with bots
			{
//			cout << "bot num: " << temp.getnumbot() << " " << y <<endl;
				if(temp.CanMove(a[i],b[j]))
				{
					Board _temp(temp);
					_temp.MakeMove(a[i],b[j]);
					if(!alrdyuse(_temp)) // checks to see if board was alrdy made
					{
				//	cout << "You got here3 " << endl;
						insertc(_temp, heuristic(_temp)+heuristic(temp),y); // need to fix level and heur counter
						openlist.push(pathlist);
						
					}
				}
			}
		}
		}
		if(openlist.empty())
		{
			cout << "You can not Win" << endl;
			return;
		}
		insert(temp); // inserts the board you just checked
		ntemp = openlist.front();
		openlist.pop();
		temp = ntemp->GetnBoard();
//		temp.Display();
		cout << ntemp->Getheur() << " " << ntemp->Getlevel() << endl;
		insertboard(temp);
	}
}*/