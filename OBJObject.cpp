#include "OBJObject.h"
#include "Window.h"
#include <iostream>

OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*6*sizeof(GLfloat), vertices_data, GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          6 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          6 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)(3 * sizeof(GLfloat))); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*3*sizeof(GLuint), indices_data, GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}

OBJObject::~OBJObject()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void OBJObject::parse(const char *filepath) 
{
    FILE* fp;
    float x,y,z;
    unsigned int a,b,c,d,e,f;
    fp = fopen(filepath, "rb");
    if (fp==NULL) { std::cerr << "error loading file" << std::endl; exit(-1); }
    
    char buf[256];
    while(fgets(buf, 256, fp)){
        if(buf[0]==0 || buf[1]==0)
            continue;
        if ((buf[0]=='v') && (buf[1]==' ')){
            sscanf(buf, "v %f %f %f", &x, &y, &z);
            vertices.emplace_back(x, y, z);
        }else if ((buf[0]=='v') && (buf[1]=='n')){
            sscanf(buf, "vn %f %f %f", &x, &y, &z);
            normals.emplace_back(x, y, z);
        }else if ((buf[0]=='f') && (buf[1]==' ')){
            sscanf(buf, "f %u//%u %u//%u %u//%u", &a,&b,&c,&d,&e,&f);
            indices.emplace_back(std::vector<unsigned int>{a, b, c, d, e, f});
            
        }
    }
    
    glm::vec3 max = vertices[0];
    glm::vec3 min = vertices[0];
    for( std::vector<glm::vec3>::iterator it=vertices.begin(); it!=vertices.end(); it++){
        if(it->x > max.x) max.x = it->x;
        if(it->y > max.y) max.y = it->y;
        if(it->z > max.z) max.z = it->z;
        if(it->x < min.x) min.x = it->x;
        if(it->y < min.y) min.y = it->y;
        if(it->z < min.z) min.z = it->z;
    }
    glm::vec3 offset {(max.x + min.x)/2.0, (max.y + min.y)/2.0, (max.z + min.z)/2.0};
    GLfloat scale = 10.0/(max.x - min.x);

    for(int i=0; i<vertices.size(); i++){
        vertices_data[i][0] = (vertices[i].x - offset.x)*scale;
        vertices_data[i][1] = (vertices[i].y - offset.y)*scale;
        vertices_data[i][2] = (vertices[i].z - offset.z)*scale;
        vertices_data[i][3] = normals[i].x;
        vertices_data[i][4] = normals[i].y;
        vertices_data[i][5] = normals[i].z;
        
    }
    for(int i=0; i<indices.size(); i++){
        indices_data[i][0] = indices[i][0]-1;
        indices_data[i][1] = indices[i][2]-1;
        indices_data[i][2] = indices[i][4]-1;
    }
    
    fclose(fp);
    
}

//void OBJObject::draw(float scale, glm::vec3 offset)
void OBJObject::draw(GLuint shaderProgram)
{
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    // Now draw the object. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    // Tell OpenGL to draw with triangles, using the number of indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size()*3, GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
}

void OBJObject::update(float scale)
{
    rescale(scale);
}

void OBJObject::spin(glm::vec3 w)
{
    // If you haven't figured it out from the last project, this is how you fix spin's behavior
//    toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    GLfloat angle = glm::length(w);
    if(angle<0.0) angle *= -1.0f;
    toWorld = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(w)) * toWorld;
}

void OBJObject::rescale(float scale){
    toWorld = toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
}

