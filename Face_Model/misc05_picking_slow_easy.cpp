// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <stack>
#include <mutex>
#include <sstream>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
using namespace std;
// Include AntTweakBar
//#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/tga.h>
#include <common/tga.c>
#include <common/ray_casting.h>

const int window_width = 1000, window_height = 1000;

//Nomrmal Vertex Struct withou UV cordinates
typedef struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}

};

//Vertex Struct with UV cordinates
typedef struct UVs {

	float Position[4];
	float Color[4];
	float Normal[3];
	float UV[2];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}

	void SetUV(float *coords) {
		UV[0] = coords[0];
		UV[1] = coords[1];
	}
};

// function prototypes
int initWindow(void);
void initOpenGL(void);
void loadObject(char*, glm::vec4, Vertex * &, GLuint* &, int);
void createVAOs(Vertex[], GLuint[], int);
void createVAO2(UVs[], GLuint[], int);
void createObjects(void);
void gridmakeone(float*, float*, Vertex[]);
void gridmaketwo(float*, float*, Vertex[]);
void calculateCam();
void SubInd();
void subDuv(UVs[]);
void subDivide(UVs[]);
void wireMesh(UVs[],GLuint[]);
void pickObject(void);
void moveVertex(int);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
int camera = 0;
float cam[] = { 0.0f, 0.0f, 14.14f };
float light1[] = { -10.0f, 0.0f, 10.0f };
float light2[] = { 10.0f, 0.0f, 10.0f };
float xb, zb = 0.0f;
int token = 0;
int pri = 0;
int first = 1;
int stick = 0;
int movi = 0;
int model = 1;
int Save = 1, Load = 1;
int subd = 0;
int Mes = 0;
int ani = 0;
int ani_s = 0;
int MoveX = 0;

ofstream Outfile;

GLfloat yLimit = 0;
GLfloat zLimit = 0;
GLfloat arm1length = 1.382f;
GLfloat arm2length = 1.0f;
GLfloat penlength = 1.0f;

GLfloat InitialRad1 = 1.414f;
GLfloat FinalRad1 = 1.414f;
GLfloat InitialAngle1 = 1.57f;
GLfloat FinalAngle1 = 1.57f;

GLfloat InitialRad2 = 2.1681f;
GLfloat FinalRad2 = 2.1681f;
GLfloat InitialAngle2 = 1.8967f;
GLfloat FinalAngle2 = 1.8967f;

glm::vec4 t, p;

glm::vec3 V;
glm::vec3 tempV;

float alpha, beta, gamma = 0.0f;
GLfloat si = 0.0;
GLfloat co = 0.0;
GLfloat tempsi = 0.0f;
GLfloat tempco = 0.0f;
GLfloat horizontalMovement = 0.0f;;

Vertex* Verts_cube;
GLuint* Idcs_cube;

UVs Grid_Vertex_three[441];
GLuint Idcs_Grid_three[2400];

UVs Sub_Vertex[3721];
GLuint Sub_Idcs[21600];

GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint texID;
GLuint pickingProgramID;

const GLuint NumObjects = 11;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint VertCount[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

size_t NumIndices[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t VertexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t IndexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t NormalBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t NumberVert[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

GLuint MatrixID;
GLuint TexMatrixID;
GLuint TexTexID;
GLuint TexModelMatrixID;
GLuint TexViewMatrixID;
GLuint TexProjMatrixID;

GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingModelMatrixID;
GLuint PickingViewMatrixID;
GLuint PickingProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayRID;
GLuint pickingColorArrayGID;
GLuint pickingColorArrayBID;
GLuint pickID;
GLuint pickingColorID;
GLuint LightID;
GLuint Light2ID;
GLuint AmbientID;
GLuint PowerID;
GLuint Texture;
long wid;
long hei;

GLint gX = 0.0;
GLint gZ = 0.0;

float pickingColorR[441];

// animation control
bool animation = false;
bool animation1 = false;
GLfloat bspeed = 4.0f;
GLfloat phi = 0.78;
GLfloat theta = 0.01;
GLfloat xanime = 0.0f;
GLfloat yanime = 0.0f;
GLfloat zanime = 0.0f;
GLfloat xanime_angle = 0;
GLfloat yanime_angle = 0;
GLfloat zanime_angle = 0;
int Move = 0;

void loadObject(char* file, glm::vec4 color, Vertex * &out_Vertices, GLuint* &out_Indices, int ObjectId)
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices,//uvs,
						normals);

	std::vector<GLuint> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices,//uvs,
			normals, indices, indexed_vertices,//indexed_uvs,
			indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();
	const size_t nrmlCount = normals.size();

	// populate output arrays
	out_Vertices = new Vertex[vertCount];
	for (int i = 0; i < vertCount; i++) {
		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);
	}
	out_Indices = new GLuint[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}
	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLuint) * idxCount;
	NormalBufferSize[ObjectId] = sizeof(GLushort) * nrmlCount;
	VertCount[ObjectId] = vertCount;
}

//Calculate Camera Position after move
void calculateCam()
{
	cam[0] = 35.0f*sinf(phi)*sinf(theta);
	cam[1] = 35.0f*cosf(phi);
	cam[2] = 35.0f*sinf(phi)*cosf(theta);

	light1[0] = 20.0f*sinf(phi)*sinf(theta + 1.0);
	light1[1] = 20.0f*cosf(phi);
	light1[2] = 20.0f*sinf(phi)*cosf(theta + 1.0);

	light2[0] = 20.0f*sinf(phi)*sinf(theta - 1.0);
	light2[1] = 20.0f*cosf(phi);
	light2[2] = 20.0f*sinf(phi)*cosf(theta - 1.0);

	if (phi < 3.14 && phi > 0)
	{
		gViewMatrix = glm::lookAt(glm::vec3(cam[0], cam[1], cam[2]),	// eye
			glm::vec3(0.0, 0.0, 0.0),	// center
			glm::vec3(0.0, 1.0, 0.0));	// up
	}
	else
	{
		gViewMatrix = glm::lookAt(glm::vec3(cam[0], cam[1], cam[2]),	// eye
			glm::vec3(0.0, 0.0, 0.0),	// center
			glm::vec3(0.0, -1.0, 0.0));	// up
	}

}
//Create objects
void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 10.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 10.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 10.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
	};

	VertexBufferSize[0] = sizeof(CoordVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordVerts, NULL, 0);
	GLfloat Grid_Corner[4][4] = {
		{ -10.0, 0.0, 10.0, 1.0 },		//-X,Z
		{ -10.0, 0.0, -10.0, 1.0 },		//-X,-Z
		{ -10.0, 0.0, 10.0, 1.0 },		//X,-Z
		{ 10.0, 0.0, 10.0, 1.0 }			//X,Z
	};
	
	GLfloat Grid_Corner2[4][4] = {
		{ -10.0, 0.0, 12.0, 1.0 },		//-X,Z
		{ -10.0, 20.0, 12.0, 1.0 },		//-X,-Z
		{ 10.0, 0.0, 12.0, 1.0 },		//X,-Z
		{ 10.0, 20.0, 12.0, 1.0 }			//X,Z
	};

	//-- GRID --//
	Vertex Grid_Vertex_one[42];
	gridmakeone(Grid_Corner[0], Grid_Corner[3], Grid_Vertex_one);
	VertexBufferSize[1] = sizeof(Grid_Vertex_one);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(Grid_Vertex_one, NULL, 1);

	Vertex Grid_Vertex_two[42];
	gridmaketwo(Grid_Corner[1], Grid_Corner[2], Grid_Vertex_two);
	VertexBufferSize[2] = sizeof(Grid_Vertex_two);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(Grid_Vertex_two, NULL, 2);

	wireMesh(Grid_Vertex_three,Idcs_Grid_three);
	VertexBufferSize[3] = sizeof(Grid_Vertex_three);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	IndexBufferSize[3] = sizeof(Idcs_Grid_three);
	createVAO2(Grid_Vertex_three, Idcs_Grid_three, 3);

	loadObject("Model3/katiyar_ashish.obj", glm::vec4(1.0, 1.0, 1.0, 1.0), Verts_cube, Idcs_cube, 4);
	createVAOs(Verts_cube, Idcs_cube, 4);

	Texture = load_texture_TGA("katiyar_ashish.tga", &wid, &hei, GL_CLAMP, GL_CLAMP);

	SubInd();

	// ATTN: create your grid vertices here!

	//-- .OBJs --//

	// ATTN: load your models here
	/*Vertex* Verts;
	GLushort* Idcs;
	loadObject("models/base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	createVAOs(Verts, Idcs, ObjectID);*/

}

//Just a basic function to move hairs
void animateHair()
{
	//float x = 0.5;
	if (ani < 100)
	{
			for (int i = 420; i < 441; i++)
			{
				Grid_Vertex_three[i].Position[2] += 0.01;
				Grid_Vertex_three[i].Position[0] += 0.01;
			}
			for (int i = 400; i < 420; i++)
			{
				Grid_Vertex_three[i].Position[1] += 0.01;
			}
	}
	else if(ani>=100 && ani <=200)
	{
			for (int i = 420; i < 441; i++)
			{
				Grid_Vertex_three[i].Position[2] -= 0.01;
				Grid_Vertex_three[i].Position[0] -= 0.01;
			}
			for (int i = 400; i < 420; i++)
			{
				Grid_Vertex_three[i].Position[1] -= 0.01;
			//	Grid_Vertex_three[i].Position[0] += 0.01;
			}
	}
	
	
}

//SubDivivde control mesh
void subDivide(UVs sub[])
{
	int hor = 0, horO = 0;
	int ver = 61, verO = 21;
	float a1[4] = { 1.0f };
	float a2[4] = { 1.0f };
	float a3[4] = { 1.0f };
	float a4[4] = { 1.0f };
	float a5[4] = { 1.0f };
	float a6[4] = { 1.0f };
	float a7[4] = { 1.0f };
	float a8[4] = { 1.0f };
	float a9[4] = { 1.0f };
	for (int i = 0; i < 20; i++)
	{
		hor = 3*i*ver;
		horO = i*verO;
		for (int j = 0; j < 20; j++)
		{
			if (i == 0)
			{
			
			}
			else
			{
				a1[0] = (16.0*Grid_Vertex_three[horO + j].Position[0] +
					4 * (Grid_Vertex_three[horO + j + 1].Position[0] +
						Grid_Vertex_three[horO + j + 21].Position[0] +
						Grid_Vertex_three[horO + j - 1].Position[0] +
						Grid_Vertex_three[horO + j - 21].Position[0]) +
						(Grid_Vertex_three[horO + j + 21 + 1].Position[0] +
							Grid_Vertex_three[horO + j + 21 - 1].Position[0] +
							Grid_Vertex_three[horO + j - 21 + 1].Position[0] +
							Grid_Vertex_three[horO + j - 21 - 1].Position[0])) / 36;

				a1[1] = (16.0*Grid_Vertex_three[horO + j].Position[1] +
					4 * (Grid_Vertex_three[horO + j + 1].Position[1] +
						Grid_Vertex_three[horO + j + 21].Position[1] +
						Grid_Vertex_three[horO + j - 1].Position[1] +
						Grid_Vertex_three[horO + j - 21].Position[1]) +
						(Grid_Vertex_three[horO + j + 21 + 1].Position[1] +
							Grid_Vertex_three[horO + j + 21 - 1].Position[1] +
							Grid_Vertex_three[horO + j - 21 + 1].Position[1] +
							Grid_Vertex_three[horO + j - 21 - 1].Position[1])) / 36;

				a1[2] = (16.0*Grid_Vertex_three[horO + j].Position[2] +
					4 * (Grid_Vertex_three[horO + j + 1].Position[2] +
						Grid_Vertex_three[horO + j + 21].Position[2] +
						Grid_Vertex_three[horO + j - 1].Position[2] +
						Grid_Vertex_three[horO + j - 21].Position[2]) +
						(Grid_Vertex_three[horO + j + 21 + 1].Position[2] +
							Grid_Vertex_three[horO + j + 21 - 1].Position[2] +
							Grid_Vertex_three[horO + j - 21 + 1].Position[2] +
							Grid_Vertex_three[horO + j - 21 - 1].Position[2])) / 36;
				sub[hor].SetPosition(a1);
				
				a2[0] = (8 * Grid_Vertex_three[horO + j].Position[0] +
					2 * (Grid_Vertex_three[horO + j + 21].Position[0] + Grid_Vertex_three[horO + j - 21].Position[0]) +
					4 * (Grid_Vertex_three[horO + j + 1].Position[0]) +
					(Grid_Vertex_three[horO + j + 21 + 1].Position[0] + Grid_Vertex_three[horO + j - 21 + 1].Position[0])) / 18;

				a2[1] = (8 * Grid_Vertex_three[horO + j].Position[1] +
					2 * (Grid_Vertex_three[horO + j + 21].Position[1] + Grid_Vertex_three[horO + j - 21].Position[1]) +
					4 * (Grid_Vertex_three[horO + j + 1].Position[1]) +
					(Grid_Vertex_three[horO + j + 21 + 1].Position[1] + Grid_Vertex_three[horO + j - 21 + 1].Position[1])) / 18;

				a2[2] = (8 * Grid_Vertex_three[horO + j].Position[2] +
					2 * (Grid_Vertex_three[horO + j + 21].Position[2] + Grid_Vertex_three[horO + j - 21].Position[2]) +
					4 * (Grid_Vertex_three[horO + j + 1].Position[2]) +
					(Grid_Vertex_three[horO + j + 21 + 1].Position[2] + Grid_Vertex_three[horO + j - 21 + 1].Position[2])) / 18;
				sub[hor+1].SetPosition(a2);
				
				a3[0] = (8 * Grid_Vertex_three[horO + j + 1].Position[0] +
					2 * (Grid_Vertex_three[horO + j + 21 + 1].Position[0] + Grid_Vertex_three[horO + j - 21 + 1].Position[0]) +
					4 * (Grid_Vertex_three[horO + j].Position[0]) +
					(Grid_Vertex_three[horO + j + 21].Position[0] + Grid_Vertex_three[horO + j - 21].Position[0])) / 18;

				a3[1] = (8 * Grid_Vertex_three[horO + j + 1].Position[1] +
					2 * (Grid_Vertex_three[horO + j + 21 + 1].Position[1] + Grid_Vertex_three[horO + j - 21 + 1].Position[1]) +
					4 * (Grid_Vertex_three[horO + j].Position[1]) +
					(Grid_Vertex_three[horO + j + 21].Position[1] + Grid_Vertex_three[horO + j - 21].Position[1])) / 18;

				a3[2] = (8 * Grid_Vertex_three[horO + j + 1].Position[2] +
					2 * (Grid_Vertex_three[horO + j + 21 + 1].Position[2] + Grid_Vertex_three[horO + j - 21 + 1].Position[2]) +
					4 * (Grid_Vertex_three[horO + j].Position[2]) +
					(Grid_Vertex_three[horO + j + 21].Position[2] + Grid_Vertex_three[horO + j - 21].Position[2])) / 18;
				sub[hor+2].SetPosition(a3);
				//sub[hor+1].Position[0] = ;
				//sub[hor+2].Position[0] = ;
			}
			//hor = hor + 61;
			if (j == 0)
			{
				
			}
			else
			{
				a4[0] = (8 * Grid_Vertex_three[horO + j].Position[0] +
					2 * (Grid_Vertex_three[horO + j - 1].Position[0] + Grid_Vertex_three[horO + j + 1].Position[0]) +
					4 * (Grid_Vertex_three[horO + j + 21].Position[0]) + (Grid_Vertex_three[horO + j + 21 - 1].Position[0] +
						Grid_Vertex_three[horO + j + 21 + 1].Position[0]))/18;

				a4[1] = (8 * Grid_Vertex_three[horO + j].Position[1] +
					2 * (Grid_Vertex_three[horO + j - 1].Position[1] + Grid_Vertex_three[horO + j + 1].Position[1]) +
					4 * (Grid_Vertex_three[horO + j + 21].Position[1]) + (Grid_Vertex_three[horO + j + 21 - 1].Position[1] +
						Grid_Vertex_three[horO + j + 21 + 1].Position[1]))/18;

				a4[2] = (8 * Grid_Vertex_three[horO + j].Position[2] +
					2 * (Grid_Vertex_three[horO + j - 1].Position[2] + Grid_Vertex_three[horO + j + 1].Position[2]) +
					4 * (Grid_Vertex_three[horO + j + 21].Position[2]) + (Grid_Vertex_three[horO + j + 21 - 1].Position[2] +
						Grid_Vertex_three[horO + j + 21 + 1].Position[2]))/18;
				sub[hor + 61].SetPosition(a4);
				
				a7[0] = (8 * Grid_Vertex_three[horO + j + 21].Position[0] +
					2 * (Grid_Vertex_three[horO + j + 21 - 1].Position[0] + Grid_Vertex_three[horO + j + 21 + 1].Position[0]) +
					4 * (Grid_Vertex_three[horO + j].Position[0])+
					(Grid_Vertex_three[horO + j + 1].Position[0] + Grid_Vertex_three[horO + j - 1].Position[0])) / 18;

				a7[1] = (8 * Grid_Vertex_three[horO + j + 21].Position[1] +
					2 * (Grid_Vertex_three[horO + j + 21 - 1].Position[1] + Grid_Vertex_three[horO + j + 21 + 1].Position[1]) +
					4 * (Grid_Vertex_three[horO + j].Position[1]) +
					(Grid_Vertex_three[horO + j + 1].Position[1] + Grid_Vertex_three[horO + j - 1].Position[1])) / 18;

				a7[2] = (8 * Grid_Vertex_three[horO + j + 21].Position[2] +
					2 * (Grid_Vertex_three[horO + j + 21 - 1].Position[2] + Grid_Vertex_three[horO + j + 21 + 1].Position[2]) +
					4 * (Grid_Vertex_three[horO + j].Position[2]) +
					(Grid_Vertex_three[horO + j + 1].Position[2] + Grid_Vertex_three[horO + j - 1].Position[2])) / 18;
				sub[hor + 122].SetPosition(a7);
			}
			a5[0] = (4 * Grid_Vertex_three[horO + j].Position[0] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[0] + Grid_Vertex_three[horO + j + 21].Position[0]) +
				Grid_Vertex_three[horO + j + 21 + 1].Position[0]) / 9;

			a5[1] = (4 * Grid_Vertex_three[horO + j].Position[1] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[1] + Grid_Vertex_three[horO + j + 21].Position[1]) +
				Grid_Vertex_three[horO + j + 21 + 1].Position[1]) / 9;

			a5[2] = (4 * Grid_Vertex_three[horO + j].Position[2] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[2] + Grid_Vertex_three[horO + j + 21].Position[2]) +
				Grid_Vertex_three[horO + j + 21 + 1].Position[2]) / 9;
			sub[hor + 61 + 1].SetPosition(a5);

			a6[0] = (4 * Grid_Vertex_three[horO + j + 1].Position[0] +
				2 * (Grid_Vertex_three[horO + j].Position[0] + Grid_Vertex_three[horO + j + 21 + 1].Position[0]) +
				Grid_Vertex_three[horO + j + 21].Position[0]) / 9;

			a6[1] = (4 * Grid_Vertex_three[horO + j + 1].Position[1] +
				2 * (Grid_Vertex_three[horO + j].Position[1] + Grid_Vertex_three[horO + j + 21 + 1].Position[1]) +
				Grid_Vertex_three[horO + j + 21].Position[1]) / 9;

			a6[2] = (4 * Grid_Vertex_three[horO + j + 1].Position[2] +
				2 * (Grid_Vertex_three[horO + j].Position[2] + Grid_Vertex_three[horO + j + 21 + 1].Position[2]) +
				Grid_Vertex_three[horO + j + 21].Position[2]) / 9;
			sub[hor + 61 + 2].SetPosition(a6);

			a8[0] = (4 * Grid_Vertex_three[horO + j + 21].Position[0] +
				2 * (Grid_Vertex_three[horO + j].Position[0] + Grid_Vertex_three[horO + j + 21 + 1].Position[0]) +
				(Grid_Vertex_three[horO + j + 1].Position[0])) / 9;

			a8[1] = (4 * Grid_Vertex_three[horO + j + 21].Position[1] +
				2 * (Grid_Vertex_three[horO + j].Position[1] + Grid_Vertex_three[horO + j + 21 + 1].Position[1]) +
				(Grid_Vertex_three[horO + j + 1].Position[1])) / 9;

			a8[2] = (4 * Grid_Vertex_three[horO + j + 21].Position[2] +
				2 * (Grid_Vertex_three[horO + j].Position[2] + Grid_Vertex_three[horO + j + 21 + 1].Position[2]) +
				(Grid_Vertex_three[horO + j + 1].Position[2])) / 9;
			sub[hor + 122 + 1].SetPosition(a8);

			a9[0] = (4 * Grid_Vertex_three[horO + j + 21 + 1].Position[0] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[0] + Grid_Vertex_three[horO + j + 21].Position[0]) +
				(Grid_Vertex_three[horO + j].Position[0])) / 9;

			a9[1] = (4 * Grid_Vertex_three[horO + j + 21 + 1].Position[1] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[1] + Grid_Vertex_three[horO + j + 21].Position[1]) +
				(Grid_Vertex_three[horO + j].Position[1])) / 9;

			a9[2] = (4 * Grid_Vertex_three[horO + j + 21 + 1].Position[2] +
				2 * (Grid_Vertex_three[horO + j + 1].Position[2] + Grid_Vertex_three[horO + j + 21].Position[2]) +
				(Grid_Vertex_three[horO + j].Position[2])) / 9;
			sub[hor + 122 + 2].SetPosition(a9);
			hor = hor + 3;
			//horO++;
		}
	}
	hor = 0;
	horO = 0;
	for (int i = 0; i < 20; i++)
	{
		sub[hor].SetPosition(Grid_Vertex_three[horO].Position);
		a1[0] = (2 * Grid_Vertex_three[horO].Position[0] +  Grid_Vertex_three[horO + 1].Position[0])/3;
		a1[1] = (2 * Grid_Vertex_three[horO].Position[1] +  Grid_Vertex_three[horO + 1].Position[1]) / 3;
		a1[2] = (2 * Grid_Vertex_three[horO].Position[2] +  Grid_Vertex_three[horO + 1].Position[2]) / 3;
		sub[hor + 1].SetPosition(a1);

		a2[0] = (2 * Grid_Vertex_three[horO+1].Position[0] + Grid_Vertex_three[horO ].Position[0]) / 3;
		a2[1] = (2 * Grid_Vertex_three[horO+1].Position[1] + Grid_Vertex_three[horO ].Position[1]) / 3;
		a2[2] = (2 * Grid_Vertex_three[horO+1].Position[2] + Grid_Vertex_three[horO ].Position[2]) / 3; 
		sub[hor + 2].SetPosition(a2);
		hor = hor + 3;
		horO++;
	}
	hor = 60;
	horO = 20;
	for (int i = 0; i < 20; i++)
	{
		sub[hor].SetPosition(Grid_Vertex_three[horO].Position);
		a1[0] = (2 * Grid_Vertex_three[horO].Position[0] + Grid_Vertex_three[horO + 21].Position[0]) / 3;
		a1[1] = (2 * Grid_Vertex_three[horO].Position[1] + Grid_Vertex_three[horO + 21].Position[1]) / 3;
		a1[2] = (2 * Grid_Vertex_three[horO].Position[2] + Grid_Vertex_three[horO + 21].Position[2]) / 3;
		sub[hor + 61].SetPosition(a1);

		a2[0] = (2 * Grid_Vertex_three[horO + 21].Position[0] + Grid_Vertex_three[horO].Position[0]) / 3;
		a2[1] = (2 * Grid_Vertex_three[horO + 21].Position[1] + Grid_Vertex_three[horO].Position[1]) / 3;
		a2[2] = (2 * Grid_Vertex_three[horO + 21].Position[2] + Grid_Vertex_three[horO].Position[2]) / 3;
		sub[hor + 122].SetPosition(a2);
		hor = hor + 183;
		horO = horO + 21;
	}
	hor = 0;
	horO = 0;
	for (int i = 0; i < 20; i++)
	{
		sub[hor].SetPosition(Grid_Vertex_three[horO].Position);
		a1[0] = (2 * Grid_Vertex_three[horO].Position[0] + Grid_Vertex_three[horO + 21].Position[0]) / 3;
		a1[1] = (2 * Grid_Vertex_three[horO].Position[1] + Grid_Vertex_three[horO + 21].Position[1]) / 3;
		a1[2] = (2 * Grid_Vertex_three[horO].Position[2] + Grid_Vertex_three[horO + 21].Position[2]) / 3;
		sub[hor + 61].SetPosition(a1);

		a2[0] = (2 * Grid_Vertex_three[horO + 21].Position[0] + Grid_Vertex_three[horO].Position[0]) / 3;
		a2[1] = (2 * Grid_Vertex_three[horO + 21].Position[1] + Grid_Vertex_three[horO].Position[1]) / 3;
		a2[2] = (2 * Grid_Vertex_three[horO + 21].Position[2] + Grid_Vertex_three[horO].Position[2]) / 3;
		sub[hor + 122].SetPosition(a2);
		hor = hor + 183;
		horO = horO + 21;
	}
	hor = 3660;
	horO = 420;
	for (int i = 0; i < 20; i++)
	{
		sub[hor].SetPosition(Grid_Vertex_three[horO].Position);
		a1[0] = (2 * Grid_Vertex_three[horO].Position[0] + Grid_Vertex_three[horO + 1].Position[0]) / 3;
		a1[1] = (2 * Grid_Vertex_three[horO].Position[1] + Grid_Vertex_three[horO + 1].Position[1]) / 3;
		a1[2] = (2 * Grid_Vertex_three[horO].Position[2] + Grid_Vertex_three[horO + 1].Position[2]) / 3;
		sub[hor + 1].SetPosition(a1);

		a2[0] = (2 * Grid_Vertex_three[horO + 1].Position[0] + Grid_Vertex_three[horO].Position[0]) / 3;
		a2[1] = (2 * Grid_Vertex_three[horO + 1].Position[1] + Grid_Vertex_three[horO].Position[1]) / 3;
		a2[2] = (2 * Grid_Vertex_three[horO + 1].Position[2] + Grid_Vertex_three[horO].Position[2]) / 3;
		sub[hor + 2].SetPosition(a2);
		hor = hor + 3;
		horO++;
	}
	sub[3720].SetPosition(Grid_Vertex_three[440].Position);
	
}


//Choose UV mapping
void subDuv(UVs uv[])
{
	float c[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float n[] = { 0.0, 1.0, 1.0 };
	float u[] = { 0.0f,0.0f };
	int ik = 0;
	for (int i = 0; i < 61; i++)
	{
		for (int j = 0; j < 61; j++)
		{
			u[0] = j / 64.5f;
			u[1] = i / 69.0f;
			uv[ik].SetColor(c);
			uv[ik].SetNormal(n);
			uv[ik].SetUV(u);
			ik++;
		}
	}
}

//select Indices for buffer
void SubInd()
{
	int hor = 0;
	int ver = 61;
	int idc = 0;
	for (int i = 0; i < 60; i++)
	{
		//Idcs[i] = i;
		hor = i*ver;
		for (int j = 0; j < 60; j++)
		{
			Sub_Idcs[idc] = hor++;
			Sub_Idcs[idc + 1] = hor;
			Sub_Idcs[idc + 2] = hor + ver;
			Sub_Idcs[idc + 3] = hor + ver;
			Sub_Idcs[idc + 4] = hor + ver - 1;
			Sub_Idcs[idc + 5] = hor - 1;
			idc += 6;
		}
	}
}

// Control Grid mesh calculation
void wireMesh( UVs uv[], GLuint Idcs[])
{
	float c[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float a[] = { -10.0, 20.0, 12.0, 1.0 };
	float n[] = { 0.0, 1.0, 1.0 };
	float u[] = {0.0f,0.0f};
	float y = 0.0f;
	float x = -10.0f;
	int k = 0;
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			a[0] = x;
			a[1] = y;
			u[0] = j / 21.5f;
			u[1] = i / 23.0f;
			uv[k].SetPosition(a);
			uv[k].SetColor(c);
			uv[k].SetNormal(n);
			uv[k].SetUV(u);
			x = x + 1.0f;
			k++;
		}
		x = -10.0f;
		y = y + 0.6f;
	}
	int hor = 0;
	int ver = 21;
	int idc = 0;
	for (int i = 0; i < 20; i++)
	{
		hor = i*ver;
		for (int j = 0; j < 20; j++)
		{
			Idcs[idc] = hor++;
			Idcs[idc + 1] = hor;
			Idcs[idc + 2] = hor + ver;
			Idcs[idc + 3] = hor + ver;
			Idcs[idc + 4] = hor + ver - 1;
			Idcs[idc + 5] = hor - 1;
			
			idc += 6;
		}
	}
}

void gridmakeone(float* a, float* b, Vertex Vert[])
{
	float c[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float n[] = { 0.0, 0.0, 1.0 };
	float z = a[2], k = 0.0f;
	for (int i = 0; i < 42; i = i + 2)
	{
		a[2] = z - (k*1.0f);
		Vert[i].SetPosition(a);
		//a[2] = a[2] - (1.0f);
		b[2] = z - (k*1.0f);
		Vert[i + 1].SetPosition(b);
		Vert[i].SetColor(c);
		Vert[i].SetNormal(n);
		Vert[i + 1].SetColor(c);
		Vert[i + 1].SetNormal(n);
		k++;
	}
}

//Ray Testing function
int rayTest(vec3 unpro, vec3 unpro1) {
	int ind = -1, k = 0;;
	for (int i = 0; i < NumIndices[4]; i=i+3)
	{
		float cg[3] = {0.0};
		
		int j = 0 ;
		float v1[3] = { Verts_cube[Idcs_cube[i]].Position[0],Verts_cube[Idcs_cube[i]].Position[1],Verts_cube[Idcs_cube[i]].Position[2] };
		float v2[3] = { Verts_cube[Idcs_cube[i+1]].Position[0],Verts_cube[Idcs_cube[i+1]].Position[1],Verts_cube[Idcs_cube[i+1]].Position[2] };
		float v3[3] = { Verts_cube[Idcs_cube[i+2]].Position[0],Verts_cube[Idcs_cube[i+2]].Position[1],Verts_cube[Idcs_cube[i+2]].Position[2] };
		float l[3] = { unpro[0],unpro[1],unpro[2] };
		float r[3] = { unpro1[0],unpro1[1],unpro1[2] };
		ray_cast(v1,v2,v3,l,r,cg);
	//	printf("%f , %f ,%f \n",cg[0],cg[1],cg[2]);
		while (cg[j] > 0)
		{
			j++;
		}
		if (j == 3 && k==0)
		{
				ind = Idcs_cube[i + 1];
				k = 1;
		}
		else if (j == 3 && k==1)
		{
			float dis = distance(unpro, vec3(v2[0], v2[1], v2[2]));
			float dis2 = distance(unpro, vec3(Verts_cube[ind].Position[0], Verts_cube[ind].Position[1], Verts_cube[ind].Position[2]));
			if(dis<dis2)
				ind = Idcs_cube[i + 1];
		}	
	}
	return ind;
}
//Setting vectors for ray testing 
void rayTest2() {
	int tr=-1;
	for (int i = 0; i < 441; i++)
	{
			vec3 f = { Grid_Vertex_three[i].Position[0],Grid_Vertex_three[i].Position[1],Grid_Vertex_three[i].Position[2] };
			vec3 center = {0,Grid_Vertex_three[i].Position[1], -2};
			tr = rayTest(f, center - f);
			if (tr >= 0)
			{
				Grid_Vertex_three[i].Position[0] = Verts_cube[tr].Position[0];
				Grid_Vertex_three[i].Position[1] = Verts_cube[tr].Position[1];
				Grid_Vertex_three[i].Position[2] = Verts_cube[tr].Position[2];
			}
			else
			{
				Grid_Vertex_three[i].Position[0] = 0.0;
				Grid_Vertex_three[i].Position[1] = Grid_Vertex_three[i].Position[1];
				Grid_Vertex_three[i].Position[2] = 0.0;
			}
	}
}

void cylender()
{
	float x = 0;
	float z = 0;
	float a = 0;
	int idc = 0;
	for (int i=20;i>=0;i--)
	{
		for (int j = 20; j >= 0;j--)
		{
			a = (2.0f * 3.14f*j / 20.0f) - 1.62;
			x = 10 * cos(a);
			z =10 * sin(a) - 2;
			Grid_Vertex_three[idc].Position[0] = x;
			Grid_Vertex_three[idc].Position[2] = z;
			idc++;
		}
	}
}
//Another Basic Animation 
void frown()
{
	if (ani <= 130)
	{
		Grid_Vertex_three[137].Position[1] += 0.01;
		Grid_Vertex_three[158].Position[1] += 0.01;
		Grid_Vertex_three[137].Position[0] += 0.002;
		Grid_Vertex_three[158].Position[0] += 0.002;
		
		Grid_Vertex_three[136].Position[1] -= 0.01;
		Grid_Vertex_three[157].Position[1] -= 0.01;

		Grid_Vertex_three[156].Position[1] += 0.01;
		Grid_Vertex_three[135].Position[1] += 0.01;
		Grid_Vertex_three[156].Position[0] -= 0.002;
		Grid_Vertex_three[135].Position[0] -= 0.002;
	}
	else if (ani <= 260 && ani > 130)
	{
		
		Grid_Vertex_three[137].Position[1] -= 0.01;
		Grid_Vertex_three[158].Position[1] -= 0.01;
		Grid_Vertex_three[137].Position[0] -= 0.002;
		Grid_Vertex_three[158].Position[0] -= 0.002;
		
		Grid_Vertex_three[136].Position[1] += 0.01;
		Grid_Vertex_three[157].Position[1] += 0.01;

		Grid_Vertex_three[135].Position[1] -= 0.01;
		Grid_Vertex_three[156].Position[1] -= 0.01;
		Grid_Vertex_three[156].Position[0] += 0.002;
		Grid_Vertex_three[135].Position[0] += 0.002;
	}
}
void gridmaketwo(float* a, float* b, Vertex Vert[])
{
	float c[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float n[] = { 0.0, 0.0, 1.0 };
	float x = a[0];
	float k = 0.0f;
	for (int i = 0; i < 42; i = i + 2)
	{
		a[0] = x + (k*1.0f);
		Vert[i].SetPosition(a);
		//a[2] = a[2] - (1.0f);
		b[0] = x + (k*1.0f);
		Vert[i + 1].SetPosition(b);
		Vert[i].SetColor(c);
		Vert[i].SetNormal(n);
		Vert[i + 1].SetColor(c);
		Vert[i + 1].SetNormal(n);
		k++;
	}
}

void renderScene(void)
{
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos = glm::vec3(light1[0], light1[1], light1[2]);
	glm::vec3 light2Pos = glm::vec3(light2[0], light2[1], light2[2]);
	glm::mat4x4 TransMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
	glm::mat4x4 ModelMatrix = glm::mat4(1.0)*TransMatrix;
	
	glUseProgram(programID);
	{
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);

		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.6f, 0.6f, 0.6f);
		glBindVertexArray(VertexArrayId[0]);								// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);

		glUniform3f(AmbientID, 0.8f, 0.8f, 0.8f);
		glUniform1f(PowerID, 0.0f);
		glBindVertexArray(VertexArrayId[1]);								// draw GridOne
		glDrawArrays(GL_LINES, 0, 42);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[2]);								// draw GridTwo
		glDrawArrays(GL_LINES, 0, 42);
		glBindVertexArray(0);

		glUniform1f(PowerID, 60.0f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);

		if (model == 1)
		{
			glBindVertexArray(VertexArrayId[4]);								// draw Base
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_cube), Verts_cube);
			glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);

			
		}
		
		if (Mes == 1)
		{
			glBindVertexArray(VertexArrayId[3]);								// draw Base
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Grid_Vertex_three), Grid_Vertex_three);
			glDrawElements(GL_LINES, 2400, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);

			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(8.0);
			glBindVertexArray(VertexArrayId[3]);								// draw MESH GRID
			glDrawArrays(GL_POINTS, 0, 441);
			glBindVertexArray(0);

			if (subd == 1)
			{
				glPointSize(4.0);
				glBindVertexArray(VertexArrayId[5]);								// draw MESH GRID
				glDrawArrays(GL_POINTS, 0, 3721);
				glBindVertexArray(0);
			}
		}

			
	}
	glUseProgram(0);
	// Draw GUI
	glUseProgram(texID);
	{
		glUniformMatrix4fv(TexViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(TexProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(TexModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

	
		if (Mes == 1)
		{
			glBindVertexArray(VertexArrayId[3]);								// draw Base
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Grid_Vertex_three), Grid_Vertex_three);
			glDrawElements(GL_TRIANGLES, 2400, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);
		}
		else if(subd==1)
		{
			glBindVertexArray(VertexArrayId[5]);								// draw Base
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Sub_Vertex), Sub_Vertex);
			glDrawElements(GL_TRIANGLES, 21600, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);
		}	
	}
	glUseProgram(0);

//	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickObject(void)
{
	// Clear the screen in white
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos = glm::vec3(light1[0], light1[1], light1[2]);
	glm::vec3 light2Pos = glm::vec3(light2[0], light2[1], light2[2]);
	glm::mat4x4 TransMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
	glm::mat4x4 ModelMatrix = glm::mat4(1.0)*TransMatrix;
	
	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		glUniform3fv(pickingColorArrayRID, 441, pickingColorR);	// here we pass in the picking marker array
	
		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		//	glUniformMatrix4fv(LightID, 1, GL_FALSE, &ModelMatrix[0][0]);

		
		glBindVertexArray(VertexArrayId[3]);									// draw MESH GRID
		glDrawArrays(GL_POINTS, 0, 441);
		glBindVertexArray(0);


	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

																					 // Convert the color back to an integer ID
	gPickedIndex =
		data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;
	//printf("%d \n", gPickedIndex);
	
	//moveVertex(gPickedIndex);
	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

void moveVertex(int gPickedIndex)
{
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	
	if(gPickedIndex < 441){
		int i = gPickedIndex;

		glm::vec3 pos = glm::vec3(xpos, (window_height - ypos), 0.0);
		glm::vec3 pos1 = glm::vec3(xpos, (window_height - ypos), 1.0);
		glm::vec3 unpro = glm::unProject(pos, gViewMatrix, gProjectionMatrix, vp);
		glm::vec3 unpro1 = glm::unProject(pos1, gViewMatrix, gProjectionMatrix, vp);
		
		glm::vec3 v1 = glm::normalize(unpro1 - unpro);
		glm::vec3 po = glm::vec3(Grid_Vertex_three[gPickedIndex].Position[0], Grid_Vertex_three[gPickedIndex].Position[1], Grid_Vertex_three[gPickedIndex].Position[2]);
		glm::vec3 v2 = po - unpro;
		float firstDot = glm::dot(v1, v1);
		float secDot = glm::dot(v2, v1);
		float t = secDot / firstDot;
		glm::vec3 fin = unpro + v1*t;

		if (stick == 1)
		{
			int j = 0;
			j=rayTest(fin,unpro1-fin);
			if (j >= 0)
			{
				Grid_Vertex_three[gPickedIndex].Position[0] = Verts_cube[j].Position[0];
				Grid_Vertex_three[gPickedIndex].Position[1] = Verts_cube[j].Position[1];
				Grid_Vertex_three[gPickedIndex].Position[2] = Verts_cube[j].Position[2];
			}
		}
		else
		{
				Grid_Vertex_three[gPickedIndex].Position[0] = fin[0];
				Grid_Vertex_three[gPickedIndex].Position[1] = fin[1];
				Grid_Vertex_three[gPickedIndex].Position[2] = fin[2];
		}

	}
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Katiyar,Ashish(48366895)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	//TwInit(TW_OPENGL_CORE, NULL);
	//TwWindowSize(window_width, window_height);
	//TwBar * GUI = TwNewBar("Picking");
	//TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	//TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void)
{

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
//	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 3:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 3.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	calculateCam();
	gViewMatrix = glm::lookAt(glm::vec3(cam[0], cam[1], cam[2]),	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		glm::vec3(0.0, 1.0, 0.0));	// up

									// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	texID = LoadShaders("Tex.vertexshader", "Tex.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	TexMatrixID = glGetUniformLocation(texID, "MVP");
	TexTexID = glGetUniformLocation(texID, "myTextureSampler");
	TexModelMatrixID = glGetUniformLocation(texID, "M");
	TexViewMatrixID = glGetUniformLocation(texID, "V");
	TexProjMatrixID = glGetUniformLocation(texID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	PickingModelMatrixID = glGetUniformLocation(pickingProgramID, "M");
	PickingViewMatrixID = glGetUniformLocation(pickingProgramID, "V");
	PickingProjMatrixID = glGetUniformLocation(pickingProgramID, "P");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayRID = glGetUniformLocation(pickingProgramID, "PickingColorArrayR");
	pickingColorArrayGID = glGetUniformLocation(pickingProgramID, "PickingColorArrayG");
	pickingColorArrayBID = glGetUniformLocation(pickingProgramID, "PickingColorArrayB");
	//pickID = glGetUniformLocation(pickingProgramID, "Pick_id");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	Light2ID = glGetUniformLocation(programID, "Light2Position_worldspace");
	AmbientID = glGetUniformLocation(programID, "AmbientColor_definit");
	PowerID = glGetUniformLocation(programID, "Light_Power");

	createObjects();
}

void createVAOs(Vertex Vertices[], GLuint Indices[], int ObjectId) {

	//printf("Numvert %d\n", NumberVert[ObjectId]);
	GLenum ErrorCheckValue = glGetError();
	std::vector<glm::vec3> normals;
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;
	//const size_t UVoffset = sizeof(Vertices[0].Normal) + Normaloffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//

													// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);

	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)UVoffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal
	//glEnableVertexAttribArray(3);	// uvs

									// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}


void createVAO2(UVs Vertices[], GLuint Indices[], int ObjectId) {

	//printf("Numvert %d\n", NumberVert[ObjectId]);
	GLenum ErrorCheckValue = glGetError();
	std::vector<glm::vec3> normals;
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;
	const size_t UVoffset = sizeof(Vertices[0].Normal) + Normaloffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//

													// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)UVoffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal
	glEnableVertexAttribArray(3);	// uvs

									// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);
	//Outfile.close();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}
//Save control Points
void SaveFile()
{
	printf("Save File Called \n");
	mutex mtx;
	mtx.lock();
	Outfile.open("Model3/cp.p3", ios::in);
	if (!Outfile) {
		cout << "Error: Can't open the file named Model3/cp.p3\n";
	}
	else {
		for (int i = 0; i < 441; i++)
		{
			Outfile << Grid_Vertex_three[i].Position[0] << " ";
			Outfile << Grid_Vertex_three[i].Position[1] << " ";
			Outfile << Grid_Vertex_three[i].Position[2] << " ";
			Outfile << Grid_Vertex_three[i].Position[3] << " ";
		}
	}
	Outfile.close();
	mtx.unlock();
}
//Load Control Points
void LoadFile()
{
	ifstream Infile;
	Infile.open("Model3/cp.p3", ios::in);
	printf("Load File Called \n");
	for (int i = 0; i < 441; i++)
	{
		Infile >> Grid_Vertex_three[i].Position[0];
		Infile >> Grid_Vertex_three[i].Position[1];
		Infile >> Grid_Vertex_three[i].Position[2];
		Infile >> Grid_Vertex_three[i].Position[3];
	}
	if (Infile.eof()) {
		printf(" File Ended");
	}
	//sprintf(str, "%d.txt", i);
	Infile.close();
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_S:
			SaveFile();
			break;
		case GLFW_KEY_L:
			LoadFile();
			break;
		case GLFW_KEY_C:
			Mes = (Mes + 1) % 2;
			//subd = Mes;
			break;
		case GLFW_KEY_I:
			printf("subdivision");
			subDivide(Sub_Vertex);
			subDuv(Sub_Vertex);
			//	SubInd();
			VertexBufferSize[5] = sizeof(Sub_Vertex);
			IndexBufferSize[5] = sizeof(Sub_Idcs);
			createVAO2(Sub_Vertex, Sub_Idcs, 5);
			subd = (subd+1)%2;
			break;
		case GLFW_KEY_A:
			ani_s = (ani_s + 1) % 2;
			break;
		case GLFW_KEY_F:
			model = (model + 1) % 2;
			break;
		default:
			break;
		}
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		//camera = (camera + 1) % 2;
		cylender();
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		rayTest2();
	}
	if (glfwGetKey(window, GLFW_KEY_R))
	{
		//giveColor();
		phi = 0.78;
		theta = 0.78;
		calculateCam();
	}
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
		moveVertex(gPickedIndex);
		subDivide(Sub_Vertex);
	}
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// initialize OpenGL pipeline
	initOpenGL();
	
	
	
	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	int poi = 0;
	
	for (int i = 0; i < 441; i++)
	{
		int r = (i & 0x000000FF) >> 0;
		int g = (i & 0x0000FF00) >> 8;
		int b = (i & 0x00FF0000) >> 16;
		pickingColorR[poi] = r / 255.0f;
		pickingColorR[poi + 1] = g / 255.0f;
		pickingColorR[poi + 2] = b / 255.0f;
		//pickingColor[i][0] = i / 441.0f;
		poi = poi + 3;
	}
	do {

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			//pickObject();
			moveVertex(gPickedIndex);
		}


		if (glfwGetKey(window, GLFW_KEY_LEFT))
		{
			theta -= 0.02;
			if (theta < 0)
				theta += 6.282;
			calculateCam();
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			theta += 0.02;
			if (theta > 6.282)
				theta -= 6.282;
			calculateCam();
		}
		if (glfwGetKey(window, GLFW_KEY_UP))
		{
			phi -= 0.02;
			if (phi < 0)
				phi += 6.282;
			calculateCam();
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN))
		{
			phi += 0.02;
			if (phi > 6.282)
				phi -= 6.282;
			calculateCam();
		}
		if (glfwGetKey(window, GLFW_KEY_R))
			pri = 1;
		else
			pri = 0;		

		if (glfwGetKey(window, GLFW_KEY_Q))
			stick = 1;
		else
			stick = 0;
			
		if (ani_s == 1)
		{
			ani = (ani + 1) % 260;
			animateHair();
			frown();
			subDivide(Sub_Vertex);
		}
		
		// DRAWING POINTS
		//giveColor();
		renderScene();
		//pickObject();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();


	return 0;
}
