//BENJAMIN BUCKEY PROGRAM 1 AI



#include "board.h"
#include <string>

int main(int argc, char* argv[])
{
	char fname[40];
//	char robot, dir;
	Board lunlock;
	ifstream instr;
	cout << "Please enter name of file for reading: ";
	cin >> fname;
	instr.open(fname);
	if (!instr.is_open())
		{
			cout << "Open failed\n";
			return 0;
		}
	lunlock.Init(instr); // reads board from file
	instr.close();
	lunlock.Display();
	ListBoard solving;
	solving.insertboard(lunlock);
/*	while (!lunlock.Solved())
		{
			cout << "Enter robot to move (Q to quit): ";
			cin >> robot;
			if (robot == 'Q')
				{
					cout << "Bye!\n";
					return 0;
				}
			cout << "Enter direction: ";
			cin >> dir;
			if (!lunlock.CanMove(robot,dir)) // if robot can't move that way
				cout << "Sorry, that doesn't work\n";
			else
				lunlock.MakeMove(robot,dir);
			lunlock.Display();
		}*/
//	cout << "You won! Congratulations!\n";
//	solving.breadth();
//	solving.depthf();
	solving.bestf();
//	cout << "You took " << lunlock.GameLength() << " moves.\n";
//	solving.print();
	return 1;
}