#include <ChatRoomServer.h>
#include <chat.h>
#include <Ice/Ice.h>

namespace chat {

ChatRoomServer::ChatRoomServer() = default;
ChatRoomServer::~ChatRoomServer() = default;

int ChatRoomServer::run(int argc, char** argv) {
	(void) argc;
	(void) argv;
	return 0; 
}

}
