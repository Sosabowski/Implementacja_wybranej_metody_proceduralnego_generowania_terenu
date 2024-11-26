#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include "shaderLoader.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <iostream>

using namespace std;

int screen_width = 640;
int screen_height = 480;

GLint uniformTex0, uniformTex1;
int MouseX; 
int MouseY;
int mbutton;
double kameraX = 1000.0;
double kameraZ = 20.0;
double kameraD = -3;
double kameraPredkosc;
double kameraKat = -20;
double kameraPredkoscObrotu;
double poprzednie_kameraX;
double poprzednie_kameraZ;
double poprzednie_kameraD;

double rotation = 0;
double blend = 0.5;
double blend2 = 0;

glm::mat4 MV;
glm::mat4 P;

GLuint programID = 0;
unsigned int VBO, ebo;
unsigned int VAO[1];

double dx = -5;
double alpha = 1.0;

std::vector<float>wierzcholki;
std::vector<unsigned int>indeks;

//Tworzymy tablicę dynamiczną
float** alokacja(int rozmiar) {
	float** tablica = new float* [rozmiar];
	for (int i = 0; i < rozmiar; ++i) {
		tablica[i] = new float[rozmiar];
	}
	return tablica;
}

//Usuwamy tablicę dynamiczną
void zwolnienie(float** tablica, int rozmiar) {
	for (int i = 0; i < rozmiar; ++i) {
		delete[] tablica[i];
	}
	delete[] tablica;

}

float randomnumber() {
	srand(time(0));
	float randomnumber = (float)(rand() % 10 - 50) / 50.0f;
	return randomnumber;

}



void diamondSquare(float** heights, int rozmiar) {
	 //Żeby za każdym razem 
	int step = rozmiar - 1; //Krok musi być mniejszy o jeden
	//Dajemy narożnikom losowe wartości
	heights[0][0] = randomnumber();
	heights[0][step] = randomnumber();
	heights[step][0] = randomnumber();
	heights[step][step] = randomnumber();

	while (step > 1)
	{
		int halfStep = step / 2;
		// Diax
		for (int i = 0; i < rozmiar - 1; i += step)
			for (int j = 0; j < rozmiar - 1; j += step)
			{
				heights[i + halfStep][j + halfStep] =
					(heights[i][j + step] +
						heights[i][j] +
						heights[i + step][j + step] +
						heights[i + step][j]) / 4.0f + randomnumber();
			}

		//Kwadracik
		for (int i = 0; i < rozmiar; i += halfStep)
			for (int j = (i + halfStep) % step; j < rozmiar; j += step)
			{
				float suma = 0.0f;
				int ile = 0;
				//Taka mała sekcja, gdzie sprawdzamy czy wychodzimy poza obszar naszej tablicy
				if (i - halfStep >= 0) {
					suma += heights[i - halfStep][j];
					ile++;
				}
				if (i + halfStep < rozmiar) {
					suma += heights[i + halfStep][j];
					ile++;
				}
				if (j - halfStep >= 0) {
					suma += heights[i][j - halfStep];
					ile++;
				}
				if (j + halfStep < rozmiar) {
					suma += heights[i][j + halfStep];
					ile++;
				}

				heights[i][j] = suma / ile + randomnumber();
			}
		step /= 2;
	}
}


void generujsiatke(float** wysokosc, int rozmiar, std::vector<float>& wierzcholki, std::vector<unsigned int>& indeks){
	for (int i=0; i < rozmiar; i++) {
		for (int j = 0; j < rozmiar; j++) {
			wierzcholki.push_back((float)j);
			wierzcholki.push_back(wysokosc[i][j]);
			wierzcholki.push_back((float)i);
		}
	}
	for (int i = 0; i < rozmiar-1; i++) {
		for (int j = 0; j < rozmiar-1; j++) {
			int goralewa = i * rozmiar + j;
			int goraprawo = goralewa + 1;
			int dollewo = goralewa + rozmiar;
			int dolprawo = dollewo + 1;

			indeks.push_back(goralewa);
			indeks.push_back(dollewo);
			indeks.push_back(goraprawo);

			indeks.push_back(goraprawo);
			indeks.push_back(dollewo);
			indeks.push_back(dolprawo);
		}
	}

}


void mouse(int button, int state, int x, int y)
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		MouseX = x;
		MouseY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		break;

	}
}


void mysz_ruch(int x, int y)
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - (MouseX - x) * 0.1;
		kameraZ = poprzednie_kameraZ - (MouseY - y) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + (MouseY - y) * 0.1;
	}

}

void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	GLuint MVP_id = glGetUniformLocation(programID, "MVP");
	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(1, -1, kameraD + dx));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));
	glm::mat4 MVP = P * MV;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));
	glBindVertexArray(VAO[0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indeks.size(), GL_UNSIGNED_INT, 0);
	
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
	glFlush();
	glutSwapBuffers();
}

void timer(int value) {

	glutTimerFunc(20, timer, 0);
}

void idle()
{

	glutPostRedisplay();
}

void size(int width, int height)
{
	screen_width = width;
	screen_height = height;

	glViewport(0, 0, screen_width, screen_height); 

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screen_width / (GLfloat)screen_height, 1.0f, 1000.0f);

	glutPostRedisplay();
}




int main(int argc, char** argv){

	int rozmiar = 257;
	float** wysokosc = alokacja(rozmiar);
	diamondSquare(wysokosc, rozmiar);

	generujsiatke(wysokosc, rozmiar, wierzcholki, indeks);
	zwolnienie(wysokosc, rozmiar);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lorem Ipsum");


	glewInit();


	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutTimerFunc(20, timer, 0);
	glutReshapeFunc(size);
	glutMouseFunc(mouse); 	
	glutMotionFunc(mysz_ruch); 

	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, wierzcholki.size()*sizeof(float), wierzcholki.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeks.size()*sizeof(unsigned int), indeks.data(), GL_STATIC_DRAW);

	programID = loadShaders("vertex_shader.glsl", "fragment_shader2.glsl");

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glutMainLoop();

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, VAO);
	return 0;
}

