// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
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


typedef struct Vertex {
	float XYZW[4];
	float RGBA[4];
	void SetCoords(float *coords) {
		XYZW[0] = coords[0];
		XYZW[1] = coords[1];
		XYZW[2] = coords[2];
		XYZW[3] = coords[3];
	}
	void SetColor(float *color) {
		RGBA[0] = color[0];
		RGBA[1] = color[1];
		RGBA[2] = color[2];
		RGBA[3] = color[3];
	}
};

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
typedef struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z) {};
	point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]) {};
	point operator -(const point& a)const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a)const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a)const {
		return point(x*a, y*a, z*a);
	}
	point operator /(const float& a)const {
		return point(x / a, y / a, z / a);
	}
	float* toArray() {
		float array[] = { x, y, z, 1.0f };
		return array;
	}
};

// function prototypes
void saveVertex(Vertex[], int);
void saveForDisplay_one();
void saveForDisplay_two();
void saveForDisplay_three();
void saveForDisplay_four();
void calculate(int);
void getRealCP();
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], unsigned short[], size_t, size_t, int);
void createObjects(void);
void pickVertex(void);
void moveVertex(void);
void drawScene(void);
void cleanup(void);
static void mouseCallback(GLFWwindow*, int, int, int);
static void key_CallBack(GLFWwindow*, int, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

int m = 0;
int Pressed = 0;

// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 8;	// number of different "objects" to be drawn
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5,6 ,7 };
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5,6 ,7 };
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5,6 ,7 };
size_t NumVert[NumObjects] = { 0, 1, 2, 3, 4, 5,6 ,7 };

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint LightID;

// Define objects
int two = 2;
int time = 8;
int pr = 0;
Vertex Previous[128], Next[128], NextC[32];
float color[] = { 0.0f, 1.0f,1.0f, 1.0f };

Vertex Vertcies_CP[32];
unsigned short Indices_CP[32];

const size_t IndexCount_CP = sizeof(Indices_CP) / sizeof(unsigned short);
Vertex Vertcies_Final[240];
unsigned short Indices_Final[240];

const size_t IndexCount_Final = sizeof(Indices_Final) / sizeof(unsigned short);

Vertex Vertcies_one[16];
unsigned short Indices_one[16];

const size_t IndexCount_one = sizeof(Indices_one) / sizeof(unsigned short);
Vertex Vertcies_two[32];
unsigned short Indices_two[32];

const size_t IndexCount_two = sizeof(Indices_two) / sizeof(unsigned short);
Vertex Vertcies_three[64];
unsigned short Indices_three[64];

const size_t IndexCount_three = sizeof(Indices_three) / sizeof(unsigned short);
Vertex Vertcies_four[128];
unsigned short Indices_four[128];

const size_t IndexCount_four = sizeof(Indices_four) / sizeof(unsigned short);

Vertex Vertices[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 0
	{ { 0.0f, 1.41f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 2
	{ { -1.41f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 4
	{ { 0.0f, -1.41f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 6
	{ { 1.41f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 7
};

unsigned short Indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};

const size_t IndexCount = sizeof(Indices) / sizeof(unsigned short);
// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f, 4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f };

Vertex Vertices_Lines[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 0
	{ { 0.0f, (sqrt(two)), 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 2
	{ { -(sqrt(two)), 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 4
	{ { 0.0f, -(sqrt(two)), 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 6
	{ { (sqrt(two)), 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 7
};

unsigned short Indices_Lines[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};

const size_t IndexCount_Lines = sizeof(Indices_Lines) / sizeof(unsigned short);
// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor_Lines[IndexCount_Lines] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f, 4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f };
// ATTN: ADD YOU PER-OBJECT GLOBAL ARRAY DEFINITIONS HERE

void createObjects(void)
{
	// ATTN: DERIVE YOUR NEW OBJECTS HERE:
	// each has one vertices {pos;color} and one indices array (no picking needed here)


}

void calculate(int time)
{
	//time = 8 * (pow(2, (m - 1)));
	int x = 0;
	for (int i = 0; i < time; i++)
	{

		Next[x].XYZW[0] = ((Previous[(i - 2 + time) % time].XYZW[0]) / 16.0f) + ((Previous[(i - 1 + time) % time].XYZW[0]) * 10.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[0]) * 5.0f / 16.0f);
		Next[x].XYZW[1] = ((Previous[(i - 2 + time) % time].XYZW[1]) / 16.0f) + ((Previous[(i - 1 + time) % time].XYZW[1]) * 10.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[1]) * 5.0f / 16.0f);
		Next[x].XYZW[2] = ((Previous[(i - 2 + time) % time].XYZW[2]) / 16.0f) + ((Previous[(i - 1 + time) % time].XYZW[2]) * 10.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[2]) * 5.0f / 16.0f);
		Next[x].XYZW[3] = 1.0f;

		x++;

		Next[x].XYZW[0] = ((Previous[(i - 1 + time) % time].XYZW[0]) * 5.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[0]) * 10.0f / 16.0f) + ((Previous[(i + 1 + time) % time].XYZW[0]) / 16.0f);
		Next[x].XYZW[1] = ((Previous[(i - 1 + time) % time].XYZW[1]) * 5.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[1]) * 10.0f / 16.0f) + ((Previous[(i + 1 + time) % time].XYZW[1]) / 16.0f);
		Next[x].XYZW[2] = ((Previous[(i - 1 + time) % time].XYZW[2]) * 5.0f / 16.0f) + ((Previous[(i + time) % time].XYZW[2]) * 10.0f / 16.0f) + ((Previous[(i + 1 + time) % time].XYZW[2]) / 16.0f);
		Next[x].XYZW[3] = 1.0f;

		x++;
	}
}
void calculate2()
{
	int i, j, u, k = 0;
	for (i = 0; i < 8; i++) {

		NextC[k].XYZW[0] = (Previous[(8 + i - 2) % 8].XYZW[0] + Previous[(8 + i + 1) % 8].XYZW[0] + 11.0f * (Previous[(8 + i - 1) % 8].XYZW[0]) + 11.0f * (Previous[(8 + i) % 8].XYZW[0])) / 24.0f;
		NextC[k].XYZW[1] = (Previous[(8 + i - 2) % 8].XYZW[1] + Previous[(8 + i + 1) % 8].XYZW[1] + 11.0f * (Previous[(8 + i - 1) % 8].XYZW[1]) + 11.0f * (Previous[(8 + i) % 8].XYZW[1])) / 24.0f;
		NextC[k].XYZW[2] = (Previous[(8 + i - 2) % 8].XYZW[2] + Previous[(8 + i + 1) % 8].XYZW[2] + 11.0f * (Previous[(8 + i - 1) % 8].XYZW[2]) + 11.0f * (Previous[(8 + i) % 8].XYZW[2])) / 24.0f;

		k++;

		NextC[k].XYZW[0] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[0]) + 7.0f * (Previous[(8 + i) % 8].XYZW[0]) + Previous[(8 + i + 1) % 8].XYZW[0]) / 12.0f;
		NextC[k].XYZW[1] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[1]) + 7.0f * (Previous[(8 + i) % 8].XYZW[1]) + Previous[(8 + i + 1) % 8].XYZW[1]) / 12.0f;
		NextC[k].XYZW[2] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[2]) + 7.0f * (Previous[(8 + i) % 8].XYZW[2]) + Previous[(8 + i + 1) % 8].XYZW[2]) / 12.0f;

		k++;
		NextC[k].XYZW[0] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[0]) + 16.0f * (Previous[(8 + i) % 8].XYZW[0]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[0])) / 24.0f;
		NextC[k].XYZW[1] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[1]) + 16.0f * (Previous[(8 + i) % 8].XYZW[1]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[1])) / 24.0f;
		NextC[k].XYZW[2] = (4.0f * (Previous[(8 + i - 1) % 8].XYZW[2]) + 16.0f * (Previous[(8 + i) % 8].XYZW[2]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[2])) / 24.0f;

		k++;
		NextC[k].XYZW[0] = (Previous[(8 + i - 1) % 8].XYZW[0] + 7.0f * (Previous[(8 + i) % 8].XYZW[0]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[0])) / 12.0f;
		NextC[k].XYZW[1] = (Previous[(8 + i - 1) % 8].XYZW[1] + 7.0f * (Previous[(8 + i) % 8].XYZW[1]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[1])) / 12.0f;
		NextC[k].XYZW[2] = (Previous[(8 + i - 1) % 8].XYZW[2] + 7.0f * (Previous[(8 + i) % 8].XYZW[2]) + 4.0f * (Previous[(8 + i + 1) % 8].XYZW[2])) / 12.0f;

		k++;

	}
}
void calculateCurve()
{
	int p = 0;
	Vertex Q[5];
	for (int i = 0; i < 32; i = i + 4) {
		for (int u = 0; u < 30; u++) {
			for (int j = 0; j < 5; j++) {
				Q[j].XYZW[0] = NextC[i + j].XYZW[0];
				Q[j].XYZW[1] = NextC[i + j].XYZW[1];
			}
			for (int j = 1; j <= 4; j++)
				for (int k = 0; k <= 4 - j; k++)
				{
					Q[k].XYZW[0] = (1.0f - (u / 30.0f))*(Q[k].XYZW[0]) + (u / 30.0f)*(Q[k + 1].XYZW[0]);
					Q[k].XYZW[1] = (1.0f - (u / 30.0f))*(Q[k].XYZW[1]) + (u / 30.0f)*(Q[k + 1].XYZW[1]);
				}
			Vertcies_Final[p].XYZW[0] = Q[0].XYZW[0];
			Vertcies_Final[p].XYZW[1] = Q[0].XYZW[1];
			Vertcies_Final[p].XYZW[2] = 0.0f;
			Vertcies_Final[p].XYZW[3] = 1.0f;
			Vertcies_Final[p].RGBA[0] = 1.0f;
			Vertcies_Final[p].RGBA[1] = 1.0f;
			Vertcies_Final[p].RGBA[2] = 0.0f;
			Vertcies_Final[p].RGBA[3] = 1.0f;
			p++;

		}

	}
}
void saveForDisplay() {

	for (int i = 0; i < 32; i++)
	{
		Vertcies_CP[i].XYZW[0] = NextC[i].XYZW[0];
		Vertcies_CP[i].XYZW[1] = NextC[i].XYZW[1];
		Vertcies_CP[i].XYZW[2] = NextC[i].XYZW[2];
		Vertcies_CP[i].XYZW[3] = 1.0f;
		Vertcies_CP[i].RGBA[0] = 1.0f;
		Vertcies_CP[i].RGBA[1] = 0.0f;
		Vertcies_CP[i].RGBA[2] = 0.0f;
		Vertcies_CP[i].RGBA[3] = 1.0f;
	}

}
void saveForDisplay_one() {

	for (int i = 0; i < 16; i++)
	{
		Vertcies_one[i].XYZW[0] = Next[i].XYZW[0];
		Vertcies_one[i].XYZW[1] = Next[i].XYZW[1];
		Vertcies_one[i].XYZW[2] = Next[i].XYZW[2];
		Vertcies_one[i].XYZW[3] = Next[i].XYZW[3];
		Vertcies_one[i].RGBA[0] = color[0];
		Vertcies_one[i].RGBA[1] = color[1];
		Vertcies_one[i].RGBA[2] = color[2];
		Vertcies_one[i].RGBA[3] = color[3];
	}

}
void saveForDisplay_two() {

	for (int i = 0; i < 32; i++)
	{
		Vertcies_two[i].XYZW[0] = Next[i].XYZW[0];
		Vertcies_two[i].XYZW[1] = Next[i].XYZW[1];
		Vertcies_two[i].XYZW[2] = Next[i].XYZW[2];
		Vertcies_two[i].XYZW[3] = Next[i].XYZW[3];
		Vertcies_two[i].RGBA[0] = color[0];
		Vertcies_two[i].RGBA[1] = color[1];
		Vertcies_two[i].RGBA[2] = color[2];
		Vertcies_two[i].RGBA[3] = color[3];
	}

}
void saveForDisplay_three() {

	for (int i = 0; i < 64; i++)
	{
		Vertcies_three[i].XYZW[0] = Next[i].XYZW[0];
		Vertcies_three[i].XYZW[1] = Next[i].XYZW[1];
		Vertcies_three[i].XYZW[2] = Next[i].XYZW[2];
		Vertcies_three[i].XYZW[3] = Next[i].XYZW[3];
		Vertcies_three[i].RGBA[0] = color[0];
		Vertcies_three[i].RGBA[1] = color[1];
		Vertcies_three[i].RGBA[2] = color[2];
		Vertcies_three[i].RGBA[3] = color[3];
	}

}
void saveForDisplay_four() {

	for (int i = 0; i < 128; i++)
	{
		Vertcies_four[i].XYZW[0] = Next[i].XYZW[0];
		Vertcies_four[i].XYZW[1] = Next[i].XYZW[1];
		Vertcies_four[i].XYZW[2] = Next[i].XYZW[2];
		Vertcies_four[i].XYZW[3] = Next[i].XYZW[3];
		Vertcies_four[i].RGBA[0] = color[0];
		Vertcies_four[i].RGBA[1] = color[1];
		Vertcies_four[i].RGBA[2] = color[2];
		Vertcies_four[i].RGBA[3] = color[3];
	}

}

void saveVertex(Vertex Vertices[], int n) {

	for (int i = 0; i < n; i++)
	{
		Previous[i].XYZW[0] = Vertices[i].XYZW[0];
		Previous[i].XYZW[1] = Vertices[i].XYZW[1];
		Previous[i].XYZW[2] = Vertices[i].XYZW[2];
		Previous[i].XYZW[3] = Vertices[i].XYZW[3];
		Previous[i].RGBA[0] = Vertices[i].RGBA[0];
		Previous[i].RGBA[1] = Vertices[i].RGBA[1];
		Previous[i].RGBA[2] = Vertices[i].RGBA[2];
		Previous[i].RGBA[3] = Vertices[i].RGBA[3];
	}
}



void drawScene(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glEnable(GL_PROGRAM_POINT_SIZE);


		glBindVertexArray(VertexArrayId[0]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		// update buffer data
		//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(VertexArrayId[1]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices_Lines), Vertices_Lines);
		glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		glDisable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0f);
		if (m == 1)
		{
			glBindVertexArray(VertexArrayId[2]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_one), Vertcies_one);
			glDrawElements(GL_LINE_STRIP, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		else if (m == 2)
		{
			glBindVertexArray(VertexArrayId[3]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_two), Vertcies_two);
			glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		else if (m == 3)
		{
			glBindVertexArray(VertexArrayId[4]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_three), Vertcies_three);
			glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		else if (m == 4)
		{
			glBindVertexArray(VertexArrayId[5]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_four), Vertcies_four);
			glDrawElements(GL_LINE_STRIP, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		if (pr == 1 || pr == 3)
		{
			glBindVertexArray(VertexArrayId[6]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_Final), Vertcies_Final);
			glDrawElements(GL_LINE_STRIP, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[7]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_CP), Vertcies_CP);
			glDrawElements(GL_POINTS, NumVert[7], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
			glBindVertexArray(0);
		}
		else if (pr == 2)
		{

			glBindVertexArray(VertexArrayId[7]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertcies_CP), Vertcies_CP);
			glDrawElements(GL_POINTS, NumVert[7], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
			glBindVertexArray(0);
		}

	}
	glUseProgram(0);
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickVertex(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1fv(pickingColorArrayID, NumVert[0], pickingColor);	// here we pass in the picking marker array

																		// Draw the ponts
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
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
	Vertices[gPickedIndex].RGBA[0] = 0.0f;
	Vertices[gPickedIndex].RGBA[1] = 1.0f;
	Vertices[gPickedIndex].RGBA[2] = 0.0f;
	Vertices[gPickedIndex].RGBA[3] = 1.0f;

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

// fill this function in!
void moveVertex(void)
{
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);

	// retrieve your cursor position
	// get your world coordinates
	// move points
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data2[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data2);

	if (gPickedIndex == 255) { // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
		int i = gPickedIndex;
		Vertices[gPickedIndex].RGBA[0] = 1.0f;
		Vertices[gPickedIndex].RGBA[1] = 0.0f;
		Vertices[gPickedIndex].RGBA[2] = 3.0f;
		Vertices[gPickedIndex].RGBA[3] = 1.0f;

		glm::vec3 pos = glm::vec3(xpos, ypos, 0.0f);
		glm::vec3 unpro = glm::unProject(pos, ModelMatrix, gProjectionMatrix, vp);
		Vertices[gPickedIndex].XYZW[0] = -unpro[0];
		Vertices[gPickedIndex].XYZW[1] = -unpro[1];
		Vertices[gPickedIndex].XYZW[2] = -unpro[2];
		Vertices_Lines[gPickedIndex].XYZW[0] = -unpro[0];
		Vertices_Lines[gPickedIndex].XYZW[1] = -unpro[1];
		Vertices_Lines[gPickedIndex].XYZW[2] = -unpro[2];
		getRealCP();
		saveVertex(Vertices, 8);
		calculate2();
		saveForDisplay();
		calculateCurve();
	}

}

void getRealCP()
{
	saveVertex(Vertices, 8);
	if (m == 1)
	{
		calculate(8);
		saveForDisplay_one();
		saveVertex(Vertcies_one, 16);
	}
	else if (m == 2)
	{
		calculate(8);
		saveForDisplay_one();
		saveVertex(Vertcies_one, 16);
		calculate(16);
		saveForDisplay_two();
		saveVertex(Vertcies_two, 32);
	}
	else if (m == 3)
	{

		calculate(8);
		saveForDisplay_one();
		saveVertex(Vertcies_one, 16);
		calculate(16);
		saveForDisplay_two();
		saveVertex(Vertcies_two, 32);
		calculate(32);
		saveForDisplay_three();
		saveVertex(Vertcies_three, 64);
	}
	else if (m == 4)
	{
		calculate(8);
		saveForDisplay_one();
		saveVertex(Vertcies_one, 16);
		calculate(16);
		saveForDisplay_two();
		saveVertex(Vertcies_two, 32);
		calculate(32);
		saveForDisplay_three();
		saveVertex(Vertcies_three, 64);
		calculate(64);
		saveForDisplay_four();
		saveVertex(Vertices, 8);
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
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

																			// Camera matrix
	gViewMatrix = glm::lookAt(
		glm::vec3(0, 0, -5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");


	createVAOs(Vertices, Indices, sizeof(Vertices), sizeof(Indices), 0);

	createVAOs(Vertices_Lines, Indices_Lines, sizeof(Vertices_Lines), sizeof(Indices_Lines), 1);

	createVAOs(Vertcies_one, Indices_one, sizeof(Vertcies_one), sizeof(Indices_one), 2);

	createVAOs(Vertcies_two, Indices_two, sizeof(Vertcies_two), sizeof(Indices_two), 3);

	createVAOs(Vertcies_three, Indices_three, sizeof(Vertcies_three), sizeof(Indices_three), 4);

	createVAOs(Vertcies_four, Indices_four, sizeof(Vertcies_four), sizeof(Indices_four), 5);

	createVAOs(Vertcies_Final, Indices_Final, sizeof(Vertcies_Final), sizeof(Indices_Final), 6);

	createVAOs(Vertcies_CP, Indices_CP, sizeof(Vertcies_CP), sizeof(Indices_CP), 7);

	createObjects();

	// ATTN: create VAOs for each of the newly created objects here:
	// createVAOs(<fill this appropriately>);

}

void createVAOs(Vertex Vertices[], unsigned short Indices[], size_t BufferSize, size_t IdxBufferSize, int ObjectId) {

	NumVert[ObjectId] = IdxBufferSize / (sizeof GLubyte);

	GLenum ErrorCheckValue = glGetError();
	size_t VertexSize = sizeof(Vertices[0]);
	size_t RgbOffset = sizeof(Vertices[0].XYZW);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_DYNAMIC_DRAW);

	// Create Buffer for indices
	glGenBuffers(1, &IndexBufferId[ObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IdxBufferSize, Indices, GL_DYNAMIC_DRAW);

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

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

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickVertex();

	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		pickVertex();

	}
}

static void key_CallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		getRealCP();
		m = (m + 1) % 5;
		if (m == 1)
		{
			time = 8 * (pow(2, (m - 1)));
			calculate(time);
			saveForDisplay_one();
			saveVertex(Vertcies_one, 16);
		}
		else if (m == 2)
		{
			time = 8 * (pow(2, (m - 1)));
			calculate(time);
			saveForDisplay_two();
			saveVertex(Vertcies_two, 32);
		}
		else if (m == 3)
		{
			time = 8 * (pow(2, (m - 1)));
			calculate(time);
			saveForDisplay_three();
			saveVertex(Vertcies_three, 64);
		}
		else if (m == 4)
		{
			time = 8 * (pow(2, (m - 1)));
			calculate(time);
			saveForDisplay_four();
			saveVertex(Vertices, 8);
		}

	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		pr = ((pr + 1) % 4);

		saveVertex(Vertices, 8);
		calculate2();
		saveForDisplay();
		calculateCurve();

	}

}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;
	for (int i = 0; i < 16; i++)
	{
		Indices_one[i] = i;
	}
	for (int i = 0; i < 32; i++)
	{
		Indices_two[i] = i;
	};
	for (int i = 0; i < 64; i++)
	{
		Indices_three[i] = i;
	}
	for (int i = 0; i < 128; i++)
	{
		Indices_four[i] = i;
	}
	for (unsigned short i = 0; i < 32; i++)
	{
		Indices_CP[i] = i;
	}
	for (int i = 0; i < 240; i++)
	{
		Indices_Final[i] = i;
	}
	saveVertex(Vertices, 8);
	calculate2();
	saveForDisplay();
	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
											 // printf and reset
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			moveVertex();
		}
		glfwSetKeyCallback(window, key_CallBack);
		// DRAWING SCENE
		createObjects();	// re-evaluate curves in case vertices have been moved
		drawScene();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	cleanup();

	return 0;
}