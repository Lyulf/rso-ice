#include <Server.h>
#include <Ice/Ice.h>
#include <chatServerI.h>
#include <ChatUtils.h>

using namespace std;

namespace chat {

int Server::run(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("Server", "default -p 10000");
    Ice::ObjectPtr object = make_shared<chatServerI>();
    adapter->add(object, Ice::stringToIdentity("Server"));
    adapter->activate();
    communicator()->waitForShutdown();
    return 0;
}

}

int main(int argc, char* argv[]) {
    chat::Server server;
    return server.main(argc, argv, "config.main_server");
}