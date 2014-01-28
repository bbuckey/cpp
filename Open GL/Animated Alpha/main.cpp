#include <windows.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>
#include <gl/glaux.h>
#include <fstream>
#include <assert.h>
#include <string>
#include <strstream>
#include <fmod.h>
#include <conio.h> 
#include "sound.h"
#include "RGBpixmap.h"
using namespace std;


FSOUND_STREAM * stream;
int checkclick = 0;

float xSpeed = -.5, ySpeed = 0, xAngle = 1, yAngle = 0;  
float AngleX = 17;
float AngleY = -22;
float AngleTop = 0;
float TransB = 0;
float TransC = 0;
float Rise = -0.4;
float sx=0.2;
float sy=0.2;
float sz=0.2;
float stretch=0.7;


fstream inf; // global in this file for convenience

RGBpixmap pix[13];

int font=(int) GLUT_STROKE_ROMAN;
void alphabet();
void myDisplay();
void display();

class Point3{ 
public: 
	float x,y,z;
	void set(float dx, float dy, float dz){x = dx; y = dy; z = dz;}
	void set(Point3& p){x = p.x; y = p.y; z = p.z;} 	
 Point3(float xx,     float yy, float zz){x = xx; y = yy; z = zz;}
	Point3(){x = y = z = 0;}
	void build4tuple(float v[])
	{// load 4-tuple with this color: v[3] = 1 for homogeneous
		v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0f;
	}	
};
//@@@@@@@@@@@@@@@@@@ Vector3 class @@@@@@@@@@@@@@@@
class Vector3{ 
public: 
	float x,y,z;
	void set(float dx, float dy, float dz){ x = dx; y = dy; z = dz;} 
	void set(Vector3& v){ x = v.x; y = v.y; z = v.z;}
	void flip(){x = -x; y = -y; z = -z;} // reverse this vector
	void setDiff(Point3& a, Point3& b)//set to difference a - b
	{ x = a.x - b.x; y = a.y - b.y; z = a.z - b.z;}
	void normalize()//adjust this vector to unit length
	{		
		double sizeSq = x * x + y * y + z * z;
		if(sizeSq < 0.0000001)
		{
			cerr << "\nnormalize() sees vector (0,0,0)!";
			return; // does nothing to zero vectors;
		}
		float scaleFactor = 1.0/(float)sqrt(sizeSq);
		x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
	}
	Vector3(float xx, float yy, float zz){x = xx; y = yy; z = zz;}
	Vector3(Vector3& v){x = v.x; y = v.y; z = v.z;}
	Vector3(){x = y = z = 0;} //default constructor
	Vector3 cross(Vector3 b) //return this cross b
	{
	   Vector3 c(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	   return c;
	}
	float dot(Vector3 b) // return this dotted with b
 {return x * b.x + y * b.y + z * b.z;}
};

//################## class VertexID ################
//used to define a Mesh
class VertexID{public: int vertIndex, normIndex;};
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
class Mesh{
private:
		int numVerts;//number of vertices in the mesh
		int numNorms;//number of normal vectors for the mesh
		int numFaces;//number of faces in mesh
		Point3 *pt; // array of points
		Vector3 *norm; // array of normals
		Face *face; // array of faces
public:
	Mesh() {numVerts=0;numNorms=0; numFaces=0;};
	void Draw();
	int readFile(char*);
	void drawEdges(void);
	void writeMesh(char *);
	void drawPoints();
 
};

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

int Mesh::readFile(char * fileName)
{
	fstream infile;
	infile.open(fileName, ios::in);
	if(infile.fail()) return -1; // error - can't open file
	if(infile.eof())  return -1; // error - empty file
	infile >> numVerts >> numNorms >> numFaces;
pt = new Point3[numVerts];
norm = new Vector3[numNorms];
face = new Face[numFaces];
//check that enough memory was found:
if( !pt || !norm || !face)return -1; // out of memory
for(int p = 0; p < numVerts; p++) // read the vertices
	infile >> pt[p].x >> pt[p].y >> pt[p].z;
for(int n = 0; n < numNorms; n++) // read the normals
	infile >> norm[n].x >> norm[n].y >> norm[n].z;
for(int f = 0; f < numFaces; f++)// read the faces
{
	infile >> face[f].nVerts;
	int k= face[f].nVerts;
	face[f].vert = new VertexID[face[f].nVerts];
	for(int i = 0; i < face[f].nVerts; i++)
		infile >> face[f].vert[i].vertIndex;
	for(int kk = 0; kk < k; kk++) 		// read normal indices for this face
		infile>>face[f].vert[kk].normIndex;
} 
return 0; // success
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


void Camera :: setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	float m[16];
	Vector3 eVec(eye.x, eye.y, eye.z); // a vector version of eye 
	m[0] =  u.x; m[4] =  u.y; m[8]  =  u.z;  m[12] = -eVec.dot(u);
	m[1] =  v.x; m[5] =  v.y; m[9]  =  v.z;  m[13] = -eVec.dot(v);
	m[2] =  n.x; m[6] =  n.y; m[10] =  n.z;  m[14] = -eVec.dot(n);
	m[3] =  0;   m[7] =  0;   m[11] =  0;    m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m); // load OpenGL’s modelview matrix
}
void Camera:: set(Point3 Eye, Point3 look, Vector3 up)
{	// create a modelview matrix and send it to OpenGL
	eye.set(Eye); // store the given eye position
n.set(eye.x - look.x, eye.y - look.y, eye.z - look.z); // make n
	u.set(up.cross(n)); // make u = up X n
	n.normalize(); u.normalize(); // make them unit length
	v.set(n.cross(u));  // make v =  n X u
	setModelViewMatrix(); // tell OpenGL 
}

void Camera :: roll(float angle)
{ // roll the camera through angle degrees
float cs = cos(3.14159265/180 * angle);
float sn = sin(3.14159265/180 * angle);
Vector3 t(u); // remember old u
u.set(cs*t.x + sn*v.x, cs*t.y + sn*v.y, cs*t.z + sn*v.z);
	v.set(-sn*t.x + cs*v.x, -sn*t.y + cs*v.y, -sn*t.z + cs*v.z);
	setModelViewMatrix();
}

void Camera::slide(float delU, float delV, float delN)
{
	eye.x += delU * u.x + delV * v.x + delN * n.x;
	eye.y += delU * u.y + delV * v.y + delN * n.y;
	eye.z += delU * u.z + delV * v.z + delN * n.z;
	setModelViewMatrix();
}

Camera cam; // global camera object

typedef unsigned short ushort;
typedef unsigned long ulong;
//fstream inf; // global in this file for convenience
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
int RGBpixmap:: readBMPFile(char * fname) 
{  // Read into memory an mRGB image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	inf.open(fname, ios::in|ios::binary); //read binary char's
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

void lighting(float red, float green, float blue)
{
	

// set properties of the surface material
	GLfloat mat_ambient[] = { red, green, blue, 1.0f }; // gray
	GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// light sources
	GLfloat lightIntensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPosition[] = {2.0f, 6.0f, 3.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);

}

void rcar() // red race car
{
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(1,0,0);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-30,1,0,0);
	glScaled(.4,.2,.4);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslated(.25,0,.08);
	gluSphere(mystuff,.06,10,10);
	glPopMatrix();

	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslated(.08,0,.25);
	gluSphere(mystuff,.06,10,10);
	glPopMatrix();
}

void pencial() // the Pencal
{

	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(1,.9,0);
	glPushMatrix();
	glRotated(-90,1,0,0);
	gluCylinder(mystuff,.05,.05,2,20,20);
	glPopMatrix();

	glColor3f(1,1.5,0);
	glPushMatrix();
	glTranslated(0,-.299,0);
	glRotated(-90,1,0,0);
	gluCylinder(mystuff,.005,.05,.3,20,20);
	glPopMatrix();
}

void bcar() // blue race car
{

	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(0,0,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-30,1,0,0);
	glScaled(.4,.2,.4);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslated(.25,0,.08);
	gluSphere(mystuff,.06,10,10);
	glPopMatrix();

	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslated(.08,0,.25);
	gluSphere(mystuff,.06,10,10);
	glPopMatrix();
}


void head() // part of lego man
{
	glPushMatrix();
	glScaled(0.4,0.4,0.4);
	glutSolidSphere(1,15,15);
	glPopMatrix();
}

void neck() // part of lego man
{
	glPushMatrix();
	glScaled(0.4,0.8,0.4);
	glutSolidCube(.5);
	glPopMatrix();

}

void body() // part of lego man
{

	glPushMatrix();
	glScaled(0.5,0.7,0.4);
	glutSolidCube(1);
	glPopMatrix();
}

void arm() // part of lego man
{
	glPushMatrix();
	glScaled(0.8,0.8,5.0);
	glutSolidSphere(1,15,15);
	glPopMatrix();
}
// one axis of a jack

void handlePart() // jack in the box
{
	glPushMatrix();
	glScaled(0.8, 0.8, 5.0);
	glutSolidSphere(1,15,15);
	glPopMatrix();
}
void handle() // jack in the box
{
	glPushMatrix();
	glRotated(90.0,1,0,0);
	handlePart();
	glPopMatrix();
}


void handlePart1() // part of jack in the box
{
	glPushMatrix();
	glScaled(0.3, 0.3, 1.0);
	glutSolidSphere(1, 15, 15);  //main bar
	glPopMatrix();


	glPushMatrix();
	glTranslated(0, 0, 1.2); // ball on one end
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}

void handle1() // part of jack in the box
{
	
	glPushMatrix();
	glRotated(90.0, 0, 1, 0);
	glRotated(50.0, 1, 0, 0); 
	glRotated(40.0,0,1,0);   
	glScaled(2.0,3.0,3.0);   
	glRotated(45.0,0,0,1);
	handlePart1();
	glPopMatrix();
}


void popjack() // pops the jack in the box
{
	int r = 0;
	int s=50;
	  AngleTop = -80;//-70;
	  TransB = 1.6;//1.3;
	  TransC = 0.175;//0.3;
	  Rise = 1.0;
	  xSpeed = 0;

		for(int l = 0; l<10; l++)
		{
			if (l>3)
			{
				s=75;
			}
			if (l>6)
			{
				s=110;
			}
			if (l>8)
			{
				s=150;
			}
			Rise= 1.1;
			sx=0.150;
			sy=0.150;
			sz=0.150;
			stretch=0.8;
			Sleep(s);
			display();
			Rise=1.0;
			sx=0.2;
			sy=0.2;
			sz=0.2;
			stretch=0.7;
			Sleep(s);
			display();
		}

}



void quilt() // bitmap quilt
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2003);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}


void awords() //bitmap many a words
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2009);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}
void xray() //bitmap xray
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2008);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}

void valley() // bitmap vally
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2007);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}

void wall() // brickwall
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2010);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}

void mouse() // brickwall
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2011);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
}

void display() // Jack in the box animation
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


		glPushMatrix();


		// Front Face
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2001);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glEnd();
		glPopMatrix();
		// Back Face
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2002);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glEnd();
		glPopMatrix();
		// Bottom Face
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2004);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glEnd();
		glPopMatrix();
		// Right face
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2005);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glEnd();
		glPopMatrix();
		// Left Face
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,2006);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
		glPopMatrix();
		// Top Face
		glPushMatrix();
		glTranslated(0.0,TransB,TransC);  //1.3  .3
		glRotated(AngleTop,1,0,0);  //-70 /ANGLETOP
		glBindTexture(GL_TEXTURE_2D,2006);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.f);
		glEnd();
		glPopMatrix();
	




	/*first Bar coming out of the box*/
	//glFlush();
		glColor3f(.7,.7,3.0);
	glPushMatrix();
	glTranslated(1.2, -0.05, 0.0);   //1.2,0.1,0.39
	glRotated(45,0,0,1);
	glRotated(-55.0,0,1,0);
	glScaled(0.1,0.1,0.1);
	handle1();
	glPopMatrix();


	glPushMatrix();
	glTranslated(1.5,0,0);
	glRotated(xAngle, 1.0,0.0,0.0);
	glRotated(yAngle, 0.0,1.0,0.0);
	glScaled(0.1,0.1,0.1);
	handle();
	glPopMatrix();


	glColor3f(0.7,2.7,0.7);	
	glPushMatrix();
	glTranslated(0.0,Rise,0.0);//-0.4

	//head
	glPushMatrix();
	glTranslated(0.0,1.0,0.0);
	head();
	glPopMatrix();	

	//neck
	glPushMatrix();
	glTranslated(0.0,0.7,0.0);
	neck();
	glPopMatrix();


	//body
	glPushMatrix();
	glTranslated(0.0,0.175,0.0);
	body();
	glPopMatrix();

	//arm
	glPushMatrix();
	glRotated(90.0,0,1,0);
	glTranslated(0.0,0.3,0.0);
	glScaled(0.1,0.1,0.1);
	arm();
	glPopMatrix();
	glPopMatrix(); 

	//Coil
	glColor3f(0.7,0.7,3.0); //blue
	glPushMatrix();
	glTranslated(0.0,stretch,0.0);  //0.7
	glRotated(-95.0,0,0,1);
	glRotated(10.0,1,0,0);
	glRotated(90.0,0,1,0);
	glScaled(sx,sy,sz);  //.2 .2 .2
	#define PI 3.14159
	#define N 100
	float step=0;
	float zstep=0;


	step = 6.0*PI/(float)N;  //2
	zstep = 6.0/(float)N;   //2
	glLineWidth(6.5);
	glBegin(GL_LINE_STRIP);
	for (int i=0; i<N; i++)
	glVertex3f(2.0*sin(step*(float)i),2.0*cos(step*(float)i),-1.0+zstep*(float)i);
	glEnd();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}



void renderBitmapCharacher(float x, float y, float z, void *font,char *string) // displays bitmap fonts
{
  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
  }
}

 
void alphabet() // creating my alphabet
{
	glColor3f(1.0f,0.0f,0.0f);

	glPushMatrix();
	glLineWidth(5);
	glTranslated(-1,1,1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'A');
	glPopMatrix();
  
	glColor3f(0.0f,1.0f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.7,1,.7);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'B');
	glPopMatrix();

	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.4,1,.4);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'C');
	glPopMatrix();

	glColor3f(0.5f,0.5f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.1,1,.1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'D');
	glPopMatrix();

	glColor3f(0.0f,0.5f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.2,1,-.2);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'E');
	glPopMatrix();

	glColor3f(0.5f,0.0f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.5,1,-.5);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'F');
	glPopMatrix();

	glColor3f(1.0f,0.0f,0.0f);

	glPushMatrix();
	glLineWidth(5);
	glTranslated(-1.1,.25,1.1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'G');
	glPopMatrix();
  
	glColor3f(0.0f,1.0f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.75,.25,.75);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'H');
	glPopMatrix();

	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.34,.25,.34);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'I');
	glPopMatrix();

	glColor3f(0.5f,0.5f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.1,.25,.1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'J');
	glPopMatrix();

	glColor3f(0.0f,0.5f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.25,.25,-.25);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'K');
	glPopMatrix();

	glColor3f(0.5f,0.0f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.6,.25,-.6);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'L');
	glPopMatrix();


	glColor3f(1.0f,0.0f,0.0f);

	glPushMatrix();
	glLineWidth(5);
	glTranslated(-1.1,-.5,1.1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'M');
	glPopMatrix();
  
	glColor3f(0.0f,1.0f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.75,-.5,.75);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'N');
	glPopMatrix();

	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.4,-.5,.4);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'O');
	glPopMatrix();

	glColor3f(0.5f,0.5f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.05,-.5,.05);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'P');
	glPopMatrix();

	glColor3f(0.0f,0.5f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.3,-.5,-.3);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'Q');
	glPopMatrix();

	glColor3f(0.5f,0.0f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.65,-.5,-.65);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'R');
	glPopMatrix();

	glColor3f(1.0f,0.0f,0.0f);

	glPushMatrix();
	glLineWidth(5);
	glTranslated(-1.1,-1.2,1.1);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'S');
	glPopMatrix();
  
	glColor3f(0.0f,1.0f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.75,-1.2,.75);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'T');
	glPopMatrix();

	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.4,-1.2,.4);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'U');
	glPopMatrix();

	glColor3f(0.5f,0.5f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.05,-1.2,.05);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'V');
	glPopMatrix();

	glColor3f(0.0f,0.5f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.3,-1.2,-.3);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'W');
	glPopMatrix();

	glColor3f(0.5f,0.0f,0.5f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(.65,-1.2,-.65);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'X');
	glPopMatrix();


	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.4,-1.8,.4);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'Y');
	glPopMatrix();

	glColor3f(0.5f,0.5f,0.0f);
	glPushMatrix();
	glLineWidth(5);
	glTranslated(-.05,-1.8,.05);
	glRotated(45,0,1,0); 
	glScaled(.004,.004,.4);
	glutStrokeCharacter(GLUT_STROKE_ROMAN,'Z');
	glPopMatrix();
}

void twocone() // part of icecream animation
 {
	glColor3f(.1,.5,.5);
	glPushMatrix();
	glTranslated(.55,-.2,-.45);
	glRotated(45,0,1,0);
	glRotated(50,1,0,0);
	glScaled(.3,.3,1);
	glutSolidCone(.5,.5,20,20);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(.5,-.1,-.5);
	glutSolidSphere(.2,20,20);
	glPopMatrix();

}

void threecone() // part of icecream animation
{
		glColor3f(.1,.5,.5);
		glPushMatrix();
		glTranslated(0,-.2,0);
		glRotated(45,0,1,0);
		glRotated(50,1,0,0);
		glScaled(.3,.3,1);
		glutSolidCone(.5,.5,20,20);
		glPopMatrix();

		glColor3f(1,1,1);
		glPushMatrix();
		glTranslated(0,-.1,0);
		glutSolidSphere(.2,20,20);
		glPopMatrix();

		glColor3f(.4,.4,0);
		glPushMatrix();
		glTranslated(0,.3,0);
		glutSolidSphere(.2,20,20);
		glPopMatrix();

}

void movethreecone() //part of icecream animation
{
	glPushMatrix();
	threecone();
	glPopMatrix();

	for(double i = 0; i >= -.5; i = i - .005)
	{
		glPushMatrix();
		glTranslated(i,0,i*(-1));
		threecone();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Sleep(10);
	}
	glPushMatrix();
	glTranslated(-.5,0,.5);
	threecone();
	glPopMatrix();

	glColor3f(.9,.5,.5);
	glPushMatrix();
	glTranslated(-.5,1.6,.5);
	glutSolidSphere(.2,20,20);
	glPopMatrix();

	for(double j = 1.6; j >= .7; j = j - .005)
	{
	glPushMatrix();
	glTranslated(-.5,0,.5);
	threecone();
	glPopMatrix();

	glColor3f(.9,.5,.5);
	glPushMatrix();
	glTranslated(-.5,j,.5);
	glutSolidSphere(.2,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Sleep(10);
	}

	glPushMatrix();
	glTranslated(-.5,0,.5);
	threecone();
	glPopMatrix();

	glColor3f(.9,.5,.5);
	glPushMatrix();
	glTranslated(-.5,.7,.5);
	glutSolidSphere(.2,20,20);
	glPopMatrix();
	Sleep(1000);
}

void twoconeplace() // part of icecream animation
{

	for(double i = 1.6; i >= .3; i = i - .005)
	{
		glColor3f(.1,.5,.5);
		glPushMatrix();
		glTranslated(0,-.2,0);
		glRotated(45,0,1,0);
		glRotated(50,1,0,0);
		glScaled(.3,.3,1);
		glutSolidCone(.5,.5,20,20);
		glPopMatrix();

		glColor3f(1,1,1);
		glPushMatrix();
		glTranslated(0,-.1,0);
		glutSolidSphere(.2,20,20);
		glPopMatrix();

		glColor3f(.4,.4,0);
		glPushMatrix();
		glTranslated(0,i,0);
		glutSolidSphere(.2,20,20);
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
Sleep(10);
	}
	glPushMatrix();
	movethreecone();
	glPopMatrix();

}

void moveconetwo() //part of icecream animation
{

	for(double i = 0 ; i >= -.5 ; i = i - .005)
	{
		glPushMatrix();
		glTranslated(i,0,i*(-1));
		twocone();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Sleep(10);
	}
	glPushMatrix();
	twoconeplace();
	glPopMatrix();

}

void Aicone()  // Icecream animation
{

	for(double i = 1.5; i >= -.1; i = i - .005)
	{

	glColor3f(.1,.5,.5);
	glPushMatrix();
	glTranslated(.55,-.2,-.45);
	glRotated(45,0,1,0);
	glRotated(50,1,0,0);
	glScaled(.3,.3,1);
	glutSolidCone(.5,.5,20,20);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(.5,i,-.5);
	glutSolidSphere(.2,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(10);
	}

	glPushMatrix();
	twocone();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();

	glPushMatrix();
	moveconetwo();
	glPopMatrix();

}
void bat() // the BAT
{
	glPushMatrix();
	glScaled(.3,.3,1.2);
	glutSolidCone(.3,.5,20,20);
	glPopMatrix();

	glPushMatrix();
	glScaled(.15,.15,.15);
	glTranslated(-.1,0,3.6);
	glutSolidTorus(.3,.1,10,10);
	glPopMatrix();

	glPushMatrix();
	glTranslated(.02,.02,-.035);
	glutSolidSphere(.095,20,20);
	glPopMatrix();

}

void Rollingball() // Bat and Ball animation
{
	for(float i = -1.50; i <= 1.50; i = i + 0.01)
	{		glColor3f(1,1,1);
		glPushMatrix();
		glTranslated(i,0,(-1*i));
		glutSolidSphere(.1,20,20);
		glPopMatrix();
		glColor3f(.8,.8,.4);
		glPushMatrix();
		glTranslated(1.6,0,-1.6);
		bat();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Sleep(10);
	}
	float r = 0;
	for(float b = -1.50; b <= 1.50; b = b + 0.01)
	{

		glColor3f(1,1,1);
		glPushMatrix();
		glTranslated(-1*b,r,b);
		glutSolidSphere(.1,20,20);
		glPopMatrix();

		r = r + .01;

		glColor3f(.8,.8,.4);
		glPushMatrix();
		glTranslated(1.6,0,-1.6);
		bat();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Sleep(10); 
	}
}

void snowman() // snowman with nose
{

	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,1.15,0);
	gluSphere(mystuff,.3,10,10);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,.25,0);
	gluSphere(mystuff,.6,10,10);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,-1.4,0);
	gluSphere(mystuff,.9,10,10);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.4,1.5,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.5,.4);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.4,1.32,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.32,.4);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.35,1.23,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.23,.35);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.2,1.15,.2);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(1,.5,0);
	glPushMatrix();
	glTranslated(.3,1.35,.3);
	glRotated(15,0,1,0);
	glScaled(.3,.3,.8);
	glutSolidCone(.15,.5,20,20);
	glPopMatrix();
}

void nsnowman() // snowman with no nose
{
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,1.15,0);
	gluSphere(mystuff,.3,10,10);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,.25,0);
	gluSphere(mystuff,.6,10,10);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,-1.4,0);
	gluSphere(mystuff,.9,10,10);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.4,1.5,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.5,.4);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.4,1.32,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.32,.4);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.35,1.23,.25);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.25,1.23,.35);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

	glColor3f(0,0,0);
	glPushMatrix();
	glTranslated(.2,1.15,.2);
	glutSolidSphere(.03,20,20);
	glPopMatrix();

}
void candy() // candy
{
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(-.2,-.25,-.2);
	glScaled(.14,2,.14);
	gluCylinder(mystuff,.2,.2,.5,30,10);
	glPopMatrix();

	glColor3f(1,0,0);
	glPushMatrix();
	glScaled(.2,.2,.2);
	glTranslated(0,1.5,0);
	glutSolidSphere(1,20,20);
	glPopMatrix();
}

void heart() // the heart
{
	glPushMatrix();
	glRotated(45,0,1,0);
	glutSolidCone(.3,.5,20,20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(.09,.095,-.09);
	glutSolidSphere(.23,20,20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-.09,.095,.09);
	glutSolidSphere(.23,20,20);
	glPopMatrix();

}

void aniYoyo() // YO YO Animation
{
	int f = 0;
	while(f < 3)
	{
	for(float i = -2;i <=1.99; i = i+.1)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(0,1,0);
	glPushMatrix();
	glTranslated(-.07,i,.07);
	glScaled(.1,.5,.1);
	glutSolidSphere(1,20,20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(.07,i,-.07);
	glScaled(.1,.5,.1);
	glutSolidSphere(1,20,20);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(0,i,0);
	glVertex3f(0,2,0);
	glEnd();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
	Sleep(30);
	}

	for(i = 2;i >= -1.99; i = i-.1)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(0,1,0);
	glPushMatrix();
	glTranslated(-.07,i,.07);
	glScaled(.1,.5,.1);
	glutSolidSphere(1,20,20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(.07,i,-.07);
	glScaled(.1,.5,.1);
	glutSolidSphere(1,20,20);
	glPopMatrix();

	glColor3f(1,1,1);
	glPushMatrix();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(0,i,0);
	glVertex3f(0,2,0);
	glEnd();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
	Sleep(30);
	}
	++f;
	}
}

void anheart() // Heart Animation
{
	float j = 1;
	for(float i = 0; i <= 20; ++i)
	{
		glPushMatrix();
		glScaled(j,j,j);
		heart();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		if(j == 1)
		{	j = 2;}
		else
		{	j = 1;}
		
		for(int r = 0; r <= 50000000; ++r);

	}
	Sleep(1000);

}

void clocker()  //Clock for Time animation
{
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);
	
	glPushMatrix();
	glRotated(40,0,1,0);
	glRotated(-30,1,0,0);
	gluDisk(mystuff,.07,1,20,20);
	glPopMatrix();
	
	glColor3f(1,0,0);
	glPushMatrix();
	glTranslated(.05,.25,.05);
	glScaled(.15,.7,.15);
	glutSolidCube(.5);
	glPopMatrix();


	glColor3f(0,1,0);
	glPushMatrix();
	glTranslated(.01,.4,.01);
	glScaled(.1,1.5,.1);
	glutSolidCube(.5);
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(-.05,.85,.05,(void *)font,"12");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.25,.7,-.25,(void *)font,"1");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.45,.41,-.45,(void *)font,"2");
	glPopMatrix();


	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.8,.22,-.22,(void *)font,"3");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.75,-.28,-.15,(void*)font,"4");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.72,-.48,.15,(void*)font,"5");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.5,-.6,.5,(void *)font,"6");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(.16,-.48,.73,(void *)font,"7");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(-.18,-.28,.78,(void *)font,"8");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(-.22,.24,.89,(void *)font,"9");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(-.5,.41,.5,(void*)font,"10");
	glPopMatrix();

	glColor3f(1,0,1);
	glPushMatrix();
	renderBitmapCharacher(-.27,.72,.27,(void*)font,"11");
	glPopMatrix();
}

void legoman() //The lego man
{
	glColor3f(00,0,1);	
	glPushMatrix();
	glTranslated(0.0,-.2,0.0);//-0.4
	glRotated(30,0,1,0);

	//head
	glPushMatrix();
	glTranslated(0.0,1.0,0.0);
	head();
	glPopMatrix();	

	//neck
	glPushMatrix();
	glTranslated(0.0,0.7,0.0);
	neck();
	glPopMatrix();


	//body
	glPushMatrix();
	glTranslated(0.0,0.175,0.0);
	body();
	glPopMatrix();

	//arm
	glPushMatrix();
	glRotated(90.0,0,1,0);
	glTranslated(0.0,0.3,0.0);
	glScaled(0.1,0.1,0.1);
	arm();

	glPopMatrix();

	glPushMatrix();
	glTranslated(.1,-.4,.1);
	glScaled(.25,1,.25);
	glutSolidCube(.5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-.1,-.4,.1);
	glScaled(.25,1,.25);
	glutSolidCube(.5);
	glPopMatrix();
	
	glPopMatrix();
}

void myMouse(int button, int state, int x, int y)  // MY Mouse fuction
{

	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);
	checkclick = 1;

	int channel = -1;
	int track=0;
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	FSOUND_STREAM * stream;

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 202 && x >= 154 && 640-y <= 560 && 640-y >= 515) //LETTER A
	{	//Airplan,Arrow,ape
	char filename[] = "sounds/A.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	awords();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(10000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 251 && x >= 209 && 640-y <= 562 && 640-y >= 517) //LETTER B
	{
	 char filename[] = "sounds/B.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //     BAT and Ball Animation
	glPushMatrix();
	glTranslated(-1.5,0,1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Rollingball();
	glPushMatrix();
	glTranslated(1.5,0,-1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 297 && x >= 257 && 640-y <= 559 && 640-y >= 515) //LETTER C
	{
	 char filename[] = "sounds/C.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = 0;i <= 2; i = i + .5)
	{
	// Candy Animation
	glPushMatrix();
	glTranslated(i,0,-1*i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(-1*i,0,i);
	candy();
	glPopMatrix();


	Sleep(100);


	glPushMatrix();
	glTranslated(i,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(i,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,i);
	candy();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();

	Sleep(100);
	}
	glutSwapBuffers();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 348 && x >= 306 && 640-y <= 558 && 640-y >= 510) //LETTER D
	{ // Diamond 
	Mesh Face;
	 char filename[] = "sounds/D.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	for(int i = 0; i < 400; ++i)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,.2,.1);
	glPushMatrix();
	glRotated(i,0,0,1);
	glRotated(-90-i,1,0,0);
	Face.readFile("mesh/DIAMOND.3vn");
	Face.Draw();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(10);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 386 && x >= 351 && 640-y <= 558 && 640-y >= 510) //LETTER E
	{
	 char filename[] = "sounds/E.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//EGG
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,-.5,0);
	glScaled(2,2,2);
	for(int i = 0; i <= 70; ++i)
	{
	glPushMatrix();
	glRotated(10+i,1,0,0);
	glRotated(30+i,0,1,0);
	glRotated(50+i,0,0,1);
	glScaled(.3,.5,.3);
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(100);
	}
	glPopMatrix();
	Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 431 && x >= 398 && 640-y <= 558 && 640-y >= 510) //LETTER F
	{
	 char filename[] = "sounds/F.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//face animation
		glColor3f(.5,.7,.8);
		Mesh Face;
		glPushMatrix();
		glRotated(-100,1,0,0);
		glRotated(40,0,0,1);
		glScaled(2,2,2);

	for(int i = 0; i <= 5; ++i)
	{
	
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/face.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();

		Sleep(500);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/frown.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
		Sleep(500);
	}
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 185 && x >= 145 && 640-y <= 475 && 640-y >= 430) //LETTER G
	{
	 char filename[] = "sounds/G.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// GROWING LEGO MAN
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = .2; i <= 2;i = i + .1)
	{
	glPushMatrix();
	glScaled(i,i,i);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(500);
	}
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 242 && x >= 201 && 640-y <= 475 && 640-y >= 430) //LETTER h
	{
	 char filename[] = "sounds/H.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Heart Animation
		glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glColor3f(1,0,0);
	glPushMatrix();
	anheart();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 289 && x >= 251 && 640-y <= 475 && 640-y >= 428) //LETTER I
	{
	 char filename[] = "sounds/I.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //   ICE CREAM CONE animation	

	glPushMatrix();
	Aicone();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 330 && x >= 300 && 640-y <= 475 && 640-y >= 430) //LETTER J
	{
	 char filename[] = "sounds/J.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Jackinthebox animation
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glPushMatrix();
	display();
	glPopMatrix();
	Sleep(3000);
	glPushMatrix();
	popjack();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(2000);
	glPopMatrix();
	xSpeed = -.5; ySpeed = 0; xAngle = 1; yAngle = 0;  //17//-22 //-35
 AngleX = 17; AngleY = -22; AngleTop = 0; TransB = 0; TransC = 0; Rise = -0.4; sx=0.2; sy=0.2; sz=0.2; stretch=0.7;
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 393 && x >= 360 && 640-y <= 475 && 640-y >= 430) //LETTER K
	{
	 char filename[] = "sounds/K.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Kite
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(-.5,.5,-.5);
	glRotated(50,1,0,0);
	glRotated(-50,0,0,1);
	glColor3f(0,0,1);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(-1,0,-1);
	glVertex3f(1,0,-1);
	glVertex3f(1.5,0,1.5);
	glVertex3f(-1,0,1);
	glEnd();
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(1.5,0,1.5);
	glVertex3f(1.5,0,1.6);
	glVertex3f(1.5,0,1.7);
	glVertex3f(1.5,0,1.8);
	glVertex3f(1.6,0,1.9);
	glVertex3f(1.7,0,2);
	glVertex3f(1.8,0,2);
	glVertex3f(1.9,0,2.1);
	glVertex3f(2,0,2.2);
	glVertex3f(2,0,2.3);
	glVertex3f(2.1,0,2.4);
	glVertex3f(2.2,0,2.5);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 443 && x >= 411 && 640-y <= 475 && 640-y >= 430) //LETTER L
	{
	 char filename[] = "sounds/L.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//LAMP
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScaled(2,2,2);
	glColor3f(1,1,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-110,1,0,0);
	glTranslated(0,-.35,0);
	glScaled(.3,.3,.3);
	gluCylinder(mystuff,.5,.2,1,5,10);
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-85,1,0,0);
	glTranslated(0,-.34,0);
	glScaled(.15,.15,.15);
	gluCylinder(mystuff,.5,.2,1,15,15);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,-.35,0);
	glScaled(.1,.1,.1);
	glutSolidSphere(1,15,15);
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 187 && x >= 146 && 640-y <= 393 && 640-y >= 349) //LETTER M
	{
	 char filename[] = "sounds/M.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// M- mouse 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	mouse();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 241 && x >= 203 && 640-y <= 393 && 640-y >= 346) //LETTER N
	{
	 char filename[] = "sounds/N.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//Numbers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i <= 5; i++)
	{
	glColor3f(0,1,1);
	glPushMatrix();
	renderBitmapCharacher((-1)*(rand()%2),rand()%2,rand()%2,(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glColor3f(1,1,0);
	glPushMatrix();
	renderBitmapCharacher(rand()%2,(-1)*(rand()%2),(-1)*(rand()%2),(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(1000);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 297 && x >= 257 && 640-y <= 393 && 640-y >= 346) //LETTER O
	{ 
			 char filename[] = "sounds/O.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// ORANGE
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glColor3f(1,.5,0);
	glPushMatrix();
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 348 && x >= 311 && 640-y <= 393 && 640-y >= 346) //LETTER P
	{ 
	char filename[] = "sounds/P.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Pencal
		for(float i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(0,0,i);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,i+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 0; i <= 2; i = i +.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,0);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(i+.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,(2-i));
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(2.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(2.16,0,.16);
		glVertex3f(i+.16,0,(2-i)+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}
		Sleep(100);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 402 && x >= 363 && 640-y <= 393 && 640-y >= 346) //LETTER Q
	{
	char filename[] = "sounds/Q.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//QUILT
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	quilt();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 456 && x >= 417 && 640-y <= 393 && 640-y >= 346) //LETTER R
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	char filename[] = "sounds/R.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	// racing animation with race cars
	for(float i= -1.5; i <= 1.5; i = i + .01) 
	{
		double j = 0;
	glPushMatrix();
	glTranslated(i,.5,-i);
	rcar();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glTranslated(j+i,-.5,(-1)*(j+i));
	bcar();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	j = j + .0001;
	Sleep(20);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 183 && x >= 146 && 640-y <= 314 && 640-y >= 272) //LETTER S
	{
	char filename[] = "sounds/S.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// snowman animation
		for(int i = 0; i<= 10; ++i)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		snowman();	
		glPopMatrix();
		glFlush();
		glutSwapBuffers();

		Sleep(300);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		nsnowman();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(300);
		}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 234 && x >= 198 && 640-y <= 315 && 640-y >= 272) //LETTER T
	{ 
		char filename[] = "sounds/T.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// T for TIME
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	clocker();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}



	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 295 && x >= 255 && 640-y <= 315 && 640-y >= 272) //LETTER U
	{ // U - UPsidedown
	char filename[] = "sounds/U.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	// U is for UpsideDown
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotated(175,1,0,0);
	glRotated(100,0,1,0);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(7000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 349 && x >= 306 && 640-y <= 315 && 640-y >= 272) //LETTER V
	{
	char filename[] = "sounds/V.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//V - Valley
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	valley();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 410 && x >= 361 && 640-y <= 315 && 640-y >= 272) //LETTER W
	{ // W - Wall
		char filename[] = "sounds/W.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	wall();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 450 && x >= 413 && 640-y <= 315 && 640-y >= 271) //LETTER X
	{
		char filename[] = "sounds/X.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//Xray animation
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	xray();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 294 && x >= 250 && 640-y <= 248 && 640-y >= 203) //LETTER Y
	{
		char filename[] = "sounds/Y.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//animated Yo-yo
		glPushMatrix();
		aniYoyo();
		glPopMatrix();
		Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 343 && x >= 304 && 640-y <= 253 && 640-y >= 202) //LETTER Z
	{
		char filename[] = "sounds/Z.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Z is for ZIG-ZAG
		float j = 0;
	for(float i = -2; i <= -1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}

	for(i = -1; i <= 1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j - .01;
	}
	for(i = 1; i <= 2; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x <= 84 && x >= 13 && 640-y <= 230 && 640-y >= 191) 
	{
	
	{
	char filename[] = "sounds/A.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//Airplan,Arrow,ape
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	awords();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(10000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/B.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //     BAT and Ball Animation
	glPushMatrix();
	glTranslated(-1.5,0,1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Rollingball();
	glPushMatrix();
	glTranslated(1.5,0,-1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/C.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = 0;i <= 2; i = i + .5)
	{
	// Candy Animation
	glPushMatrix();
	glTranslated(i,0,-1*i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(-1*i,0,i);
	candy();
	glPopMatrix();


	Sleep(100);


	glPushMatrix();
	glTranslated(i,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(i,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,i);
	candy();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();

	Sleep(100);
	}
	glutSwapBuffers();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{// Diamond
		FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);	
	char filename[] = "sounds/D.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	Mesh Face;
	for(int i = 0; i < 400; ++i)
	{

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,.2,.1);
	glPushMatrix();
	glRotated(i,0,0,1);
	glRotated(-90-i,1,0,0);
	Face.readFile("mesh/DIAMOND.3vn");
	Face.Draw();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(10);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/E.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		//EGG
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,-.5,0);
	glScaled(2,2,2);
	for(int i = 0; i <= 70; ++i)
	{
	glPushMatrix();
	glRotated(10+i,1,0,0);
	glRotated(30+i,0,1,0);
	glRotated(50+i,0,0,1);
	glScaled(.3,.5,.3);
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(100);
	}
	glPopMatrix();
	Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/F.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//face animation
		glColor3f(.5,.7,.8);
		Mesh Face;
		glPushMatrix();
		glRotated(-100,1,0,0);
		glRotated(40,0,0,1);
		glScaled(2,2,2);

	for(int i = 0; i <= 5; ++i)
	{
	
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/face.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();

		Sleep(500);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/frown.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
		Sleep(500);
	}
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}



	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/G.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// GROWING LEGO MAN
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = .2; i <= 2;i = i + .1)
	{
	glPushMatrix();
	glScaled(i,i,i);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(500);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/H.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// Heart Animation
		glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glColor3f(1,0,0);
	glPushMatrix();
	anheart();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/I.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //   ICE CREAM CONE animation	
	glPushMatrix();
	Aicone();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/J.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// Jackinthebox animation
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glPushMatrix();
	display();
	glPopMatrix();
	Sleep(3000);
	glPushMatrix();
	popjack();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(2000);
	glPopMatrix();
	xSpeed = -.5; ySpeed = 0; xAngle = 1; yAngle = 0;  //17//-22 //-35
 AngleX = 17; AngleY = -22; AngleTop = 0; TransB = 0; TransC = 0; Rise = -0.4; sx=0.2; sy=0.2; sz=0.2; stretch=0.7;
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/K.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// Kite
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(-.5,.5,-.5);
	glRotated(50,1,0,0);
	glRotated(-50,0,0,1);
	glColor3f(0,0,1);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(-1,0,-1);
	glVertex3f(1,0,-1);
	glVertex3f(1.5,0,1.5);
	glVertex3f(-1,0,1);
	glEnd();
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(1.5,0,1.5);
	glVertex3f(1.5,0,1.6);
	glVertex3f(1.5,0,1.7);
	glVertex3f(1.5,0,1.8);
	glVertex3f(1.6,0,1.9);
	glVertex3f(1.7,0,2);
	glVertex3f(1.8,0,2);
	glVertex3f(1.9,0,2.1);
	glVertex3f(2,0,2.2);
	glVertex3f(2,0,2.3);
	glVertex3f(2.1,0,2.4);
	glVertex3f(2.2,0,2.5);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/L.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//LAMP
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScaled(2,2,2);
	glColor3f(1,1,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-110,1,0,0);
	glTranslated(0,-.35,0);
	glScaled(.3,.3,.3);
	gluCylinder(mystuff,.5,.2,1,5,10);
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-85,1,0,0);
	glTranslated(0,-.34,0);
	glScaled(.15,.15,.15);
	gluCylinder(mystuff,.5,.2,1,15,15);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,-.35,0);
	glScaled(.1,.1,.1);
	glutSolidSphere(1,15,15);
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/M.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// M- mouse 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	mouse();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/N.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		//Numbers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i <= 6; i++)
	{
	glColor3f(0,1,1);
	glPushMatrix();
	renderBitmapCharacher((-1)*(rand()%2),rand()%2,rand()%2,(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glColor3f(1,1,0);
	glPushMatrix();
	renderBitmapCharacher(rand()%2,(-1)*(rand()%2),(-1)*(rand()%2),(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(1000);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{ 
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/O.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// ORANGE
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glColor3f(1,.5,0);
	glPushMatrix();
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{ 
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/P.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// Pencal
		for(float i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(0,0,i);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,i+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 0; i <= 2; i = i +.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,0);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(i+.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,(2-i));
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(2.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(2.16,0,.16);
		glVertex3f(i+.16,0,(2-i)+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}
		Sleep(100);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/Q.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		//QUILT
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	quilt();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/R.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// racing animation with race cars
	for(float i= -1.5; i <= 1.5; i = i + .01) 
	{
		double j = 0;
	glPushMatrix();
	glTranslated(i,.5,-i);
	rcar();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glTranslated(j+i,-.5,(-1)*(j+i));
	bcar();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	j = j + .0001;
	Sleep(20);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/S.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// snowman animation
		for(int i = 0; i<= 10; ++i)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		snowman();	
		glPopMatrix();
		glFlush();
		glutSwapBuffers();

		Sleep(300);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		nsnowman();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(300);
		}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/T.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// T for TIME
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	clocker();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}



	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
		char filename[] = "sounds/U.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
		// U is for UpsideDown
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotated(175,1,0,0);
	glRotated(100,0,1,0);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(7000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	char filename[] = "sounds/V.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//V - Valley
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	valley();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}

	{ 
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/W.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// W - Wall
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	wall();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/X.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//Xray animation
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	xray();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}


	{
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/Y.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//animated Yo-yo
		glPushMatrix();
		aniYoyo();
		glPopMatrix();
		Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}	



		{

	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
			char filename[] = "sounds/Z.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	
			// Z is for ZIG-ZAG
		float j = 0;
	for(float i = -2; i <= -1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}

	for(i = -1; i <= 1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j - .01;
	}
	for(i = 1; i <= 2; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	
}

	}
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutPostRedisplay();

}

void myKeyboard(unsigned char key, int x, int y)
{
	checkclick = 1;
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);
	int channel = -1;
	int track=0;
	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);
	FSOUND_STREAM * stream;

  switch(key)
  {	
  case 'A':
  case 'a':		{
	  	char filename[] = "sounds/A.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  //Airplan,Arrow,ape
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	awords();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;	}

  case 'B':
  case 'b':		{
	char filename[] = "sounds/B.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  
	  glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //     BAT and Ball Animation
	glPushMatrix();
	glTranslated(-1.5,0,1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Rollingball();
	glPushMatrix();
	glTranslated(1.5,0,-1.5);
	glutSolidSphere(.1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;	}


  case 'C':
  case 'c': 	{
	char filename[] = "sounds/C.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = 0;i <= 2; i = i + .5)
	{
	// Candy Animation
	glPushMatrix();
	glTranslated(i,0,-1*i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(-1*i,0,i);
	candy();
	glPopMatrix();


	Sleep(100);


	glPushMatrix();
	glTranslated(i,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,i);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(i,i,0);
	candy();
	glPopMatrix();


	Sleep(100);

	glPushMatrix();
	glTranslated(0,-1*i,i);
	candy();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();

	Sleep(100);
	}
	glutSwapBuffers();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
	}   

  case 'D':
  case 'd': 	{
	 	char filename[] = "sounds/D.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200); 
	  // Diamond 
	Mesh Face;
	for(int i = 0; i < 400; ++i)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,.2,.1);
	glPushMatrix();
	glRotated(i,0,0,1);
	glRotated(-90-i,1,0,0);
	Face.readFile("mesh/DIAMOND.3vn");
	Face.Draw();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(10);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
break;	}  

  case 'E':
  case 'e': 	{
	 	char filename[] = "sounds/E.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200); 
	  //EGG
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(0,-.5,0);
	glScaled(2,2,2);
	for(int i = 0; i <= 70; ++i)
	{
	glPushMatrix();
	glRotated(10+i,1,0,0);
	glRotated(30+i,0,1,0);
	glRotated(50+i,0,0,1);
	glScaled(.3,.5,.3);
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(100);
	}
	glPopMatrix();
	Sleep(1000);
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
} 

  case 'F':   
  case 'f':	{

	char filename[] = "sounds/F.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//face animation
		glColor3f(.5,.7,.8);
		Mesh Face;
		glPushMatrix();
		glRotated(-100,1,0,0);
		glRotated(40,0,0,1);
		glScaled(2,2,2);

	for(int i = 0; i <= 5; ++i)
	{
	
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/face.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();

		Sleep(500);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("mesh/frown.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
		Sleep(500);
	}
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;}
	  
  case 'G':
  case 'g':	{
	  	char filename[] = "sounds/G.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // GROWING LEGO MAN
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(float i = .2; i <= 2;i = i + .1)
	{
	glPushMatrix();
	glScaled(i,i,i);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(500);
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;}
	
  case 'H': 
  case 'h':	{
	  	char filename[] = "sounds/H.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // Heart Animation
		glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glColor3f(1,0,0);
	glPushMatrix();
	anheart();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix(); 
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	break;
}

  case 'I':
  case 'i':{

	char filename[] = "sounds/I.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 //   ICE CREAM CONE animation	
	glPushMatrix();
	Aicone();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glPopMatrix();
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	break;
}


  case 'J':
  case 'j':{
	 	char filename[] = "sounds/J.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200); 
	  // Jackinthebox animation
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glPushMatrix();
	display();
	glPopMatrix();
	Sleep(3000);
	glPushMatrix();
	popjack();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(2000);
	glPopMatrix();
	xSpeed = -.5; ySpeed = 0; xAngle = 1; yAngle = 0;  //17//-22 //-35
 AngleX = 17; AngleY = -22; AngleTop = 0; TransB = 0; TransC = 0; Rise = -0.4; sx=0.2; sy=0.2; sz=0.2; stretch=0.7;
	glDisable(GL_TEXTURE_2D);	
	FSOUND_Stream_Close(stream);
	FSOUND_Close(); break;
}
  case 'K':
  case 'k':	{
	  	char filename[] = "sounds/K.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // Kite
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(-.5,.5,-.5);
	glRotated(50,1,0,0);
	glRotated(-50,0,0,1);
	glColor3f(0,0,1);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(-1,0,-1);
	glVertex3f(1,0,-1);
	glVertex3f(1.5,0,1.5);
	glVertex3f(-1,0,1);
	glEnd();
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(1.5,0,1.5);
	glVertex3f(1.5,0,1.6);
	glVertex3f(1.5,0,1.7);
	glVertex3f(1.5,0,1.8);
	glVertex3f(1.6,0,1.9);
	glVertex3f(1.7,0,2);
	glVertex3f(1.8,0,2);
	glVertex3f(1.9,0,2.1);
	glVertex3f(2,0,2.2);
	glVertex3f(2,0,2.3);
	glVertex3f(2.1,0,2.4);
	glVertex3f(2.2,0,2.5);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
}

  case 'L':
  case 'l':{
	  	char filename[] = "sounds/L.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  //LAMP
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScaled(2,2,2);
	glColor3f(1,1,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-110,1,0,0);
	glTranslated(0,-.35,0);
	glScaled(.3,.3,.3);
	gluCylinder(mystuff,.5,.2,1,5,10);
	glPopMatrix();
	
	glColor3f(1,0,1);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-85,1,0,0);
	glTranslated(0,-.34,0);
	glScaled(.15,.15,.15);
	gluCylinder(mystuff,.5,.2,1,15,15);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,-.35,0);
	glScaled(.1,.1,.1);
	glutSolidSphere(1,15,15);
	glPopMatrix();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000); 
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
}

  case 'M':
  case 'm':{
	  	char filename[] = "sounds/M.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // M- mouse 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	mouse();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D); 
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
}

  case 'N':
  case'n':{	
	  	char filename[] = "sounds/N.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  //Numbers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i <= 5; i++)
	{
	glColor3f(0,1,1);
	glPushMatrix();
	renderBitmapCharacher((-1)*(rand()%2),rand()%2,rand()%2,(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glColor3f(1,1,0);
	glPushMatrix();
	renderBitmapCharacher(rand()%2,(-1)*(rand()%2),(-1)*(rand()%2),(void *)font,"1 2 3 4 5 6 7 8 9 0");
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Sleep(1000);
	}
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	
	break;
	}

  case 'O':
  case 'o':{ 
	  	char filename[] = "sounds/O.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // ORANGE
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glColor3f(1,.5,0);
	glPushMatrix();
	glutSolidSphere(1,20,20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000); 
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	break;
	}

  case 'P':
  case 'p':
	  { 
		  	char filename[] = "sounds/P.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		  // Pencal
		for(float i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(0,0,i);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,i+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 0; i <= 2; i = i +.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,0);
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(i+.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}

		for(i = 2; i >= 0; i = i -.1)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,(2-i));
		pencial();
		glPopMatrix();
		
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.16,0,.25);
		glVertex3f(2.16,0,.25);
		glEnd();
		glPopMatrix();
		glColor3f(1,1,1);
		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(.25,0,2.16);
		glVertex3f(.25,0,.16);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex3f(2.16,0,.16);
		glVertex3f(i+.16,0,(2-i)+.16);
		glEnd();
		glPopMatrix();

		glFlush();
		glutSwapBuffers();
		Sleep(100);
		}
		Sleep(100); 
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
		break;
	}

  case 'Q':
  case 'q':{
	  	char filename[] = "sounds/Q.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  //QUILT
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	quilt();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
		FSOUND_Stream_Close(stream);
	FSOUND_Close();
	
	break;
	}

  case 'R':
  case 'r':
	  {
	char filename[] = "sounds/R.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// racing animation with race cars
	for(float i= -1.5; i <= 1.5; i = i + .01) 
	{
		double j = 0;
	glPushMatrix();
	glTranslated(i,.5,-i);
	rcar();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glTranslated(j+i,-.5,(-1)*(j+i));
	bcar();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	j = j + .0001;
	Sleep(20);
	}
	Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	break;
	}


  case 'S':
  case 's':
	  {
	char filename[] = "sounds/S.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		// snowman animation
		for(int i = 0; i<= 10; ++i)
		{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		snowman();	
		glPopMatrix();
		glFlush();
		glutSwapBuffers();

		Sleep(300);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		nsnowman();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(300);
		}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
break;
	}

  case 'T':
  case 't':{ 
	  
	  // T for TIME
	char filename[] = "sounds/T.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	glPushMatrix();
	clocker();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000); 
	FSOUND_Stream_Close(stream);
	FSOUND_Close();	
	break;
	}



  case 'U':
  case 'u':
	  {
		 	char filename[] = "sounds/U.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200); 
		  // U is for UpsideDown
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotated(175,1,0,0);
	glRotated(100,0,1,0);
	legoman();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(6000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
	}


  case 'V':
  case 'v':
	  {
	char filename[] = "sounds/V.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
		//V - Valley
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	valley();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
	}

  case 'W':
  case 'w':
	  { // W - Wall
		char filename[] = "sounds/W.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	wall();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
	}


  case 'X':
  case 'x':
	  {
		char filename[] = "sounds/X.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	  
		  //Xray animation
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(45,0,1,0);
	glRotated(-25,1,0,0);
	xray();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	Sleep(5000);
	glDisable(GL_TEXTURE_2D);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;
}


  case 'Y':
  case 'y':
	  {	
		char filename[] = "sounds/Y.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);	  
		  //animated Yo-yo
		glPushMatrix();
		aniYoyo();
		glPopMatrix();
		Sleep(1000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
		break;
}	



  case 'Z':
  case 'z':{
	  	char filename[] = "sounds/Z.wav";
	stream = FSOUND_Stream_Open(filename, 0,0,0);
	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);
	FSOUND_SetVolume(channel,200);
	  // Z is for ZIG-ZAG
		float j = 0;
	for(float i = -2; i <= -1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}

	for(i = -1; i <= 1; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j - .01;
	}
	for(i = 1; i <= 2; i = i + .01)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glTranslated(i,0,j);
		bcar();
		glPopMatrix();
		glFlush();
		glutSwapBuffers();
		Sleep(10);
		j = j + .01;
	}
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
	break;	
}
	
	default: break;
  }	
  glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glutSwapBuffers();	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutPostRedisplay(); 
}

void Init()
{
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	glEnable(GL_LIGHT0);								
	glEnable(GL_LIGHTING);								
	glEnable(GL_COLOR_MATERIAL);


	pix[0].readBMPFile("image/box.bmp");
	pix[0].setTexture(2001);
	pix[1].readBMPFile("image/box2.bmp");
	pix[1].setTexture(2002);
	pix[2].readBMPFile("image/quilt.bmp");
	pix[2].setTexture(2003);
	pix[3].readBMPFile("image/box2.bmp");
	pix[3].setTexture(2004);
 	pix[4].readBMPFile("image/box2.bmp");
	pix[4].setTexture(2005);
	pix[5].readBMPFile("image/box2.bmp");
	pix[5].setTexture(2006);
	pix[6].readBMPFile("image/valley.bmp");
	pix[6].setTexture(2007);
	pix[7].readBMPFile("image/x-ray.bmp");
	pix[7].setTexture(2008);
	pix[8].readBMPFile("image/aexamples.bmp");
	pix[8].setTexture(2009);
	pix[9].readBMPFile("image/brickwall.bmp");
	pix[9].setTexture(2010);
	pix[10].readBMPFile("image/mouse.bmp");
	pix[10].setTexture(2011);



	FSOUND_SetOutput(-1);
	FSOUND_SetDriver(0);
	FSOUND_Init(44100, 32, 0);

	char filename[] = "sounds/ProgramIntro.wav";


	int channel = -1;
	int track=0;
	stream = FSOUND_Stream_Open(filename, 0,0,0);

	channel = FSOUND_Stream_Play(FSOUND_FREE,stream);

	FSOUND_SetVolume(channel,200);

}


void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glClearColor(0,0,0,1.0f);		
	glColor3f(1.0f,1.0f,1.0f);
	GLUquadricObj * mystuff;
	mystuff = gluNewQuadric();
	gluQuadricNormals(mystuff, GLU_SMOOTH);
	gluQuadricTexture(mystuff, GL_TRUE);
	gluQuadricDrawStyle(mystuff,GLU_FILL);


	glPushMatrix();
	glRotated(-25,1,0,0);
	glRotated(22,0,0,1);
	alphabet();
	glPopMatrix();

	glColor3f(1,0,0);
	glPushMatrix();
	renderBitmapCharacher(.2,0,3,(void *)font,"Tour");
	glPopMatrix();

	glColor3f(1,1,1);

	glFlush();

	glutSwapBuffers();

if(checkclick == 0)
{	
	
	Sleep(11000);
	FSOUND_Stream_Close(stream);
	FSOUND_Close();
}

} 
//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char **argv)
{
	Point3 e;
	Point3 l;
	Vector3 u;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // double buffering
	glutInitWindowSize(640,480);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("ABC's");
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutDisplayFunc(myDisplay);
//	glClearColor(1.0f,1.0f,1.0f,1.0f);  // background is white 
	glClearColor(0,0,0,1.0f);		// Background is black
	glColor3f(1.0f,1.0f,1.0f);			// set color of stuff 
	glViewport(0, 0, 640, 480);
	Init();
	e.set(4,4,4);
	l.set(0, 0, 0);
	u.set(0, 1, 0);
	cam.set(e, l, u); // make the initial camera
	cam.setShape(35.0f, 64.0f/48.0f, 0.5f, 100.0f);

	glutMainLoop();
}

