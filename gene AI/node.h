#include <iostream>
using namespace std;


class node
{
private:
	int a;
	int b;
	int c;
	int d;
	int e;
	int f;
	int g;
	int h;
	int proba;
	int probb;
	int probc;
	int probd;
	int probe;
	int probf;
	int probg;
	int probh;

public:
	node(){a=b=c=d=e=f=g=h=proba=probb=probc=probd=probe=probf=probg=probh=0;}
	int geta() {return a;}
	int getb() {return b;}
	int getd() {return d;}
	int gete() {return e;}
	int getf() {return f;}
	int getg() {return g;}
	int geth() {return h;}
	int getj() {return c;}
	int getprob(){return (proba+probb+probc+probd+probe+probf+probg+probh);}
	void seta(int _a){a = _a;}
	void setb(int _b){b = _b;}
	void setc(int _c){c = _c;}
	void setd(int _d){d = _d;}
	void sete(int _e){e = _e;}
	void setf(int _f){f = _f;}
	void setg(int _g){g = _g;}
	void seth(int _h){h = _h;}
	void setproba(int _a){proba = _a;}
	void setprobb(int _b){probb = _b;}
	void setprobc(int _c){probc = _c;}
	void setprobd(int _d){probd = _d;}
	void setprobe(int _e){probe = _e;}
	void setprobf(int _f){probf = _f;}
	void setprobg(int _g){probg = _g;}
	void setprobh(int _h){probh = _h;}
	int getproba(){return proba;}
	int getprobb(){return probb;}
	int getprobc(){return probc;}
	int getprobd(){return probd;}
	int getprobe(){return probe;}
	int getprobf(){return probf;}
	int getprobg(){return probg;}
	int getprobh(){return probh;}

};
