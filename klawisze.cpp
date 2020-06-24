#define _CRT_SECURE_NO_WARNINGS
#include "klawisze.h"

int modeUp = GLFW_KEY_C;
int modeDown = GLFW_KEY_X;
int playKeySet[12] = { GLFW_KEY_F, GLFW_KEY_T, GLFW_KEY_G, GLFW_KEY_Y, GLFW_KEY_H,
	GLFW_KEY_J,GLFW_KEY_I,GLFW_KEY_K,GLFW_KEY_O,GLFW_KEY_L,GLFW_KEY_P,GLFW_KEY_SEMICOLON};
bool pressed = false;
bool zwolnij;

int mode = 0;
int num;

int indeksy_klawiszy[89];
int indeksy_mlotkow[89];
int indeksy_filcy[89];

//Liczy index w tablicy OBJs dla wszytskich ruchomych czêœci pianina
void determine_indeces(std::vector<OBJ*> OBJs) {
	int first_dig, second_dig, numer;
	for (int i = 0; i < OBJs.size(); i++) {
		if (OBJs[i]->name[0] == 'K' && OBJs[i]->name[1] == '_') {        //klawisz  K_cube w .obj
			first_dig = OBJs[i]->name[2] - '0';                //nadajemy numer posczegolnym klawiszom
			second_dig = OBJs[i]->name[3] - '0';				// na podstawie nazw klawiszy w pliku .obj
			if (second_dig >= 0 && second_dig <= 9) numer = 10 * first_dig + second_dig;   //ostateczny numer
			else numer = first_dig;		
			indeksy_klawiszy[numer] = i;
			OBJs[i]->is_it_klawisz = true;
			OBJs[i]->max_angle = -4.0 / 180 * 3.1415;			//ustawienie głębokości wciśniecia 
			OBJs[i]->step = OBJs[i]->max_angle / 8.0;
			if (strcmp(OBJs[i]->mat->name, "CzarneKlawisze") == 0) {
				OBJs[i]->max_angle = -1.0 / 180 * 3.1415;			 		//dla czarnych troche inaczej bo sa mniejsze
				OBJs[i]->step = OBJs[i]->max_angle / 8.0;
			}
		}
		else if (OBJs[i]->name[0] == 'F' && OBJs[i]->name[1] == '_') {			//filce
			first_dig = OBJs[i]->name[2] - '0';											
			second_dig = OBJs[i]->name[3] - '0';
			if (second_dig >= 0 && second_dig <= 9) numer = 10 * first_dig + second_dig;
			else numer = first_dig;
			indeksy_filcy[numer] = i;
		}
		else if (OBJs[i]->name[0] == 'M' && OBJs[i]->name[1] == '_') {				//młoteczek
			first_dig = OBJs[i]->name[2] - '0';
			second_dig = OBJs[i]->name[3] - '0';
			if (second_dig >= 0 && second_dig <= 9) numer = 10 * first_dig + second_dig;
			else numer = first_dig;
			indeksy_mlotkow[numer] = i;
		}
	}
}

bool handleKeyInput(GLFWwindow* window, std::vector<OBJ*> OBJs) {			//przesuwanie na kolejne klawisze w prawo
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (!pressed) {
			pressed = true;
			if(mode < 8) mode++;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {			//przesuwanie na koljne klawisze w lewo
		if (!pressed) {
			pressed = true;
			if (mode > 0) mode--;
		}
	}

	for (int i = 0; i < 12; i++) {	//na linuxie nie działa	ten dzwiek chyba    -- ok 
		if (glfwGetKey(window, playKeySet[i]) == GLFW_PRESS) {
			if (!pressed) {
				pressed = true;
				num = (mode == 0 && i < 3)* (i + 1) +	//pierwsze 3
					(mode == 8 && i == 0) * 88 +		//7 zestawów po 12
					(mode > 0 && mode < 8)* (3 + 12 * (mode - 1) + i + 1); //ostatni
				if (num == 0) continue;
				char path[50] = "~/Desktop/Grafika/src/sounds/";
				//char buff[3];
				//_itoa_s(num, buff, 10);
				std::string s=std::to_string(num);
				//std::to_chars(buff.data(), buff.data() + buff.size(), num);
				const char * buff = s.c_str();

				strcat(path, buff);
				strcat(path, ".wav");
				char command[60]="aplay ";

				strcat(command,path);
				//system(command);
				printf("%s\n",command );
				//PlaySound(path, NULL, SND_NODEFAULT | SND_ASYNC | SND_FILENAME);
				OBJs[indeksy_klawiszy[num]]->animate = true;		//Odpal animacje 
				OBJs[indeksy_mlotkow[num]]->animate = true;
				OBJs[indeksy_filcy[num]]->animate = true;
			}
		}
	}

	zwolnij = true;
	for (int i = 0; i < 12; i++) {
		if (glfwGetKey(window, playKeySet[i]) == GLFW_PRESS) {			//zwolnij = true jak puścisz przycisk
			//num = modeIndex() * 8 + i + 1;
			zwolnij = false;									
			//std::cout << num << std::endl;
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		zwolnij = false;

	if (zwolnij) pressed = false;
	return true;
}

void determine_key_centers(std::vector<OBJ*> &OBJs) {
	int index;							//obliczamy transform point czyli miejsce wzgledem którego bedzie
	glm::vec3 temp;						//obracał sie klawisz
	double minx, miny, minz, maxx, maxy, maxz;
	for (int i = 1; i < 89; i++) {			//89 klawiszy
		index = indeksy_klawiszy[i];				//dla kazdego dlawisza 
		minx = OBJs[index]->vertices[0].x; miny = OBJs[index]->vertices[0].y;
		minz = OBJs[index]->vertices[0].z; maxx = OBJs[index]->vertices[0].x;
		maxy = OBJs[index]->vertices[0].y; maxz = OBJs[index]->vertices[0].z;
		for (int j = 0; j < OBJs[index]->vertices.size(); j++) {
			temp = OBJs[index] -> vertices[j];
			if (temp.x < minx) minx = temp.x;
			else if (temp.x > maxx) maxx = temp.x;
			if (temp.y < miny) miny = temp.y;						//sprawdzamy czy któryś z temp wierzchołków jest 
			else if (temp.y > maxy) maxy = temp.y;					//mniejszy/wiekszy niz dotychczasz najmniejszy/najwiekszy
			if (temp.z < minz) minz = temp.z;						//wierzchołek
			else if (temp.z > maxz) maxz = temp.z;
		}
		OBJs[index]->transform_point = glm::vec3((minx + maxx) / 2, (miny + maxy) / 2, (minz + maxz) / 2) -
			glm::vec3(1.25, 0, 0);	//okreslamy punkt srodkowy wzgledem ktorego bedziemy obracac na podstawie nowych max i min (jesli są nowe) 
	}
}

void determine_hammer_rotation_points(std::vector<OBJ*> &OBJs) {   //okreslamy punkt wzgledem ktorego 
	int index;												//bedzie obracał sie młoteczek
	glm::vec3 min_y_vector;
	for (int i = 1; i < 89; i++){
		index = indeksy_mlotkow[i];
		min_y_vector = OBJs[index]->vertices[0];
		for (int j = 0; j < OBJs[index]->vertices.size(); j++) {			//podobne działanie co do funkcji wyzej
			if (OBJs[index]->vertices[j].y < min_y_vector.y)				//wybieramy wierzchołek 
				min_y_vector = OBJs[index]->vertices[j];
		}
		std::cout << min_y_vector.x << min_y_vector.y << min_y_vector.z << std::endl;
		OBJs[index]->transform_point = glm::vec3(min_y_vector.x, min_y_vector.y, min_y_vector.z);
		OBJs[indeksy_filcy[i]]->transform_point = glm::vec3(min_y_vector.x, min_y_vector.y, min_y_vector.z);
		//para młoteczek-filc ma wspólny punkt obrotu
	}
}