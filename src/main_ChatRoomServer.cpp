#include <ChatRoomServer.h>
#include <Ice/Ice.h>

using chat::ChatRoomServer;

int main(int argc, char* argv[]) {
	ChatRoomServer server;
	return server.main(argc, argv);
}
