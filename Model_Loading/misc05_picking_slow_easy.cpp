// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
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
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

const int window_width = 1024, window_height = 768;

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

// function prototypes
int initWindow(void);
void initOpenGL(void);
void loadObject(char*, glm::vec4, Vertex * &, GLushort* &, int);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void gridmakeone(float*, float*, Vertex[]);
void gridmaketwo(float*, float*, Vertex[]);
void calculateCam();
void calculateRot();
void radius1();
void radius2();
void giveColor();
//void J6();
void pickObject(void);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
int camera = 0;
float cam[] = { 0.0f, 0.0f, 14.14f };
float light1[] = { -10.0f, 0.0f, 10.0f };
float light2[] = { 10.0f, 0.0f, 10.0f };
int J_1, j2, j3, j4, j5, j6, b = 0;
float rangle1, rangle2, rangle3, rangle4, rangle5, rangle6 = 0.0f;
float xb, zb = 0.0f;
int token = 0;
int pri = 0;
int first = 1;
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

Vertex* Verts_base;
GLushort* Idcs_base;


Vertex* Verts_top;
GLushort* Idcs_top;

Vertex* Verts_arm1;
GLushort* Idcs_arm1;

Vertex* Verts_joint;
GLushort* Idcs_joint;

Vertex* Verts_arm2;
GLushort* Idcs_arm2;

Vertex* Verts_pen;
GLushort* Idcs_pen;

Vertex* Verts_button;
GLushort* Idcs_button;

Vertex* Verts_bullet;
GLushort* Idcs_bullet;

GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;
glm::mat4 RotationMatrix1;
glm::mat4 RotationMatrix2;
glm::mat4 RotationMatrix3;
glm::mat4 RotationMatrix4;
glm::mat4 RotationMatrix5;
glm::mat4 RotationMatrix6;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
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
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingModelMatrixID;
GLuint PickingViewMatrixID;
GLuint PickingProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickID;
GLuint pickingColorID;
GLuint LightID;
GLuint Light2ID;
GLuint AmbientID;
GLuint PowerID;

GLint gX = 0.0;
GLint gZ = 0.0;

float pickingColor[NumObjects] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f, 4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f , 8 / 255.0f, 9 / 255.0f };

// animation control
bool animation = false;
bool animation1 = false;
GLfloat bspeed = 4.0f;
GLfloat time = 0.0f;
GLfloat phi = 0.78;
GLfloat theta = 0.78;
GLfloat xanime = 0.0f;
GLfloat yanime = 0.0f;
GLfloat zanime = 0.0f;
GLfloat xanime_angle = 0;
GLfloat yanime_angle = 0;
GLfloat zanime_angle = 0;
int Move = 0;

void loadObject(char* file, glm::vec4 color, Vertex * &out_Vertices, GLushort* &out_Indices, int ObjectId)
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

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
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}
	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
	NormalBufferSize[ObjectId] = sizeof(GLushort) * nrmlCount;
	VertCount[ObjectId] = vertCount;
	//printf("%d", vertCount);
}

void calculateCam()
{
	cam[0] = 14.14f*sinf(phi)*sinf(theta);
	cam[1] = 14.14f*cosf(phi);
	cam[2] = 14.14f*sinf(phi)*cosf(theta);

	light1[0] = 10.0f*sinf(phi)*sinf(theta + 1.0);
	light1[1] = 10.0f*cosf(phi);
	light1[2] = 10.0f*sinf(phi)*cosf(theta + 1.0);

	light2[0] = 10.0f*sinf(phi)*sinf(theta - 1.0);
	light2[1] = 10.0f*cosf(phi);
	light2[2] = 10.0f*sinf(phi)*cosf(theta - 1.0);

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
void defaultColor(int token)
{
	if (token == 3 || token == 10)
	{
		for (int i = 0; i<VertCount[3]; i++)
		{
			Verts_base[i].Color[0] = 0.6;
		}
		createVAOs(Verts_base, Idcs_base, 3);
	}
	if (token == 4 || token == 10)
	{
		for (int i = 0; i<VertCount[4]; i++)
		{
			Verts_top[i].Color[1] = 0.6;
		}
		createVAOs(Verts_top, Idcs_top, 4);
	}
	if (token == 5 || token == 10)
	{
		for (int i = 0; i<VertCount[5]; i++)
		{
			Verts_arm1[i].Color[2] = 0.5;
		}
		createVAOs(Verts_arm1, Idcs_arm1, 5);
	}
	if (token == 7 || token == 10)
	{
		for (int i = 0; i<VertCount[7]; i++)
		{
			Verts_arm2[i].Color[1] = 0.6;
			Verts_arm2[i].Color[2] = 0.6;
		}
		createVAOs(Verts_arm2, Idcs_arm2, 7);
	}
	if (token == 8 || token == 10)
	{
		for (int i = 0; i<VertCount[8]; i++)
		{
			Verts_pen[i].Color[0] = 0.6;
			Verts_pen[i].Color[1] = 0.6;
		}
		createVAOs(Verts_pen, Idcs_pen, 8);
	}
}
void giveColor()
{
	defaultColor(token);
	if (b == 1)
	{
		for (int i = 0; i<VertCount[3]; i++)
		{
			Verts_base[i].Color[0] = 1.0;
		}
		createVAOs(Verts_base, Idcs_base, 3);
		token = 3;
	}
	if (J_1 == 1)
	{
		for (int i = 0; i<VertCount[4]; i++)
		{
			Verts_top[i].Color[1] = 1.0;
		}
		createVAOs(Verts_top, Idcs_top, 4);
		token = 4;
	}
	if (j2 == 1)
	{
		for (int i = 0; i<VertCount[5]; i++)
		{
			Verts_arm1[i].Color[2] = 1.0;
		}
		createVAOs(Verts_arm1, Idcs_arm1, 5);
		token = 5;
	}
	if (j3 == 1)
	{
		for (int i = 0; i<VertCount[7]; i++)
		{
			Verts_arm2[i].Color[1] = 1.0f;
			Verts_arm2[i].Color[2] = 1.0f;
		}
		createVAOs(Verts_arm2, Idcs_arm2, 7);
		token = 7;
	}
	if (j5 == 1)
	{
		int i = 0;
		for (i = 0; i<VertCount[8]; i++)
		{
			Verts_pen[i].Color[0] = 1.0f;
			Verts_pen[i].Color[1] = 1.0f;
		}
		createVAOs(Verts_pen, Idcs_pen, 8);
		token = 8;
	}
}

void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 5.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 5.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 5.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
	};

	VertexBufferSize[0] = sizeof(CoordVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordVerts, NULL, 0);
	GLfloat Grid_Corner[4][4] = {
		{ -5.0, 0.0, 5.0, 1.0 },		//-X,Z
		{ -5.0, 0.0, -5.0, 1.0 },		//-X,-Z
		{ -5.0, 0.0, 5.0, 1.0 },		//X,-Z
		{ 5.0, 0.0, 5.0, 1.0 }			//X,Z
	};
	//-- GRID --//
	Vertex Grid_Vertex_one[40];
	gridmakeone(Grid_Corner[0], Grid_Corner[3], Grid_Vertex_one);
	VertexBufferSize[1] = sizeof(Grid_Vertex_one);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(Grid_Vertex_one, NULL, 1);

	Vertex Grid_Vertex_two[40];
	gridmaketwo(Grid_Corner[1], Grid_Corner[2], Grid_Vertex_two);
	VertexBufferSize[2] = sizeof(Grid_Vertex_two);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(Grid_Vertex_two, NULL, 2);


	// ATTN: create your grid vertices here!

	//-- .OBJs --//

	// ATTN: load your models here
	/*Vertex* Verts;
	GLushort* Idcs;
	loadObject("models/base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	createVAOs(Verts, Idcs, ObjectID);*/

	//Base
	loadObject("Model3/Base.obj", glm::vec4(0.6, 0.0, 0.0, 1.0), Verts_base, Idcs_base, 3);
	createVAOs(Verts_base, Idcs_base, 3);

	//Top
	loadObject("Model3/Top.obj", glm::vec4(0.0, 0.6f, 0.0, 1.0), Verts_top, Idcs_top, 4);
	createVAOs(Verts_top, Idcs_top, 4);

	//Arm1
	loadObject("Model3/Arm1.obj", glm::vec4(0.0, 0.0, 0.5, 1.0), Verts_arm1, Idcs_arm1, 5);
	createVAOs(Verts_arm1, Idcs_arm1, 5);

	//Joint
	loadObject("Model3/Joint.obj", glm::vec4(0.6, 0.0, 0.6, 1.0), Verts_joint, Idcs_joint, 6);
	createVAOs(Verts_joint, Idcs_joint, 6);

	//Arm2
	loadObject("Model3/Arm2.obj", glm::vec4(0.0, 0.6, 0.6, 1.0), Verts_arm2, Idcs_arm2, 7);
	createVAOs(Verts_arm2, Idcs_arm2, 7);

	//Pen
	loadObject("Model3/Pen.obj", glm::vec4(0.6, 0.6, 0.0, 1.0), Verts_pen, Idcs_pen, 8);
	createVAOs(Verts_pen, Idcs_pen, 8);

	//button
	loadObject("Model3/Button.obj", glm::vec4(0.8, 0.0, 0.0, 1.0), Verts_button, Idcs_button, 9);
	createVAOs(Verts_button, Idcs_button, 9);

	//bullet
	loadObject("Model3/Bullet.obj", glm::vec4(0.8, 0.0, 0.0, 1.0), Verts_bullet, Idcs_bullet, 10);
	createVAOs(Verts_bullet, Idcs_bullet, 10);
}
void gridmakeone(float* a, float* b, Vertex Vert[])
{
	float c[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float n[] = { 0.0, 0.0, 1.0 };
	float z = a[2], k = 0.0f;
	for (int i = 0; i < 22; i = i + 2)
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
void gridmaketwo(float* a, float* b, Vertex Vert[])
{
	float c[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float n[] = { 0.0, 0.0, 1.0 };
	float x = a[0];
	float k = 0.0f;
	for (int i = 0; i < 22; i = i + 2)
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
	glm::mat4 RotMatrix;
	glm::mat4x4 ModelMatrix = glm::mat4(1.0)*TransMatrix;
	glm::vec4 BulletPos = glm::vec4(0.0f, 1.31f, 2.1f, 1.0f);
	glm::mat4 Transbase = glm::translate(mat4(), vec3(xb, 0.0f, zb));
	glm::mat4 Trans0 = glm::translate(mat4(), vec3(-xb, 0.0f, -zb));
	RotationMatrix1 = glm::rotate(mat4(), rangle1, vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Trans1 = glm::translate(mat4(), vec3(0, -1.5f, 0));
	glm::mat4 Trans2 = glm::translate(mat4(), vec3(0, 1.5f, 0));
	glm::mat4 RotaCom2 = glm::rotate(mat4(), -(rangle1), vec3(0.0f, 1.0f, 0.0f));
	RotationMatrix2 = glm::rotate(mat4(), rangle2, vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 Trans3 = glm::translate(mat4(), vec3(0, -2.2f, 1.3f));
	glm::mat4 Trans4 = glm::translate(mat4(), vec3(0, 2.2f, -(1.3f)));
	glm::mat4 RotaCom3 = glm::rotate(mat4(), -(rangle2), vec3(1.0f, 0.0f, 0.0f));
	RotationMatrix3 = (glm::rotate(mat4(), rangle3, vec3(1.0f, 0.0f, 0.0f)));
	glm::mat4 RotaCom4 = (glm::rotate(mat4(), -rangle3, vec3(1.0f, 0.0f, 0.0f)));
	RotationMatrix4 = glm::rotate(mat4(), rangle4, vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Trans5 = glm::translate(mat4(), vec3(0, -1.3f, 1.25f));
	glm::mat4 Trans6 = glm::translate(mat4(), vec3(0, 1.3f, -(1.25f)));
	RotationMatrix5 = glm::rotate(mat4(), rangle5, vec3(1.0f, 0.0f, 0.0f));
	RotationMatrix6 = glm::rotate(mat4(), rangle6, vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 RotaCom5 = (glm::rotate(mat4(), -rangle4, vec3(0.0f, 1.0f, 0.0f)));

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
		glDrawArrays(GL_LINES, 0, 22);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[2]);								// draw GridTwo
		glDrawArrays(GL_LINES, 0, 22);
		glBindVertexArray(0);

		//Base
		ModelMatrix = Transbase*ModelMatrix;
		glUniform1f(PowerID, 60.0f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[3]);								// draw Base
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_base), Verts_base);
		glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;

		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.4f, 0.4f, 0.4f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Top
		glBindVertexArray(VertexArrayId[4]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_top), Verts_top);
		glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;

		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Arm1
		glBindVertexArray(VertexArrayId[5]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_arm1), Verts_arm1);
		glDrawElements(GL_TRIANGLES, NumIndices[5], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Joint
		
		
		glBindVertexArray(VertexArrayId[6]);								// draw Joint
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_joint), Verts_joint);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_joint), Verts_joint);
		glDrawElements(GL_TRIANGLES, NumIndices[6], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		//Arm2
		
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotaCom4*ModelMatrix;
		ModelMatrix = RotationMatrix4*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[7]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_arm2), Verts_arm2);
		glDrawElements(GL_TRIANGLES, NumIndices[7], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);


		//Pen
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotaCom4*ModelMatrix;

		ModelMatrix = RotaCom5*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans5*ModelMatrix;
		ModelMatrix = RotationMatrix5*ModelMatrix;
		glm::mat4 RotaCom6 = glm::rotate(mat4(), -rangle5, vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = RotaCom6*ModelMatrix;
		ModelMatrix = RotationMatrix6*ModelMatrix;
		ModelMatrix = RotationMatrix5*ModelMatrix;
		ModelMatrix = Trans6*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotationMatrix4*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;

		glUniform1f(PowerID, 60.0f);
		glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[8]);								// draw Pen
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_pen), Verts_pen);
		glDrawElements(GL_TRIANGLES, NumIndices[8], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		//button
		glBindVertexArray(VertexArrayId[9]);								// draw Button
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_button), Verts_button);
		glDrawElements(GL_TRIANGLES, NumIndices[9], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		glm::vec4 PenPos = glm::vec4(0.0f, 1.30f, 1.6f, 1.0f);
		p = ModelMatrix*glm::vec4(PenPos[0], PenPos[1], PenPos[2], PenPos[3]);

		//bullet
		if (animation == 1)
		{
			glm::mat4 Trans7 = glm::translate(mat4(), vec3(xanime, yanime, zanime));
			ModelMatrix = Trans7*ModelMatrix;
			glUniform1f(PowerID, 60.0f);
			glUniform3f(AmbientID, 0.2f, 0.2f, 0.2f);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
			glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			glBindVertexArray(VertexArrayId[10]);								// draw Bullet
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[10]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_bullet), Verts_bullet);
			glDrawElements(GL_TRIANGLES, NumIndices[10], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
			glBindVertexArray(0);
		}
		t = ModelMatrix*glm::vec4(Verts_bullet[0].Position[0], Verts_bullet[0].Position[1], Verts_bullet[0].Position[2], Verts_bullet[0].Position[3]);

	}
	glUseProgram(0);
	// Draw GUI

	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickObject(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos = glm::vec3(light1[0], light1[1], light1[2]);
	glm::vec3 light2Pos = glm::vec3(light2[0], light2[1], light2[2]);
	glm::mat4x4 TransMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 RotMatrix;
	glm::mat4x4 ModelMatrix = glm::mat4(1.0)*TransMatrix;
	glm::vec4 BulletPos = glm::vec4(0.0f, 1.31f, 2.1f, 1.0f);
	glm::mat4 Transbase = glm::translate(mat4(), vec3(xb, 0.0f, zb));
	glm::mat4 Trans0 = glm::translate(mat4(), vec3(-xb, 0.0f, -zb));
	RotationMatrix1 = glm::rotate(mat4(), rangle1, vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Trans1 = glm::translate(mat4(), vec3(0, -1.5f, 0));
	glm::mat4 Trans2 = glm::translate(mat4(), vec3(0, 1.5f, 0));
	glm::mat4 RotaCom2 = glm::rotate(mat4(), -(rangle1), vec3(0.0f, 1.0f, 0.0f));
	RotationMatrix2 = glm::rotate(mat4(), rangle2, vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 Trans3 = glm::translate(mat4(), vec3(0, -2.2f, 1.3f));
	glm::mat4 Trans4 = glm::translate(mat4(), vec3(0, 2.2f, -(1.3f)));
	glm::mat4 RotaCom3 = glm::rotate(mat4(), -(rangle2), vec3(1.0f, 0.0f, 0.0f));
	RotationMatrix3 = (glm::rotate(mat4(), rangle3, vec3(1.0f, 0.0f, 0.0f)));
	glm::mat4 RotaCom4 = (glm::rotate(mat4(), -rangle3, vec3(1.0f, 0.0f, 0.0f)));
	RotationMatrix4 = glm::rotate(mat4(), rangle4, vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Trans5 = glm::translate(mat4(), vec3(0, -1.3f, 1.25f));
	glm::mat4 Trans6 = glm::translate(mat4(), vec3(0, 1.3f, -(1.25f)));
	RotationMatrix5 = glm::rotate(mat4(), rangle5, vec3(1.0f, 0.0f, 0.0f));
	RotationMatrix6 = glm::rotate(mat4(), rangle6, vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 RotaCom5 = (glm::rotate(mat4(), -rangle4, vec3(0.0f, 1.0f, 0.0f)));
	glm::mat4 RotaCom6 = glm::rotate(mat4(), -rangle5, vec3(1.0f, 0.0f, 0.0f));

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); 
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		//glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1fv(pickingColorArrayID, 10, pickingColor);	// here we pass in the picking marker array
		glUniform1i(pickID, 0);

		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		//	glUniformMatrix4fv(LightID, 1, GL_FALSE, &ModelMatrix[0][0]);


		glBindVertexArray(VertexArrayId[0]);								// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);

		glUniform1i(pickID, 1);
		glBindVertexArray(VertexArrayId[1]);								// draw GridOne
		glDrawArrays(GL_LINES, 0, 22);
		glBindVertexArray(0);

		glUniform1i(pickID, 2);
		glBindVertexArray(VertexArrayId[2]);								// draw GridTwo
		glDrawArrays(GL_LINES, 0, 22);
		glBindVertexArray(0);

		//Base
		ModelMatrix = Transbase*ModelMatrix;

		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glUniform1i(pickID, 3);
		glBindVertexArray(VertexArrayId[3]);								// draw Base
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_base), Verts_base);
		glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);


		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;

		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Top
		glUniform1i(pickID, 4);
		glBindVertexArray(VertexArrayId[4]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_top), Verts_top);
		glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Arm1
		glUniform1i(pickID, 5);
		glBindVertexArray(VertexArrayId[5]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_arm1), Verts_arm1);
		glDrawElements(GL_TRIANGLES, NumIndices[5], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//Joint
		glUniform1i(pickID, 6);
		glBindVertexArray(VertexArrayId[6]);								// draw Joint
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_joint), Verts_joint);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_joint), Verts_joint);
		glDrawElements(GL_TRIANGLES, NumIndices[6], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		//Arm2
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotaCom4*ModelMatrix;
		ModelMatrix = RotationMatrix4*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform1i(pickID, 7);
		glBindVertexArray(VertexArrayId[7]);								// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_arm2), Verts_arm2);
		glDrawElements(GL_TRIANGLES, NumIndices[7], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);


		//Pen
		ModelMatrix = Trans0*ModelMatrix;
		ModelMatrix = RotaCom2*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotaCom3*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotaCom4*ModelMatrix;

		ModelMatrix = RotaCom5*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans5*ModelMatrix;
		ModelMatrix = RotationMatrix5*ModelMatrix;
		ModelMatrix = RotaCom6*ModelMatrix;
		ModelMatrix = RotationMatrix6*ModelMatrix;
		ModelMatrix = RotationMatrix5*ModelMatrix;
		ModelMatrix = Trans6*ModelMatrix;
		ModelMatrix = Trans3*ModelMatrix;
		ModelMatrix = RotationMatrix4*ModelMatrix;
		ModelMatrix = RotationMatrix3*ModelMatrix;
		ModelMatrix = Trans4*ModelMatrix;
		ModelMatrix = Trans1*ModelMatrix;
		ModelMatrix = RotationMatrix2*ModelMatrix;
		ModelMatrix = Trans2*ModelMatrix;
		ModelMatrix = RotationMatrix1*ModelMatrix;
		ModelMatrix = Transbase*ModelMatrix;
		glUniformMatrix4fv(PickingViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(PickingProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(PickingModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glUniform1i(pickID, 8);
		glBindVertexArray(VertexArrayId[8]);								// draw Pen
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_pen), Verts_pen);
		glDrawElements(GL_TRIANGLES, NumIndices[8], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		//button
		glUniform1i(pickID, 9);
		glBindVertexArray(VertexArrayId[9]);								// draw Button
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts_button), Verts_button);
		glDrawElements(GL_TRIANGLES, NumIndices[9], GL_UNSIGNED_SHORT, (void*)0);
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
	gPickedIndex = int(data[0]);

	if (gPickedIndex == 255) 
	{ // Full white, must be the background !
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "background";
	}
	else if (gPickedIndex == 0)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "Axis";
	}
	else if (gPickedIndex == 1)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "Grid";
	}
	else if (gPickedIndex == 2)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "Grid";
	}
	else if (gPickedIndex == 3)
	{
		b = 1;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		giveColor();
		gMessage = "Base";
	}
	else if (gPickedIndex == 4)
	{
		b = 0;
		J_1 = 1;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		giveColor();
		gMessage = "Top";
	}
	else if (gPickedIndex == 5)
	{
		b = 0;
		J_1 = 0;
		j2 = 1;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		giveColor();
		gMessage = "Arm1";
	}
	else if (gPickedIndex == 6)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "Joint";
	}
	else if (gPickedIndex == 7)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 1;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		giveColor();
		gMessage = "Arm2";
	}
	else if (gPickedIndex == 8)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 1;
		j6 = 0;
		giveColor();
		gMessage = "Pen";
	}
	else if (gPickedIndex == 9)
	{
		b = 0;
		J_1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;
		defaultColor(token);
		gMessage = "Button";
	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
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
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

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
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	calculateCam();
	gViewMatrix = glm::lookAt(glm::vec3(cam[0], cam[1], cam[2]),	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		glm::vec3(0.0, 1.0, 0.0));	// up

									// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	PickingModelMatrixID = glGetUniformLocation(pickingProgramID, "M");
	PickingViewMatrixID = glGetUniformLocation(pickingProgramID, "V");
	PickingProjMatrixID = glGetUniformLocation(pickingProgramID, "P");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickID = glGetUniformLocation(pickingProgramID,"Pick_id");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	Light2ID = glGetUniformLocation(programID, "Light2Position_worldspace");
	AmbientID = glGetUniformLocation(programID, "AmbientColor_definit");
	PowerID = glGetUniformLocation(programID, "Light_Power");

	createObjects();
}

void createVAOs(Vertex Vertices[], GLushort Indices[], int ObjectId) {

	NumberVert[ObjectId] = IndexBufferSize[ObjectId]/sizeof(GLubyte);
	//printf("Numvert %d\n", NumberVert[ObjectId]);
	GLenum ErrorCheckValue = glGetError();
	std::vector<glm::vec3> normals;
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;

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
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

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

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_P:
			break;
		case GLFW_KEY_B:
			break;
		case GLFW_KEY_T:
			break;
		case GLFW_KEY_1:
			break;
		case GLFW_KEY_2:
			break;
		default:
			break;
		}
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		camera = (camera + 1) % 2;
	}
	if (glfwGetKey(window, GLFW_KEY_B) || b == 1)
	{
		giveColor();
	}
	if (glfwGetKey(window, GLFW_KEY_T) || J_1 == 1)
	{
		giveColor();
	}
	if (glfwGetKey(window, GLFW_KEY_1) || j2 == 1)
	{
		giveColor();
	}
	if (glfwGetKey(window, GLFW_KEY_2) || j3 == 1)
	{
		giveColor();
	}
	if (glfwGetKey(window, GLFW_KEY_P) || j5 == 1)
	{
		giveColor();
	}
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
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
	do {
		
		if (camera == 1)
		{
			//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
			std::ostringstream oss;
			oss << "Camera";
			gMessage = oss.str();
			defaultColor(10);
			b = 0;
			J_1 = 0;
			j2 = 0;
			j3 = 0;
			j4 = 0;
			j5 = 0;
			j6 = 0;
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
		}
		else
		{
			glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
		}
		if (camera == 0)
		{
			if (glfwGetKey(window, GLFW_KEY_B) || b == 1)
			{
				std::ostringstream oss;
				oss << "Base";
				gMessage = oss.str();
				b = 1;
				J_1 = 0;
				j2 = 0;
				j3 = 0;
				j4 = 0;
				j5 = 0;
				j6 = 0;
				if (glfwGetKey(window, GLFW_KEY_UP) && xb < 4.5f)
				{
					xb += 0.1;
				}
				if (glfwGetKey(window, GLFW_KEY_DOWN) && xb > -4.5f)
				{
					xb -= 0.1;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT) && zb < 4.5f)
				{
					zb += 0.1;
				}
				if (glfwGetKey(window, GLFW_KEY_LEFT) && zb> -4.5f)
				{
					zb -= 0.1;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_T) || J_1 == 1)
			{
				std::ostringstream oss;
				oss << "Top";
				gMessage = oss.str();
				J_1 = 1;
				j2 = 0;
				j3 = 0;
				b = 0;
				j5 = 0;

				if (glfwGetKey(window, GLFW_KEY_LEFT))
				{
					rangle1 += 0.05;
					if (rangle1 > 6.282)
						rangle1 -= 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					rangle1 -= 0.05;
					if (rangle1 < 0)
						rangle1 += 6.282;
				}

			}
			if (glfwGetKey(window, GLFW_KEY_1) || j2 == 1)
			{
				std::ostringstream oss;
				oss << "Arm1";
				gMessage = oss.str();
				j2 = 1;
				J_1 = 0;
				j3 = 0;
				b = 0;
				j5 = 0;
				if (glfwGetKey(window, GLFW_KEY_LEFT))
				{
					rangle2 += 0.05;
					if (rangle2 > 6.282)
						rangle2 -= 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					rangle2 -= 0.05;
					if (rangle2 < 0)
					rangle2 += 6.282;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_2) || j3 == 1)
			{
				std::ostringstream oss;
				oss << "Arm2";
				gMessage = oss.str();
				j3 = 1;
				j2 = 0;
				J_1 = 0;
				b = 0;
				j5 = 0;
				if (glfwGetKey(window, GLFW_KEY_LEFT))
				{
					rangle3 += 0.05;
					if (rangle3 > 6.282)
						rangle3 -= 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					rangle3 -= 0.05;
					if (rangle3 < 0)
						rangle3 += 6.282;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_P) || j5 == 1)
			{
				std::ostringstream oss;
				oss << "Pen";
				gMessage = oss.str();
				j2 = 0;
				J_1 = 0;
				j3 = 0;
				b = 0;
				j5 = 1;
				if (glfwGetKey(window, GLFW_KEY_LEFT) && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
				{
					j4 = 1;
					rangle4 += 0.05;
					if (rangle4 > 6.282)
						rangle4 -= 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT) && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
				{
					j4 = 1;
					rangle4 -= 0.05;
					if (rangle4 < 0)
						rangle4 += 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_UP))
				{
					j5 = 1;
					rangle5 += 0.05;
					if (rangle5 > 6.282)
						rangle5 -= 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_DOWN))
				{
					j5 = 1;
					rangle5 -= 0.05;
					if (rangle5 < 0)
						rangle5 += 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					j6 = 1;
					rangle6 -= 0.09;
					if (rangle6 < 0)
						rangle6 += 6.282;
				}
				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_LEFT))
				{
					j6 = 1;
					rangle6 += 0.09;
					if (rangle6 > 6.282)
						rangle6 -= 6.282;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_J) || animation == 1)
			{
				std::ostringstream oss;
				oss << "Jump";
				gMessage = oss.str();
				defaultColor(10);
				animation = 1;
				b = 0;
				J_1 = 0;
				j2 = 0;
				j3 = 0;
				j4 = 0;
				j5 = 0;
				j6 = 0;
				if (first == 1)
				{
					alpha = t[0] - p[0];
					beta = t[1] - p[1];
					gamma = t[2] - p[2];
					V = glm::normalize(vec3(alpha, beta, gamma));
					tempV = glm::normalize(vec3(alpha, 0, gamma));
					first = 0;
				}
				if (alpha >= 0)
				{
					si = glm::dot(V, vec3(0, 1, 0));
					tempsi = glm::dot(tempV, vec3(0, 0, 1));
					co = cosf(glm::asin(si));
					horizontalMovement = (bspeed*co*time);
					zanime = horizontalMovement*tempsi;
					tempco = cosf(glm::asin(tempsi));
					xanime = horizontalMovement*tempco;
					yanime = ((bspeed*si*time) - (0.5f*9.8f*time*time));
				}
				if (alpha<0)
				{
					si = glm::dot(V, vec3(0, 1, 0));
					tempsi = glm::dot(tempV, vec3(0, 0, 1));
					co = cosf(glm::asin(si));
					horizontalMovement = (bspeed*co*time);
					zanime = horizontalMovement*tempsi;
					tempco = cosf(glm::asin(tempsi));
					xanime = -(horizontalMovement*tempco);
					yanime = ((bspeed*si*time) - (0.5f*9.8f*time*time));
				}
				if (t[1] < 0)
				{
					animation = 0;
					xanime = 0;
					zanime = 0;
					yanime = 0;
					time = 0;
					xb = t[0];
					zb = t[2];
					first = 1;
					t = glm::vec4(0.0f, 10.0f, 10.0f, 10.0f);
					createVAOs(Verts_bullet, Idcs_bullet, 10);

				}
				time = time + 0.01f;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_K))
			pri = 1;
		else
			pri = 0;

		// DRAWING POINTS
		//giveColor();
		renderScene();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}