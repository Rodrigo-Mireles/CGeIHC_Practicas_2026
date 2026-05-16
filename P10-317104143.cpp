/*
PRACTICA Nº 10: Animación por Keyframes (Recorrido Senoidal y Aleteo)
NOMBRE: Mireles Peña Rodrigo Javier
CUENTA: 317104143
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <fstream> // Librería para exportar el archivo .txt

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

// Iluminación y Materiales
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Model Nave_M;
Model Ala_M;
Skybox skybox;
Material Material_opaco;

// =========================================================
// VARIABLES GLOBALES PARA ANIMACIÓN
// =========================================================
float movAvion = 0.0f;
float movAvion_y = 0.0f;
float rotAvion = 0.0f;
float tiempoAleteo = 0.0f; // Controla la velocidad de la oscilación del ala

// =========================================================
// ESTRUCTURA Y VARIABLES DE KEYFRAMES
// =========================================================
#define MAX_FRAMES 20 
struct FRAME {
	float movAvion;
	float movAvion_y;
	float rotAvion;
};

FRAME KeyFrame[MAX_FRAMES];
int FrameCount = 0;
bool play = false;
int numFrame = 0;
int i_steps = 0;
int i_max_steps = 90;

float movAvion_inc = 0;
float movAvion_y_inc = 0;
float rotAvion_inc = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

void CreateObjects() {
	unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
	GLfloat floorVertices[] = {
		-20.0f, 0.0f, -20.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 20.0f, 0.0f, -20.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-20.0f, 0.0f,  20.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		 20.0f, 0.0f,  20.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	Mesh* objPiso = new Mesh();
	objPiso->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(objPiso);
}

void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {
	mainWindow = Window(1200, 800);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 0.3f, 0.3f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();

	Nave_M.LoadModel("Models/nave.obj");
	Ala_M.LoadModel("Models/ala.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_opaco = Material(0.3f, 4);
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.0f, -1.0f, -1.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0, uniformColor = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// =========================================================
	// PRECARGA DE LA TRAYECTORIA SENOIDAL (Los 12 Keyframes)
	// =========================================================
	/*KeyFrame[0].movAvion = -6.0f;  KeyFrame[0].movAvion_y = 2.0f; KeyFrame[0].rotAvion = 180.0f;
	KeyFrame[1].movAvion = -2.0f;  KeyFrame[1].movAvion_y = 4.0f; KeyFrame[1].rotAvion = 180.0f;
	KeyFrame[2].movAvion = 2.0f;   KeyFrame[2].movAvion_y = 0.0f; KeyFrame[2].rotAvion = 180.0f;
	KeyFrame[3].movAvion = 6.0f;   KeyFrame[3].movAvion_y = 4.0f; KeyFrame[3].rotAvion = 180.0f;
	KeyFrame[4].movAvion = 10.0f;  KeyFrame[4].movAvion_y = 0.0f; KeyFrame[4].rotAvion = 180.0f;
	KeyFrame[5].movAvion = 14.0f;  KeyFrame[5].movAvion_y = 2.0f; KeyFrame[5].rotAvion = 180.0f;
	KeyFrame[6].movAvion = 14.0f;  KeyFrame[6].movAvion_y = 2.0f; KeyFrame[6].rotAvion = 0.0f;
	KeyFrame[7].movAvion = 10.0f;  KeyFrame[7].movAvion_y = 0.0f; KeyFrame[7].rotAvion = 0.0f;
	KeyFrame[8].movAvion = 6.0f;   KeyFrame[8].movAvion_y = 4.0f; KeyFrame[8].rotAvion = 0.0f;
	KeyFrame[9].movAvion = 2.0f;   KeyFrame[9].movAvion_y = 0.0f; KeyFrame[9].rotAvion = 0.0f;
	KeyFrame[10].movAvion = -2.0f; KeyFrame[10].movAvion_y = 4.0f; KeyFrame[10].rotAvion = 0.0f;
	KeyFrame[11].movAvion = -6.0f; KeyFrame[11].movAvion_y = 2.0f; KeyFrame[11].rotAvion = 0.0f;*/

	FrameCount = 0; // Ya tenemos 12 cuadros en memoria

	// Colocamos la nave en la posición inicial (Frame 0)
	movAvion = KeyFrame[0].movAvion;
	movAvion_y = KeyFrame[0].movAvion_y;
	rotAvion = KeyFrame[0].rotAvion;

	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		float manualSpeed = 0.25f * deltaTime;
		float rotSpeed = 2.0f * deltaTime;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// =========================================================
		// MODO MANUAL (Si quieres agregar más keyframes con 'L')
		// =========================================================
		if (!play) {
			// Eje X
			if (mainWindow.getsKeys()[GLFW_KEY_4] || mainWindow.getsKeys()[GLFW_KEY_KP_4]) movAvion -= manualSpeed;
			if (mainWindow.getsKeys()[GLFW_KEY_6] || mainWindow.getsKeys()[GLFW_KEY_KP_6]) movAvion += manualSpeed;

			// Eje Y
			if (mainWindow.getsKeys()[GLFW_KEY_2] || mainWindow.getsKeys()[GLFW_KEY_KP_2]) movAvion_y -= manualSpeed;
			if (mainWindow.getsKeys()[GLFW_KEY_8] || mainWindow.getsKeys()[GLFW_KEY_KP_8]) movAvion_y += manualSpeed;

			// Rotación Z
			if (mainWindow.getsKeys()[GLFW_KEY_9] || mainWindow.getsKeys()[GLFW_KEY_KP_9]) rotAvion -= rotSpeed;
			if (mainWindow.getsKeys()[GLFW_KEY_7] || mainWindow.getsKeys()[GLFW_KEY_KP_7]) rotAvion += rotSpeed;

			// Guardar Frame
			static bool pressedL = false;
			if (mainWindow.getsKeys()[GLFW_KEY_L]) {
				if (!pressedL && FrameCount < MAX_FRAMES) {
					printf("Frame %d guardado: X=%f, Y=%f, R=%f\n", FrameCount, movAvion, movAvion_y, rotAvion);
					KeyFrame[FrameCount].movAvion = movAvion;
					KeyFrame[FrameCount].movAvion_y = movAvion_y;
					KeyFrame[FrameCount].rotAvion = rotAvion;
					FrameCount++;
				}
				pressedL = true;
			}
			else pressedL = false;

			// Exportar a TXT
			static bool pressedG = false;
			if (mainWindow.getsKeys()[GLFW_KEY_G]) {
				if (!pressedG && FrameCount > 0) {
					std::ofstream archivo("keyframes_guardados.txt");
					if (archivo.is_open()) {
						archivo << "// --- KEYFRAMES GENERADOS PARA REPRODUCCIÓN ---\n";
						for (int i = 0; i < FrameCount; i++) {
							archivo << "KeyFrame[" << i << "].movAvion = " << KeyFrame[i].movAvion << "f;\n";
							archivo << "KeyFrame[" << i << "].movAvion_y = " << KeyFrame[i].movAvion_y << "f;\n";
							archivo << "KeyFrame[" << i << "].rotAvion = " << KeyFrame[i].rotAvion << "f;\n\n";
						}
						archivo.close();
						printf("\nEXITO: Keyframes exportados a 'keyframes_guardados.txt'\n");
					}
				}
				pressedG = true;
			}
			else pressedG = false;
		}

		// =========================================================
		// INICIAR REPRODUCCIÓN (Barra Espaciadora)
		// =========================================================
		if (mainWindow.getsKeys()[GLFW_KEY_SPACE] && !play && FrameCount > 1) {
			play = true;
			numFrame = 0;
			i_steps = 0;
			tiempoAleteo = 0.0f; // Reiniciamos el aleteo

			movAvion = KeyFrame[0].movAvion;
			movAvion_y = KeyFrame[0].movAvion_y;
			rotAvion = KeyFrame[0].rotAvion;

			movAvion_inc = (KeyFrame[1].movAvion - KeyFrame[0].movAvion) / i_max_steps;
			movAvion_y_inc = (KeyFrame[1].movAvion_y - KeyFrame[0].movAvion_y) / i_max_steps;
			rotAvion_inc = (KeyFrame[1].rotAvion - KeyFrame[0].rotAvion) / i_max_steps;
		}

		// =========================================================
		// LÓGICA DE INTERPOLACIÓN (Ejecución de la animación)
		// =========================================================
		if (play) {
			movAvion += movAvion_inc;
			movAvion_y += movAvion_y_inc;
			rotAvion += rotAvion_inc;

			if (i_steps >= i_max_steps) {
				numFrame++;
				if (numFrame >= FrameCount - 1) play = false; // Se detiene al terminar el arreglo
				else {
					movAvion_inc = (KeyFrame[numFrame + 1].movAvion - KeyFrame[numFrame].movAvion) / i_max_steps;
					movAvion_y_inc = (KeyFrame[numFrame + 1].movAvion_y - KeyFrame[numFrame].movAvion_y) / i_max_steps;
					rotAvion_inc = (KeyFrame[numFrame + 1].rotAvion - KeyFrame[numFrame].rotAvion) / i_max_steps;
					i_steps = 0;
				}
			}
			i_steps++;
		}

		// Limpieza de buffers y setup
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		shaderList[0].SetDirectionalLight(&mainLight);

		// Renderizado del Piso
		glm::mat4 model(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);
		brickTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// =========================================================
		// RENDERIZADO JERÁRQUICO (Padre: Nave -> Hijos: 2 Alas)
		// =========================================================

		// 1. Matriz de la NAVE PRINCIPAL (Padre)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movAvion, movAvion_y+1, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

		// Rotación de la animación (Inclinación del recorrido senoidal)
		model = glm::rotate(model, rotAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		// GIRAMOS LA NAVE 180° EN Y PARA QUE MIRE AL FRENTE 
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 orientacionNave = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		// --- LÓGICA DE ALETEO PROCEDURAL ---
		float anguloAleteo = 0.0f;
		if (play) {
			tiempoAleteo += 0.5f * deltaTime;
			anguloAleteo = sin(tiempoAleteo) * 45.0f;
		}

		// 2. Matriz del ALA 1 (Costado Frontal)
		model = orientacionNave;
		model = glm::translate(model, glm::vec3(-0.2f, -0.1f, 0.25f));

		// CORRECCIÓN: Damos la vuelta al ala 180° sobre su propio eje para que apunte bien
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::rotate(model, anguloAleteo * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// 3. Matriz del ALA 2 (Costado Trasero / Opuesto)
		model = orientacionNave;

		// Espejo en el eje Z (Izquierda/Derecha)
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, -1.0f));

		// Misma posición que el Ala 1
		model = glm::translate(model, glm::vec3(-0.2f, -0.1f, 0.25f));

		// CORRECCIÓN: También le damos la vuelta a esta ala 180°
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		// Invertimos el ángulo para que suba y baje al mismo tiempo
		model = glm::rotate(model, -anguloAleteo * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		mainWindow.swapBuffers();
	}
	return 0;
}