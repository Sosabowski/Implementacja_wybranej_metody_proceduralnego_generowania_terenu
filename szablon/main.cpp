#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"

#include "shaderLoader.h" //narzŕdzie do │adowania i kompilowania shaderˇw z pliku
#include "tekstura.h"

//funkcje algebry liniowej
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <vector>


//Wymiary okna
int screen_width = 640;
int screen_height = 480;

GLint uniformTex0 , uniformTex1;
int pozycjaMyszyX; // na ekranie
int pozycjaMyszyY;
int mbutton; // wcisiety klawisz myszy

double kameraX = 10.0;
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
//macierze
glm::mat4 MV; //modelview - macierz modelu i świata
glm::mat4 P;  //projection - macierz projekcji, czyli naszej perspektywy


float vertices [ ] = {//floor
	-5,1,4,
	5,1,4,
	5,1,-8,
	-5,1,-8,

};
float vertices2 [ ] = { //green
	-1, 1, -2,
	1, 1, -2,
	1, 3, -2,
	-1, 3, -2,
};

float vertices3 [ ] = { //blue
	-3, 3, -4,
3, 3, -4,
3, 1, -4,
-3, 1, -4,

};


GLuint elements [ ] = {
	0,1,2,3

};
GLuint elements2 [ ] = {
	3,2,1,0
};
float textureCoords [ ] = {
	0,0,
	5,0,
	5,5,
	0,5, };

float textureCoords2 [ ] = {
	0,0,
	0,2,
	2,2,
	2,0 };


//shaders
GLuint programID = 0, programID2=0;

unsigned int VBO , VBO2 , VBO3 , ebo , ebo2 , ebo3 , vtex , vtex2;
GLuint  tex_id0 , tex_id1;
unsigned int VAO [ 3 ], terrainVAO;

double dx = -5;
double alpha = 1.0;
/*###############################################################*/
void mysz ( int button , int state , int x , int y )
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		pozycjaMyszyX = x;
		pozycjaMyszyY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		break;

	}
}
/*******************************************/
void mysz_ruch ( int x , int y )
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - ( pozycjaMyszyX - x ) * 0.1;
		kameraZ = poprzednie_kameraZ - ( pozycjaMyszyY - y ) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + ( pozycjaMyszyY - y ) * 0.1;
	}

}
/******************************************/



void klawisz ( GLubyte key , int x , int y )
{
	switch (key) {

	case 27:    /* Esc - koniec */
		exit ( 1 );
		break;

	case 'x':
	
		break;
	case 'c':
	
		break;


	case '1':

		break;
	case '2':

		break;
	}

	
}
/*###############################################################*/
void rysuj ( void )
{


	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Kasowanie ekranu
	glUseProgram ( programID );
	GLuint MVP_id = glGetUniformLocation ( programID , "MVP" );
	MV = glm::mat4 ( 1.0f );  //macierz jednostkowa
	MV = glm::translate ( MV , glm::vec3 ( 1 , -1 , kameraD + dx ) );
	MV = glm::rotate ( MV , ( float ) glm::radians ( kameraZ ) , glm::vec3 ( 1 , 0 , 0 ) );
	MV = glm::rotate ( MV , ( float ) glm::radians ( kameraX ) , glm::vec3 ( 0 , 1 , 0 ) );
	glm::mat4 MVP = P * MV;

	glUniformMatrix4fv ( MVP_id , 1 , GL_FALSE , &( MVP [ 0 ][ 0 ] ) );

	glBindVertexArray ( VAO [ 0 ] );
	glPolygonMode ( GL_FRONT_AND_BACK , GL_FILL );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D , tex_id0 );
	uniformTex1 = glGetUniformLocation(programID, "tex0");
	glUniform1i ( uniformTex1 , 0 );
	glDrawElements ( GL_QUADS , 4 , GL_UNSIGNED_INT , 0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	//zielony kwadrat
	glUseProgram(programID2);
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));
	glBindVertexArray(VAO[1]);
	glVertexAttrib4f(2, 0.0, 1.0, 0.0, 1.0);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);

	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainElements.size(), GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);


	//blue

	glBindVertexArray ( VAO [ 2 ] );
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_id1);
	glUniform1i(uniformTex1, 0);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	


	glFlush ( );
	glutSwapBuffers ( );

}
/*###############################################################*/
void rozmiar ( int width , int height )
{
	screen_width = width;
	screen_height = height;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport ( 0 , 0 , screen_width , screen_height );

	P = glm::perspective ( glm::radians ( 60.0f ) , ( GLfloat ) screen_width / ( GLfloat ) screen_height , 1.0f , 1000.0f );

	glutPostRedisplay ( ); // Przerysowanie sceny
}

/*###############################################################*/
void idle ( )
{

	glutPostRedisplay ( );
}

/*###############################################################*/

void timer ( int value ) {

	/////////////////////////
	glutTimerFunc ( 20 , timer , 0 );
}

const int size = 129; // Siatka 129x129, czyli rozdzielczość
float heights[size][size]; // Tablica wysokości dla algorytmu Diament-Kwadrat

void diamondSquare(float roughness) {
	int step = size - 1;
	heights[0][0] = heights[0][step] = heights[step][0] = heights[step][step] = (float)(rand() % 100) / 50.0f - 1.0f; // losowe narożniki

	while (step > 1) {
		int halfStep = step / 2;

		// Krok diamentowy
		for (int x = 0; x < size - 1; x += step) {
			for (int y = 0; y < size - 1; y += step) {
				float avg = (heights[x][y] + heights[x + step][y] + heights[x][y + step] + heights[x + step][y + step]) / 4.0f;
				heights[x + halfStep][y + halfStep] = avg + (float)(rand() % 100 - 50) / 50.0f * roughness;
			}
		}

		// Krok kwadratowy
		for (int x = 0; x < size - 1; x += halfStep) {
			for (int y = (x + halfStep) % step; y < size - 1; y += step) {
				float avg = (heights[(x - halfStep + size) % size][y] +
					heights[(x + halfStep) % size][y] +
					heights[x][(y + halfStep) % size] +
					heights[x][(y - halfStep + size) % size]) / 4.0f;
				heights[x][y] = avg + (float)(rand() % 100 - 50) / 50.0f * roughness;
			}
		}
		roughness *= 0.5f; // zmniejszamy szum dla kolejnych iteracji
		step /= 2;
	}
}


// Funkcja do generowania siatki wierzchołków na podstawie wysokości
void generateTerrainMesh() {
	std::vector<float> terrainVertices;
	std::vector<GLuint> terrainElements;

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			terrainVertices.push_back((float)i - size / 2.0f);  // X
			terrainVertices.push_back(heights[i][j]);           // Y (wysokość)
			terrainVertices.push_back((float)j - size / 2.0f);  // Z
		}
	}

	for (int i = 0; i < size - 1; ++i) {
		for (int j = 0; j < size - 1; ++j) {
			int topLeft = i * size + j;
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + size;
			int bottomRight = bottomLeft + 1;

			// Dodajemy dwa trójkąty na kwadrat siatki
			terrainElements.push_back(topLeft);
			terrainElements.push_back(bottomLeft);
			terrainElements.push_back(topRight);
			terrainElements.push_back(topRight);
			terrainElements.push_back(bottomLeft);
			terrainElements.push_back(bottomRight);
		}
	}

	// Ładowanie siatki do bufora OpenGL
	GLuint terrainVBO, terrainEBO, terrainVAO;
	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainElements.size() * sizeof(GLuint), terrainElements.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
}


/*###############################################################*/
int main ( int argc , char** argv )
{
	glutInit ( &argc , argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize ( screen_width , screen_height );
	glutInitWindowPosition ( 0 , 0 );
	glutCreateWindow ( "Szablon" );

	glewInit ( ); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc ( rysuj );			// def. funkcji rysuj¦cej
	glutIdleFunc ( idle );			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutTimerFunc ( 20 , timer , 0 );
	glutReshapeFunc ( rozmiar ); // def. obs-ugi zdarzenia resize (GLUT)

	glutKeyboardFunc ( klawisz );		// def. obsługi klawiatury
	glutMouseFunc ( mysz ); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc ( mysz_ruch ); // def. obsługi zdarzenia ruchu myszy (GLUT)

	//moje zmiany
	glEnable ( GL_DEPTH_TEST );

	//green

	glGenBuffers ( 1 , &VBO2 );
	glBindBuffer ( GL_ARRAY_BUFFER , VBO2 );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( vertices2 ) , vertices2 , GL_STATIC_DRAW );

	glGenBuffers ( 1 , &ebo2 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo2 );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , sizeof ( elements ) , elements , GL_STATIC_DRAW );

	//blue
	glGenBuffers ( 1 , &VBO3 );
	glBindBuffer ( GL_ARRAY_BUFFER , VBO3 );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( vertices3 ) , vertices3 , GL_STATIC_DRAW );

	glGenBuffers ( 1 , &ebo3 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo3 );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , sizeof ( elements2 ) , elements2 , GL_STATIC_DRAW );


	//podloga
	glGenBuffers ( 1 , &VBO );
	glBindBuffer ( GL_ARRAY_BUFFER , VBO );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( vertices ) , vertices , GL_STATIC_DRAW );

	glGenBuffers ( 1 , &ebo );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , sizeof ( elements2 ) , elements2 , GL_STATIC_DRAW );

	glGenBuffers ( 1 , &vtex );
	glBindBuffer ( GL_ARRAY_BUFFER , vtex );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( textureCoords ) , textureCoords , GL_STATIC_DRAW );

	
	glGenBuffers ( 1 , &vtex2 );
	glBindBuffer ( GL_ARRAY_BUFFER , vtex2 );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( textureCoords2 ) , textureCoords2 , GL_STATIC_DRAW );

	diamondSquare(0.7f);    // Generujemy teren
	generateTerrainMesh();   // Przygotowujemy siatkę
	glutMainLoop();



	programID = loadShaders ( "vertex_shader.glsl" , "fragment_shader.glsl" );

	programID2 = loadShaders ( "vertex_shader.glsl" , "fragment_shader2.glsl" );

	tex_id0 = WczytajTeksture ( "stone.bmp" );
	if (tex_id0 == -1)
	{
		MessageBox ( NULL , "Nie znaleziono pliku z teksturą" , "Problem" , MB_OK | MB_ICONERROR );
		exit ( 0 );
	}

	tex_id1 = WczytajTeksture ( "dot.bmp" );
	if (tex_id1 == -1)
	{
		MessageBox ( NULL , "Nie znaleziono pliku z teksturą" , "Problem" , MB_OK | MB_ICONERROR );
		exit ( 0 );
	}


	glGenVertexArrays ( 3 , VAO );

	glBindVertexArray ( VAO [ 0 ] );
	glBindBuffer ( GL_ARRAY_BUFFER , vtex );
	glEnableVertexAttribArray ( 1 );
	glVertexAttribPointer ( 1 , 2 , GL_FLOAT , GL_FALSE , 0 , 0 );

	glBindBuffer ( GL_ARRAY_BUFFER , VBO );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo );
	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 0 , ( void* ) 0 );


	glBindVertexArray ( VAO [ 2 ] );
	glBindBuffer ( GL_ARRAY_BUFFER , VBO3 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo3 );
	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 0 , ( void* ) 0 );


	glBindVertexArray ( VAO [ 1 ] );

	glBindBuffer(GL_ARRAY_BUFFER, vtex2);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer ( GL_ARRAY_BUFFER , VBO2 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ebo2 );
	glEnableVertexAttribArray ( 0 ); 
	glVertexAttribPointer (0 , 3 , GL_FLOAT ,GL_FALSE ,0 ,( void* ) 0 );


	glutMainLoop ( );					// start

	glDeleteBuffers ( 1 , &VBO );
	glDeleteBuffers ( 1 , &ebo );
	glDeleteBuffers ( 1 , &vtex );
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &ebo2);
	glDeleteBuffers(1, &vtex2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &ebo3);
	glDeleteBuffers ( 3 , VAO );
	return( 0 );

}

