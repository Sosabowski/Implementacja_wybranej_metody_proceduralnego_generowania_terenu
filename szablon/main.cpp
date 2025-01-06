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
int klikniecia_z = 0;
int klikniecia_x = 0;
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
glm::vec3 lightPos(50.0f, 100.0f, 75.0f);
//glm::vec3 lightPos(1.0f, 0.0f, 0.0f);
GLuint programID = 0;
GLuint display = 3;

GLuint objectColor_id = 0;
GLuint lightColor_id = 0;
GLuint lightPos_id = 0;

unsigned int VBO, ebo, NBO;
unsigned int VAO[1];

//glm::vec3 centerOfGrid(0.0f, 0.0f, 0.0f); // Przechowuje środek siatki

double dx = 3;
double dy = 1;
double alpha = 1.0;
float lightStr = 75.0;

std::vector<float>wierzcholki;
std::vector<unsigned int>indeks;
std::vector<float> normalne;


//Tworzymy tablicę dynamiczną
float** alokacja(int rozmiar) {
	float** tablica = new float* [rozmiar];
	for (int i = 0; i < rozmiar; ++i) {
		tablica[i] = new float[rozmiar];
	}
	return tablica;
}

//Usuwamy tablicę dynamiczną
void zwolnienie(float** tablica, int rozmiar) { //TODO WYWALIĆ
	for (int i = 0; i < rozmiar; ++i) {
		delete[] tablica[i];
	}
	delete[] tablica;

}

float randomnumber() {
	//srand(time(0));
	float randomnumber = (float)(rand() % 100) / 50.0f-1.0f;
	return randomnumber;

}



void diamondSquare(float** heights, int rozmiar) { //TODO ZAMIENIĆ NA OBSŁUGĘ <vector>
	int step = rozmiar - 1;
	//Dajemy narożnikom losowe wartości
	heights[0][0] = randomnumber();
	heights[0][step] = randomnumber();
	heights[step][0] = randomnumber();
	heights[step][step] = randomnumber();

	while (step > 1)
	{
		int halfStep = step / 2; //Zmienna wykorzystywana w kroku kwadratowym
		// Krok diamentowy
		for (int i = 0; i < rozmiar - 1; i += step)
			for (int j = 0; j < rozmiar - 1; j += step)
			{
				heights[i + halfStep][j + halfStep] =
					(heights[i][j + step] +
						heights[i][j] +
						heights[i + step][j + step] +
						heights[i + step][j]) * 0.25f + randomnumber();
			}

		//Krok kwadratowy
		for (int i = 0; i < rozmiar; i += halfStep)
			for (int j = (i + halfStep) % step; j < rozmiar; j += step)
			{
				float sum = 0.0f;
				int count = 0;
				//Sprawdzamy, czy przypadkiem nie wychodzimy poza obszar tablicy
				if (i - halfStep >= 0) {
					sum += heights[i - halfStep][j];
					count++;
				}
				if (i + halfStep < rozmiar) {
					sum += heights[i + halfStep][j];
					count++;
				}
				if (j - halfStep >= 0) {
					sum += heights[i][j - halfStep];
					count++;
				}
				if (j + halfStep < rozmiar) {
					sum += heights[i][j + halfStep];
					count++;
				}

				heights[i][j] = sum / count + randomnumber();
			}
		step *= 0.5f;
	}
}


void generujsiatke(float** wysokosc, int rozmiar, std::vector<float>& wierzcholki, std::vector<unsigned int>& indeks) {
	for (int i = 0; i < rozmiar; i++) {
		for (int j = 0; j < rozmiar; j++) {
			wierzcholki.push_back((float)j);
			wierzcholki.push_back(wysokosc[i][j]);
			wierzcholki.push_back((float)i);
		}
	}
	for (int i = 0; i < rozmiar - 1; i++) {
		for (int j = 0; j < rozmiar - 1; j++) {
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

void obliczNormalne(const std::vector<float>& wierzcholki, const std::vector<unsigned int>& indeks, std::vector<float>& normalne, int rozmiar) {
	normalne.resize(wierzcholki.size(), 0.0f);

	for (size_t i = 0; i < indeks.size(); i += 3) {
		int idx0 = indeks[i] * 3;
		int idx1 = indeks[i + 1] * 3;
		int idx2 = indeks[i + 2] * 3;

		glm::vec3 v0(wierzcholki[idx0], wierzcholki[idx0 + 1], wierzcholki[idx0 + 2]);
		glm::vec3 v1(wierzcholki[idx1], wierzcholki[idx1 + 1], wierzcholki[idx1 + 2]);
		glm::vec3 v2(wierzcholki[idx2], wierzcholki[idx2 + 1], wierzcholki[idx2 + 2]);

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		for (int j = 0; j < 3; ++j) {
			normalne[idx0 + j] += normal[j];
			normalne[idx1 + j] += normal[j];
			normalne[idx2 + j] += normal[j];
		}
	}

	for (size_t i = 0; i < normalne.size(); i += 3) {
		glm::vec3 normal(normalne[i], normalne[i + 1], normalne[i + 2]);
		normal = glm::normalize(normal);
		normalne[i] = normal.x;
		normalne[i + 1] = normal.y;
		normalne[i + 2] = normal.z;
	}
}


enum { EXIT, siatka, kolor };
//funkcja do obsługi menu
void Menu(int value)
{

	switch (value)
	{
	case siatka:
		display = 1;
		break;
	case kolor:
		display = 2;
		break;
	case EXIT:
		exit(0);
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

void klawiatura(int key, int x, int y)
{
	double predkosc = 1.0;
	double predkosc_obrotu = 1.0;
	switch (key) {
	case GLUT_KEY_UP:
		dx += predkosc * cos(glm::radians(kameraX));
		kameraD += predkosc * sin(glm::radians(kameraX));
		break;
	case GLUT_KEY_DOWN:
		dx -= predkosc * cos(glm::radians(kameraX));
		kameraD -= predkosc * sin(glm::radians(kameraX));
		break;
	case GLUT_KEY_RIGHT:
		kameraX -= predkosc_obrotu;
		break;
	case GLUT_KEY_LEFT:
		kameraX += predkosc_obrotu;
		break;
	case GLUT_KEY_PAGE_DOWN:
		kameraZ += predkosc_obrotu;
		break;
	case GLUT_KEY_PAGE_UP:
		kameraZ -= predkosc_obrotu;
	}
}

void klawiatura_ascii(unsigned char key, int x, int y)
{
	if (key == 'z')
	{
		klikniecia_z++;

		int rozmiar = sqrt(wierzcholki.size() / 3); // Założenie: siatka jest kwadratowa
		int srodek = rozmiar * 0.5;                  // Środkowy punkt
		float promien = 8.0f;                      // Promień obszaru do modyfikacji

		float min_wysokosc = std::numeric_limits<float>::max();
		for (size_t i = 1; i < wierzcholki.size(); i += 3) {
			if (wierzcholki[i] < min_wysokosc) {
				min_wysokosc = wierzcholki[i];
			}
		}


		for (int i = 0; i < rozmiar; i++)
		{
			for (int j = 0; j < rozmiar; j++)
			{
				float dx = (float)j - srodek;
				float dy = (float)i - srodek;
				float odleglosc = sqrt(dx * dx + dy * dy);

				if (odleglosc <= promien)
				{
					int index = (i * rozmiar + j) * 3 + 1;
					if (klikniecia_z == 1)
					{
						wierzcholki[index] = min_wysokosc;
					}
					else
					{
						float zmiana = 0.5f * (promien - odleglosc) / promien;
						wierzcholki[index] -= zmiana;
					}
				}
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, wierzcholki.size() * sizeof(float), wierzcholki.data());
	}

	if (key == 'x')
	{
		klikniecia_z++;

		int rozmiar = sqrt(wierzcholki.size() / 3); // Założenie: siatka jest kwadratowa
		int srodek = rozmiar * 0.5;                  // Środkowy punkt
		float promien = 8.0f;                      // Promień obszaru do modyfikacji

		float min_wysokosc = std::numeric_limits<float>::max();
		for (size_t i = 1; i < wierzcholki.size(); i += 3) {
			if (wierzcholki[i] < min_wysokosc) {
				min_wysokosc = wierzcholki[i];
			}
		}


		for (int i = 0; i < rozmiar; i++)
		{
			for (int j = 0; j < rozmiar; j++)
			{
				float dx = (float)j - srodek;
				float dy = (float)i - srodek;
				float odleglosc = sqrt(dx * dx + dy * dy);

				if (odleglosc <= promien)
				{
					int index = (i * rozmiar + j) * 3 + 1;
					if (klikniecia_x == 1)
					{
						wierzcholki[index] = min_wysokosc;
					}
					else
					{
						float zmiana = 0.5f * (promien - odleglosc) / promien;
						wierzcholki[index] += zmiana;
					}
				}
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, wierzcholki.size() * sizeof(float), wierzcholki.data());
	}

	else if (key == 27)
	{
		exit(0);
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
	
	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(1, -1, kameraD + dx));

	MV = glm::scale(MV, glm::vec3(1, dy, 1));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));
	//MV = glm::rotate(MV, (float)glm::radians(-90.0f), glm::vec3(1, 0, 0));
	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programID, "MVP");
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);

	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));

	GLuint display_id = glGetUniformLocation(programID, "display");
	glUniform1i(display_id, display);

	glUniform3f(lightColor_id, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPos_id, lightPos[0], lightPos[1], lightPos[2]);

	GLuint lightStr_id = glGetUniformLocation(programID, "lightStr");
	glUniform1f(lightStr_id, lightStr);
	glBindVertexArray(VAO[0]);
	switch (display)
	{
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform3f(objectColor_id, 1.0f, 1.0f, 1.0f);
		break;

	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glUniform3f(objectColor_id, 1.0f, 1.0f, 0.0f);
		break;
	}

	glDrawElements(GL_TRIANGLES, indeks.size(), GL_UNSIGNED_INT, 0);
	
	//glDisable(GL_BLEND);
	//glDepthMask(GL_TRUE);

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




int main(int argc, char** argv) {

	int rozmiar = 4097;
	float** wysokosc = alokacja(rozmiar);
	diamondSquare(wysokosc, rozmiar);
	generujsiatke(wysokosc, rozmiar, wierzcholki, indeks);
	obliczNormalne(wierzcholki, indeks, normalne, rozmiar);
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
	glutSpecialFunc(klawiatura);
	glutKeyboardFunc(klawiatura_ascii);

	int wizualizacja = glutCreateMenu(Menu);
	glutAddMenuEntry("Siatka", siatka);//dodanie pozycji do menu
	glutAddMenuEntry("Kolor", kolor);//dodanie pozycji do menu
	// menu główne
	glutCreateMenu(Menu);
	glutAddSubMenu("Wizualizacja", wizualizacja);


	glutAddMenuEntry("Wyjscie", EXIT); //dodanie pozycji do menu
	glutAttachMenu(GLUT_RIGHT_BUTTON); //określenie przycisku myszki do obsługo menu

	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, wierzcholki.size() * sizeof(float), wierzcholki.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeks.size() * sizeof(unsigned int), indeks.data(), GL_STATIC_DRAW);


	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, wierzcholki.size() * sizeof(int), normalne.data(), GL_STATIC_DRAW);

	programID = loadShaders("vertex_shader.glsl", "fragment_shader2.glsl");


	
	glUseProgram(programID);
	objectColor_id = glGetUniformLocation(programID, "objectColor");
	lightColor_id = glGetUniformLocation(programID, "lightColor");
	lightPos_id = glGetUniformLocation(programID, "lightPos");

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);



	glutMainLoop();

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, VAO);
	glDeleteBuffers(1, &NBO);
	return 0;
}