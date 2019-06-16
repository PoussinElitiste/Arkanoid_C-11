// Arkanoid_C++11.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "pch.h"

#include "Arkanoid_ECS.h"
//#include "Arkanoid_Classic.h"

// Main
//------
using namespace Arkanoid;

int main()
{
	Game_v2{}.run();
	//Game{}.run();
	return 0;
}
