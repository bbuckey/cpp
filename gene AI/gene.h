#include "node.h"

const int SIZE = 20;
const int critdepth = 40;
int p = 0;

class gene
{
private:
	node population[SIZE];

public:
	gene(){;}

//	void setpop()
//	{for(int i = 0; i < SIZE; i++)population[i] = NULL;}

	node Crossover(node temp,int j)
	{
		int i = rand()%4;
		node _temp;
		_temp = population[i];
//		while(/*_temp.getprob() == temp.getprob()*/ /*_temp.getprob() < 49*/)
	/*	{
			i = rand()%SIZE;
			_temp = population[i];
		}*/
		while(i == j)
		{
			i = rand()%4;
			_temp = population[i];
		}
		
//		temp.seta(_temp.geta());
//		temp.setb(_temp.getb());
//		temp.setc(_temp.getj());
//		temp.setd(_temp.getd());
//		temp.sete(_temp.gete());
//		temp.setf(_temp.getf());
//		temp.setg(_temp.getg());
//		temp.seth(_temp.geth());
			temp.setproba(7);
			temp.setprobb(7);
			temp.setprobc(7);
			temp.setprobd(7);
			temp.setprobe(7);
			temp.setprobf(7);
			temp.setprobg(7);
			temp.setprobh(7);
		return temp;

	}
	node Mutation(node temp)
	{

			if(temp.getproba() < 6)
				temp.seta((rand()%8)+1);
			else if(temp.getprobb() < 6)
				temp.setb((rand()%8)+1);
		   	else if(temp.getprobc() < 6)
				temp.setc((rand()%8)+1);
			else if(temp.getprobd() < 6)
				temp.setd((rand()%8)+1);
			else if(temp.getprobe() < 6)
				temp.sete((rand()%8)+1);
			else if(temp.getprobf() < 6)
				temp.setf((rand()%8)+1);
		    else if(temp.getprobg() < 6)
				temp.setg((rand()%8)+1);
			else if(temp.getprobh() < 6)
				temp.seth((rand()%8)+1);

			temp.setproba(7);
			temp.setprobb(7);
			temp.setprobc(7);
			temp.setprobd(7);
			temp.setprobe(7);
			temp.setprobf(7);
			temp.setprobg(7);
			temp.setprobh(7);
			return temp;

	}

	void checkfitness()
	{
		for(int i = 0; i < SIZE; ++i)
		{
		node temp = population[i];
//		cout << temp.getproba();
		if(temp.geta() == temp.getb()+1 || temp.geta() == temp.getb()-1 || temp.geta() == temp.getb())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT1" << endl;*/}
		if(temp.geta() == temp.getj()+2 || temp.geta() == temp.getj()-2 || temp.geta() == temp.getj())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT2" << endl;*/}
		if(temp.geta() == temp.getd()+3 || temp.geta() == temp.getd()-3 || temp.geta() == temp.getd())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT3" << endl;*/}
		if(temp.geta() == temp.gete()+4 || temp.geta() == temp.gete()-4 || temp.geta() == temp.gete())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT4" << endl;*/}
		if(temp.geta() == temp.getf()+5 || temp.geta() == temp.getf()-5 || temp.geta() == temp.getf())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT5" << endl;*/}
		if(temp.geta() == temp.getg()+6 || temp.geta() == temp.getg()-6 || temp.geta() == temp.getg())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT6" << endl;*/}
		if(temp.geta() == temp.geth()+7 || temp.geta() == temp.geth()-7 || temp.geta() == temp.geth())
		{temp.setproba(temp.getproba()-1); /*cout << "HIT7" << endl;*/}
	
//		cout << temp.getproba();
	
		if(temp.getb() == temp.geta()+1 || temp.getb() == temp.geta()-1 || temp.getb() == temp.geta())
		{temp.setprobb(temp.getprobb()-1); /*cout << "HITME1";*/}
		if(temp.getb() == temp.getj()+1 || temp.getb() == temp.getj()-1 || temp.getb() == temp.getj())
		{	temp.setprobb(temp.getprobb()-1); /*cout << "HITME2";*/}
		if(temp.getb() == temp.getd()+2 || temp.getb() == temp.getd()-2 || temp.getb() == temp.getd())
		{temp.setprobb(temp.getprobb()-1);/*cout << "HITME3";*/}
		if(temp.getb() == temp.gete()+3 || temp.getb() == temp.gete()-3 || temp.getb() == temp.gete())
		{temp.setprobb(temp.getprobb()-1);/*cout << "HITME4";*/}
		if(temp.getb() == temp.getf()+4 || temp.getb() == temp.getf()-4 || temp.getb() == temp.getf())
		{	temp.setprobb(temp.getprobb()-1);/*cout << "HITME5";*/}
		if(temp.getb() == temp.getg()+5 || temp.getb() == temp.getg()-5 || temp.getb() == temp.getg())
		{	temp.setprobb(temp.getprobb()-1);/*cout << "HITME6";*/}
		if(temp.getb() == temp.geth()+6 || temp.getb() == temp.geth()-6 || temp.getb() == temp.geth())
		{	temp.setprobb(temp.getprobb()-1);/*cout << "HITME7";*/}
		
	//	cout << temp.getprobb();

		if(temp.getj() == temp.getb()+1 || temp.getj() == temp.getb()-1 || temp.getj() == temp.getb())
		{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.geta()+2 || temp.getj() == temp.geta()-2 || temp.getj() == temp.geta())
				{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.getd()+1 || temp.getj() == temp.getd()-1 || temp.getj() == temp.getd())
				{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.gete()+2 || temp.getj() == temp.gete()-2 || temp.getj() == temp.gete())
				{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.getf()+3 || temp.getj() == temp.getf()-3 || temp.getj() == temp.getf())
				{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.getg()+4 || temp.getj() == temp.getg()-4 || temp.getj() == temp.getg())
					{temp.setprobc(temp.getprobc()-1);}
		if(temp.getj() == temp.geth()+5 || temp.getj() == temp.geth()-5 || temp.getj() == temp.geth())
				{temp.setprobc(temp.getprobc()-1);}

		//		cout << temp.getprobc();
		if(temp.getd() == temp.getb()+2 || temp.getd() == temp.getb()-2 || temp.getd() == temp.getb())
		{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.getj()+1 || temp.getd() == temp.getj()-1 || temp.getd() == temp.getj())
		{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.geta()+3 || temp.getd() == temp.geta()-3 || temp.getd() == temp.geta())
				{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.gete()+1 || temp.getd() == temp.gete()-1 || temp.getd() == temp.gete())
				{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.getf()+2 || temp.getd() == temp.getf()-2 || temp.getd() == temp.getf())
				{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.getg()+3 || temp.getd() == temp.getg()-3 || temp.getd() == temp.getg())
				{temp.setprobd(temp.getprobd()-1);}
		if(temp.getd() == temp.geth()+4 || temp.getd() == temp.geth()-4 || temp.getd() == temp.geth())
		{temp.setprobd(temp.getprobd()-1);}

	//			cout << temp.getprobd();
		if(temp.gete() == temp.getb()+3 || temp.gete() == temp.getb()-3 || temp.gete() == temp.getb())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.getj()+2 || temp.gete() == temp.getj()-2 || temp.gete() == temp.getj())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.getd()+1 || temp.gete() == temp.getd()-1 || temp.gete() == temp.getd())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.geta()+4 || temp.gete() == temp.geta()-4 || temp.gete() == temp.geta())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.getf()+1 || temp.gete() == temp.getf()-1 || temp.gete() == temp.getf())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.getg()+2 || temp.gete() == temp.getg()-2 || temp.gete() == temp.getg())
		{temp.setprobe(temp.getprobe()-1);}
		if(temp.gete() == temp.geth()+3 || temp.gete() == temp.geth()-3 || temp.gete() == temp.geth())
		{temp.setprobe(temp.getprobe()-1);}

//		cout << temp.getprobe();
		if(temp.getf() == temp.getb()+4 || temp.getf() == temp.getb()-4 || temp.getf() == temp.getb())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.getj()+3 || temp.getf() == temp.getj()-3 || temp.getf() == temp.getj())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.getd()+2 || temp.getf() == temp.getd()-2 || temp.getf() == temp.getd())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.gete()+1 || temp.getf() == temp.gete()-1 || temp.getf() == temp.gete())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.geta()+5 || temp.getf() == temp.geta()-5 || temp.getf() == temp.geta())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.getg()+1 || temp.getf() == temp.getg()-1 || temp.getf() == temp.getg())
		{temp.setprobf(temp.getprobf()-1);}
		if(temp.getf() == temp.geth()+2 || temp.getf() == temp.geth()-2 || temp.getf() == temp.geth())
		{temp.setprobf(temp.getprobf()-1);}

	//		cout << temp.getprobf();
		if(temp.getg() == temp.getb()+5 || temp.getg() == temp.getb()-5 || temp.getg() == temp.getb())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.getj()+4 || temp.getg() == temp.getj()-4 || temp.getg() == temp.getj())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.getd()+3 || temp.getg() == temp.getd()-3 || temp.getg() == temp.getd())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.gete()+2 || temp.getg() == temp.gete()-2 || temp.getg() == temp.gete())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.getf()+1 || temp.getg() == temp.getf()-1 || temp.getg() == temp.getf())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.geta()+6 || temp.getg() == temp.geta()-6 || temp.getg() == temp.geta())
		{temp.setprobg(temp.getprobg()-1);}
		if(temp.getg() == temp.geth()+1 || temp.getg() == temp.geth()-1 || temp.getg() == temp.geth())
		{temp.setprobg(temp.getprobg()-1);}

//				cout << temp.getprobg();
		if(temp.geth() == temp.getb()+6 || temp.geth() == temp.getb()-6 || temp.geth() == temp.getb())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.getj()+5 || temp.geth() == temp.getj()-5 || temp.geth() == temp.getj())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.getd()+4 || temp.geth() == temp.getd()-4 || temp.geth() == temp.getd())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.gete()+3 || temp.geth() == temp.gete()-3 || temp.geth() == temp.gete())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.getf()+2 || temp.geth() == temp.getf()-2 || temp.geth() == temp.getf())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.getg()+1 || temp.geth() == temp.getg()-1 || temp.geth() == temp.getg())
		{temp.setprobh(temp.getprobh()-1);}
		if(temp.geth() == temp.geta()+7 || temp.geth() == temp.geta()-7 || temp.geth() == temp.geta())
		{temp.setprobh(temp.getprobh()-1);}

//		cout << temp.getprobh();

		population[i] = temp;
		}
	}

	bool checkProb(node check)
	{
		if(check.getprob() == 56) 
			return true;
		else
			return false;
	}
	void genpopulation()
	{
		for(int i = 0; i < SIZE; i++)
		{
			int j = 0;
			node temp;
			temp.seta((rand()%8)+1);
			j = ((rand()%8)+1);
//			while(j == temp.geta())
			{j = ((rand()%8)+1);}
			temp.setb(j);
//			while(j == temp.geta() || j == temp.getb())
			{j = ((rand()%8)+1);}
			temp.setc(j);
//			while(j == temp.geta() || j == temp.getb() || j == temp.getj())
			{j = ((rand()%8)+1);}
			temp.setd(j);
//			while(j == temp.geta() || j == temp.getb() || j == temp.getj() || j == temp.getd())
			{j = ((rand()%8)+1);}
			temp.sete(j);
//			while(j == temp.geta() || j == temp.getb() || j == temp.getj() || j == temp.getd() || j == temp.gete())
			{j = ((rand()%8)+1);}
			temp.setf(j);
//			while( j = temp.getf() || j == temp.geta() || j == temp.getb() || j == temp.getj() || j == temp.getd() || j == temp.gete())
			{j = ((rand()%8)+1);}
			temp.setg(j);
//			while(j == temp.getg() || j == temp.getf() ||j == temp.geta() || j == temp.getb() || j == temp.getj() || j == temp.getd() || j == temp.gete())
			{j = ((rand()%8)+1);}
			temp.seth(j);
			temp.setproba(7);
			temp.setprobb(7);
			temp.setprobc(7);
			temp.setprobd(7);
			temp.setprobe(7);
			temp.setprobf(7);
			temp.setprobg(7);
			temp.setprobh(7);
			population[i]=temp;
		}
	}

	node findbest()
	{
		node best;
		node temp;
		node _temp;
		best = population[0];
		for(int i = 0; i < SIZE; ++i)
		{
			temp = population[i];
			if(i+1 == SIZE) break;
			_temp = population[i+1];
			if(temp.getprob() > _temp.getprob())
			{
				if(temp.getprob() >= best.getprob())
					best = temp;
			}
			else
			{
				if(_temp.getprob() >= best.getprob())
					best = _temp;
			}
		}
		return best;
	}

	void makeithappen()
	{
		int i = 0;
		node newpop[SIZE];
		node temp;
	
		checkfitness();
		sortpop();
		for(i = 0; i < SIZE; ++i)
		{
			temp = population[i];
			if(checkProb(temp))
			{
				cout << "The solution is: " << temp.geta() << " " << temp.getb() << " " << temp.getj() << " " << temp.getd() << " ";
				cout << temp.gete() << " " << temp.getf() << " " << temp.getg() << " " << temp.geth() << "    " << temp.getprob() << "   " << p << endl;
				return;
			}
//			cout << temp.getprob() << endl;

		}

		sortpop();
		
		for(i = 0; i < SIZE; ++i)
		{
			temp = population[i];
			if(temp.getprob() <= 50)
			newpop[i] = Mutation(temp);
			else
			newpop[i] = Crossover(temp,i);
		}
		
		for(i = 0; i < SIZE; i++)
		{
			population[i] = newpop[i];
		}
	
//		cout << "Hit depth " << p << endl;

		if(critdepth == p)
		{
			cout << "Hit MAX depth returning the population :" << endl;
			print();
			return;
		}
		++p;
		makeithappen();

	}
	
	void print()
	{
		node temp;
			checkfitness();
		for( int i = 0; i < SIZE; ++i)
		{
			temp = population[i];
			cout << temp.geta() << " " << temp.getb() << " " << temp.getj() << " " << temp.getd() << " ";
			cout << temp.gete() << " " << temp.getf() << " " << temp.getg() << " " << temp.geth() << endl;
			cout << "THE PROB IS: " << temp.getproba() << " " << temp.getprobb() << " " << temp.getprobc() << " " << temp.getprobd() << " " << temp.getprobe() << " " << temp.getprobf() <<" " << temp.getprobg() << " " <<temp.getprobh() << "    " << temp.getprob() << endl;
		}
	}

	void sortpop() // Sorting from highest to lowest
	{
		int i,j,indexl,smallest;
		for(i = SIZE-1; i >= 0; --i)
		{
			node temp;
			temp = population[0];
			smallest = temp.getprob();
			indexl = 0;
			for(j = 0; j < SIZE; ++j)
			{
				temp = population[j];
				if(temp.getprob() >= smallest)
				{
					smallest = temp.getprob();
					indexl = j;
				}
			}
				node _temp;
				node tem;
				_temp = population[indexl];
				tem = population[i];
				population[indexl] = tem;
				population[i] = _temp;

		}
	}

};


/*		int j = (rand()%8)+1;
			if(j == 1)
				temp.seta((rand()%8)+1);
			if(j == 2)
				temp.setb((rand()%8)+1);
			if(j == 3)
				temp.setc((rand()%8)+1);
			if(j == 4)
				temp.setd((rand()%8)+1);
			if(j == 5)
				temp.sete((rand()%8)+1);
			if(j == 6)
				temp.setf((rand()%8)+1);
			if(j == 7)
				temp.setg((rand()%8)+1);
			if(j == 8)
				temp.seth((rand()%8)+1);*/