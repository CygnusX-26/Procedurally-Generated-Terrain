#include <glm/glm.hpp>
#include "gl_core_3_3.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class RockMesh {
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t indexCount;
public:
    
    enum TreeVertType {
        TRUNK,
        LEAVES
    };

    RockMesh(float radius, float roughness = 0.2f, int slices = 12 ) {
        std::vector<glm::vec4> vertices;
        std::vector<unsigned int> indices;

        //TODO
    
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(0);
        
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        indexCount = indices.size();

        glBindVertexArray(0);
        
        
    }

    ~RockMesh() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    size_t getIndexCount() {
        return indexCount;
    }

    GLuint getVao() {
        return vao;
    }

    GLuint getVbo() {
        return vbo;
    }

    GLuint getEbo() {
        return ebo;
    }
};