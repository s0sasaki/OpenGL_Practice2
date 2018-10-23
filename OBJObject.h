#ifndef OBJOBJECT_H
#define OBJOBJECT_H

//#ifdef __APPLE__
//#include <OpenGL/gl3.h>
//#include <OpenGL/glext.h>
//#include <OpenGL/gl.h> // Remove this line in future projects
//#else
//#include <GL/glew.h>
//#endif
//
//#include <GLFW/glfw3.h>
//#include <glm/mat4x4.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <vector>



//sasaki
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


class OBJObject
{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<unsigned int> > indices;

    GLfloat vertices_data[1000000][6];
    GLuint indices_data[1000000][3];
    
public:
	OBJObject(const char* filepath);
    ~OBJObject();
    glm::mat4 toWorld;
    glm::vec3 color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat ambientStrength{0.3f};
    GLfloat diffuseStrength{0.5f};
    GLfloat specularStrength{1.0f};
    
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview;
    
    void parse(const char* filepath);
    void draw(GLuint shaderProgram);
    void update(float scale);
    void spin(glm::vec3 w);
    void rescale(float scale);
};

#endif
