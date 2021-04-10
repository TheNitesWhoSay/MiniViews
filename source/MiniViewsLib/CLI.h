#ifndef CLI_H
#define CLI_H
#include <Windows.h>
#include <stdio.h>

class CLI
{
	public: 
		CLI() : console(NULL)
		{ 
			::AllocConsole();
			::freopen_s(&console, "CONOUT$", "w", stdout);
			::freopen_s(&console, "CONIN$", "r", stdin);
		}

		~CLI()
		{
			::fclose(console);
			::FreeConsole();
		}

	private:
		FILE* console;
};

#endif