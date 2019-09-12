// PingingWebsite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//From: https://stackoverflow.com/questions/32205981/reading-json-files-in-c

#include <chrono>
#include <thread>

#include "HandleInputSystem.h"

#define TESTWEB false
#define TESTSYS false
#define TESTECS true
#define PRINTKEYBOARD false
#define PRINTMOUSE false
#define PRINTWEB TRUE


void initInputECS(ecs::EntityComponentSystem& rECS);

int main()
{
	InputSystem inputSys;


	ecs::EntityComponentSystem myECS;

	initInputECS(myECS);

	while (TESTECS)
	{
		myECS.update(0.9f);
	}
	





	if (TESTSYS)
	{
	}
	while (TESTSYS)
	{


		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		if (inputSys.updateKeyboard())
		{
			if (PRINTKEYBOARD)
			{
				std::cout << "UPDATED KEYBOARD" << std::endl;

			}
		}
		if (PRINTKEYBOARD)
		{
			std::cout << "Keys WASD R MBR MBL"
				<< std::endl << "     "
				<< inputSys.wsad->keyU.pressed
				<< inputSys.wsad->keyD.pressed
				<< inputSys.wsad->keyL.pressed
				<< inputSys.wsad->keyR.pressed
				<< " "
				<< inputSys.ressetKey->key.pressed
				<< " "
				<< inputSys.mouseRKey->key.pressed
				<< "   "
				<< inputSys.mouseLKey->key.pressed
				//<< std::endl
				<< std::endl;
		}
		

		if (inputSys.updateMouse())
		{
			if (PRINTMOUSE)
			{
				std::cout << "UPDATED MOUSE"
					<< std::endl
					<< inputSys.mouse->diffLength
					<< std::endl;

				std::cout << "Diff Float2: " << inputSys.mouse->diffFloat2.x
					<< ", " << inputSys.mouse->diffFloat2.y << std::endl;
			}
		}
		
		if (inputSys.updateWeb())
		{
			if (PRINTWEB)
			{
				for (size_t i = 0; i < 4; i++)
				{
					std::cout
						<< "P" << i
						<< std::endl
						<< "	"
						<< inputSys.playerControll[i]->keyU.pressed
						<< "	"
						<< inputSys.playerControll[i]->keyD.pressed
						<< std::endl
						<< "	"
						<< inputSys.playerControll[i]->keyL.pressed
						<< "	"
						<< inputSys.playerControll[i]->keyR.pressed
						<< "	| Tile: " 
						<< inputSys.players[i]->currButton0 
						<< ", " 
						<< inputSys.players[i]->currButton1
						<< std::endl
						<< std::endl;
				}
				
				std::cout
					<< "--------------"
					<< std::endl
					<< std::endl;
			}
		}

	}
}


void initInputECS(ecs::EntityComponentSystem& rECS)
{
	// SYSTEMS
	rECS.createSystem<ecs::systems::HandleInputSystem>(1);// parameter är layer

	rECS.createSystem<ecs::systems::HandleKeyboardSystem>(2);
	rECS.createSystem<ecs::systems::HandleMouseSystem>(2);
	rECS.createSystem<ecs::systems::HandleWebSystem>(2);

	// COMPONENTS

	ecs::components::InputBackendComp *tempBackend = new ecs::components::InputBackendComp();
	tempBackend->backend = new InputSystem();

	//rECS.createComponent(ecs::components::InputBackendComp::typeID, );

	// ENTITIES
	rECS.createEntity(*tempBackend);
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
