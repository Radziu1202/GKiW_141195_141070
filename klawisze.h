#pragma once

#include <iostream>
//#include <tchar.h>
//#include <conio.h>
#include <thread>
#include "OBJ.h"

bool handleKeyInput(GLFWwindow* window, std::vector<OBJ*> OBJs);
void determine_indeces(std::vector<OBJ*> OBJs);
void determine_key_centers(std::vector<OBJ*> &OBJs);
void determine_hammer_rotation_points(std::vector<OBJ*> &OBJs);