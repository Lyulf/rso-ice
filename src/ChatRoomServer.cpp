#include <ChatRoomServer.h>
#include <chat.h>
#include <Ice/Ice.h>
#include <chatRoomFactoryI.h>
#include <ChatUtils.h>
#include <csignal>

using namespace std;

namespace chat {

int ChatRoomServer::run(int argc, char** argv) {
	if(argc < 2) {
		throw "Invalid number of args";
	}
	callbackOnInterrupt();
	auto proxy = communicator()->stringToProxy("Server:default -p 10000");
	mainServerPrx = Ice::checkedCast<chatServerPrx>(proxy); 
	string endPoints = "default -p " + string(argv[1]);
	Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("ChatRoomServer", endPoints);
	chatRoomFactoryPtr factory = make_shared<chatRoomFactoryI>();
	proxy = adapter->addWithUUID(factory);
	adapter->activate();
	factoryPrx = Ice::uncheckedCast<chatRoomFactoryPrx>(proxy);
	mainServerPrx->registerFactory(factoryPrx);
	communicator()->waitForShutdown();
	return 0; 
}

void ChatRoomServer::interruptCallback(int signal) {
	try {
		if(mainServerPrx) {
			if(factoryPrx) {
				mainServerPrx->unregisterFactory(factoryPrx);
			}
		}
	} catch(Ice::ConnectionRefusedException& e) {
		cerr << e.what() << endl;
	}
	destroyOnInterrupt();
	raise(signal);
}

}

int main(int argc, char** argv) {
	chat::ChatRoomServer server;
	return server.main(argc, argv, "config.chat_server");
}