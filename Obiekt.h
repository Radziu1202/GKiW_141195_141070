#pragma once
//#include <iostream.h>
#include <stdlib.h>
#include "material.h"
//#include "constants.h"
class Obiekt {
public:
	char name[80];
	std::vector<glm::vec3>vertices;
	std::vector<glm::vec3>normals;
	std::vector<glm::vec2> uvs;
	material* mat;
	bool animate = false;
	double angle = 0.0;
	float step = -10.0/180*3.1415/8.0;
	double max_angle = -10.0 / 180 * 3.1415;
	glm::vec3 transform_point;	//środki klawiszy, końce młoteczków
	bool is_it_klawisz = false;
	

	void move(glm::mat4 &M);
	static std::vector<Obiekt*> readOBJfile(const char * path, std::vector<material*> materials,
		std::vector<Obiekt*> &OBJs);
	static void pushCurrentObject(std::vector< Obiekt* > OBJs,
		int nr,
		std::vector< unsigned int > vertexIndices,
		std::vector< unsigned int > uvIndices,
		std::vector< unsigned int > normalIndices,
		std::vector< glm::vec3 > temp_vertices,
		std::vector< glm::vec2 > temp_uvs,
		std::vector< glm::vec3 > temp_normals);
};

