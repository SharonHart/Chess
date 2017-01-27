#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // exit

#include "ChessCommonDefs.h"
#include "InterfaceDefinitions.h"
#include "ChessFlowController.h"

int main(int argc, const char* argv[])
{
	const char* interfaceModeString = NULL;
	INTERFACE_MODE interfaceMode = INTERFACE_MODE_CONSOLE; 

	if (argc > 1)
	{
		interfaceModeString = argv[1];
		if (0 == strcmp(interfaceModeString, CLI_ARG_STRING_INTERFACE_MODE_GUI))
		{
			interfaceMode = INTERFACE_MODE_GRAPHICAL;
		}
		else if (0 == strcmp(interfaceModeString, CLI_ARG_STRING_INTERFACE_MODE_CONSOLE))
		{
			interfaceMode = INTERFACE_MODE_CONSOLE;
		}

		else
		{
			interfaceMode = INTERFACE_MODE_DEFAULT;
		}
	}
	else
	{
		interfaceMode = INTERFACE_MODE_DEFAULT;
	}

	ChessControllerInit(interfaceMode);
	ChessControllerRun();

	printf("press any key to exit\n");
	getchar();
	ChessControllerTerminate();
	return 0;
}
