#include <ChatRoomServer.h>
#include <chat.h>
#include <Ice/Ice.h>
#include <chatRoomFactoryI.h>

using namespace std;

namespace chat {

int ChatRoomServer::run(int argc, char** argv) {
	if(argc < 2) {
		throw "Invalid number of args";
	}
	auto proxy = communicator()->stringToProxy("Server:default -p 10000");
	chatServerPrxPtr mainServer = Ice::checkedCast<chatServerPrx>(proxy); 
	string endPoints = "default -p " + string(argv[1]);
	Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("ChatRoomServer", endPoints);
	chatRoomFactoryPtr factory = make_shared<chatRoomFactoryI>(mainServer, adapter);
	proxy = adapter->addWithUUID(factory);
	adapter->activate();
	communicator()->waitForShutdown();
	return 0; 
}

}

int main(int argc, char** argv) {
	chat::ChatRoomServer server;
	return server.main(argc, argv);
}