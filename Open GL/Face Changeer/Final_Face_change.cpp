#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
using namespace std;

#include <windows.h> //change if using xWindows
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>

//@@@@@@@@@@@@@@@@@@ Point3 class @@@@@@@@@@@@@@@@
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
	  Camera(){}; // default constructor
	void set(Point3 eye, Point3 look, Vector3 up); // like gluLookAt()
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

Camera cam; // global camera object
Mesh Face;

int random(int seed){
	float value;
	value=((rand())%seed);
	return value;
}

	
void myKeyboard(unsigned char key, int x, int y)
{
	if(key=='a' || key!='a')
	{
	int val;
	val=random(6);
	if(val==1){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("face.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
	}
	else if(val==2){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("smile.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
	}
	else if(val==3){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("smirk.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
	}
	else if(val==4){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("frown.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
	}
	else if(val==5){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("silly.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
	}
	else{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Face.readFile("kiss.txt");
		Face.Draw();
		glFlush();
		glutSwapBuffers();
		}
	}
   glutPostRedisplay(); // draw it again
}


void displaySolid()
{	
	int color=random(8);
	if(color==0){
	float f=27.8974;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.329412f,0.223529f,0.027451f, 1.0f }; // brass
	GLfloat mat_diffuse[] = {0.780392f, 0.568627f, 0.113725f, 1.0f };
	GLfloat mat_specular[] = {0.992157f, 0.941176f, 0.807843f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==1){
	float f=25.6;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.2125f,0.1275f,0.054f, 1.0f }; // bronze
	GLfloat mat_diffuse[] = {0.714f, 0.4284f, 0.18144f, 1.0f };
	GLfloat mat_specular[] = {0.393548f, 0.271906f, 0.166721f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==2){
	float f=76.8;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.25f,0.25f,0.25f, 1.0f }; // chrome
	GLfloat mat_diffuse[] = {0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat mat_specular[] = {0.774597f, 0.774597f, 0.774597f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==3){
	float f=12.8;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.19125f,0.0735f,0.0225f, 1.0f }; // copper
	GLfloat mat_diffuse[] = {0.7038f, 0.27048f, 0.0828f, 1.0f };
	GLfloat mat_specular[] = {0.25677f, 0.137622f, 0.086014f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==4){
	float f=51.2;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.24725f,0.1995f,0.0745f, 1.0f }; // gold
	GLfloat mat_diffuse[] = {0.75164f, 0.60648f, 0.22648f, 1.0f };
	GLfloat mat_specular[] = {0.628281f, 0.555802f, 0.366065f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==5){
	float f=9.84615;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.10588f,0.058824f,0.113725f, 1.0f }; // pewter
	GLfloat mat_diffuse[] = {0.427451f, 0.470588f, 0.541176f, 1.0f };
	GLfloat mat_specular[] = {0.3333f, 0.3333f, 0.521569f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==6){
	float f=51.2;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.19225f,0.19225f,0.19225f, 1.0f }; // silver
	GLfloat mat_diffuse[] = {0.50754f, 0.50754f, 0.50754f, 1.0f };
	GLfloat mat_specular[] = {0.508273f, 0.508273f, 0.508273f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	else if(color==7){
	float f=89.6;
	// set properties of the surface material
	GLfloat mat_ambient[] = { 0.23125f,0.23125f,0.23125f, 1.0f }; // polished silver
	GLfloat mat_diffuse[] = {0.2775f, 0.2775f, 0.2775f, 1.0f };
	GLfloat mat_specular[] = {0.773911f,0.773911f, 0.773911f, 1.0f };
	GLfloat mat_shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	// light sources
	GLfloat lightIntensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPosition[] = {2.0f, 6.0f, 3.0f, 0.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightIntensity);

	
	static bool first=true;
	if(first){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glFlush();
	glutSwapBuffers();
	first=false;
	}


}



void main(int argc, char** argv)
{

	Point3 e;
	Point3 l;
	Vector3 u;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // double buffering
	glutInitWindowSize(1024,768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Changing Faces");
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(displaySolid);
	glClearColor(1.0f, 1.0f, 1.0f,1.0f);  // background is white 
	glViewport(0, 5, 640, 480);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	e.set(0,-.75,-0.25);
	l.set(0,1.5,-1);
	u.set(0,1,0);
	cam.set(e, l, u); // make the initial camera
//	cam.setShape(30.0f, 1.3f, 0.5f, 50.0f);
	glutMainLoop();



}