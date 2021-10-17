#ifndef CLI_H
#define CLI_H
#include <Windows.h>
#include <iostream>
#include <stdio.h>

class CLI
{
	public: 
		CLI() : consoleIn(NULL), consoleOut(NULL)
		{ 
			if ( ::GetConsoleWindow() == NULL )
			{
				if ( ::AllocConsole() != 0 )
				{
					if ( ::freopen_s(&consoleIn, "CONIN$", "r", stdin) == 0 )
						std::cin.clear();
					if ( ::freopen_s(&consoleOut, "CONOUT$", "w", stdout) == 0 )
						std::cout.clear();
				}
			}
		}

		~CLI()
		{
			if ( consoleIn != NULL )
				::fclose(consoleIn);

			if ( consoleOut != NULL )
				::fclose(consoleOut);

			::FreeConsole();
		}

	private:
		FILE* consoleIn;
		FILE* consoleOut;
};

#endif