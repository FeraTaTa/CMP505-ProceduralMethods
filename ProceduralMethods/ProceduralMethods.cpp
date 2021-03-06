// ProceduralMethods.cpp

#include "stdafx.h"
#include "SystemClass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR lpCmdLine,
					  _In_ int nCmdShow)
{
	// Create the system object.
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return -1;
	}

	// Initialize and run the system object.
	if (System->Initialize())
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}