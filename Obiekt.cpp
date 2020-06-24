
#define _CRT_SECURE_NO_DEPRECATE

#include "Obiekt.h"

void Obiekt::move(glm::mat4 &M) {
	angle += step; //jedna trzydziesta dziesięciu stopni
	if (angle < max_angle) step *= -1;
	if (angle > 0) {
		animate = false;
		angle = 0.0;
		step *= -1;
	}
	M = glm::translate(M, transform_point);
	M = glm::rotate(M, (2*is_it_klawisz - 1)*(float)angle, glm::vec3(0.0f, 0.0f, 1.0f));
	M = glm::translate(M, -transform_point);
}

std::vector<Obiekt*> Obiekt::readOBJfile(const char * path, std::vector <material*> materials,
	std::vector<Obiekt*> &OBJs) {
	int sumOfPrevSizesV = 0;
	int sumOfPrevSizesU = 0;
	int sumOfPrevSizesN = 0;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;			//wierzchołki 		tymczasowe
	std::vector< glm::vec2 > temp_uvs;				//
	std::vector< glm::vec3 > temp_normals;			//normalne

	FILE* file;
	file=fopen(path, "r");

	int nr = -1;
	while (true) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		//printf("%s\n", lineHeader);
		if (res == EOF) {
			pushCurrentObject(OBJs, nr, vertexIndices, uvIndices, normalIndices, temp_vertices,
				temp_uvs, temp_normals);
			break;
		}
		else if (strcmp(lineHeader, "o") == 0) {     //jesli obiekt
			if (nr > -1) {
				pushCurrentObject(OBJs, nr, vertexIndices, uvIndices, normalIndices, temp_vertices,
					temp_uvs, temp_normals);  
				sumOfPrevSizesV += temp_vertices.size();
				sumOfPrevSizesU += temp_uvs.size();
				sumOfPrevSizesN += temp_normals.size();
			}
			char name[80];
			fscanf(file, "%s\n", name);
			nr = OBJs.size();
			Obiekt* object = new Obiekt();
			strcpy(object->name, name);
			OBJs.push_back(object);
			vertexIndices.clear(); uvIndices.clear(); normalIndices.clear();
			temp_vertices.clear(); temp_uvs.clear(); temp_normals.clear();
		}
		else if (strcmp(lineHeader, "v") == 0) {              //jesli wierzchołek
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {   //jesli texture vertex (wierzchołek tekstury?)
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);

			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {                //normalne 
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {        //f- face 
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
				&normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			vertexIndices.push_back(vertexIndex[0] - sumOfPrevSizesV);
			vertexIndices.push_back(vertexIndex[1] - sumOfPrevSizesV);
			vertexIndices.push_back(vertexIndex[2] - sumOfPrevSizesV);
			uvIndices.push_back(uvIndex[0] - sumOfPrevSizesU);
			uvIndices.push_back(uvIndex[1] - sumOfPrevSizesU);
			uvIndices.push_back(uvIndex[2] - sumOfPrevSizesU);
			normalIndices.push_back(normalIndex[0] - sumOfPrevSizesN);
			normalIndices.push_back(normalIndex[1] - sumOfPrevSizesN);
			normalIndices.push_back(normalIndex[2] - sumOfPrevSizesN);
		}
		else if (strcmp(lineHeader, "usemtl") == 0) {
			fscanf(file, "%s\n", lineHeader);
			for (int i = 0; i < materials.size(); i++) {
				if (strcmp(materials[i]->name, lineHeader) == 0) {
					OBJs[nr]->mat = materials[i];
				}
			}
		}
	}

	return OBJs;
}

void Obiekt::pushCurrentObject(std::vector< Obiekt* > OBJs,
	int nr,
	std::vector< unsigned int > vertexIndices,
	std::vector< unsigned int > uvIndices,
	std::vector< unsigned int > normalIndices,
	std::vector< glm::vec3 > temp_vertices,
	std::vector< glm::vec2 > temp_uvs,
	std::vector< glm::vec3 > temp_normals) {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normies;

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		//printf("vetex index %d\n",vertexIndex );
		//printf("temp_vert size %ld",temp_vertices.size());
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];

		OBJs[nr]->vertices.push_back(vertex);
		//verts.push_back(vertex);
	}
	//OBJs[nr]->vertices.push_back(verts);

	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		OBJs[nr]->uvs.push_back(uv);
	}

	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		OBJs[nr]->normals.push_back(normal);
		//normies.push_back(normal);
		}

	//OBJs[nr]->normals.push_back(normies);
}