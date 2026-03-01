//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
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
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

// Modificado: Shaders nuevos de colores para instanciar cubo y pirámide
static const char* vShaderrojo = "shaders/shaderrojo.vert";
static const char* vShaderverde = "shaders/shaderverde.vert";
static const char* vShaderazul = "shaders/shaderazul.vert";
static const char* vShadercafe = "shaders/shadercafe.vert";
static const char* vShaderverdeoscuro = "shaders/shaderverdeoscuro.vert";

float angulo = 0.0f;

//color café en RGB : 0.478, 0.255, 0.067

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3

	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
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

void CrearLetrasyFiguras()
{
	// Modificado: Se insertaron los vértices de las letras R, J, M perfeccionadas (432 datos flotantes)
	GLfloat vertices_letras[] = {
		// --- LETRA R MEJOR DEFINIDA (Amarillo) --- (180 datos)
		// Barra vertical izquierda
		-0.9f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.9f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.8f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.9f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.8f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.8f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,
		// Barra superior
		-0.8f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.8f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.8f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.4f, 0.0f, 1.0f, 1.0f, 0.0f,
		// Barra vertical derecha (cierre del bucle)
		-0.6f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.6f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
		// Barra media
		-0.8f, -0.6f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.8f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.8f, -0.6f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.6f, 0.0f, 1.0f, 1.0f, 0.0f,
		// Pata diagonal mejorada
		-0.7f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.7f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.5f, -0.9f, 0.0f, 1.0f, 1.0f, 0.0f,  -0.6f, -0.7f, 0.0f, 1.0f, 1.0f, 0.0f,

		// --- LETRA J (Cyan) --- (108 datos)
		 0.0f,  0.3f, 0.0f, 0.0f, 1.0f, 1.0f,   0.0f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,
		 0.0f,  0.3f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f,  0.3f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.2f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,  -0.2f, -0.2f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f, -0.2f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.2f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f, -0.2f, 0.0f, 0.0f, 1.0f, 1.0f,   0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.2f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  -0.2f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,  -0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.2f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  -0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f,  -0.1f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		// --- LETRA M CON PICO CENTRAL PERFECTO (Magenta) --- (144 datos)
		// Barra izquierda (gruesa y recta)
		 0.4f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.4f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,   0.5f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.4f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.5f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,   0.5f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,
		 // Barra derecha (gruesa y recta)
		  0.8f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.8f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,   0.9f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,
		  0.8f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.9f,  0.4f, 0.0f, 1.0f, 0.0f, 1.0f,   0.9f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,
		  // Diagonal izquierda (forma un pico afilado en el centro exacto)
		   0.4f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f,
		   0.4f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f,   0.5f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,
		   // Diagonal derecha (forma un pico afilado en el centro exacto)
			0.9f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.9f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.8f,  0.9f, 0.0f, 1.0f, 0.0f, 1.0f,   0.65f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f
	};
	MeshColor* letras = new MeshColor();
	// Modificado: Se cambia de 18 a 432 para que lea todos los vértices de las letras
	letras->CreateMeshColor(vertices_letras, 432);
	meshColorList.push_back(letras);

	GLfloat vertices_triangulorojo[] = {
		//X			Y			Z			R		G		B
		-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,

	};

	MeshColor* triangulorojo = new MeshColor();
	triangulorojo->CreateMeshColor(vertices_triangulorojo, 18);
	meshColorList.push_back(triangulorojo);

	GLfloat vertices_trianguloazul[] = {
		//X			Y			Z			R		G		B
		-1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
		1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
		0.0f,	1.0f,		0.5f,			0.0f,	0.0f,	1.0f,

	};

	MeshColor* trianguloazul = new MeshColor();
	trianguloazul->CreateMeshColor(vertices_trianguloazul, 18);
	meshColorList.push_back(trianguloazul);


	GLfloat vertices_cuadradoverde[] = {
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,

	};

	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde->CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde);

	GLfloat vertices_cuadradorojo[] = {
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		-0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,

	};

	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo->CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo);



}


void CreateShaders()
{

	Shader* shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1); // Indice 0

	Shader* shader2 = new Shader();//shader para usar color como parte del VAO: letras 
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2); // Indice 1

	// Modificado: Agregamos los shaders necesarios para los colores de la casa 3D
	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles(vShaderrojo, fShaderColor);
	shaderList.push_back(*shaderRojo); // Indice 2

	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles(vShaderverde, fShaderColor);
	shaderList.push_back(*shaderVerde); // Indice 3

	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles(vShaderazul, fShaderColor);
	shaderList.push_back(*shaderAzul); // Indice 4

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles(vShadercafe, fShaderColor);
	shaderList.push_back(*shaderCafe); // Indice 5

	Shader* shaderVerdeOscuro = new Shader();
	shaderVerdeOscuro->CreateFromFiles(vShaderverdeoscuro, fShaderColor);
	shaderList.push_back(*shaderVerdeOscuro); // Indice 6
}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearLetrasyFiguras(); //usa MeshColor, índices en MeshColorList
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	// Modificado: Se cambia a proyección ortogonal para el Ejercicio 2
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana

		// Modificado: Se mantiene el fondo blanco 
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad


		// ---------------------------------------------------------
		// EJERCICIO 1: Dibujar Iniciales R, J, M (COMENTADO)
		// ---------------------------------------------------------
		/*
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[0]->RenderMeshColor();
		*/

		// ---------------------------------------------------------
		// EJERCICIO 2: Casa instanciando Cubos y Pirámides 3D con Shaders
		// meshList[1] = Cubo, meshList[0] = Pirámide
		// ---------------------------------------------------------

		// 1. Cuerpo de la casa (Cubo, Rojo) -> Shader 2
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, -5.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // Se dibuja el Cubo

		// 2. Techo (Pirámide, Azul) -> Shader 4
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.6f, -4.9f));
		model = glm::scale(model, glm::vec3(1.2f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // Se dibuja la Pirámide

		// 3. Ventanas y Puerta (Cubo, Verde) -> Shader 3
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Ventana Izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.25f, -0.1f, -4.8f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Ventana Derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.25f, -0.1f, -4.8f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Puerta
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, -4.8f));
		model = glm::scale(model, glm::vec3(0.25f, 0.3f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// 4. Troncos de árboles (Cubo, Café) -> Shader 5
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Tronco Izquierdo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.75f, -0.8f, -5.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.2f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Tronco Derecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.75f, -0.8f, -5.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.2f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// 5. Hojas de árboles (Pirámide, Verde Oscuro) -> Shader 6
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Hojas Izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.75f, -0.3f, -4.9f));
		model = glm::scale(model, glm::vec3(0.4f, 0.8f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Hojas Derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.75f, -0.3f, -4.9f));
		model = glm::scale(model, glm::vec3(0.4f, 0.8f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/