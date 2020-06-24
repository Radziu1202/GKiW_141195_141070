#define _CRT_SECURE_NO_DEPRECATE
#define WIDTH 1920
#define HEIGHT 1080
#define Z_MOVING true

#include <GL/glew.h>
#include "stale.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "Obiekt.h"
#include "material.h"
#include "ruch.h"
#include <stdio.h>
#include <iostream
#include <stdlib.h>
#include "klawisze.h"

using namespace glm;

//-----------------------------------------deklaracje--------------------------------
float aspect = (float) WIDTH / HEIGHT;
ShaderProgram *shaderProgram;

std::vector<GLuint> vao;
std::vector<GLuint> bufVertices;
std::vector<GLuint> bufTexCoords;
std::vector<GLuint> bufNormals;
std::vector<GLuint> tex;

std::vector< material* > materials;
std::vector< Obiekt* > OBJs;
std::vector< float > NS;			//}material parametry
std::vector< glm::vec3 > KA;		//}
std::vector< glm::vec3 > KS;		//}
std::vector< float > D;				//}




//--------------------obsługa błêdów, klawiatury i okna--------------------------------
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

GLuint readTexture(std::string filename, int objNum) {
	GLuint texx;
	char path[150] = "/home/radziu/Desktop/Grafika/src/piano/";
	const char* cfilename = filename.c_str();
	strcat(path, cfilename);
	glActiveTexture(GL_TEXTURE0);
	//Wczytanie do pamieci komputera
	std::vector<unsigned char> image; //Alokuj wektor do wczytania obrazka
	unsigned width, height; //Zmienne do których wczytamy wymiary obrazka
							//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, path);
	//Import do pamiêci karty graficznej
	glGenTextures(1, &texx); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, texx); //Uaktywnij uchwyt
									   //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return texx;
}

void freeOpenGLProgram() {
	delete shaderProgram; //Usunięcie programu cieniującego
	for (int i = 0; i < bufVertices.size(); i++) {
		//glDeleteVertexArrays(1, &vao[i]); //Usunięcie vao
		//glDeleteBuffers(1, &bufVertices[i]); //Usunięcie VBO z wierzchołkami
		//glDeleteBuffers(1, &bufTexCoords[i]); //Usunięcie VBO z kolorami
		//glDeleteBuffers(1, &bufNormals[i]); //Usunięcie VBO z wektorami normalnymi
		//glDeleteBuffers(1, &bufNS[i]);
		//glDeleteBuffers(1, &bufKA[i]);
		//glDeleteBuffers(1, &bufKS[i]);
		//glDeleteBuffers(1, &bufD[i]);
	}
}

void key_callback(GLFWwindow* window, int key,int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		freeOpenGLProgram();
		glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
		glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
		exit(EXIT_SUCCESS);
	}
	if (key == GLFW_KEY_Z) {
		OBJs[0]->animate = true;
	}
}

void windowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //Obraz ma byæ generowany w oknie o tej rozdzielczoœci
	if (height != 0) {
		aspect = (float)width / (float)height; //Stosunek szerokoœci do wysokości okna
	}
	else {
		aspect = 1;
	}
}


//--------------VBO, VAO i te sprawy----------------------------------------------------
template <class T> GLuint makeBuffer(std::vector<T> data) {
	GLuint handle;
	glGenBuffers(1, &handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierać tablicę danych
	glBindBuffer(GL_ARRAY_BUFFER, handle);  //Uaktywnij wygenerowany uchwyt VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), &data[0], GL_STATIC_DRAW);
	return handle;
}

void assignVBOtoAttribute(ShaderProgram *shaderProgram, const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); //Pobierz numer slotu dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  //Uaktywnij uchwyt VBO
	glEnableVertexAttribArray(location); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location, vertexSize, GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location mają być brane z aktywnego VBO
}

void prepareObiekt(int objNum) {
	std::vector<GLuint> tempBufVerts;
	std::vector<GLuint> tempBufNormies;
	bufVertices.push_back(makeBuffer(OBJs[objNum]->vertices));
	bufTexCoords.push_back(makeBuffer(OBJs[objNum]->uvs));
	bufNormals.push_back(makeBuffer(OBJs[objNum]->normals));

	GLuint single_vao;
	glGenVertexArrays(1, &single_vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
	vao.push_back(single_vao);
	glBindVertexArray(vao[objNum]); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram, "vertex", bufVertices[objNum], 3);
	assignVBOtoAttribute(shaderProgram, "texCoord", bufTexCoords[objNum], 2);
	assignVBOtoAttribute(shaderProgram, "normal", bufNormals[objNum], 3);

	glBindVertexArray(0); //Dezaktywuj VAO
}


//-------------------------rysowanie---------------------------------------------------------
void drawObiekt(Obiekt* object, int objNum, glm::mat4 P, glm::mat4 V, glm::vec3 camera, GLFWwindow* window) {
	glUniform1i(shaderProgram->getUniformLocation("texture"), 0);
	glUniform1f(shaderProgram->getUniformLocation("Ns"), object->mat->Ns);
	glUniform3f(shaderProgram->getUniformLocation("Ka"), object->mat->Ka.x, object->mat->Ka.y, object->mat->Ka.z);
	glUniform3f(shaderProgram->getUniformLocation("Ks"), object->mat->Ks.x, object->mat->Ks.y, object->mat->Ks.z);
	glUniform1f(shaderProgram->getUniformLocation("D"), object->mat->d);
	
	glm::mat4 M = glm::mat4(1.0f);
	if (object->animate) object->move(M);
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(M));
	
	glBindVertexArray(vao[objNum]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->mat->tex);

	glDrawArrays(GL_TRIANGLES, 0, object->vertices.size());
	glBindVertexArray(0);
}

void drawScene(GLFWwindow* window) {
	glm::mat4 P;
	glm::mat4 V;
	glm::vec3 camera;
	camera = computeMatricesFromInputs(window, P, V, aspect, Z_MOVING);   //mover

	shaderProgram->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(V));
	glUniform3f(shaderProgram->getUniformLocation("cam"), camera.x, camera.y, camera.z);
	for (int i = 0; i < OBJs.size(); i++) drawObiekt(OBJs[i], i, P, V, camera, window);
	glfwSwapBuffers(window);
}


//-----------------------init i main------------------------------------------------------
void wczytaj() {
	materials = material::readMTL("/home/radziu/Desktop/Grafika/src/piano3/s29.mtl");
	Obiekt::readOBJfile("/home/radziu/Desktop/Grafika/src/piano3/s29.obj", materials, OBJs);
	for (int i = 0; i < materials.size(); i++) {
		std::cout << materials[i]->textures[0];
		materials[i]->tex = readTexture(materials[i]->textures[0], i);
	}

	for (int i = 0; i < OBJs.size(); i++) prepareObiekt(i);
	determine_indeces(OBJs);
	determine_key_centers(OBJs);
	determine_hammer_rotation_points(OBJs);
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, windowResize);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_BLEND);					//przezroczystość
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthFunc(GL_LESS);

	shaderProgram = new ShaderProgram("/home/radziu/Desktop/Grafika/src/vertexshader.txt", NULL, "/home/radziu/Desktop/Grafika/src/fragmentshader.txt");
	wczytaj();
}

int main(void)
{
	//glfwGetPrimaryMonitor()
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno
	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów
	glfwInit();
	window = glfwCreateWindow(WIDTH, HEIGHT, "Piano", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	glewInit();
	initOpenGLProgram(window); //Operacje inicjujące
	glfwSetTime(0); //Wyzeruj licznik czasu
	

	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		drawScene(window); //Wykonaj procedurę rysującą
		handleKeyInput(window, OBJs);
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeñ jakie zaszły.
	}

	freeOpenGLProgram();
	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
