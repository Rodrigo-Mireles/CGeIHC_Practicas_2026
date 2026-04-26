/*
Práctica 8: Iluminación Avanzada - Helicóptero con Focos Dinámicos
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

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
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Texture pisoTexture;

// Modelos
Model Blackhawk_M;
Model Luz_M;
Model Pecera_M;
Model Cuerpo_M;
Model Antena_M;
Model Foco_M;

Skybox skybox;
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


void CreateObjects()
{
	unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 2.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	Blackhawk_M.LoadModel("Models/nave.obj");
	Luz_M.LoadModel("Models/lampara.obj");
	Pecera_M.LoadModel("Models/pecera.obj");
	Cuerpo_M.LoadModel("Models/cuerpo_pez.obj");
	Antena_M.LoadModel("Models/antena_pez.obj");
	Foco_M.LoadModel("Models/foco_pez.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.0f, -1.0f, -0.5f);

	unsigned int pointLightCount = 0;
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	// Luz del Helicóptero (Amarilla)
	spotLights[0] = SpotLight(1.0f, 1.0f, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	PointLight pointLights_toShader[MAX_POINT_LIGHTS];
	unsigned int pointLightCount_toShader = 0;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// =====================================================================
		// ACTUALIZACIÓN DE JERARQUÍAS Y LÓGICA DE LUZ DINÁMICA
		// =====================================================================

		// 1. Jerarquía Lámpara (PointLight)
		glm::mat4 modelLampara = glm::mat4(1.0);
		modelLampara = glm::translate(modelLampara, glm::vec3(-5.0f, -1.0f, -5.0f));
		modelLampara = glm::rotate(modelLampara, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 posLuzLampara = glm::vec3(modelLampara * glm::vec4(-6.0f, 1.0f, 2.0f, 1.0f));
		pointLights[0].SetPosition(posLuzLampara);

		// 2. Jerarquía Helicóptero (SpotLight Dinámico)
		glm::mat4 modelHeli = glm::mat4(1.0);

		// --- AJUSTE DE ALTURA ---
		modelHeli = glm::translate(modelHeli, glm::vec3(0.0f + mainWindow.getmueveHeli(), 15.0f, 6.0));
		modelHeli = glm::rotate(modelHeli, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		glm::vec3 posLuzHeli = glm::vec3(modelHeli * glm::vec4(0.0f, 0.0f, -0.5f, 1.0f));

		// LÓGICA DE DIRECCIÓN Y ENFOQUE DE LUZ
		glm::vec3 dirLuzHeli = glm::vec3(0.0f, -1.0f, 0.0f); // Abajo por defecto

		// --- AJUSTE DE LENTE ---
		// Apertura más cerrada (8 grados) para compensar la nueva altura
		GLfloat aperturaFoco = 8.0f;

		// Verificamos teclas de movimiento
		if (mainWindow.getsKeys()[GLFW_KEY_I]) {
			// Inclinación sutil (-0.5f en X) para evitar deformación elíptica masiva
			dirLuzHeli = glm::vec3(-0.5f, -1.0f, 0.0f);
			// Cerramos aún más el lente al moverse
			aperturaFoco = 5.0f;
		}
		else if (mainWindow.getsKeys()[GLFW_KEY_O]) {
			// Inclinación sutil hacia el otro lado (0.3f en X)
			dirLuzHeli = glm::vec3(0.5f, -1.0f, 0.0f);
			aperturaFoco = 5.0f;
		}

		// Re-instanciamos la luz inyectando la nueva aperturaFoco
		spotLights[0] = SpotLight(1.0f, 1.0f, 0.0f, 0.0f, 5.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, aperturaFoco);

		// Aplicamos posición y dirección
		spotLights[0].SetFlash(posLuzHeli, dirLuzHeli);


		if (mainWindow.getLuzLamparaOn()) {
			pointLights_toShader[0] = pointLights[0];
			pointLightCount_toShader = pointLightCount;
		}
		else {
			pointLightCount_toShader = 0;
		}

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights_toShader, pointLightCount_toShader);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		// =====================================================================
		// RENDER DE OBJETOS
		// =====================================================================
		glm::mat4 model(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		// Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// Lampara
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLampara));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Luz_M.RenderModel();

		// Helicóptero
		modelHeli = glm::scale(modelHeli, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHeli));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();

		// =====================================================================
		// PECERA (Con canal Alpha y Traslucidez)
		// =====================================================================

		// 1. Creamos una matriz limpia y exclusiva para la pecera
		glm::mat4 modelPecera = glm::mat4(1.0);
		// Posiciónala donde quieras en tu escenario (ej. al centro, sobre el piso)
		modelPecera = glm::translate(modelPecera, glm::vec3(0.0f, 3.5f, 0.0f));
		// Ajusta el tamaño si salió muy grande o muy chica de Blender
		modelPecera = glm::scale(modelPecera, glm::vec3(1.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPecera));

		// Utilizamos un material para que reaccione a la luz de tu helicóptero
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// 2. ACTIVAR EL CANAL ALPHA (Transparencia / Blending)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Opcional: Desactivar la escritura en el búfer de profundidad 
		// (Ayuda a que el cristal no "corte" o tape de forma fea al pez que pondremos adentro)
		glDepthMask(GL_FALSE);

		// 3. Renderizar el modelo (ya con transparencia)
		Pecera_M.RenderModel();

		// 4. DESACTIVAR LA TRANSPARENCIA Y RESTAURAR PROFUNDIDAD
		// Es vital apagar esto para que en el siguiente cuadro de animación
		// el piso y el helicóptero no se vuelvan transparentes por accidente.
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		
		// =====================================================================
		// RENDERIZADO DEL PEZ ABISAL (Jerarquía con Rotación)
		// =====================================================================

		// 1. MATRIZ DEL CUERPO (Nodo Padre)
		glm::mat4 modelCuerpo = glm::mat4(1.0f);

		// Posicionamiento base y movimiento diagonal
		modelCuerpo = glm::translate(modelCuerpo, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCuerpo = glm::translate(modelCuerpo, glm::vec3(mainWindow.getMuevePez(), mainWindow.getMuevePez(), 0.0f));
		modelCuerpo = glm::rotate(modelCuerpo, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cuerpo_M.RenderModel();

		// 2. MATRIZ DE LA ANTENA
		// Hereda la posición, el movimiento Y la nueva rotación del cuerpo
		glm::mat4 modelAntena = modelCuerpo;
		modelAntena = glm::translate(modelAntena, glm::vec3(1.0f, 5.0f, -2.0f));
		//modelAntena = glm::scale(modelAntena, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAntena));
		Antena_M.RenderModel();

		// 3. MATRIZ DEL FOCO
		// Hereda todo lo anterior de forma automática
		glm::mat4 modelFoco = modelAntena;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelFoco));
		Foco_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}