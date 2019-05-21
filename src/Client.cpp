#include <Client.h>
#include <Ice/Ice.h>
#include <UserI.h>
#include <chatServerI.h>

using namespace std;

namespace chat {

int Client::run(int argc, char* argv[]) {
    if(argc < 2) {
        throw "Invalid number of arguments";
    }
    string endPoints = "default -p " + string(argv[1]);

    auto proxy = communicator()->stringToProxy("Server:default -p 10000");
    chatServerPrxPtr mainServer = Ice::checkedCast<chatServerPrx>(proxy);
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("Client", endPoints);
    UserPtr user = make_shared<UserI>();
    proxy = adapter->addWithUUID(user);
    adapter->activate();
    UserPrxPtr userPrx = Ice::uncheckedCast<UserPrx>(proxy);
    chatRoomPrxPtr room;
    try {
        room = mainServer->getRoom("Room");
    } catch(const NoSuchRoom& e) {
        room = mainServer->newChatRoom("room");
    }
    room->join(argv[1], userPrx);
    room->postMessage("Message", argv[1]);
    communicator()->waitForShutdown();
    return 0;
}

}

int main(int argc, char* argv[]) {
    chat::Client client;
    return client.main(argc, argv);
}