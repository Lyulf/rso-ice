#include <Client.h>
#include <UserI.h>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace utils;
using namespace chat;


vector<string> splitString(const string& str, const char delimiter) {
    vector<string> result;
    stringstream ss(str);
    string token;
    
    while(getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

int Client::run(int argc, char* argv[]) {
    if(argc < 2) {
        throw "Invalid number of arguments";
    }
    string endPoint = "Server:default -p 10000";
    connectToChatServer(endPoint);
    auto adapter = createAdapter(argv[1]);
    if(!adapter) {
        cout << "Failed to create adapter\n";
        return -1;
    }
    createUser(adapter);
    adapter->activate();
    auto lobbyMenu = createLobbyMenu();
    auto roomMenu = createRoomMenu();
    try {
        lobbyMenu.invokeCommand("c", "room " + string(argv[1]));
    } catch(NameAlreadyExists& e) {
        lobbyMenu.invokeCommand("j", "room " + string(argv[1]));
    }
    leaveRoom();
    cout << "Connected to server\n";
    while(true) {
        string line;
        // TODO: User interface
        return 0;
    }
    communicator()->waitForShutdown();
    return 0;
}

//------------------------------
// Init
//------------------------------

void Client::connectToChatServer(const string& serverProxy) {
    auto proxy = communicator()->stringToProxy(serverProxy);
    server = Ice::checkedCast<chatServerPrx>(proxy); 
}

Ice::ObjectAdapterPtr Client::createAdapter(const string& port) {
    try {
        return communicator()->createObjectAdapterWithEndpoints("Client", "default -p " + port);
    } catch(exception& e) {
        return nullptr;
    }
}

void Client::createUser(Ice::ObjectAdapterPtr adapter) {
    user = make_shared<UserI>();
    auto proxy = adapter->addWithUUID(user);
    userPrx = Ice::uncheckedCast<UserPrx>(proxy);
}

Menu&& Client::createLobbyMenu() {
    Menu menu;

    return move(menu);
}

Menu&& Client::createRoomMenu() {
    Menu menu;

    return move(menu);
}

//------------------------------
//  Lobby
//------------------------------

RoomList Client::listRooms() {
    return server->getRooms();
}

bool Client::createRoom(const std::string& username, const std::string& roomName) {
    try {
        room = server->newChatRoom(roomName);
    } catch(NameAlreadyExists& e) {
        return false;
    }
    name = username;
    room->join(name, userPrx);
    return true;
}

bool Client::joinRoom(const std::string& username, const std::string& roomName)
{
    try {
        room = server->getRoom(roomName);
        name = username;
        room->join(name, userPrx);
        return true;
    } catch(NoSuchRoom& e) {
        return false;
    } catch(NickNotAvailable& e) {
        return false;
    }
}

//------------------------------
//  Room
//------------------------------

userList Client::listUsers() {
    return room->listUsers();
}

void Client::sendMsg(const std::string& msg) {
    room->postMessage(msg, name);
}

bool Client::sendPrivateMsg(const std::string& toWho, const std::string& msg) {
    try {
        auto user = room->getUser(toWho);
        user->sendMessage(msg, name);
        return true;
    } catch(NoSuchUser& e) {
        return false;
    }
}

void Client::leaveRoom() {
    room->Leave(name);
}

//------------------------------
//  Main
//------------------------------
int main(int argc, char* argv[]) {
    auto client = make_shared<Client>();
    return client->main(argc, argv);
}