//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}

void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList

	CrearCilindro(20, 1.0f);//índice 2 en MeshList
	CrearCono(20, 1.0f);//índice 3 en MeshList

	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Fondo Blanco 
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// -------------------------------------------------------------
		// EJERCICIO: Pyraminx (Rubik de Pirámides) Ensamblaje Matemático Perfecto
		// -------------------------------------------------------------

		auto DrawFace = [&](glm::mat4 faceTransform, glm::vec3 faceColor, bool isBlack) {
			// Coordenadas perfectas Y para un Triángulo Equilátero
			float y1 = -0.43301f;
			float y2 = 0.43301f;
			float y3 = 1.29904f;

			struct P { float x, y, rot; };
			P ps[9] = {
				{-1.0f, y1,   0.0f}, {-0.5f, y1, 180.0f}, { 0.0f, y1,   0.0f},
				{ 0.5f, y1, 180.0f}, { 1.0f, y1,   0.0f}, {-0.5f, y2,   0.0f},
				{ 0.0f, y2, 180.0f}, { 0.5f, y2,   0.0f}, { 0.0f, y3,   0.0f}
			};

			glUniform3fv(uniformColor, 1, glm::value_ptr(isBlack ? glm::vec3(0.0f, 0.0f, 0.0f) : faceColor));

			for (int i = 0; i < 9; i++) {
				glm::mat4 m = faceTransform;
				// Empujamos la capa de color muy ligeramente hacia el frente para evitar parpadeos
				m = glm::translate(m, glm::vec3(ps[i].x, ps[i].y, isBlack ? 0.0f : 0.01f));

				if (ps[i].rot > 0.0f) m = glm::rotate(m, glm::radians(ps[i].rot), glm::vec3(0.0f, 0.0f, 1.0f));

				// APLASTAMOS LA PIRÁMIDE EN Z (0.02f)
				// Esto la convierte en una lámina delgada, evitando que atraviese la pirámide por dentro
				if (!isBlack) {
					m = glm::scale(m, glm::vec3(0.92f, 0.92f * 0.86602f, 0.02f)); // Capa color (Más chica para dejar margen)
				}
				else {
					m = glm::scale(m, glm::vec3(1.02f, 1.02f * 0.86602f, 0.02f)); // Capa negra (Fondo)
				}

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
				meshList[1]->RenderMesh();
			}
			};

		// Rotaciones Base controladas por teclado (E, R, T)
		glm::mat4 baseModel(1.0f);
		baseModel = glm::translate(baseModel, glm::vec3(0.0f, -0.3f, -5.5f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Constantes matemáticas de un Tetraedro Perfecto
		float rin = 0.61237f;     // Distancia del centro a cada cara
		float tilt = 19.4712f;    // Inclinación de las caras laterales

		// CARA FRENTE (Roja)
		glm::mat4 f1Base = glm::rotate(baseModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		f1Base = glm::rotate(f1Base, glm::radians(-tilt), glm::vec3(1.0f, 0.0f, 0.0f));
		f1Base = glm::translate(f1Base, glm::vec3(0.0f, 0.0f, rin));
		DrawFace(f1Base, glm::vec3(0), true);
		DrawFace(f1Base, glm::vec3(1.0f, 0.0f, 0.0f), false);

		// CARA DERECHA (Verde)
		glm::mat4 f2Base = glm::rotate(baseModel, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		f2Base = glm::rotate(f2Base, glm::radians(-tilt), glm::vec3(1.0f, 0.0f, 0.0f));
		f2Base = glm::translate(f2Base, glm::vec3(0.0f, 0.0f, rin));
		DrawFace(f2Base, glm::vec3(0), true);
		DrawFace(f2Base, glm::vec3(0.0f, 1.0f, 0.0f), false);

		// CARA IZQUIERDA (Azul)
		glm::mat4 f3Base = glm::rotate(baseModel, glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		f3Base = glm::rotate(f3Base, glm::radians(-tilt), glm::vec3(1.0f, 0.0f, 0.0f));
		f3Base = glm::translate(f3Base, glm::vec3(0.0f, 0.0f, rin));
		DrawFace(f3Base, glm::vec3(0), true);
		DrawFace(f3Base, glm::vec3(0.0f, 0.0f, 1.0f), false);

		// CARA ABAJO (Amarilla)
		glm::mat4 f4Base = glm::rotate(baseModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		f4Base = glm::rotate(f4Base, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Gira la base para cerrar el tetraedro
		f4Base = glm::translate(f4Base, glm::vec3(0.0f, 0.0f, rin));
		DrawFace(f4Base, glm::vec3(0), true);
		DrawFace(f4Base, glm::vec3(1.0f, 1.0f, 0.0f), false);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}