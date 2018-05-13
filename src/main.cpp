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

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 planePos = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 planeFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 planeUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int season;

Plane planeControl(glm::vec3(1.0f, 1.0f, 1.0f));

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	season = 2;


														 // glfw window creation
														 // --------------------
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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	// glew stuff
	glewExperimental = GL_TRUE;
	glewInit();

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("../shaders/vtx.vert", "../shaders/fmt.frag");
	Shader modelShader("../shaders/model.vert","../shaders/model.frag");
	Shader lampShader("../shaders/lamp.vert", "../shaders/lamp.frag");

	


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
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
	

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();

	Model terrain("../Assets/test5/first.obj");
	Model lightSource("../Assets/test4/omg.obj");
	Model ourModel("../Assets/model/ask21mi.blend");
	ourShader.setInt("texture1", 0);
	
	float lampSpeed = 0.005f;
	glm::vec3 lampPos(1.0f, 3.0f, 0.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 model;



		lampPos.z -= lampSpeed;
		lampShader.use();
		model = glm::mat4();
		model = glm::translate(model, lampPos); // translate it down so it's at the center of the scene
		lampShader.setMat4("model", model);
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lightSource.Draw(lampShader);



		ourShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		planePos += planeFront * 0.002f;
		model = glm::mat4();
		model = glm::translate(model, planePos); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(planeControl.pitch), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(planeControl.bank), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("projection", projection);		
		ourShader.setMat4("view", view);
		ourShader.setMat4("model", model);
		
		ourModel.Draw(ourShader);



		//	landscape things and seasons change
		glm::vec2 seasonVec;
		switch (season)
		{
		case 1: seasonVec.x = -0.6f; seasonVec.y = -0.03f; break;
		case 2: seasonVec.x = -1.5f; seasonVec.y = -0.09f; break;
		case 3: seasonVec.x = -1.0f; seasonVec.y = -0.05f; break;
		case 4: seasonVec.x = -0.01f; seasonVec.y = -0.01f; break;
		}

		modelShader.use();
		modelShader.setVec2("season", seasonVec);
		model = glm::mat4();
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.2f, 2.2f, 2.2f));
		modelShader.setMat4("model", model);
		modelShader.setVec3("lampPos", lampPos);
		terrain.Draw(modelShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
		planeControl.changePitch(0.1f);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		planeControl.changeBank(-0.1);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		planeControl.changePitch(-0.1);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		planeControl.changeBank(0.1);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		season = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		season = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		season = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		season = 4;

	glm::vec3 front;
	front.x = cos(glm::radians(planeControl.bank)) * cos(glm::radians(planeControl.pitch));
	front.y = sin(glm::radians(planeControl.pitch));
	front.z = sin(glm::radians(planeControl.bank)) * cos(glm::radians(planeControl.pitch));
	planeFront = glm::normalize(front);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}
