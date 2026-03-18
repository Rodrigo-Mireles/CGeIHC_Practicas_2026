/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
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

}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(30, 1.0f);//índice 2 en MeshList (Resolución a 30 para el cuerpo del perrito)
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía

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

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// Declaraciones de matrices pivote para la jerarquía del perrito
		glm::mat4 modelCuerpo;
		glm::mat4 modelCabeza;
		glm::mat4 modelPierna;
		glm::mat4 modelRodillaPivot;
		glm::mat4 modelRodilla;
		glm::mat4 modelPiePivot;
		glm::mat4 modelBaseCola;

		// ==========================================
		// PISO (Plano base)
		// ==========================================
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, -4.0f));
		model = glm::scale(model, glm::vec3(40.0f, 1.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.4f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		// ==========================================
		// PIVOTE CENTRAL: CUERPO (Prisma Rectangular Sólido)
		// ==========================================
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -10.0f)); // Z = -10 para enfocar con tu cámara
		modelCuerpo = model;

		// Usamos meshList[0] (Cubo/Prisma) para garantizar formas sólidas sin errores
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 4.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); // Blanco/Gris claro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		// ==========================================
		// CABEZA Y HOCICO
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.0f, 1.8f, -2.8f)); // Cuello hacia el frente
		modelCabeza = model;

		// Cabeza (Esfera)
		model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Hocico (Esfera)
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, -1.3f));
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Nariz (Esfera negra)
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.9f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();


		// ==========================================
		// OREJAS (Prismas Sólidos con Movimiento)
		// ==========================================
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Oreja Izquierda
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(1.2f, 0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Oreja Derecha
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(-1.2f, 0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// ==========================================
		// COLA (Prisma Sólido rotando desde base esférica)
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 2.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelBaseCola = model;

		// Base Esfera
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Prisma Cola
		model = modelBaseCola;
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 2.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		// ==========================================
		// PATA DELANTERA IZQUIERDA (FL)
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(1.4f, -1.0f, -1.6f)); // X, Y, Z relativos al cuerpo
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render(); // Hombro

		model = modelPierna;
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Inclinada al frente
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 2.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); // Muslo (Prisma)

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render(); // Rodilla

		model = modelPierna;
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); // Canilla (Prisma)

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -0.2f)); // Pie hacia el frente
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // Pie Sólido


		// ==========================================
		// PATA DELANTERA DERECHA (FR)
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-1.4f, -1.0f, -1.6f));
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 2.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -0.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// ==========================================
		// PATA TRASERA IZQUIERDA (BL)
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(1.4f, -1.0f, 1.6f));
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 2.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getLlantaDI()), glm::vec3(1.0f, 0.0f, 0.0f)); // Tecla K
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -0.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// ==========================================
		// PATA TRASERA DERECHA (BR)
		// ==========================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-1.4f, -1.0f, 1.6f));
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 2.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getLlantaDD()), glm::vec3(1.0f, 0.0f, 0.0f)); // Tecla L
		modelPierna = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelPierna;
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -0.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}