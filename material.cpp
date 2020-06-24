#define _CRT_SECURE_NO_WARNINGS
#include "material.h"
using namespace std;

std::vector<material*> material::readMTL(const char* path) {
	FILE* file;
	file = fopen(path, "r");
	std::vector<material*> materials;

	int nr = -1;
	while (true) {
		//read mtl file and convert it's contents to vector of type material
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}
		
		else if (strcmp(lineHeader, "newmtl") == 0) {
			material* m = new material();
			materials.push_back(m);
			nr++;
			fscanf(file, "%s\n", materials[nr]->name);
		}
		else if (strcmp(lineHeader, "Ns") == 0) {
			double temp;
			fscanf(file, "%lf\n", &temp);
			//printf_s("%lf kkkk\n", temp);
			materials[nr]->Ns = temp;
		}
		else if (strcmp(lineHeader, "Ka") == 0){
			glm::vec3 KA;
			fscanf(file, "%f %f %f\n", &KA.x, &KA.y, &KA.z);
			materials[nr]->Ka = KA;
		}
		else if (strcmp(lineHeader, "Kd") == 0) {
			glm::vec3 KD;
			fscanf(file, "%f %f %f\n", &KD.x, &KD.y, &KD.z);
			materials[nr]->Kd = KD;
		}
		else if (strcmp(lineHeader, "Ks") == 0) {
			glm::vec3 KS;
			fscanf(file, "%f %f %f\n", &KS.x, &KS.y, &KS.z);
			materials[nr]->Ks = KS;
		}
		else if (strcmp(lineHeader, "Ke") == 0) {
			glm::vec3 KE;
			fscanf(file, "%f %f %f\n", &KE.x, &KE.y, &KE.z);
			materials[nr]->Ke = KE;
		}	
		else if (strcmp(lineHeader, "Ni") == 0) {
			double temp;
			fscanf(file, "%lf\n", &temp);
			materials[nr]->Ni = temp;
		}
		else if (strcmp(lineHeader, "d") == 0) {
			double temp;
			fscanf(file, "%lf\n", &temp);
			materials[nr]->d = temp;
		}
		else if (strcmp(lineHeader, "illum") == 0) {
			int temp;
			fscanf(file, "%d\n", &temp);
			materials[nr]->illum = temp;
		}
		else if (!strcmp(lineHeader, "map_Kd") | !strcmp(lineHeader, "map_Ks") | 
			!strcmp(lineHeader, "map_Ka")) {
			char tex[120];
			fscanf(file, "%s\n", tex);
			materials[nr]->textures.push_back(tex);
		}
	}

	return materials;
}

std::ostream& operator<<(std::ostream& os, const material& obj)
{
	os << obj.name << endl << obj.Ns << endl << glm::to_string(obj.Ka) << endl << 
		glm::to_string(obj.Kd) << endl << glm::to_string(obj.Ks) << endl <<
		glm::to_string(obj.Ke) << endl << obj.Ni << endl << obj.d << endl << obj.illum << endl;
	return os;
}

bool material::makeArrays(std::vector<material*> materials,
	std::vector< float > &NS,
	std::vector< glm::vec3 > &KA,
	std::vector< glm::vec3 > &KS,
	std::vector< float > &D) {
	
	for (int i = 0; i<materials.size(); i++) {
		NS.push_back(materials[i]->Ns);
		KA.push_back(materials[i]->Ka);
		KS.push_back(materials[i]->Ks);
		D.push_back(materials[i]->d);
	}
	return true;
}