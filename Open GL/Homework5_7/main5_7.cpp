#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <windows.h>
#include <assert.h>
#include <math.h>
#include <GL\GL.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>
using namespace std;
typedef unsigned char uchar;
typedef unsigned int texten;

class mRGB{   // the name RGB is already used by Windows
public: uchar r,g,b;//a;
		mRGB(){r = g = b = 0; } // a = 1 or 255 depending on how you set it;
		mRGB(mRGB& p){r = p.r; g = p.g; b = p.b;} // a = p.a;
		mRGB(uchar rr, uchar gg, uchar bb){r = rr; g = gg; b = bb;} //a = aa; 
			void set(uchar rr, uchar gg, uchar bb){r = rr; g = gg; b = bb;}// a = aa;
};
//$$$$$$$$$$$$$$$$$ RGBPixmap class $$$$$$$$$$$$$$$
class RGBpixmap{
private: 
	mRGB* pixel; // array of pixels

public:
	int nRows, nCols; // dimensions of the pixmap
	float nnear; 
	float ffar; 
	float xmax; 
	float ymax; 
	float xmin; 
	float ymin;
	int dis;
	RGBpixmap() {dis = nRows = nCols = 0; pixel = 0;nnear=ffar=xmax=ymax=xmin=ymin=0;}
	RGBpixmap(int rows, int cols) //constructor
	{
		nRows = rows;
		nCols = cols;
		pixel = new mRGB[rows*cols]; 
	}
	
	void setprojcoor(float n, float f, float xma, float xmi, float yma, float ymi, int _dis)
	{
		nnear = n; ffar = f; xmax = xma; xmin = xmi; ymax = yma; ymin = ymi; dis = _dis;
	}
	int readBMPFile2(string fname); // read BMP file into this pixmap

	void freeIt() // give back memory for this pixmap
	{
		delete []pixel; nRows = nCols = 0;
	}
	//<<<<<<<<<<<<<<<<<< copy >>>>>>>>>>>>>>>>>>>
/*	void copy(IntPoint from, IntPoint to, int x, int y, int width, int height)
	{ // copy a region of the display back onto the display
		if(nRows == 0 || nCols == 0) return;
		glCopyPixels(x, y, width, height,GL_COLOR);
	}*/
	//<<<<<<<<<<<<<<<<<<< draw >>>>>>>>>>>>>>>>>
	void draw()
	{ // draw this pixmap at current raster position
		if(nRows == 0 || nCols == 0) return;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glDrawPixels(nCols, nRows,GL_RGB, GL_UNSIGNED_BYTE,pixel);
	}
	//<<<<<<<<<<<<<<<<< read >>>>>>>>>>>>>>>>
	int read(int x, int y, int wid, int ht)
	{ // read a rectangle of pixels into this pixmap
		nRows = ht;
		nCols = wid;
		pixel = new mRGB[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(x, y, nCols, nRows, GL_RGB,GL_UNSIGNED_BYTE,pixel);
		return 0;
	}
	//<<<<<<<<<<<<<<<<< read from IntRect >>>>>>>>>>>>>>>>
/*	int read(IntRect r)
	{ // read a rectangle of pixels into this pixmap
		nRows = r.top - r.bott;
		nCols = r.right - r.left;
		pixel = new mRGB[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(r.left,r.bott, nCols, nRows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		return 0;
	}*/
	//<<<<<<<<<<<<<< setPixel >>>>>>>>>>>>>
	void setPixel(int x, int y, mRGB color)
	{
		if(x>=0 && x <nCols && y >=0 && y < nRows)
			pixel[nCols * y + x] = color;
	}
	//<<<<<<<<<<<<<<<< getPixel >>>>>>>>>>>
	mRGB getPixel(int x, int y)
	{ 		
		mRGB bad(255,255,255);
		assert(x >= 0 && x < nCols);
		assert(y >= 0 && y < nRows);
		return pixel[nCols * y + x];
	}
	void blend()
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_BLEND);
	}
	void setupProjection()
	{
	//	glGentextures(1,i);
		float eyePlaneS[] = {1.0, 0.0, 0.0, 0.0};
		float eyePlaneT[] = {0.0, 1.0, 0.0, 0.0};
		float eyePlaneR[] = {0.0, 0.0, 1.0, 0.0};
		float eyePlaneQ[] = {0.0, 0.0, 0.0, 1.0};
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);
	}
	
	void setTexture(int i)
	{
		glBindTexture(GL_TEXTURE_2D, i);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, nCols, nRows,0, GL_RGBA,GL_UNSIGNED_BYTE,pixel);
	}
/*	glBindTexture(GL_TEXTURE_2D, texture[2]); mip map
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); */
void loadTextureProjection()
{
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5, 0.5, 0.5);
  glScalef(0.5, 0.5, 1.0);
  glFrustum(xmin, xmax, ymin, ymax, nnear, ffar);
  glTranslatef(0.0, 0.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void drawprojection(void)
{
  float t = ffar / nnear;
  GLfloat n[4][3];
  GLfloat f[4][3];

  n[0][0] = xmin;
  n[0][1] = ymin;
  n[0][2] = -(nnear + dis);

  n[1][0] = xmax;
  n[1][1] = ymin;
  n[1][2] = -(nnear + dis);

  n[2][0] = xmax;
  n[2][1] = ymax;
  n[2][2] = -(nnear + dis);

  n[3][0] = xmin;
  n[3][1] = ymax;
  n[3][2] = -(nnear + dis);

  f[0][0] = xmin * t;
  f[0][1] = ymin * t;
  f[0][2] = -(ffar + dis);

  f[1][0] = xmax * t;
  f[1][1] = ymin * t;
  f[1][2] = -(ffar + dis);

  f[2][0] = xmax * t;
  f[2][1] = ymax * t;
  f[2][2] = -(ffar + dis);

  f[3][0] = xmin * t;
  f[3][1] = ymax * t;
  f[3][2] = -(ffar + dis);

  glColor4ub(1.0,3.0,.5,.5);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(n[0]);
  glVertex3fv(n[1]);
  glVertex3fv(n[2]);
  glVertex3fv(n[3]);
  glVertex3fv(f[3]);
  glVertex3fv(f[2]);
  glVertex3fv(f[1]);
  glVertex3fv(f[0]);
  glVertex3fv(n[0]);
  glVertex3fv(n[1]);
  glVertex3fv(f[1]);
  glVertex3fv(f[0]);
  glVertex3fv(f[3]);
  glVertex3fv(f[2]);
  glVertex3fv(n[2]);
  glVertex3fv(n[3]);
  glEnd();
}
};



typedef unsigned short ushort;
typedef unsigned long ulong;
fstream inf; 
//<<<<<<<<<<<<<<<<<<<<< getShort >>>>>>>>>>>>>>>>>>>>
ushort getShort() //helper function
{ //BMP format uses little-endian integer types
  // get a 2-byte integer stored in little-endian form
		char ic;
		ushort ip;
		inf.get(ic); ip = ic;  //first byte is little one 
		inf.get(ic);  ip |= ((ushort)ic << 8); // or in high order byte
		return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong() //helper function
{  //BMP format uses little-endian integer types
   // get a 4-byte integer stored in little-endian form
		ulong ip = 0;
		char ic = 0;
		unsigned char uc = ic;
		inf.get(ic); uc = ic; ip = uc;
		inf.get(ic); uc = ic; ip |=((ulong)uc << 8);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 16);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 24);
		return ip;
	}
//<<<<<<<<<<<<<<<<<< RGBPixmap:: readBmpFile>>>>>>>>>>>>>
int RGBpixmap::readBMPFile2(string fname) 
{  // Read into memory an mRGB image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	inf.open(fname.c_str(), ios::in | ios::binary); //read binary char's
	if(!inf){ cout << " can't open file: " << fname << endl; return 0;}
	int k, row, col, numPadBytes, nBytesInRow;
	// read the file header information
	char ch1, ch2;
	inf.get(ch1); inf.get(ch2); //type: always 'BM'
	ulong fileSize =      getLong();
	ushort reserved1 =    getShort();    // always 0
	ushort reserved2= 	getShort();     // always 0 
	ulong offBits =		getLong(); // offset to image - unreliable
	ulong headerSize =     getLong();     // always 40
	ulong numCols =		getLong(); // number of columns in image
	ulong numRows = 		getLong(); // number of rows in image
	ushort planes=    	getShort();      // always 1 
	ushort bitsPerPixel=   getShort();    //8 or 24; allow 24 here
	ulong compression =    getLong();      // must be 0 for uncompressed 
	ulong imageSize = 	getLong();       // total bytes in image 
	ulong xPels =    	getLong();    // always 0 
	ulong yPels =    	getLong();    // always 0 
	ulong numLUTentries = getLong();    // 256 for 8 bit, otherwise 0 
	ulong impColors = 	getLong();       // always 0 
	if(bitsPerPixel != 24) 
	{ // error - must be a 24 bit uncompressed image
		cout << "not a 24 bit/pixelimage, or is compressed!\n";
		inf.close(); return 0;
	} 
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	nRows = numRows; // set class's data members
	nCols = numCols;
	pixel = new mRGB[nRows * nCols]; //make space for array
	if(!pixel) return 0; // out of memory!
	long count = 0;
	char dum;
	for(row = 0; row < nRows; row++) // read pixel values
	{
		for(col = 0; col < nCols; col++)
		{
			char r,g,b;
			inf.get(b); inf.get(g); inf.get(r); //read bytes
			pixel[count].r = r; //place them in colors
			pixel[count].g = g;
			pixel[count++].b = b;
		}
   		for(k = 0; k < numPadBytes ; k++) //skip pad bytes at row's end
			inf >> dum;
	}
	inf.close(); return 1; // success
}


class Point3
{
private:
	float x, y, z;
public:
	Point3(float _x, float _y, float _z){x = _x; y = _y; z = _z;}
	Point3(){x=y=z=0;}
	void set(Point3 a){x = a.getx(); y = a.gety(); z = a.getz();}
	void setxyz(float _x, float _y, float _z)
	{ x = _x; y = _y; z = _z;}
	void setx(float _x){x = _x;}
	void sety(float _y){y = _y;}
	void setz(float _z){z = _z;}
	float getx(){return x;}
	float gety(){return y;}
	float getz(){return z;}
	void build4tuple(float v[])
	{
		v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0f;
	}
};

class Vector3
{
private:
	float x,y,z;
public:
	Vector3(float _x, float _y, float _z){x = _x; y = _y; z = _z;}
	Vector3(){x = y = z = 0;}
	void set(float _x, float _y, float _z)
	{ x = _x; y = _y; z = _z;}
	void set(Vector3 a){x= a.getx(); y = a.gety(); z = a.getz();}
	void flip(){x = -x; y = -y; z = -z;}
	void setDiff(Point3 a, Point3 b)
	{ x = a.getx() - b.getx(); y = a.gety()-b.gety(); z = a.getz() - b.getz();}
	void normalize()
	{
		double sizeSq = x * x + y * y + z * z;
		if(sizeSq < 0.0000001)
		{
			cerr << "\nnormalize() sees vector(0,0,0)!";
			return;
		}
		float scaleFactor = 1.0/(float)sqrt(sizeSq);
		x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
	}
	float getx(){return x;}
	float gety(){return y;}
	float getz(){return z;}
	void setx(float _x){x = _x;}
	void sety(float _y){y = _y;}
	void setz(float _z){z = _z;}
	Vector3 cross(Vector3 b)
	{
		Vector3 c(y * b.getz() - z * b.gety(), z*b.getx() - x * b.gety(), x * b.gety() - y * b.getx());
			return c;
	}
	float dot(Vector3 b)
	{
		return x*b.getx()+y*b.gety()+z*b.getz();
	}
};

class Color3
{
private:
	float red, green, blue;
public:
	Color3() {red = green = blue = 0;}
	Color3(float r, float g, float b)
	{ red = r ; green = g; blue = b;}
	void set(float r, float g, float b)
	{ red = r; green = g; blue = b;}
	void setr(float r)
	{
		red = r;
	}
	void setg(float g)
	{
		green = g;
	}
	void setb(float b)
	{
		blue = b;
	}
	void set(Color3 a)
	{ red = a.getr(); green = a.getg(); blue = a.getb();}
	float getr(){return red;}
	float getg(){return green;}
	float getb(){return blue;}
	void add(float r, float g, float b)
	{red += r; green += g; blue +=b;}
	void add(Color3 a, Color3 b)
	{
		red += a.getr() * b.getr();
		green += a.getg() * b.getg();
		blue += a.getb() * b.getb();
	}
	void add(Color3 b)
	{ red += b.getr(); green += b.getg(); blue += b.getb();}
	void build4tuple(float v[])
	{
		v[0] = red; v[1] = green; v[2] = blue; v[3] = 1.0f;
	}
};

class Light
{
private:
	float angleC;
	float fxpo;
	float spot;
	Point3 pos;
	Color3 color;
	Light * next;
public:
	Light(){next = NULL; spot = 0;}
	void setPosition(Point3 p){pos.set(p);}
	void setColor(Color3 c){color.set(c);}
	void setspot(float spots,float ang,float _fxpo){spot = spots;angleC=ang;fxpo = _fxpo;}
	Light * gnext(){return next;}
	void setnext(Light * _next){next = _next;}
	Point3 getpos(){return pos;}
	Color3 getcolor(){return color;}
	float getspot(){return spot;}
	float getf(){return fxpo;}
	float getangleC(){return angleC;}
};

class Affine4
{
private:
	float m[16];
public:
	Affine4()
	{
		m[0]=m[5]=m[10]=m[15]=1.0;
		m[1]=m[2]=m[3]=m[4]=0.0;
		m[6]=m[7]=m[8]=m[9]=0.0;
		m[11]=m[12]=m[13]=m[14]=0.0;
	}
	void setIdentityMatrix()
	{
		m[0]=m[5]=m[10]=m[15]=1.0;
		m[1]=m[2]=m[3]=m[4]=0.0;
		m[6]=m[7]=m[8]=m[9]=0.0;
		m[11]=m[12]=m[13]=m[14]=0.0;
	}
	float * getmh(){return m;}
	float getm(int i){return m[i];}
	void setm(int i,float num){m[i] = num;}
	void set(Affine4 a)
	{
		for(int i = 0; i < 16; i++)
			m[i]=a.getm(i);
	}
	void postMult(Affine4 n)
	{
		float sum;
		Affine4 tmp;
		tmp.set(*this);
		for(int c = 0; c < 4; c++)
			for(int r = 0; r < 4; r++)
			{
				sum = 0.0;
				for(int k = 0; k < 4; k++)
					sum += tmp.getm(4*k+r) * n.getm(4*c+k);
				setm(4*c+r,sum);
			}
	}
	void preMult(Affine4 n)
	{
		float sum;
		Affine4 tmp;
		tmp.set(*this);
		for(int c = 0; c < 4; c++)
			for(int r = 0; r < 4; r++)
			{
				sum = 0.0;
				for(int k = 0; k < 4; k++)
					sum += n.getm(4*k+r) * tmp.getm(4*c+k);
				setm(4*c+r,sum);
			}
	}
};

class AffineNode
{
private:
	Affine4 * affn;
	Affine4 * invAffn;
	AffineNode * next;
public:
	AffineNode()
	{
		next = NULL;
		affn = new Affine4;
		invAffn = new Affine4;
	}
	~AffineNode()
	{
		delete affn;
		delete invAffn;
	}
	AffineNode * gnext(){return next;}
	void setnext(AffineNode * _next){next = _next;}
	void setaffn(Affine4 * _affn){ affn = _affn;}
	void setinvAffn(Affine4 * _invAffn){invAffn = _invAffn;}
	Affine4 * gaffn(){return affn;}
	Affine4 * ginvAffn(){return invAffn;}
	
};

class AffineStack
{
private:
	AffineNode * tos;
public:
	AffineStack()
	{
		tos = new AffineNode;
		tos->setnext(NULL);
	}
	AffineNode * gtos(){return tos;}
	void reset()
	{
		tos = new AffineNode;
		tos->setnext(NULL);
	}
	void dup()
	{
		AffineNode * tmp = new AffineNode;
		tmp->setaffn(new Affine4(*(tos->gaffn())));
		tmp->setinvAffn(new Affine4(*(tos->ginvAffn())));
		tmp->setnext(tos);
		tos = tmp;
	}
	void setIdentity()
	{
		Affine4 * tmp;
		assert(tos != NULL);
		tmp=tos->gaffn();
		tmp->setIdentityMatrix();
		tos->setaffn(tmp);
		tmp=tos->ginvAffn();
		tmp->setIdentityMatrix();
		tos->setinvAffn(tmp);
	}
	void popAndDrop()
	{
		if(tos == NULL) return;
		AffineNode * tmp;
		tmp = tos;
		tos = tos->gnext();
		delete tmp;
	}
	void releaseAffines()
	{
		while(tos) popAndDrop();
	}
	void rotate(float angle, Vector3 u)
	{
		Affine4 rm;
		Affine4 invRm;
		u.normalize();
		float ang = angle * 3.14159265/ 180;
		float c = cos(ang), s = sin(ang);
		float mc = 1.0 - c;
		rm.setm(0,c+mc*u.getx()*u.getx());
		rm.setm(1,mc*u.getx()*u.gety()+s*u.getz());
		rm.setm(2,mc*u.getx()*u.getz()-s*u.gety());
		rm.setm(4,mc*u.gety()*u.getx()-s*u.getz());
		rm.setm(5,c+mc*u.gety()*u.gety());
		rm.setm(6,mc*u.gety()*u.getz()+s*u.getx());
		rm.setm(8,mc*u.getz()*u.getx()+s*u.gety());
		rm.setm(9,mc*u.getz()*u.gety()-s*u.getx());
		rm.setm(10,c+mc*u.getz()*u.getz());

		invRm.setm(0,c+mc*u.getx()*u.getx());
		invRm.setm(1,mc*u.getx()*u.gety()-s*u.getz());
		invRm.setm(2,mc*u.getx()*u.getz()+s*u.gety());
		invRm.setm(4,mc*u.gety()*u.getx()+s*u.getz());
		invRm.setm(5,c+mc*u.gety()*u.gety());
		invRm.setm(6,mc*u.gety()*u.getz()-s*u.getx());
		invRm.setm(8,mc*u.getz()*u.getx()-s*u.gety());
		invRm.setm(9,mc*u.getz()*u.gety()+s*u.getx());
		invRm.setm(10,c+mc*u.getz()*u.getz());
		Affine4 * tmp;
		tmp = tos->gaffn();
		tmp->postMult(rm);
		tos->setaffn(tmp);
		tmp = tos->ginvAffn();
		tmp->preMult(invRm);
		tos->setinvAffn(tmp);
	}
	void scale(float sx, float sy, float sz)
	{
#define sEps 0.00001
		Affine4 scl;
		Affine4 invScl;
		scl.setm(0,sx);
		scl.setm(5,sy);
		scl.setm(10,sz);
		if(fabs(sx) < sEps || fabs(sy) < sEps || fabs(sz) < sEps)
		{
			cerr << "scale transformation\n";
		}
		invScl.setm(0,1/sx);
		invScl.setm(5,1/sy);
		invScl.setm(10,1/sz);
		Affine4 * tmp;
		tmp = tos->gaffn();
		tmp->postMult(scl);
		tos->setaffn(tmp);
		tmp = tos->ginvAffn();
		tmp->preMult(invScl);
		tos->setinvAffn(tmp);
	}
	void translate(Vector3 d)
	{
		Affine4 tr;
		Affine4 invTr;
		tr.setm(12,d.getx());
		tr.setm(13,d.gety());
		tr.setm(14,d.getz());
		invTr.setm(12,-d.getx());
		invTr.setm(13,-d.gety());
		invTr.setm(14,-d.getz());
		Affine4 * tmp;
		tmp = tos->gaffn();
		tmp->postMult(tr);
		tos->setaffn(tmp);
		tmp = tos->ginvAffn();
		tmp->preMult(invTr);
		tos->setinvAffn(tmp);
	}
};

class Material
{
private:
	Color3 ambient, diffuse, specular, emissive;
	int numParams;
	float params[10];
	int textureType;
	float specularExponent, reflectivity, transparency, speedOfLight;
	float specularFraction, surfaceRoughness;
public:
	void setDefault()
	{
		textureType = 0;
		numParams = 0;
		reflectivity = transparency = 0.0;
		speedOfLight = specularExponent = 1.0;
		specularFraction = 0.0;
		surfaceRoughness = 1.0;
		ambient.set(0.1f,0.1f,0.1f);
		diffuse.set(0.8f,0.8f,0.8f);
		specular.set(0,0,0);
		emissive.set(0,0,0);
	}
	Material()
	{
		textureType = 0;
		numParams = 0;
		reflectivity = transparency = 0.0;
		speedOfLight = specularExponent = 1.0;
		specularFraction = 0.0;
		surfaceRoughness = 1.0;
		ambient.set(0.1f,0.1f,0.1f);
		diffuse.set(0.8f,0.8f,0.8f);
		specular.set(0,0,0);
		emissive.set(0,0,0);
	}
	int gtt(){return textureType;}
	int gnpar(){return numParams;}
	float gams(int i){return params[i];}
	float gtran(){return transparency;}
	float gspeed(){return speedOfLight;}
	float grefl(){return reflectivity;}
	float gspecE(){return specularExponent;}
	float gspecF(){return specularFraction;}
	float gsurR(){return surfaceRoughness;}
	Color3 gamb(){return ambient;}
	Color3 gdif(){return diffuse;}
	Color3 gspec(){return specular;}
	Color3 gemi(){return emissive;}
	void setpar(int i, float n){params[i] = n;}
	void setText(int i){textureType = i;}
	void setnpar(int i){numParams = i;}
	void setsexp(float i){ specularExponent = i;}
	void setamb(Color3 a){ ambient.set(a);}
	void setdif(Color3 a){ diffuse.set(a);}
	void setspec(Color3 a){specular.set(a);}
	void setemis(Color3 a){emissive.set(a);}
	void setrough(float i){ surfaceRoughness = i;}
	void setsfra(float i){ 	specularFraction = i;}
	void settran(float i){ transparency = i;}
	void setspeed(float i){speedOfLight = i;}
	void setrefl(float i){reflectivity = i;}
	void set(Material a)
	{
		textureType = a.gtt();
		numParams = a.gnpar();
		for(int i = 0; i < numParams; i++) params[i] = a.gams(i);
		transparency = a.gtran();
		reflectivity = a.grefl();
		speedOfLight = a.gspeed();
		specularExponent = a.gspecE();
		specularFraction = a.gspecF();
		surfaceRoughness = a.gsurR();
		ambient.set(a.gamb());
		diffuse.set(a.gdif());
		specular.set(a.gspec());
		emissive.set(a.gemi());
	}

};

class GeomObj
{
public:
	GeomObj * next;
	GeomObj(): next(NULL){}
	virtual void loadStuff(){}
	virtual void drawOpenGL(){}
	virtual void tellMaterialGL(){}
	virtual void Texmap(){}
};

class Boolean: public GeomObj
{
public:
	GeomObj *left, *right;
	Boolean():left(NULL),right(NULL){}
	virtual void drawOpenGL()
	{
		if(left)left->drawOpenGL();
		if(right)right->drawOpenGL();
	}
};

class UnionBool : public Boolean
{
public:
	UnionBool(){Boolean();}
};

class IntersectionBool : public Boolean
{
public:
	IntersectionBool(){Boolean();}
};

class DiffereceBool : public Boolean
{
public:
	DiffereceBool(){Boolean();}
};

class Shape: public GeomObj
{
public:
	Material mtrl;
	Affine4 transf, invTransf;
	Shape(){}
	void setMaterial(Material a){ mtrl.set(a);}
	void tellMaterialsGL()
	{
		cout << "setting the Material" << endl;
		float zero[] = {0,0,0,1};
		Color3 colo;
		colo.set(mtrl.gamb());
		float amb[] = {colo.getr(),colo.getg(),colo.getb(),1.0};
		cout << amb[0] << " " << amb[1] << " " << amb[2] << endl;
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
		colo.set(mtrl.gemi());
		float emiss[] = {colo.getr(),colo.getg(),colo.getb(),1.0};
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emiss);
		colo.set(mtrl.gdif());
		float diff[] = {colo.getr(),colo.getg(),colo.getb(),1.0};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
		colo.set(mtrl.gspec());
		float spec[] = {colo.getr(),colo.getg(),colo.getb(),1.0};
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
		float exponet[] = {mtrl.gspecE()};
		cout << "expo is " << exponet[0] << endl;
		glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,exponet);
	}
	virtual void Texmap(){}
	virtual void drawOpenGL(){}
};

class Cube: public Shape
{
public:
	void drawOpenGL()
	{
		cout << "DRAWLING CUBE" << endl;
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		glutSolidCube(2.0);
		glPopMatrix();
	}
};

class Sphere : public Shape
{
public:

	void drawOpenGL()
	{
		cout << "DRAWLING Sphere" << endl;
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		glutSolidSphere(1.0,10,12);
		glPopMatrix();
	}
	Sphere(){ }
};

class TaperedCylinder : public Shape
{
public:
	float smallRadius;
	TaperedCylinder(){}
	void drawOpenGL()
	{
	}
};

class Square : public Shape
{
public:
	Square() {}
	void drawOpenGL()
	{
	}
};

class Plane : public Shape
{
public:
	Plane() {}
	void drawOpenGL()
	{
	}
};

class Cone : public Shape
{
public:
	Cone(){}
	void drawOpenGL()
	{
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		glutSolidCone(1.0,1.0,10,12);
		glPopMatrix();
	}
};

class Torus : public Shape
{
public:
	Torus() {}
	void drawOpenGL()
	{
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		glutSolidTorus(0.2,1.0,10,12);
		glPopMatrix();
	}
};

class Teapot : public Shape
{
public:
	Teapot(){}
	void drawOpenGL()
	{
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		glutSolidTeapot(1.0);
		glPopMatrix();
	}
};

class VertexID
{
private:
	int vertIndex, normIndex;
public:
	VertexID(){vertIndex = normIndex = 0;}
	int getvert(){return vertIndex;}
	int getnorm(){return normIndex;}
	void setvert(int i){vertIndex = i;}
	void setnorm(int i){normIndex = i;}
};

class Face
{
private:
	int nVerts;
	VertexID * vert;
public:
	Face(){nVerts =0; vert = NULL;}
	~Face(){delete[] vert; nVerts = 0;}
	void setvertv(int i, int j){ vert[i].setvert(j);}
	void setvertn(int i, int j){vert[i].setnorm(j);}
	int getvertv(int i){return vert[i].getvert();}
	int getvertn(int i){return vert[i].getnorm();}
	VertexID * getvert(){return vert;}
	void setnvert(int i){nVerts = i;}
	int getnvert(){return nVerts;}
	void makevert(int i){vert = new VertexID[i];}
};

class Mesh : public Shape
{
private:
	int numVerts, numNorms, numFaces;
	Point3 *pt;
	Vector3 *norm;
	Face * face;
	int lastVertUsed;
	int lastNormUsed;
	int lastFaceUsed;
public:
	string meshFileName;
	void readMesh(string fname)
	{
		float x , y , z;
		fstream inStream;
		inStream.open(fname.c_str(), ios::in);
		if(inStream.fail() || inStream.eof())
		{
			cout << "can't open file or eof: " << fname << endl;
			makeEmpty();
			return;
		}
		inStream >> numVerts >> numNorms >> numFaces;
		pt = new Point3[numVerts]; assert(pt != NULL);
		norm = new Vector3[numNorms]; assert(norm != NULL);
		face = new Face[numFaces]; assert(face != NULL);
		for( int i = 0; i < numVerts; i++)
		{
			inStream >> x >> y >> z;
			pt[i].setxyz(x,y,z);
		}
		for(int ii = 0; ii < numNorms ; ii++)
		{
			inStream >> x >> y >> z;
			norm[ii].set(x,y,z);
		}
		for(int f = 0; f < numFaces; f++)
		{
			int n = 0;
			inStream >> n;
			face[f].setnvert(n);
			face[f].makevert(n);
			assert(face[f].getvert() != NULL);
			for(int k = 0; k < n; k++)
			{
				inStream >> x;
				face[f].setvertv(k,x);
			}
			/*n = 0;
			inStream >> n;
			face[f].setnvert(n);
			face[f].makevert(n);*/
			for(int kk = 0; kk < n; kk++)
			{
				inStream >> x;
				face[f].setvertn(kk,x);
			}
		}
		inStream.close();
	}
	void writeMesh(char* fname)
	{
		if((numVerts == 0) || (numFaces == 0) ||(numNorms == 0)) return;
		fstream outStream(fname, ios ::out);
		if(outStream.fail())
		{
			cout << "can't make new file: " << fname << endl;
			return;
		}
		outStream << numVerts << " " << numNorms << " " << numFaces << "\n";
		for(int i = 0; i < numVerts; i++)
			outStream << pt[i].getx() << " " << pt[i].gety() << " " << pt[i].getz() << "\n";
		for(int ii = 0; ii < numNorms ; ii++)
			outStream << norm[ii].getx() << " " << norm[ii].gety() << " " << norm[ii].getz() << "\n";
		for(int f = 0; f < numFaces; f++)
		{
			int n = face[f].getnvert();
			outStream << n << "\n";
			for(int v = 0; v < n; v++)
				outStream << face[f].getvertv(v) << " ";
			outStream << "\n";
			for(int k = 0; k < n; k++)
				outStream << face[f].getvertn(k) << " ";
			outStream << "\n";
		}
		outStream.close();
	}
//	void printMesh()
//	{
//	}
	void drawMesh()
	{
		if(isEmpty()) return;
		for(int f = 0; f < numFaces; f++)
		{
			int n = face[f].getnvert();
			glBegin(GL_POLYGON);
			for(int v = 0; v < n; v++)
			{
				int in = face[f].getvertn(v);
				assert(in >= 0 && in < numNorms);
				glNormal3f(norm[in].getx(),norm[in].gety(),norm[in].getz());
				int iv = face[f].getvertv(v);
				assert(iv >= 0 && iv < numVerts);
				glVertex3f(pt[iv].getx(),pt[iv].gety(),pt[iv].getz());
			}
			glEnd();
		}
		glFlush();
	}
	void freeMesh()
	{
		delete [] pt;
		delete [] norm;
		for(int f = 0; f < numFaces; f++)
			delete [] face[f].getvert();
		delete [] face;
	}
	int isEmpty()
	{
		return (numVerts == 0) || (numFaces == 0) ||(numNorms == 0);
	}
	void makeEmpty()
	{
		numVerts = numFaces = numNorms = 0;
	}
	Mesh()
	{
		numVerts = numFaces=numNorms =0;
		pt = NULL; norm = NULL; face = NULL;
		lastVertUsed = lastNormUsed = lastFaceUsed = -1;
	}
	virtual void drawOpenGL()
	{
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		drawMesh();
		glPopMatrix();
	}
//	string meshFileName;
	Mesh(string fname)
	{
		numVerts = numFaces = numNorms = 0;
		pt = NULL; norm = NULL; face = NULL;
		lastVertUsed = lastNormUsed = lastFaceUsed = -1;
		readMesh(fname);
	}
	Vector3 newell4(int indx[])
	{
		Vector3 m;
		for(int i = 0; i < 4; i++)
		{
			int next = (i == 3) ? 0 : i+1;
			int f = indx[i], n = indx[next];
			m.set(m.getx() + (pt[f].gety() - pt[n].gety()) * (pt[f].getz() + pt[n].getz()),m.gety()+ (pt[f].getz() - pt[n].getz()) * (pt[f].getx() + pt[n].getx()),m.getz() + (pt[f].getx() - pt[n].getx()) * (pt[f].gety() + pt[n].gety()));
		}
		m.normalize();
		return m;
	}
};/*
//################## class VertexID ################
//used to define a Mesh
class VertexID{public: float vert1, vert2, vert3, lastnum;};
//################# class FACE ##############
//used to define a Mesh
class Face{
public:
		int nVerts;
		VertexID * vert; // array of vertex and normal indices
		Face(){ nVerts = 0; vert = NULL;}
		~Face(){delete[] vert; nVerts = 0;}
};
//@$@$@$@$@$@$@$@$@$@ Mesh class @$@$@$@$@$@$@$@$@$
class Mesh : public Shape
{
private:
		int numVerts;//number of vertices in the mesh
		int numFaces;//number of faces in mesh
		Vector3 *verts; // array of normals
		Face *face; // array of faces
public:
	Mesh() {numVerts=0; numFaces=0;};
	Mesh(string fname)
	{ numVerts = numFaces = 0; readFile(fname);}
	virtual void drawOpenGL()
	{
		tellMaterialsGL();
		glPushMatrix();
		glMultMatrixf(transf.getmh());
		Draw();
		glPopMatrix();
	}
	string meshFileName;
	void Draw();
	int readFile(string);
//	void drawEdges(void);
//	void writeMesh(char *);

};

void Mesh::drawEdges()
{
	for (int f=0; f<numFaces; f++) //draw face
	{
		glBegin(GL_LINE_STRIP);
		for(int v =0; v<face[f].nVerts; v++) // for each one
		{
			int in =face[f].vert[v].normIndex; //index of this normal
			int iv =face[f].vert[v].vertIndex; //index of this vertex
			glNormal3f(norm[in].x, norm[in].y, norm[in].z);
			glVertex3f(pt[iv].x,pt[iv].y,pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::Draw()
{
	for (int f=0; f<numFaces; f++) //draw face
	{
		glBegin(GL_POLYGON);
		for(int v =0; v<face[f].nVerts; v++) // for each one
		{
			int in =face[f].vert[v].normIndex; //index of this normal
			int iv =face[f].vert[v].vertIndex; //index of this vertex
			glNormal3f(norm[in].x, norm[in].y, norm[in].z);
			glVertex3f(pt[iv].x,pt[iv].y,pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::Draw()
{
	glBegin(GL_TRIANGLE_STRIP);
	for(int f =0; f < numFaces; f++)
	{
		float i,j,k;
		i=j=k=0;
		i = face[f].vert[f].vert1;
		j = face[f].vert[f].vert2;
		k = face[f].vert[f].vert3;
		glVertex3f(verts[(int)i].getx(),verts[(int)i].gety(),verts[(int)i].getz());
		glVertex3f(verts[(int)j].getx(),verts[(int)j].gety(),verts[(int)j].getz());
		glVertex3f(verts[(int)k].getx(),verts[(int)k].gety(),verts[(int)k].getz());
	}
	glEnd();
}


void Mesh:: writeMesh(char * fname)
{ 
	if(numVerts == 0 || numNorms   == 0 || numFaces == 0) return; //empty
	fstream outStream(fname, ios ::out); // open the output stream
	if(outStream.fail()) {cout << "can't make new file: " << fname << endl; return;}
	outStream << numVerts << " " << numNorms << " " << numFaces << "\n";
	// write the vertex and vertex normal list
	for(int i = 0; i < numVerts; i++)
		outStream << pt[i].x   << " " << pt[i].y   << " " << pt[i].z << "\n";
	for(int ii = 0; ii < numNorms; ii++)
		outStream  << norm[ii].x << " " << norm[ii].y << " " << norm[ii].z << "\n";
	// write the face data
	for(int f = 0; f < numFaces; f++)
	{
		int n = face[f].nVerts;
		outStream << n << "\n";
		for(int v = 0; v < n; v++)// write vertex indices for this face
			outStream << face[f].vert[v].vertIndex << " ";	outStream << "\n";
		for(int k = 0; k < n; k++)	// write normal indices for this face 
			outStream << face[f].vert[k].normIndex << " "; outStream << "\n";
	}
	outStream.close();
}

int Mesh::readFile(string fileName)
{
	cout << "readen the File" << endl;
		fstream infile;
		infile.open(fileName.c_str(), ios::in);
		if(infile.fail() || infile.eof())
		{
			cout << "can't open file or eof: " << fileName << endl;
			return 1;
		}
	infile >> numVerts;
	int i;
	verts = new Vector3[numVerts];
	cout << numVerts << endl;
	float s ,t , d;
	//check that enough memory was found:
	if(!verts)return -1; // out of memory
	for(int p = 0; p < numVerts; p++) // read the vertices
	{infile >> s >> t >> d >> i;
	verts[p].setx(s);verts[p].sety(t);verts[p].setz(d);}
	
	infile >> numFaces;
	face = new Face[numFaces];
	cout << numFaces << endl;
	for(int f = 0; f < numFaces; f++)// read the faces
{
	infile >> face[f].nVerts;
	int k= face[f].nVerts;
	face[f].vert = new VertexID[face[f].nVerts];
	for(int i = 0; i < face[f].nVerts; i++)
		infile >> face[f].vert[i].vert1 >> face[f].vert[i].vert2 >> face[f].vert[i].vert3 >> face[f].vert[i].lastnum;

} 
return 0; // success
} 
*/
class DefUnit
{
private:
	string name, stuff;
public:
	DefUnit(string a, string b)
	{ stuff = b; name = a;}
	string getname(){return name;}
	string getstuff(){return stuff;}
	void setname(string _name){ name = _name;}
	void setstuff(string _stuff){stuff = _stuff;}
};

class DefUnitStack
{
private:
	struct D4S
	{
		DefUnit * current;
		struct D4S * next;
	} d4s;
	D4S * stack;

public:
	DefUnitStack() { stack = NULL;}
	void push(string n, string s)
	{
		D4S * temp_d4s = new D4S;
		temp_d4s->current = new DefUnit(n,s);
		temp_d4s->next = stack;
		stack = temp_d4s;	
	}
	void print()
	{
		D4S * temp = stack;
		string t;
		while (temp)
		{
			cout << temp->current->getname() << ":";
			cout << temp->current->getstuff() << endl;
			temp = temp->next;
		}
	}
	int search(string s)
	{
		D4S * temp = stack;
		while(temp)
		{
			if(temp->current->getname() == s)
			{
				return 1;
			}
			temp = temp->next;
		}
		return 0;
	}
	string contents(string s)
	{
		D4S * temp = stack;
		while(temp)
		{
			if(temp->current->getname() == s)
			{
				return(temp->current->getstuff());
			}
			temp = temp->next;
		}
		return(NULL);
	}
	void release()
	{
		while(stack)
		{
			D4S * tmp = stack;
			stack = stack->next;
			delete tmp->current;
			delete tmp;
		}
		stack = NULL;
	}
};

enum mTokenType {IDENT, LIGHT, ROTATE,TRANSLATE,SCALE,PUSH,POP,IDENTITYAFFINE, TORUS, PLANE,
GLOBALAMBIENT, BACKGROUND, MINREFLECTIVITY, MINTRANSPARENCY, MAXRECURSIONDEPTH, CUBE, SQUARE,
SPHERE,CYLINDER, CONE, TAPEREDCYLINDER,TETRAHEDRON, OCTAHEDRON, DODECAHEDRON,ICOSAHEDRON,BUCKYBALL,EMISSIVE,
TEAPOT,DIAMOND, UNION, INTERSECTION, DIFFERENCEa,MAKEPIXMAP, MESH, DEFAULTMATERIALS, AMBIENT,
DIFFUSE, SPECULAR, SPECULARFRACTION, SPECULAREXPONENT, SPEEDOFLIGHT, SURFACEROUGHNESS, TRANSPARENCY, REFLECTIVITY,
PARAMETERS, TEXTURE, LFTCURLY, RGHTCURLY, DEF, USE, T_NULL, F_EOF, UNKNOWN, SPOTLIGHT, TEXTUREPROJRETION, GLASS};

class Scene
{
private:
	int line;
	int nextline;
	ifstream * file_in;
	strstream * f_in;
	strstream temp_fin;
	DefUnitStack * def_stack;
	GeomObj * tail;
	AffineStack affStk;
	Material currMtrl;
	string nexttoken(void);
	float getFloat();
	bool isidentifier(string keyword);
	void cleanUp();
	mTokenType whichtoken(string keyword);
	float eyeplanS[4];
	float eyeplanT[4];
	float eyeplanR[4];
	float eyeplanQ[4];
public:
	RGBpixmap pixmap[8];
	Light * light;
	GeomObj * obj;
	Color3 background, ambient;
	int maxRecursionDepth;
	float minReflectivity, minTransparency;
	Scene():light(NULL),obj(NULL),tail(NULL)
	{
		currMtrl.setDefault();
		background.set(1.0f,1.0f,1.0f);
		ambient.set(0.1f,0.1f,0.1f);
		minReflectivity = 0.5;
		minTransparency = 0.5;
		maxRecursionDepth = 3;
	}
	Scene(string fname){Scene(); read(fname);}
	void freeScene();
	void makeLightsOpenGL(); 
	void drawSceneOpenGL();
	bool read(string fname);
	GeomObj * getObject();
	void makeBackground()
	{glClearColor(background.getr(),background.getg(),background.getb(),0.0f);}
	void makeProjectTexture();
	void setProjectTexture();

};

void Scene::setProjectTexture()
{	
//	pixmap[currMtrl.gtt()].setProjection(currMtrl.gtt());
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGenfv(GL_S,GL_OBJECT_PLANE,eyeplanS);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGenfv(GL_T,GL_OBJECT_PLANE,eyeplanT);
	glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGenfv(GL_R,GL_OBJECT_PLANE,eyeplanR);
	glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGenfv(GL_Q,GL_OBJECT_PLANE,eyeplanQ);
}
void Scene::makeProjectTexture()
{			
	AffineNode * temper;
	temper = affStk.gtos();
	Affine4 transf;
	transf.set(*(temper->gaffn()));
	Affine4 invTransf;
	invTransf.set(*(temper->ginvAffn()));
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMultMatrixf(transf.getmh());
	glFrustum(-3.0,3.0,-3.0,3.0,.1,10.0);
	glTranslatef(0.0,0.0,-1.0);
	glMultMatrixf(invTransf.getmh());
	glMatrixMode(GL_MODELVIEW);

}

string Scene::nexttoken(void)
{
	char c;
	string token;
	int lastchar = 1;
	if(!f_in){return(token);}
	if(f_in->eof()){return(token);}
	while(f_in->get(c))
	{
		if(f_in->eof())
		{return token;}
		switch(c)
		{
		case '\n':nextline +=1;
		case ' ' :
		case '\t':
		case '\a':
		case '\b':
		case '\v':
		case '\f':
		case '\r':
			{ if (lastchar == 0) {return token;} break;}
		case '{':
			{ token = c; return token; break;}
		case '}':{ token = c; return token; break;}
		case '!':
			{while(c != '\n' && f_in->get(c)){}
			nextline++; break;}
		default:
			{
				token = token + c; lastchar = 0;
				if((f_in->peek() == '{') || (f_in->peek() == '}'))
				{
					if(lastchar == 0)
					{
						return token;
					}
					else
					{
						f_in->get(c);
						token = c;
						return token;
					}
				}
				line = nextline;
			}
		}
	}
	return (" ");
}

float Scene::getFloat()
{
	strstream tmp;
	float number;
	string str = nexttoken();
	tmp << str;
	if(!(tmp >> number))
	{
		cerr << "line " << line << ": error getting float" << endl;
		exit(-1);
	}
	else
	{
		char t;
		if(tmp >> t)
		{
			cerr << "LINE " << line << ": bum chars in number" << endl;
			exit(-1);
		}
	}
	return number;
}

bool Scene::isidentifier(string keyword)
{
	string temp = keyword;
	if( !isalpha(temp[0])) return false;
	for(int count = 1; count < temp.length(); count++)
	{
		if((!isalnum(temp[count])) && (temp[count] != '.'))
			return false;
	}
	return true;
}

void Scene::cleanUp()
{
	affStk.releaseAffines();
	def_stack->release();
	delete def_stack;
}

void Scene::freeScene()
{
	GeomObj * p = obj;
	while(p)
	{
		GeomObj * q = p;
		p = p->next;
		delete q;
	}
	Light * q = light;
	while(q)
	{
		Light * r = q;
		q = q->gnext();
		delete r;
	}
}

mTokenType Scene::whichtoken(string keyword)
{
	string temp = keyword;
	if(temp == "glass") return GLASS;
	if(temp == "textureProjection") return TEXTUREPROJRETION;
	if(temp == "spotlight") return SPOTLIGHT;
	if(temp == "light") return LIGHT;
	if(temp == "rotate") return ROTATE;
	if(temp == "translate") return TRANSLATE;
	if(temp == "scale") return SCALE;
	if(temp == "push") return PUSH;
	if(temp == "pop") return POP;
	if(temp == "cube") return CUBE;
	if(temp == "identityAffine") return IDENTITYAFFINE;
	if(temp == "sphere") return SPHERE;
	if(temp == "torus") return TORUS;
	if(temp == "plane") return PLANE;
	if(temp == "square") return SQUARE;
	if(temp == "cylinder") return CYLINDER;
	if(temp == "taperedCylinder") return TAPEREDCYLINDER;
	if(temp == "cone") return CONE;
	if(temp == "tetrahedron") return TETRAHEDRON;
	if(temp == "octahedron") return OCTAHEDRON;
	if(temp == "dodecahedron") return DODECAHEDRON;
	if(temp == "icosahedron") return ICOSAHEDRON;
	if(temp == "buckyball") return BUCKYBALL;
	if(temp == "diamond") return DIAMOND;
	if(temp == "teapot") return TEAPOT;
	if(temp == "union") return UNION;
	if(temp == "intersection") return INTERSECTION;
	if(temp == "difference") return DIFFERENCEa;
	if(temp == "mesh")return MESH;
	if(temp == "makePixmap") return MAKEPIXMAP;
	if(temp == "defualtMaterials") return DEFAULTMATERIALS;
	if(temp == "ambient") return AMBIENT;
	if(temp == "diffuse") return DIFFUSE;
	if(temp == "specular") return SPECULAR;
	if(temp == "specularFraction") return SPECULARFRACTION;
	if(temp == "surfaceRoughness") return SURFACEROUGHNESS;
	if(temp == "emissive") return EMISSIVE;
	if(temp == "specularExponent") return SPECULAREXPONENT;
	if(temp == "speedOfLight") return SPEEDOFLIGHT;
	if(temp == "transparency") return TRANSPARENCY;
	if(temp == "reflectivity") return REFLECTIVITY;
	if(temp == "parameters") return PARAMETERS;
	if(temp == "texture") return TEXTURE;
	if(temp == "globalAmbient") return GLOBALAMBIENT;
	if(temp == "minReflectivity") return MINREFLECTIVITY;
	if(temp == "minTransparency") return MINTRANSPARENCY;
	if(temp == "maxRecursionDepth") return MAXRECURSIONDEPTH;
	if(temp == "background") return BACKGROUND;
	if(temp == "{") return LFTCURLY;
	if(temp == "}") return RGHTCURLY;
	if(temp == "def") return DEF;
	if(temp == "use") return USE;
	if(temp == " ") return T_NULL;
	if(isidentifier(temp)) return IDENT;
	cout << temp << ":" << temp.length() << endl;
	return UNKNOWN;
}

void Scene::drawSceneOpenGL()
{
	for(GeomObj * p = obj; p; p=p->next)
		p->drawOpenGL();
}

GeomObj * Scene::getObject()
{
	string s;
	GeomObj * newShape;
	mTokenType typ;
	while(( typ = (whichtoken(s = nexttoken()))) != T_NULL)
	{		
		if(typ == UNION || typ == INTERSECTION || typ == DIFFERENCEa)
		{
			switch(typ)
			{
			case UNION: 
				newShape = new UnionBool; 
				break;
			case INTERSECTION: 
				newShape = new IntersectionBool; 
				break;
			case DIFFERENCEa: 
				newShape = new DiffereceBool; 
				break;
			}
			GeomObj * p = newShape;
			p = getObject();
			if(!p) return NULL;
			((Boolean*)newShape)->left = p;
			p = getObject();
			if(!p) return NULL;
			((Boolean*)newShape)->right = p;
			return newShape;
		}
		switch(typ)
		{
		case TEXTUREPROJRETION:
			{
				currMtrl.setText((int)getFloat());
				eyeplanS[0] = getFloat();eyeplanS[1] = getFloat();eyeplanS[2] = getFloat();eyeplanS[3] = getFloat();
				eyeplanT[0] = getFloat();eyeplanT[1] = getFloat();eyeplanT[2] = getFloat();eyeplanT[3] = getFloat();
				eyeplanR[0] = getFloat();eyeplanR[1] = getFloat();eyeplanR[2] = getFloat();eyeplanR[3] = getFloat();			
				eyeplanQ[0] = getFloat();eyeplanQ[1] = getFloat();eyeplanQ[2] = getFloat();eyeplanQ[3] = getFloat();				
					break;
			}
		case LIGHT: 
			{
			Point3 p;
			Color3 c;
			p.setx(getFloat());p.sety(getFloat()); p.setz(getFloat());
			c.setr(getFloat()); c.setg(getFloat());	c.setb(getFloat());
			Light *l = new Light;
			l->setPosition(p);
			l->setColor(c);
			l->setnext(light); 
			light = l; 
			break;
			}
		case SPOTLIGHT:
			{
				Point3 p;
				Color3 c;
			p.setx(getFloat());p.sety(getFloat()); p.setz(getFloat());
			c.set(0,0,0);
			Light *l = new Light;
			l->setPosition(p);
			l->setColor(c);
			l->setspot(getFloat(),getFloat(),getFloat());
			l->setnext(light); 
			light = l; 
			break;
			}
		case ROTATE: 
			{
			float angle;
			Vector3 u;
			angle = getFloat(); u.setx(getFloat()); 
			u.sety(getFloat()); u.setz(getFloat());
			affStk.rotate(angle,u);
			break;
			}
		case TRANSLATE:
			{
			Vector3 d;
			d.setx(getFloat()); d.sety( getFloat()); d.setz(getFloat());
			affStk.translate(d);
			break;
			}
		case SCALE: 
			{
			float sx, sy, sz;
			sx = getFloat(); sy = getFloat(); sz = getFloat();
			affStk.scale(sx, sy, sz);
			break;
			}
		case PUSH: 
			affStk.dup(); 
			break; 
		case POP:  
			affStk.popAndDrop();
			break;
		case IDENTITYAFFINE: 
			affStk.setIdentity();
			break;
		case AMBIENT:
			{
			float dr, dg, db;
			dr = getFloat(); dg = getFloat(); db = getFloat();
			Color3 a;
			a.set(dr,dg,db);
			currMtrl.setamb(a); 
			break;
			}
		case DIFFUSE: 
			{
			float dr,dg,db;
			dr = getFloat(); dg = getFloat(); db = getFloat();
			Color3 a;
			a.set(dr,dg,db);
			currMtrl.setdif(a); 
			break;
			 }
		case SPECULAR:
			{ 
			float dr,dg,db;
			dr = getFloat(); dg = getFloat(); db = getFloat();
			Color3 a;
			a.set(dr,dg,db);
			currMtrl.setspec(a);
			break;
			}
		case EMISSIVE: 
			{
			float dr,dg,db;
			dr = getFloat(); dg = getFloat(); db = getFloat();
			Color3 a;
			a.set(dr,dg,db);
			currMtrl.setemis(a); 
			break;
			}
		case PARAMETERS: 
			{ 
			currMtrl.setnpar((int)getFloat());
			for(int i = 0; i < currMtrl.gnpar(); i++)
				currMtrl.setpar(i,getFloat());
			break;
			}			
		case SPECULARFRACTION: currMtrl.setsfra(getFloat()); break;
		case SURFACEROUGHNESS: currMtrl.setrough(getFloat()); break;
		case TEXTURE: 
			{ 
				currMtrl.setText((int)getFloat());	
				break;
			}
		case DEFAULTMATERIALS: 	currMtrl.setDefault();break;
		case SPEEDOFLIGHT: currMtrl.setspeed(getFloat()); break;
		case SPECULAREXPONENT: currMtrl.setsexp(getFloat()); break;
		case TRANSPARENCY:currMtrl.settran(getFloat()); break;
		case REFLECTIVITY: currMtrl.setrefl(getFloat()); break;
		case GLOBALAMBIENT:{
			ambient.setr(getFloat()); ambient.setg(getFloat());
			ambient.setb(getFloat()); break;
						   }
		case BACKGROUND:{ 
			background.setr(getFloat());
			background.setg(getFloat()); 
			background.setb(getFloat());
			break;
						}
		case MINREFLECTIVITY: minReflectivity = getFloat(); break;
		case MINTRANSPARENCY:minTransparency = getFloat(); break;
		case MAXRECURSIONDEPTH: maxRecursionDepth = getFloat(); break;
		case MAKEPIXMAP: {	// get BMP file name for a pixmap			
		  int which = getFloat();// index of this pixmap in pixmap array
		  if(which < 0 || which > 7){cout << "\nbad index of RGBpixmap!\n";}
		  string fname = nexttoken(); // get file name for mesh	  
		  cout << "I got fname = " << fname << endl;
		  if(!pixmap[which].readBMPFile2(fname))
		  {// read BMP file into this pixmap
				cout << " \ncan't read that RGBpixmap file!\n";
				return NULL;
		  }
		  break;}
		case T_NULL: break; 
		case DEF: {
			string name, temp, lb, rb;
			int l = line;
			string inp;
			name = nexttoken();
			if ( whichtoken(name) != IDENT ) {
				cout << "Error:  Identifier expected." << endl;
				return NULL;
			}
			if ( def_stack->search(name) ) {
				cout << line << ": " << name;
				cout << ": attempt to redefine. " << endl;
				return NULL;
			}
			lb = nexttoken();
			if ( whichtoken(lb) != LFTCURLY ) 
			{
				cout << "Error: { expected." << endl;
				return NULL;
			}
			while ( whichtoken( temp = nexttoken()) != RGHTCURLY ) 
			{
				cout << temp << endl;
				inp = inp + temp + " ";
				if (!f_in) {
					cout << "Error: end of file detected." << endl;
					return NULL;
				}
			}
			// Push the contents of the string on the stack.
			def_stack->push(name, inp);
			break;
				  } 
		case USE: 
			{
			string name;
			name = nexttoken();
			if ( whichtoken(name) != IDENT ) 
			{
				cout << line << ": " << name;
				cout << ": identifier expected.";
				return NULL;
			}
			if (! def_stack->search(name) ) 
			{
				cout << line << ": " << name;
				cout << ": not defined.";
				return NULL;
			}
			cout << def_stack->contents(name) << endl;
			strstream *temp_fin = new strstream;
			*temp_fin << def_stack->contents(name) << " ";
			*temp_fin << f_in->rdbuf();
			delete (f_in);
			f_in = temp_fin;
			break; 
			}
		default:  
			{ // inner switch for Shapes
			switch(typ) 
			{
			case CUBE:			newShape = new Cube;break;
			case SPHERE:		newShape = new Sphere;break;
			case TETRAHEDRON:	newShape = new Mesh("mesh/tetra.3vn");break; 
			case TORUS:		newShape = new Torus;break;
			case PLANE:		newShape = new Plane;break;
			case SQUARE:		newShape = new Square;break;
			case TAPEREDCYLINDER:	newShape = new TaperedCylinder; 
				((TaperedCylinder*)newShape)->smallRadius = getFloat(); break;
			case CONE:			newShape = new Cone;  break;
			case CYLINDER:		newShape = new TaperedCylinder; 
				((TaperedCylinder*)newShape)->smallRadius = 1; break;
			case OCTAHEDRON:		newShape = new Mesh("mesh/octa.3vn");break; 
			case DODECAHEDRON:	newShape = new Mesh("mesh/dodeca.3vn"); break;  
			case ICOSAHEDRON:	newShape = new Mesh("mesh/icosa.3vn"); break;   
			case BUCKYBALL:		newShape = new Mesh("mesh/bucky.3vn");	break; 
			case DIAMOND:		newShape = new Mesh("mesh/diamond.3vn"); break; 
			case TEAPOT:		newShape = new Teapot; break;
			case GLASS:			newShape = new Mesh("mesh/glass.off"); break;
			case MESH: 
				{		
				string fname = nexttoken(); 
				newShape = new Mesh(fname); 
				break;
				}
		   default: {
				cerr << "Line " << nextline << ": unknown keyword " << s << endl;
				return NULL;
					 }	
			} 
			
			((Shape*)newShape)->mtrl.set(currMtrl);
			AffineNode * temper;
			temper = affStk.gtos();
			((Shape*)newShape)->transf.set(*(temper->gaffn()));
			((Shape*)newShape)->invTransf.set(*(temper->ginvAffn()));
			return newShape;
		 }			
    } 
  } 
  return NULL;
} 


bool Scene::read(string fname)
{
	file_in = new ifstream(fname.c_str());
	if(!(*file_in))
	{
		cout << "I can't find or open file: " << fname << endl;
		return false;
	}
	f_in= new strstream();
	line = nextline = 1;
	def_stack = new DefUnitStack();
	char ch;
	freeScene();
	obj = tail = NULL;
	light = NULL;
	affStk.reset();
	while(file_in->get(ch))
	{
		*f_in << ch;
	}
	while(1)
	{
		GeomObj * shp = getObject();
		if(!shp)break;
		shp->next = NULL;
		if(obj == NULL)
		{
			obj = tail = shp;
		}
		else
		{
			tail->next = shp;
			tail = shp;
		}
	}
	file_in->close();
	cleanUp();
	return true;
}

void Scene::makeLightsOpenGL()
{
	Color3 a;
	Point3 b;
//	float c;
	for(Light * l = light; l != NULL; l = l->gnext())
	{
		a = l->getcolor();
		b = l->getpos();
		if(l->getspot() == 1)
		{
			cout << "SpotLight set" << endl;
			glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,l->getangleC());
			glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,l->getf());
			float h[] = {b.getx(),b.gety(),b.getz()};
			glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,h);
		}
		else
		{
		cout << b.getx() << " " << b.gety() << " " << b.getz() << endl;
		cout << "the light is set" << endl;
		float h[] = {b.getx(),b.gety(),b.getz(),0.0};
		float r[] = {a.getr(),a.getg(),a.getb(),1.0};
		glLightfv(GL_LIGHT0, GL_POSITION,h);
		glLightfv(GL_LIGHT0, GL_DIFFUSE,r);
		}
}
}

class Camera{
  private:
	Point3 eye;
	Vector3 u,v,n;
	double viewAngle, aspect, nearDist, farDist; // view volume shape
	void setModelViewMatrix(); // tell OpenGL where the camera is

  public:
	Camera(); // default constructor
	void set(Point3 eye, Point3 look, Vector3 up); // like gluLookAt()
	void roll(float angle); // roll it
	void pitch(float angle); // increase pitch
	void yaw(float angle); // yaw it
	void slide(float delU, float delV, float delN); // slide it
	void setShape(float vAng, float asp, float nearD, float farD)
	{
		viewAngle = vAng;
		aspect = asp;
		nearDist= nearD;
		farDist = farD;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(viewAngle, aspect, nearDist, farDist);
		setModelViewMatrix();
	}
};

Camera :: Camera()
{
}


void Camera::setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	float m[16];
	Vector3 eVec(eye.getx(), eye.gety(), eye.getz()); // a vector version of eye 
	m[0] =  u.getx(); m[4] =  u.gety(); m[8]  =  u.getz();  m[12] = -eVec.dot(u);
	m[1] =  v.getx(); m[5] =  v.gety(); m[9]  =  v.getz();  m[13] = -eVec.dot(v);
	m[2] =  n.getx(); m[6] =  n.gety(); m[10] =  n.getz();  m[14] = -eVec.dot(n);
	m[3] =  0;   m[7] =  0;   m[11] =  0;    m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m); // load OpenGL’s modelview matrix
}
void Camera::set(Point3 Eye, Point3 look, Vector3 up)
{	// create a modelview matrix and send it to OpenGL
	eye.set(Eye); // store the given eye position
	n.set(eye.getx() - look.getx(), eye.gety() - look.gety(), eye.getz() - look.getz()); // make n
	u.set(up.cross(n)); // make u = up X n
	n.normalize(); u.normalize(); // make them unit length
	v.set(n.cross(u));  // make v =  n X u
	setModelViewMatrix(); // tell OpenGL 
}

void Camera::roll(float angle)
{ // roll the camera through angle degrees
float cs = cos(3.14159265/180 * angle);
float sn = sin(3.14159265/180 * angle);
Vector3 t(u); // remember old u
u.set(cs*t.getx() + sn*v.getx(), cs*t.gety() + sn*v.gety(), cs*t.getz() + sn*v.getz());
	v.set(-sn*t.getx() + cs*v.getx(), -sn*t.gety() + cs*v.gety(), -sn*t.getz() + cs*v.getz());
	setModelViewMatrix();
}

void Camera::slide(float delU, float delV, float delN)
{
	eye.setx( eye.getx() + (delU * u.getx() + delV * v.getx() + delN * n.getx()));
	eye.sety(eye.gety() + (delU * u.gety() + delV * v.gety() + delN * n.gety()));
	eye.setz(eye.getz() + (delU * u.getz() + delV * v.getz() + delN * n.getz()));
	setModelViewMatrix();
}

Camera cam; // global camera object
Scene scn;

void myrotate()
{
	int i =0;
	while(i < 100)
	{
		glPushMatrix();
		glRotated(2,1,0,0);
		glutSolidCube(.3);
		glPopMatrix();
		glFlush();
		for(int j = 0; j < 10000; j++)
			;
		glutPostRedisplay();
	}
}

//class Sphere

void myKeyboard(unsigned char key, int x, int y)
{
  switch(key)
  {	
	// controls for camera
	case 'b':    cam.slide(0,0, 0.2); break; // slide camera forward
	case 'f': cam.slide(0,0,-0.2); break; //slide camera back
	case 'u': cam.slide(0,0.2,0);  break;
	case 'd': cam.slide(0,-0.2,0); break;
	case 'r': cam.slide(0.2,0,0); break;
	case 'l': cam.slide(-0.2,0,0); break;
	// add up/down and left/right controls	
	case 'z':      cam.roll(-1.0); break;
	case 'x': cam.roll( 1.0); break;
	// add roll and yaw controls
  }
	glutPostRedisplay(); // draw it again
}

void displaysdl(void)
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	scn.drawSceneOpenGL();
	glFlush();
}


void main(int argc, char **argv)
{	
	Point3 e;
	Point3 l;
	Vector3 u;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(50,50);
	glutCreateWindow("SDL case study");
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(displaysdl);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	scn.read("test.dat");
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	scn.makeBackground();
	scn.makeLightsOpenGL();
	glViewport(0,0,640,480);
	e.setxyz(1.5,1.3,1.5);
	l.setxyz(0, 0.25, 0);
	u.set(0, 1, 0);
	cam.set(e, l, u); // make the initial camera
	cam.setShape(60.0f, 640.f/480.f, 0.1f, 50.0f);
	glutMainLoop();
}


/*int Mesh::readFile(string fileName)
{
	cout << "readen the File" << endl;
	fstream infile;
	infile.open(fileName, ios::in);
	if(infile.fail()) return -1; // error - can't open file
	if(infile.eof())  return -1; // error - empty file
	infile >> numVerts;
	int i;
	verts = new Vector3[numVerts];
	cout << numVerts << endl;
	float s ,t , d;
	//check that enough memory was found:
	if(!verts)return -1; // out of memory
	for(int p = 0; p < numVerts; p++) // read the vertices
	{infile >> s >> t >> d >> i;
	verts[p].setx(s);verts[p].sety(t);verts[p].setz(d);}
	
	infile >> numFaces;
	face = new Face[numFaces];
	cout << numFaces << endl;
	for(int f = 0; f < numFaces; f++)// read the faces
{
	infile >> face[f].nVerts;
	int k= face[f].nVerts;
	face[f].vert = new VertexID[face[f].nVerts];
	for(int i = 0; i < face[f].nVerts; i++)
		infile >> face[f].vert[i].vert1 >> face[f].vert[i].vert2 >> face[f].vert[i].vert3 >> face[f].vert[i].lastnum;

} 
return 0; // success
} */



