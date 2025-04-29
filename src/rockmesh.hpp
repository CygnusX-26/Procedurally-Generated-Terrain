#include <glm/glm.hpp>
#include "gl_core_3_3.h"
#include <vector>
#include <random>
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

    RockMesh(float radius, float roughness = 0.2f, int slices = 12, int seed = 0 ) {
        std::vector<glm::vec4> vertices;
        std::vector<unsigned int> indices;

        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> offset(-roughness, roughness);

        vertices.push_back(glm::vec4(0.0f,  radius, 0.0f, 1.0f));
        vertices.push_back(glm::vec4( radius, 0.0f, 0.0f, 1.0f));
        vertices.push_back(glm::vec4(0.0f, 0.0f,  radius, 1.0f));
        vertices.push_back(glm::vec4(-radius, 0.0f, 0.0f, 1.0f));
        vertices.push_back(glm::vec4(0.0f, 0.0f, -radius, 1.0f));
        vertices.push_back(glm::vec4(0.0f, -radius, 0.0f, 1.0f));

        for (auto& v : vertices) {
            v.x += offset(rng);
            v.y += offset(rng);
            v.z += offset(rng);
        }

        indices = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1,
            5, 2, 1,
            5, 3, 2,
            5, 4, 3,
            5, 1, 4
        };
    
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