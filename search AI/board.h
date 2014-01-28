#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdio.h>
using namespace std;

#define SIZE 5


const CAPACITY = 10000;


class Spot {
	private:
		int row,col;
	public:
		Spot(int r=-1, int c=-1) : row(r),col(c) {}
		void SetRow(int r) { row = r; }
		void SetCol(int c) { col = c; }
		int GetRow() const { return row; }
		int GetCol() const { return col; }
		bool BadSpot() const { return (row<0 || row>=SIZE || col<0 || col>=SIZE); }
		void UpOne() { row--; }
		void DownOne() { row++; }
		void LeftOne() { col--; }
		void RightOne() { col++; }
		bool operator!=(Spot rhs)
			{return ((row != rhs.row) || (col != rhs.col));}
};

//#include "spot.h"

class Board {
	public:
		Board(); // constructor
		Board(Board &b); // copy constructor
		void SetSpot(Spot s, char robot);
			// place robot at coordinates
		int getnumbot() {return numBots;}
		void Init(ifstream  &str);
			// set up board from file
		bool CanMove(char robot, char dir) const;
			// can robot go in specified direction
		void MakeMove(char robot, char dir);
		bool Solved() const {return 
			(grid[(SIZE-1)/2][(SIZE-1)/2] == 'X'); }
		bool checkmid(){return (grid[(SIZE-1)/2][(SIZE-1)/2] != 'A') || (grid[(SIZE-1)/2][(SIZE-1)/2] != 'B') || (grid[(SIZE-1)/2][(SIZE-1)/2] != 'C') || (grid[(SIZE-1)/2][(SIZE-1)/2] != 'D') || (grid[(SIZE-1)/2][(SIZE-1)/2] != 'E') || (grid[(SIZE-1)/2][(SIZE-1)/2] != 'F');}
			// is the pod at the station?
		void Display() const; // print to screen
		int GameLength() { return movesMade; }
		bool operator==(const Board& otherBoard);
		bool gettaken(Spot s)
		{
			return Taken(s);
		}
	private:
		int numBots;
		int movesMade;
		char grid[SIZE][SIZE];
		bool Taken(Spot s) const
			{ return (s.BadSpot() || 
				grid[s.GetRow()][s.GetCol()] != '_'); }
		Spot GetSpot(char robot) const;
	};

class Node
{
private:

	Board nboard; // for parent node
	Node * nextboard;
	char bot;
	char move;

public:

	Node(){nextboard = NULL;}
	void SetnBoard(Board _nboard){ nboard = _nboard;}
	Board GetnBoard() {return nboard;}
	Node * GetBoardL() {return nextboard;}
	void SetBoardL(Node * _nboard) { nextboard = _nboard;}
	void Setbot(char a){bot = a;}
	char Getbot(){return bot;}
	void Setmove(char a){move = a;}
	char Getmove(){return move;}

};

class Node2
{
private:
	Board data;
	int heur;
	Node * parent; // holds parent nodes
public:
	Node2(){heur = 0;parent = NULL;}
	void SetnBoard(Board _nboard){ data = _nboard;}
	void Setheur(int h){heur = h;}
	int Getheur(){return heur;}
	Board GetnBoard() {return data;}
	void insertp(Board _nboard,char bot, char move)//list of parents
	{ // inserts a evaluated board
		Node * temp = new Node();
		temp->SetnBoard(_nboard);
		temp->Setbot(bot);
		temp->Setmove(move);
		temp->SetBoardL(parent);
		parent = temp;
	}

	void printmove()
	{
		for(Node * temp = parent; temp != NULL; temp = temp->GetBoardL())
		{
			cout << temp->Getbot() << " " << temp->Getmove() << endl;
		//	Board tem;
		//	tem = temp->GetnBoard();
		//	tem.Display();
		}
	}
};


class Stack // STACK IMP
{
private:
	Node2 data[CAPACITY];
	int used;
public:
	Stack(){used = 0;}
	void push(Node2 x)
	{
		data[used]=x;
		++used;
	}
	void pop()
	{
		if(used > 0)
			--used;
		else cout << "stack is empty" << endl;
	}

	bool empty() {return(used == 0);}
	Node2 top()
	{
		return data[used-1];
	}
};

class Queue // QUEUE IMP
{
private:
	Node2 data[CAPACITY];
	int first;
	int last;
	int count;
	int next_index(int i) {return (i+1)%CAPACITY;}
public:
	Queue()
	{first = last = count = 0;}
	void pop()
	{
		if(count > 0)
		{first = next_index(first); --count;}
		else
			cout << "queue is empty" << endl;
	}
	Node2 front()
	{
		return data[first];
	}
	void push(Node2 x)
	{
		data[last]=x;
		last = next_index(last);
		++count;
	}
	bool empty(){return(count == 0);}
	
	void sortq() // Sorting from lowest to highest
	{
		int i,j,indexl,largest;
		if(count == 0)
		{cout << "no data in Queue to sort" << endl;return;}

		for(i = last-1; i > first; --i)
		{
			Node2 temp;
			temp = data[0];
			largest = temp.Getheur();
			indexl = 0;
			for(j = first; j < (last); ++j)
			{
				temp = data[j];
				if(temp.Getheur() >= largest)
				{
					largest = temp.Getheur();
					indexl = j;
				}
			}
			Node2 _temp;
			Node2 tem;
			_temp = data[i];
			tem = data[indexl];
			data[i] = tem;
			data[indexl] = _temp;
	//		cout << i << "gets " << tem.Getheur() << " " << indexl << " gets " <<  _temp.Getheur() <<  endl;
		}
	}

};

/*class Stack
{
private:
	Board data[CAPACITY];
	int used;
public:
	Stack(){used = 0;}
	void push(Board x)
	{
		data[used]=x;
		++used;
	}
	void pop()
	{
		if(used > 0)
			--used;
		else cout << "stack is empty" << endl;
	}

	bool empty() {return(used == 0);}
	Board top()
	{
		return data[used-1];
	}
};

class Queue
{
private:
	Board data[CAPACITY];
	int first;
	int last;
	int count;
	int next_index(int i) {return (i+1)%CAPACITY;}
public:
	Queue()
	{first = last = count = 0;}
	void pop()
	{
		if(count > 0)
		{++first; --count;}
		else
			cout << "queue is empty" << endl;
	}
	Board front()
	{
		return data[first];
	}
	void push(Board x)
	{
		data[last]=x;
		++last;
		++count;
	}
	bool empty(){return(count == 0);}
};*/


class ListBoard
{
private:
//	queue<Board> openlist;
	Board eboard;
	Node2 board;
	Node * closelist;//for evaluated boards
//	Node * pathlist;
public:
	ListBoard(){closelist = NULL;}
//	void GenBoard();
	void breadth(); // for BOARD QUEUE
	void depthf(); // FOR BOARD STACK
	void bestf();  // for NODE2 QUEUE and heuristic A*
	void insert(Board _nboard)
	{ // inserts a evaluated board
		Node * temp = new Node();
		temp->SetnBoard(_nboard);
		temp->SetBoardL(closelist);
		closelist = temp;
	}
	
/*	void insertb(Board _nboard)
	{ // inserts a evaluated board
		Node * temp = new Node();
		temp->SetnBoard(_nboard);
		temp->SetBoardL(closelist);
		pathlist = temp;
	}*/
	bool alrdyuse(Board _temp)
	{//checks to see if a board has alrdy been evaluated
		for(Node * temp = closelist; temp != NULL; temp= temp->GetBoardL())
		{
			if(_temp == temp->GetnBoard())
				return true;
		}
		return false;
	}

	void insertboard(Board _temp) {eboard = _temp;}

	int heuristic(Board temp)
	{ // This checks bot locations giveing thier locations a value
		Spot b; // estimate path cost
		int x =0;
		for (int i=0; i<SIZE; i++)
		{
			b.SetCol(i);
			for (int j=0; j<SIZE; j++)
			{
				b.SetRow(j);
				if( i == 0 && j == 4 || j == 0 && i == 4 || i == 0 && j == 0 || i == 4 && j == 4) 
				{	
					if(temp.gettaken(b))
						x = x+3;
				}
				else if(i == 0 && j == 1 || i == 1 && j == 0 || i ==1 && j == 1 || i == 3 && j == 0 || i == 4 && j == 1 || i == 0 && j == 3 || i == 1 && j == 3 || i == 1 && j == 4 || i == 3 && j == 3 || i == 3 && j == 4 || i == 4 && j == 3)
				{
					if(temp.gettaken(b))
					x = x +2;
				}
				else if(temp.gettaken(b))
					x = x+1;
			}
		}
		return x;
	}

	void print()
	{
		for(Node * temp = closelist; temp != NULL; temp = temp->GetBoardL())
		{
			Board tem;
			tem = temp->GetnBoard();
			tem.Display();
		}
	}

};