// Arkanoid_C++11.cpp : Ce fichier contient la fonction 'main'. 
// program execution start here
//
#include "pch.h"

#include "ArkanoidConfig.h"
#include "Arkanoid_ECS.h"
//#include "Arkanoid_Classic.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

// Main
//------
using namespace Arkanoid;

int main(int argc, char *argv[])
{
	if (argc < 2) 
	{
		// report version
		std::cout << argv[0] << " Version " << Arkanoid_VERSION_MAJOR << "."
              << Arkanoid_VERSION_MINOR << std::endl;
		std::cout << "Usage: " << argv[0] << " number" << std::endl;

		Game_v2{}.run();
		//Game{}.run();

		return 1;
	}

	return 0;
}