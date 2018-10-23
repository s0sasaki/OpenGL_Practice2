#include "Window.h"

const char* window_title = "GLFW Starter Project";
//Cube * cube;
GLint shaderProgram;

// On some systems you need to change this to the absolute path
//#define VERTEX_SHADER_PATH "../shader.vert"
//#define FRAGMENT_SHADER_PATH "../shader.frag"
#define VERTEX_SHADER_PATH "shader.vert"    //sasaki
#define FRAGMENT_SHADER_PATH "shader.frag"  //sasaki

OBJObject *bunny;
OBJObject *dragon;
OBJObject *bear;
OBJObject *object;
OBJObject *point_light;
OBJObject *spot_light;
OBJObject *dir_light;

float scale = 1.0;
int mousebottun_on = 0;
int flag_norm_color = 0;
int toggle_move_object = 1;
int toggle_move_point_light = 1;
int toggle_move_spot_light = 1;
int toggle_move_dir_light = 1;
int toggle_on_point_light = 1;
int toggle_on_spot_light = 1;
int toggle_on_dir_light = 1;
double xpos_old;
double ypos_old;
glm::vec3 point_light_pos(50.0f,50.0f, 0.0f);
glm::vec3 point_light_color(1.0f, 1.0f, 0.0f);
glm::vec3 spot_light_pos(-50.0f, 50.0f, 0.0f);
glm::vec3 spot_light_color(0.0f, 1.0f, 1.0f);
glm::vec3 dir_light_pos(0.0f, -50.0f*1.414, 0.0f);
glm::vec3 dir_light_color(1.0f, 0.0f, 1.0f);
GLfloat spot_cutoff = 0.999f;
GLfloat spot_exponent = 5.0f;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
//    cube = new Cube();
    bunny = new OBJObject("bunny.obj");
    dragon = new OBJObject("dragon.obj");
    bear = new OBJObject("bear.obj");
    dragon->color = glm::vec3(0.4f, 0.8f, 0.2f);
    dragon->diffuseStrength = 0.0f;
    bear->color = glm::vec3(0.8f, 0.5f, 1.0f);
    bear->specularStrength = 0.0f;
    object = bunny;
    point_light = new OBJObject("sphere.obj");
    point_light->toWorld = point_light->toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    point_light->toWorld = glm::translate(point_light->toWorld, glm::vec3(point_light_pos));
    spot_light = new OBJObject("sphere.obj");
    spot_light->toWorld = spot_light->toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    spot_light->toWorld = glm::translate(spot_light->toWorld, glm::vec3(spot_light_pos));
    dir_light = new OBJObject("sphere.obj");
    dir_light->toWorld = dir_light->toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    dir_light->toWorld = glm::translate(dir_light->toWorld, glm::vec3(dir_light_pos));
    // Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
//    delete(cube);
    delete bunny;
    delete dragon;
    delete bear;
    delete point_light;
    delete spot_light;
    delete dir_light;
    glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
//    cube->update();
    object->update(scale);
    point_light->update(1.0f);
    spot_light->update(1.0f);
    dir_light->update(1.0f);
    scale = 1.0f;
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
    // Render the cube
    //    cube->draw(shaderProgram);

    GLint loc;
    loc = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3f(loc, cam_pos.x, cam_pos.y, cam_pos.z);
    loc = glGetUniformLocation(shaderProgram, "ambientLightColor");
    glUniform3f(loc, 1.0f, 1.0f, 1.0f);
    loc = glGetUniformLocation(shaderProgram, "pointLight.color");
    glUniform3f(loc, point_light_color.x, point_light_color.y, point_light_color.z);
    loc = glGetUniformLocation(shaderProgram, "pointLight.position");
    glUniform3f(loc, point_light_pos.x, point_light_pos.y, point_light_pos.z);
    loc = glGetUniformLocation(shaderProgram, "spotLight.color");
    glUniform3f(loc, spot_light_color.x, spot_light_color.y, spot_light_color.z);
    loc = glGetUniformLocation(shaderProgram, "spotLight.position");
    glUniform3f(loc, spot_light_pos.x, spot_light_pos.y, spot_light_pos.z);
    loc = glGetUniformLocation(shaderProgram, "spotLight.direction");
    glUniform3f(loc, -spot_light_pos.x, -spot_light_pos.y, -spot_light_pos.z-20.0f);
    loc = glGetUniformLocation(shaderProgram, "spotLight.spotCutoff");
    glUniform1f(loc, spot_cutoff);
    loc = glGetUniformLocation(shaderProgram, "spotLight.spotExponent");
    glUniform1f(loc, spot_exponent);
    loc = glGetUniformLocation(shaderProgram, "directionalLightColor");
    glUniform3f(loc, dir_light_color.x, dir_light_color.y, dir_light_color.z);
    loc = glGetUniformLocation(shaderProgram, "directionalLightPos");
    glUniform3f(loc, dir_light_pos.x, dir_light_pos.y, dir_light_pos.z);
    
    loc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3f(loc, object->color.x, object->color.y, object->color.z);
    loc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1f(loc, object->ambientStrength);
    loc = glGetUniformLocation(shaderProgram, "diffuseStrength");
    glUniform1f(loc, object->diffuseStrength);
    loc = glGetUniformLocation(shaderProgram, "specularStrength");
    glUniform1f(loc, object->specularStrength);
    loc = glGetUniformLocation(shaderProgram, "flagNormColor");
    glUniform1i(loc, flag_norm_color);
    loc = glGetUniformLocation(shaderProgram, "toggleOnPoint");
    glUniform1i(loc, toggle_on_point_light);
    loc = glGetUniformLocation(shaderProgram, "toggleOnSpot");
    glUniform1i(loc, toggle_on_spot_light);
    loc = glGetUniformLocation(shaderProgram, "toggleOnDir");
    glUniform1i(loc, toggle_on_dir_light);
    object->draw(shaderProgram);
    
    loc = glGetUniformLocation(shaderProgram, "diffuseStrength");
    glUniform1f(loc, 0.0f);
    loc = glGetUniformLocation(shaderProgram, "specularStrength");
    glUniform1f(loc, 0.0f);
    loc = glGetUniformLocation(shaderProgram, "flagNormColor");
    glUniform1i(loc, 0);
    
    loc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1f(loc, 0.3f+0.7f*toggle_on_point_light);
    loc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3f(loc, point_light_color.x, point_light_color.y, point_light_color.z);
    point_light->draw(shaderProgram);
    
    loc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1f(loc, 0.3f+0.7f*toggle_on_spot_light);
    loc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3f(loc, spot_light_color.x, spot_light_color.y, spot_light_color.z);
    spot_light->draw(shaderProgram);
    
    loc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1f(loc, 0.3f+0.7f*toggle_on_dir_light);
    loc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3f(loc, dir_light_color.x, dir_light_color.y, dir_light_color.z);
    dir_light->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
		}
        if (key == GLFW_KEY_F1) object = bunny;
        if (key == GLFW_KEY_F2) object = dragon;
        if (key == GLFW_KEY_F3) object = bear;
        if (key == GLFW_KEY_N) flag_norm_color = !flag_norm_color;
        if (key == GLFW_KEY_0) toggle_move_object = !toggle_move_object;
        if (key == GLFW_KEY_1 && mods != GLFW_MOD_SHIFT) toggle_move_point_light = !toggle_move_point_light;
        if (key == GLFW_KEY_2 && mods != GLFW_MOD_SHIFT) toggle_move_spot_light = !toggle_move_spot_light;
        if (key == GLFW_KEY_3 && mods != GLFW_MOD_SHIFT) toggle_move_dir_light = !toggle_move_dir_light;
        if (key == GLFW_KEY_1 && mods == GLFW_MOD_SHIFT) toggle_on_point_light = !toggle_on_point_light;
        if (key == GLFW_KEY_2 && mods == GLFW_MOD_SHIFT) toggle_on_spot_light = !toggle_on_spot_light;
        if (key == GLFW_KEY_3 && mods == GLFW_MOD_SHIFT) toggle_on_dir_light = !toggle_on_dir_light;
        if (key == GLFW_KEY_W && mods != GLFW_MOD_SHIFT) spot_cutoff = 1.0f - (1.0f - spot_cutoff)*2.0f;
        if (key == GLFW_KEY_W && mods == GLFW_MOD_SHIFT) spot_cutoff = 1.0f - (1.0f - spot_cutoff)*0.5f;
        if (key == GLFW_KEY_E && mods != GLFW_MOD_SHIFT) spot_exponent *= 2.0f;
        if (key == GLFW_KEY_E && mods == GLFW_MOD_SHIFT) spot_exponent *= 0.5f;
	}
}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    scale += yoffset*0.01;
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos){
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(state == GLFW_PRESS){
        if(mousebottun_on == 1){
            double x1 = (2.0*xpos_old-Window::width)/Window::width;
            double y1 = (2.0*ypos_old-Window::height)/Window::height;
            double x2 = (2.0*xpos    -Window::width)/Window::width;
            double y2 = (2.0*ypos    -Window::height)/Window::height;
            glm::vec3 u = glm::vec3(x1, -y1, 1 - x1*x1 - y1*y1);
            glm::vec3 v = glm::vec3(x2, -y2, 1 - x2*x2 - y2*y2);
            glm::vec3 w = glm::cross(u, v);
            GLfloat angle = glm::length(w);
            if(angle<0.0) angle *= -1.0f;
            if(toggle_move_object) {
                object->spin(w);
            }
            if(toggle_move_point_light) {
                point_light->spin(w);
                point_light_pos = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(w)) * glm::vec4(point_light_pos, 0.0f);
            }
            if(toggle_move_spot_light) {
                spot_light->spin(w);
                spot_light_pos = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(w)) * glm::vec4(spot_light_pos, 0.0f);
            }
            if(toggle_move_dir_light) {
                dir_light->spin(w);
                dir_light_pos = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(w)) * glm::vec4(dir_light_pos, 0.0f);
            }
        }else{
            mousebottun_on = 1;
        }
        xpos_old = xpos;
        ypos_old = ypos;
    }else{
        mousebottun_on = 0;
    }
    
}
