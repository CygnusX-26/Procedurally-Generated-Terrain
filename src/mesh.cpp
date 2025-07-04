#define NOMINMAX
#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Helper functions
int indexOfNumberLetter(std::string& str, int offset);
int lastIndexOfNumberLetter(std::string& str);
std::vector<std::string> split(const std::string &s, char delim);

// Constructor - load mesh from file
Mesh::Mesh(std::string filename, const ObjType mType, bool keepLocalGeometry) {
	minBB = glm::vec3(std::numeric_limits<float>::max());
	maxBB = glm::vec3(std::numeric_limits<float>::lowest());

	meshType = mType;

	vao = 0;
	vbuf = 0;
	vcount = 0;
	load(filename, keepLocalGeometry);
}

// Draw the mesh
void Mesh::draw() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vcount);
	glBindVertexArray(0);
}

// Load a wavefront OBJ file
void Mesh::load(std::string filename, bool keepLocalGeometry) {
	// Release resources
	release();

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::stringstream ss;
		ss << "Error reading " << filename << ": failed to open file";
		throw std::runtime_error(ss.str());
	}

	// Store vertex data while reading
	std::vector<glm::vec3> raw_vertices;
	std::vector<std::vector<unsigned int>> v_elements;  // indices of vertex positions, texture coordinates and normals
	std::vector<glm::vec3> raw_normals;
	std::vector<glm::vec2> raw_uvs;  // texture coordinates

	std::string line;
	while (getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			// Read position data
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			glm::vec3 vert(stof(values[0]), stof(values[1]), stof(values[2]));
			raw_vertices.push_back(vert);

			// Update bounding box
			minBB = glm::min(minBB, vert);
			maxBB = glm::max(maxBB, vert);
		}
		else if (line.substr(0, 2) == "vt") {
			// Read texture coorindates
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			glm::vec2 uv(stof(values[0]), stof(values[1]));
			raw_uvs.push_back(uv);
		}
		else if (line.substr(0, 2) == "vn") {
			// Read the normals
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			glm::vec3 norm(stof(values[0]), stof(values[1]), stof(values[2]));
			raw_normals.push_back(norm);
		}
		else if (line.substr(0, 2) == "f ") {
			// Read face data
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			for (int i = 0; i < int(values.size()) - 2; i++) {
				// Split up vertex indices
				std::vector<std::string> v1 = split(values[0], '/');  // Triangle fan for ngons
				std::vector<std::string> v2 = split(values[i+1], '/');
				std::vector<std::string> v3 = split(values[i+2], '/');

				// Store position indices
				std::vector<unsigned int> indices1, indices2, indices3;
				indices1.push_back(stoul(v1[0]) - 1);
				indices1.push_back(stoul(v1[1]) - 1);
				indices1.push_back(stoul(v1[2]) - 1);
				indices2.push_back(stoul(v2[0]) - 1);
				indices2.push_back(stoul(v2[1]) - 1);
				indices2.push_back(stoul(v2[2]) - 1);
				indices3.push_back(stoul(v3[0]) - 1);
				indices3.push_back(stoul(v3[1]) - 1);
				indices3.push_back(stoul(v3[2]) - 1);
				v_elements.push_back(indices1);
				v_elements.push_back(indices2);
				v_elements.push_back(indices3);
			}
		}
	}
	file.close();

	// Check if the file was invalid
	if (raw_vertices.empty() || v_elements.empty()) {
		std::stringstream ss;
		ss << "Error reading " << filename << ": invalid file or no geometry";
		throw std::runtime_error(ss.str());
	}

	// TODO 1 Calculate tangent and bitangent vectors for each triangle, and store the results in the arrays: "tangent" and "bitangent"
	// TODO 1-1: Calculate tangent and bitangent vectors for each triangle

	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;
	glm::vec3 e1;
	glm::vec3 e2;
	glm::vec2 uv1;
	glm::vec2 uv2;
	float det;
	for (int i = 0; i < int(v_elements.size()); i += 3) {
		e1 = raw_vertices[v_elements[i+1][0]] - raw_vertices[v_elements[i][0]];
		e2 = raw_vertices[v_elements[i+2][0]] - raw_vertices[v_elements[i][0]];
		uv1 = raw_uvs[v_elements[i+1][1]] - raw_uvs[v_elements[i][1]];
		uv2 = raw_uvs[v_elements[i+2][1]] - raw_uvs[v_elements[i][1]];
		det = (uv1.x*uv2.y - uv2.x*uv1.y);
		if (det != 0) {
			det = 1/det;
		}
		tangent.push_back(det*(uv2.y*e1 - uv1.y*e2));
		bitangent.push_back(det*(-uv2.x*e1 + uv1.x*e2));
	}

	// Create vertex array
	vertices = std::vector<Vertex>(v_elements.size());
	for (int i = 0; i < int(v_elements.size()); i += 3) {
		// Store positions
		vertices[i+0].pos = raw_vertices[v_elements[i+0][0]];
		vertices[i+1].pos = raw_vertices[v_elements[i+1][0]];
		vertices[i+2].pos = raw_vertices[v_elements[i+2][0]];

		// Store normals
		vertices[i+0].norm = raw_normals[v_elements[i+0][2]];
		vertices[i+1].norm = raw_normals[v_elements[i+1][2]];
		vertices[i+2].norm = raw_normals[v_elements[i+2][2]];

		// Store texture coordinates:
		vertices[i+0].uv = raw_uvs[v_elements[i+0][1]];
		vertices[i+1].uv = raw_uvs[v_elements[i+1][1]];
		vertices[i+2].uv = raw_uvs[v_elements[i+2][1]];

		// TODO 1-2 Store tangent and bitangent
		vertices[i+0].tangent = tangent[i/3];
		vertices[i+1].tangent = tangent[i/3];
		vertices[i+2].tangent = tangent[i/3];
		vertices[i+0].bitangent = bitangent[i/3];
		vertices[i+1].bitangent = bitangent[i/3];
		vertices[i+2].bitangent = bitangent[i/3];
	}
	vcount = (GLsizei)vertices.size();

	// Load vertices into OpenGL
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(1);  // norm
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);  // uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(glm::vec3)));  // the last parameter: offset
	glEnableVertexAttribArray(3);  // tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Mesh::Vertex, tangent));
	glEnableVertexAttribArray(4);  // bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Mesh::Vertex, bitangent));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Delete local copy of geometry
	if (!keepLocalGeometry)
		vertices.clear();
}

// Release resources
void Mesh::release() {
	minBB = glm::vec3(std::numeric_limits<float>::max());
	maxBB = glm::vec3(std::numeric_limits<float>::lowest());

	vertices.clear();
	if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
	if (vbuf) { glDeleteBuffers(1, &vbuf); vbuf = 0; }
	vcount = 0;
}

int indexOfNumberLetter(std::string& str, int offset) {
	for (int i = offset; i < int(str.length()); ++i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return (int)str.length();
}
int lastIndexOfNumberLetter(std::string& str) {
	for (int i = int(str.length()) - 1; i >= 0; --i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return 0;
}
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;

	std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}
