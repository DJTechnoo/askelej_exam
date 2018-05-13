#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "stb_image.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"


#include "shader_loader_lo.h"

#include "model.h"
#include "plane.h"
#include "const.h"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void characterCallback(GLFWwindow* window, unsigned int keyCode);
void processInput(GLFWwindow *window);

// settings


// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// plane view and position, just like above's camera
glm::vec3 planePos = glm::vec3(1.0f, 3.0f, 2.0f);
glm::vec3 planeFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 planeUp = glm::vec3(0.0f, 1.0f, 0.0f);

// for camera movement. Not in use because we follow the plane instead
bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int season;		// Toggle between 4 seasons

// The plane controller attitude
Plane planeControl(glm::vec3(1.0f, 1.0f, 1.0f));

// misc: cam orbit, free camera roam, and save positions
float orbit = 0.0f;
int toggleView;
int nextSavedPosition;
std::vector<glm::vec3> savedPositions;		// When user presses "T", save position, and "G" go there

int main()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	season = 2;


													
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lovely Exam", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCharCallback(window, characterCallback);

	
	// glew stuff
	glewExperimental = GL_TRUE;
	glewInit();

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// three shaders used: for plane, terrain, and lightsource respectively
	Shader planeShader("../shaders/vtx.vert", "../shaders/fmt.frag");
	Shader modelShader("../shaders/model.vert","../shaders/model.frag");
	Shader lampShader("../shaders/lamp.vert", "../shaders/lamp.frag");

	


	// Load the plane texture since it didn't load into the model somehow.
	unsigned int planeTexture;
	glGenTextures(1, &planeTexture);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("../Assets/plane.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	

	
	// The physical models: Terrain, lightsource, and plane respectively
	Model terrain("../Assets/correct/first.obj");
	Model lightSource("../Assets/test4/omg.obj");
	Model planeModel("../Assets/model/ask21mi.blend");

	// we only need to set the texture once, so it is outside the game-loop
	planeShader.use();
	planeShader.setInt("texture1", 0);
	


	// lightsource properties
	float lampSpeed = 0.001f;
	glm::vec3 lampPos(1.0f, -10.0f, 2.0f);
	

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// deltatime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// key listener
		processInput(window);

		// clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// properties to follow the plane
		float radius = 2.0f;
		float camX = sin(orbit) * radius;
		float camZ = cos(orbit) * radius;
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view;
		switch (toggleView) {
		case 0: view = glm::lookAt(planePos - glm::vec3(camX, 0.0f, camZ), planePos + planeFront, planeUp); // plane follow
			break;
		case 1: view = glm::lookAt(cameraPos, planePos + planeFront, planeUp); // use this to groundmove but lock plane
			break;
		case 2: view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // use this to move freely with WASD
			break;
		}
			
		
		glm::mat4 model;


		// Update and draw the lightSource
		//lampPos.x -= lampSpeed;
		float sunRadius = 100.0f;
		float lampX = sin(currentFrame/10.0f) * sunRadius;
		float lampZ = cos(currentFrame/10.0f) * sunRadius;
		lampPos.y = lampX;
		lampPos.z = lampZ;
		lampShader.use();
		model = glm::mat4();
		model = glm::translate(model, lampPos); // translate it down so it's at the center of the scene
		lampShader.setMat4("model", model);
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lightSource.Draw(lampShader);


		// update and draw the plane
		planeShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		planePos += planeFront * planeControl.airSpeed * deltaTime;
		model = glm::mat4();
		model = glm::translate(model, planePos); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// just a poor way to correct the plane
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(planeControl.pitch), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(planeControl.bank), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
		planeShader.setMat4("projection", projection);		
		planeShader.setMat4("view", view);
		planeShader.setMat4("model", model);
		
		planeModel.Draw(planeShader);



		

		glm::vec2 seasonVec;
		switch (season)
		{
		case 1: seasonVec.x = -0.5f; seasonVec.y = 0.5f; break;
		case 2: seasonVec.x = -0.5f; seasonVec.y = 0.9f; break;
		case 3: seasonVec.x = -0.55f; seasonVec.y = 0.7f; break;
		case 4: seasonVec.x = -0.6f; seasonVec.y = -0.6f; break;
		}

		// update season and draw terrain
		modelShader.use();
		modelShader.setVec2("season", seasonVec);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
		model = glm::scale(model, glm::vec3(2.2f, 2.2f, 2.2f));
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);
		modelShader.setMat4("model", model);
		modelShader.setVec3("lampPos", lampPos);
		terrain.Draw(modelShader);

		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	glfwTerminate();
	
	return 0;
}

/**
* @param	a GLFWindow reference to the window
* @brief	Listen to which keys are pressed, and updates 
*			camera position, glider attitude and camera orbit
*/
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;



	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) 
		planeControl.changeBank(0.1f);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		planeControl.changePitch(-0.1);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		planeControl.changeBank(-0.1);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		planeControl.changePitch(0.1);
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
		planeControl.changeAirSpeed(0.01f);
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
		planeControl.changeAirSpeed(-0.01f);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		season = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		season = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		season = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		season = 4;


	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		orbit += 0.001f;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		orbit -= 0.001f;

	glm::vec3 front;
	front.z = cos(glm::radians(planeControl.bank)) * cos(glm::radians(planeControl.pitch));
	front.y = sin(glm::radians(planeControl.pitch));
	front.x = sin(glm::radians(planeControl.bank)) * cos(glm::radians(planeControl.pitch));
	planeFront = glm::normalize(front);

}

/**
*	@Param	A reference to GLFWindow and two integers for width and height
*	@brief	Resizes the window relatively to the new window size
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/**
*	@Param	A reference to GLFWwindow and two double mouse coordinates
*	@Brief	Chekcs if we moved the mouse a first time, and finds mouse position.
			Also makes sure we don't go off limits.
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

/**
*	@Param	A reference to GLFWwindow and two double differences in mouse scroll.
	@Brief	To check how much we scroll (zoom, side scroll)
*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}



void characterCallback(GLFWwindow* window, unsigned int keyCode)
{
	std::cout << keyCode << '\n';
	if (keyCode == 102) {
		toggleView++; if (toggleView > 2) toggleView = 0;
	}

	if (keyCode == 103) {
		if (savedPositions.size() > 0) {
			planePos = savedPositions[nextSavedPosition++];		// GOES TO SAVED POSITION
			if (nextSavedPosition >= savedPositions.size())
				nextSavedPosition = 0;
		}
	}

	if (keyCode == 116)
		savedPositions.push_back(planePos);			// CAN SAVE POSITION
}




