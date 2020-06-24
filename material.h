#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <ostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "glm/ext.hpp"
#include <glm/gtx/string_cast.hpp>


using namespace glm;
class material
{
public:
	char name[120];
	double Ns;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	double Ni;
	double d;
	int illum;
	std::vector<std::string> textures;
	GLuint tex;

	static std::vector<material*> readMTL(const char* path);
	friend std::ostream& operator<<(std::ostream& os, const material& obj);
	static bool makeArrays(std::vector<material*> materials,
		std::vector< float > &NS,
		std::vector< glm::vec3 > &KA,
		std::vector< glm::vec3 > &KS,
		std::vector< float > &D);
};

