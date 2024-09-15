#pragma once
#include <array>
#include <vector>
#include <string>

#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh();
	std::vector<glm::vec4> GetVertices();
};

class HardCodedMesh : Mesh
{
private:
	std::vector<glm::vec4> vertices_;
	std::vector<glm::vec4> normals_;
	std::vector<glm::vec4> texture_;
public:
	HardCodedMesh(float vertices[], size_t vertices_count) : Mesh()
	{
		for (size_t i = 0; i < vertices_count; i += 3)
		{
			this->vertices_.push_back(glm::vec4(
				vertices[i],
				vertices[i+1],
				vertices[i+2],
				1.0
			));
		}
	}

	std::vector<glm::vec4> GetVertices() { return this->vertices_; }
};

class MeshUSD : Mesh
{
private:
	void LoadData();
public:
	MeshUSD(const std::string& filename) : Mesh() {}
};

class MeshOBJ : Mesh
{
public:
	MeshOBJ(const std::string& filename) : Mesh() {}
};