/*
Práctica: 9
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
#include "Model.h"
#include "Skybox.h"

// Iluminación
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

// Texturas en uso
Texture pisoTexture;
Texture fuego;
Texture humo;

// Modelos en uso
Model Aeolipile_base_M;
Model Aeolipile_M;
Model Base_M;
Model Brazo_M;
Model CamaraH_M;
Model Pelota_M;
Model Hoyo_M;

Skybox skybox;

// Materiales
Material Material_brillante;
Material Material_opaco;

// Control de tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Luz principal
DirectionalLight mainLight;

// Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	// =========================================================
	// 1. PLANO DEL PISO (meshList[0])
	// =========================================================
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh* objPiso = new Mesh();
	objPiso->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(objPiso);

	// =========================================================
	// 2. PLANO PARA EL FUEGO (meshList[1])
	// =========================================================
	GLfloat verticesFuego[] = {
		-0.5f,  0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-0.5f,  1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f
	};
	unsigned int indicesFuego[] = { 0, 1, 2, 0, 2, 3 };

	Mesh* objFuego = new Mesh();
	objFuego->CreateMesh(verticesFuego, indicesFuego, 32, 6);
	meshList.push_back(objFuego);

	// =========================================================
	// 3. PLANO PARA EL HUMO DE LA EOLÍPILA (meshList[2])
	// =========================================================
	GLfloat verticesHumo[] = {
		-0.5f,  0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-0.5f,  1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f
	};
	unsigned int indicesHumo[] = { 0, 1, 2, 0, 2, 3 };

	Mesh* objHumo = new Mesh();
	objHumo->CreateMesh(verticesHumo, indicesHumo, 32, 6);
	meshList.push_back(objHumo);
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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	// Carga de texturas
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	fuego = Texture("Textures/fuego.tga");
	fuego.LoadTextureA();
	humo = Texture("Textures/humo.tga");
	humo.LoadTextureA();

	// Carga de modelos
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");

	Base_M = Model();
	Base_M.LoadModel("Models/base.obj");
	Brazo_M = Model();
	Brazo_M.LoadModel("Models/brazo.obj");
	CamaraH_M = Model();
	CamaraH_M.LoadModel("Models/camarahumo.obj");
	Pelota_M = Model();
	Pelota_M.LoadModel("Models/pelota.obj");

	Hoyo_M = Model();
	Hoyo_M.LoadModel("Models/hoyo.obj");

	// Skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	// Materiales y Luces
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f, -1.0f, -1.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0, uniformColor = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Variables Físicas de la Eolípila
	float anguloAeolipile = 0.0f;
	float tiempoCalentamiento = 0.0f;
	float velocidadGiro = 0.0f;
	float escalaHumoV = 0.0f;

	// Variables Físicas y Animación de la Catapulta
	float anguloBrazo = 0.0f;
	float tiempoHumoCamara = 0.0f;
	bool disparoActivado = false;
	bool pelotaEnVuelo = false;

	// Variables del Delay
	bool esperandoReaparicion = false;
	float tiempoEsperaReaparicion = 0.0f;

	glm::vec3 posPelotaMundo;
	glm::vec3 velPelota;
	
	// GRAVEDAD 
	float gravedad = -1.2f;
	int contadorRebotes = 0;
	
	// CONSERVACIÓN DE ENERGÍA
	float factorRebote = 0.80f;

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
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		shaderList[0].SetDirectionalLight(&mainLight);

		// =========================================================
		// 1. RENDERIZADO DEL PISO PRINCIPAL
		// =========================================================
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// =========================================================
		// 2. RENDERIZADO DEL HOYO
		// =========================================================
		glm::mat4 modelHoyo = glm::mat4(1.0);
		modelHoyo = glm::translate(modelHoyo, glm::vec3(-13.0f, -1.8f, 4.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHoyo));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Hoyo_M.RenderModel();


		// =========================================================
		// 3. AEOLIPILE (Base -> Fuego -> Esfera)
		// =========================================================
		glm::mat4 modelBaseAeolipile = glm::mat4(1.0);
		modelBaseAeolipile = glm::translate(modelBaseAeolipile, glm::vec3(0.0f, -2.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBaseAeolipile));
		Aeolipile_base_M.RenderModel();

		if (mainWindow.getFuegoOn())
		{
			static float offsetFuegoV = 1.0f;
			offsetFuegoV -= 0.05f * deltaTime;
			if (offsetFuegoV < 0.0f) offsetFuegoV += 1.0f;

			glUniform2f(uniformTextureOffset, 0.0f, offsetFuegoV);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);

			glm::mat4 modelFuego = modelBaseAeolipile;
			modelFuego = glm::translate(modelFuego, glm::vec3(0.0f, 0.5f, 0.0f));
			modelFuego = glm::scale(modelFuego, glm::vec3(1.9f, 1.9f, 1.9f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelFuego));
			fuego.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[1]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);

		// Lógica de calentamiento e inercia
		if (mainWindow.getFuegoOn())
		{
			tiempoCalentamiento += deltaTime;
			if (tiempoCalentamiento > 100.0f)
			{
				if (velocidadGiro < 2.0f) velocidadGiro += 0.01f * deltaTime;
				escalaHumoV = 0.15f + (velocidadGiro * 0.3f);
			}
			else {
				escalaHumoV = 0.4f;
			}
		}
		else
		{
			tiempoCalentamiento = 0.0f;
			if (velocidadGiro > 0.0f) velocidadGiro -= 0.005f * deltaTime; else velocidadGiro = 0.0f;
			if (escalaHumoV > 0.0f) escalaHumoV -= 0.02f * deltaTime; else escalaHumoV = 0.0f;
		}

		anguloAeolipile += velocidadGiro * deltaTime;
		if (anguloAeolipile >= 360.0f) anguloAeolipile -= 360.0f;

		glm::mat4 modelEsfera = modelBaseAeolipile;
		modelEsfera = glm::translate(modelEsfera, glm::vec3(0.15f, 5.0f, -0.1f));
		modelEsfera = glm::rotate(modelEsfera, anguloAeolipile * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelEsfera));
		Aeolipile_M.RenderModel();

		// Renderizado Humo Eolípila
		if (escalaHumoV > 0.05f)
		{
			static float offsetHumoV = 0.0f;
			float velocidadScroll = 0.005f + (velocidadGiro * 0.01f);
			offsetHumoV -= velocidadScroll * deltaTime;
			if (offsetHumoV < 0.0f) offsetHumoV += 1.0f;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);

			glUniform2f(uniformTextureOffset, 0.0f, offsetHumoV);
			humo.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			glm::mat4 modelHumo1 = modelEsfera;
			modelHumo1 = glm::translate(modelHumo1, glm::vec3(0.14f, 0.63f, 0.0f));
			modelHumo1 = glm::rotate(modelHumo1, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			modelHumo1 = glm::scale(modelHumo1, glm::vec3(0.1f, escalaHumoV, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo1));
			meshList[2]->RenderMesh();

			glm::mat4 modelHumo2 = modelEsfera;
			modelHumo2 = glm::translate(modelHumo2, glm::vec3(-0.14f, -0.57f, 0.0f));
			modelHumo2 = glm::rotate(modelHumo2, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			modelHumo2 = glm::scale(modelHumo2, glm::vec3(0.1f, escalaHumoV, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo2));
			meshList[2]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glUniform2f(uniformTextureOffset, 0.0f, 0.0f);
		}

		// =========================================================
		// 4. LÓGICA DE FÍSICA Y DELAY (CATAPULTA)
		// =========================================================

		// Manejo del estado de ESPERA (Delay después de caer al hoyo)
		if (esperandoReaparicion) {
			tiempoEsperaReaparicion += deltaTime * 10.0f;
			if (tiempoEsperaReaparicion > 250.0f) {
				esperandoReaparicion = false;
				tiempoHumoCamara = 0.0f; // Reseteamos la acumulación de humo
			}
		}
		// Acumulación de humo SOLO si la pelota está físicamente lista para dispararse
		else if (!pelotaEnVuelo && !disparoActivado) {
			if (velocidadGiro > 1.8f) {
				tiempoHumoCamara += deltaTime * 1.0f;
			}
			else {
				if (tiempoHumoCamara > 0.0f) tiempoHumoCamara -= deltaTime * 20.0f;
			}

			if (tiempoHumoCamara > 200.0f) {
				disparoActivado = true;
			}
		}

		if (disparoActivado) {
			anguloBrazo += 10.0f * deltaTime;

			if (anguloBrazo >= 45.0f && !pelotaEnVuelo) {
				pelotaEnVuelo = true;

				glm::mat4 modelTemp = glm::mat4(1.0);
				modelTemp = glm::translate(modelTemp, glm::vec3(8.0f, -1.9f, 4.6f));
				modelTemp = glm::rotate(modelTemp, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				modelTemp = glm::translate(modelTemp, glm::vec3(-0.5f, 0.5f, 0.0f));
				modelTemp = glm::rotate(modelTemp, -anguloBrazo * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::vec4 posMundoCalc = modelTemp * glm::vec4(-2.1f, 2.2f, 0.0f, 1.0f);
				posPelotaMundo = glm::vec3(posMundoCalc);

				float fuerzaVapor = 1.8f;
				velPelota = glm::vec3(-fuerzaVapor * cos(45.0f * toRadians), (fuerzaVapor * sin(45.0f * toRadians)) * 2.5f, 0.0f);
			}

			if (anguloBrazo >= 90.0f) {
				anguloBrazo = 0.0f;
				disparoActivado = false;
			}
		}

		if (pelotaEnVuelo) {
			velPelota.y += gravedad * deltaTime;
			posPelotaMundo += velPelota * deltaTime;

			// COMPORTAMIENTO NORMAL: Rebota las primeras 3 veces
			if (posPelotaMundo.y <= -2.3f && contadorRebotes < 3) {
				posPelotaMundo.y = -2.3f;
				velPelota.y = -velPelota.y * factorRebote;
				velPelota.x *= 0.95f; // Fricción mínima
				contadorRebotes++;
			}
			// COMPORTAMIENTO ÚLTIMO REBOTE: Simplemente cae al vacío, no invierte su velocidad en Y
			else if (posPelotaMundo.y <= -2.3f && contadorRebotes == 3) {
				// No hacemos nada, la gravedad sigue jalando la pelota hacia el abismo (-Y)
			}

			// RESET: Una vez que ha caído profundamente por el hoyo, iniciamos el delay de reaparición
			if (posPelotaMundo.y < -30.0f) {
				pelotaEnVuelo = false;
				contadorRebotes = 0;
				anguloBrazo = 0.0f;
				disparoActivado = false;

				esperandoReaparicion = true;
				tiempoEsperaReaparicion = 0.0f;
			}
		}

		// =========================================================
		// 5. RENDERIZADO MECANISMO CATAPULTA
		// =========================================================
		glm::mat4 modelBaseCatapulta = glm::mat4(1.0);
		modelBaseCatapulta = glm::translate(modelBaseCatapulta, glm::vec3(8.0f, -1.9f, 4.6f));
		modelBaseCatapulta = glm::rotate(modelBaseCatapulta, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBaseCatapulta));
		Base_M.RenderModel();

		glm::mat4 modelCamara = modelBaseCatapulta;
		modelCamara = glm::translate(modelCamara, glm::vec3(3.0f, 0.5f, 0.0f));
		modelCamara = glm::scale(modelCamara, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCamara));
		CamaraH_M.RenderModel();

		glm::mat4 modelBrazoCatapulta = modelBaseCatapulta;
		modelBrazoCatapulta = glm::translate(modelBrazoCatapulta, glm::vec3(-0.5f, 0.5f, 0.0f));
		modelBrazoCatapulta = glm::rotate(modelBrazoCatapulta, -anguloBrazo * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBrazoCatapulta));
		Brazo_M.RenderModel();

		// RENDERIZADO DE LA PELOTA
		if (pelotaEnVuelo) {
			// En el aire o cayendo por el agujero
			glm::mat4 modelPelotaVuelo = glm::mat4(1.0f);
			modelPelotaVuelo = glm::translate(modelPelotaVuelo, posPelotaMundo);
			modelPelotaVuelo = glm::scale(modelPelotaVuelo, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPelotaVuelo));
			Pelota_M.RenderModel();
		}
		else {
			// SOLO SE RENDERIZA EN EL BRAZO SI NO ESTAMOS EN LA FASE DEL DELAY (ESPERANDO REAPARICIÓN)
			if (!esperandoReaparicion && (!disparoActivado || anguloBrazo < 45.0f)) {
				glm::mat4 modelPelotaCatapulta = modelBrazoCatapulta;
				modelPelotaCatapulta = glm::translate(modelPelotaCatapulta, glm::vec3(-2.1f, 2.2f, 0.0f));
				modelPelotaCatapulta = glm::scale(modelPelotaCatapulta, glm::vec3(0.5f, 0.5f, 0.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPelotaCatapulta));
				Pelota_M.RenderModel();
			}
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}