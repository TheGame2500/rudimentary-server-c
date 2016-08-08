#include <stdio.h>
#include "helpers.h"
#include "eventLoop.h"

//accepted args : int number specifying the port on which the server should listen for requests
int main(int argc, char* argv[]) {
	
	if(argc == 1) {
		printf("No port specified, please input an int number so I can listen to a port. Thanks, buddy!");
		return 1;
	}
	
	int port = parseCharToInt(argv[1]);
	listenToPort(port);

	return 0;
}
