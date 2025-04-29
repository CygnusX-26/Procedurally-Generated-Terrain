#include <glm/glm.hpp>
#include "gl_core_3_3.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class TreeMesh {
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

    TreeMesh(float trunkHeight, float trunkWidth, float coneHeight, float coneWidth, int slices = 12) {
        std::vector<glm::vec4> vertices;
        std::vector<unsigned int> indices;

        // trunk verticies
        float trunkRadius = trunkWidth / 2.0f;
        for (int i = 0; i <= slices; i++) {
            float angle = 2.0f * M_PI * i / slices;
            float x = cos(angle) * trunkRadius;
            float z = sin(angle) * trunkRadius;
            vertices.push_back({x, 0.0f, z, TRUNK});
            vertices.push_back({x, trunkHeight, z, TRUNK});
            
        }

        vertices.push_back({0, trunkHeight, 0, TRUNK});
        vertices.push_back({0, 0.0f, 0, TRUNK});
        unsigned int topCenter = vertices.size() - 2;
        unsigned int bottomCenter = vertices.size() - 1;
    
        for (int i = 0; i < slices; i++) {
            int base = 2 * i;
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back((base + 2) % (2 * slices));
            indices.push_back((base + 2) % (2 * slices));
            indices.push_back(base + 1);
            indices.push_back((base + 3) % (2 * slices));
            indices.push_back(base);
            indices.push_back((base + 2) % (2 * slices));
            indices.push_back(topCenter);
            indices.push_back(base + 1);
            indices.push_back(bottomCenter);
            indices.push_back((base + 3) % (2 * slices));
        }
    
        // cone vertices
        float coneRadius = coneWidth / 2.0f;
        unsigned int coneBase = vertices.size();
        for (int i = 0; i <= slices; i++) {
            float angle = 2.0f * M_PI * i / slices;
            float x = cos(angle) * coneRadius;
            float z = sin(angle) * coneRadius;
            vertices.push_back({x, trunkHeight, z, LEAVES});
        }
        vertices.push_back({0, trunkHeight + coneHeight, 0, LEAVES});
        unsigned int coneTip = vertices.size() - 1;
        
        for (int i = 0; i < slices; i++) {
            indices.push_back(coneBase + i);
            indices.push_back(coneBase + (i + 1) % slices);
            indices.push_back(coneTip);
            indices.push_back(coneBase + i);
            indices.push_back(topCenter);
            indices.push_back(coneBase + (i + 1) % slices);
        }
    
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

    ~TreeMesh() {
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