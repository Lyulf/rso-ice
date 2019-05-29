#include <Client.h>
#include <UserI.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ChatUtils.h>

using namespace std;
using namespace utils;
using namespace chat;


int Client::run(int argc, char* argv[]) {
    try {
        if(argc < 2) {
            throw "Invalid number of arguments";
        }
        string endPoint = "Server:default -p 10000";
        connectToChatServer(endPoint);
        auto adapter = createAdapter(argv[1]);
        if(!adapter) {
            throw "Failed to create adapter\n";
        }
        createUser(adapter);
        adapter->activate();
        cout << "Creating lobby menu\n";
        auto lobbyMenu = createLobbyMenu();
        cout << "Creating room menu\n";
        auto roomMenu = createRoomMenu();

        cout << "Connected to server\n";
        while(true) {
            string line;
            getline(cin, line);
            try {
                if(room) {
                    if(line.front() == '/') {
                        line.erase(line.begin());
                        auto tokens = splitString(line, ' ');
                        if(tokens.empty()) {
                            continue;
                        }
                        auto command = tokens[0];
                        stringstream ss;
                        for_each(tokens.begin() + 1, tokens.end(), [&ss](auto token) { ss << token << " "; });
                        auto args = ss.str();
                        roomMenu.invokeCommand(command, args);
                    } else {
                        sendMsg(line);
                    }
                } else {
                    if(line.front() == '/') {
                        line.erase(line.begin());
                    }
                    auto tokens = splitString(line, ' ');
                    if(tokens.empty()) {
                        continue;
                    }
                    auto command = tokens[0];
                    stringstream ss;
                    for_each(tokens.begin() + 1, tokens.end(), [&ss](auto token) { ss << token << " "; });
                    auto args = ss.str();
                    lobbyMenu.invokeCommand(command, args);
                }
            } catch(UnknownCommandException& e) {
                cout << "Unknown command \"" << e.getCommand() << "\"\n";
            } catch(TooManyMatchingCommandsException& e) {
                cout << "Too many matching commands\n";
                cout << "Candidates are:\n";
                for(auto command : e.getCommands()) {
                    cout << command << ", ";
                }
                removeNCharacters(cout, 2) << "\n";
            } catch(InvalidNumberOfArgsException& e) {
                cout << "Invalid number of arguments\n";
                cout << "got: " << e.getNumberOfArgs() << endl;
                cout << "expected: " << e.getExpectedNumberOfArgs() << endl;
            }
        }
    } catch(exception& e) {
        cout << e.what() << endl;
    }
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

Menu Client::createLobbyMenu() {
    Menu menu;
    Command listRooms( "list",
        [this](auto args) {
            UNUSED(args);
            auto rooms = this->listRooms();
            for_each(rooms.begin(), rooms.end(), [](auto room) { cout << room.name << ": " << room.activeUsers << endl; });
        },
        "list: Lists current rooms\n");
    listRooms.addAlias("rooms");
    menu.addCommand(listRooms);

    Command createRoom(
        "create",
        [this](auto args) {
            if(auto size = args.size() != 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto name = args[0];
            auto nick = args[1];
            this->createRoom(nick, name);
        },
        "create <room_name> <nick>: Creates new room if it doesnt exist as <nick>"
    );
    menu.addCommand(createRoom);

    Command joinRoom(
        "join",
        [this](auto args) {
            if(auto size = args.size() != 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto name = args[0];
            auto nick = args[1];
            this->joinRoom(nick, name);
        },
        "join <room_name> <nick>: Joins room if it exists as <nick>"
    );
    menu.addCommand(joinRoom);

    return menu;
}

Menu Client::createRoomMenu() {
    Menu menu;
    Command listUsers(
        "list",
        [this](auto args) {
            UNUSED(args);
            auto users = this->listUsers();
            for(auto user : users) {
                cout << user << endl;
            }
        },
        "list: Lists current users"
    );
    listUsers.addAlias("users");
    menu.addCommand(listUsers);

    Command sendPrivateMsg (
        "private",
        [this](auto args) {
            if(auto size = args.size() < 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto toWho = args[0];
            stringstream ss;
            for(auto arg : args) {
                ss << arg << " ";
            }
            removeNCharacters(ss, 1) << '\n';
            this->sendPrivateMsg(toWho, ss.str());
        },
        "private <nick>: Sends private message to <nick>"
    );
    sendPrivateMsg.addAlias("whisper");
    menu.addCommand(sendPrivateMsg);

    Command leave(
        "leave",
        [this](auto args) {
            UNUSED(args);
            this->leaveRoom();
            cout << "Left the room" << endl;
        },
        "leave: Leaves current room"
    );
    menu.addCommand(leave);

    return menu;
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
        user->sendPrivateMessage(msg, name);
        return true;
    } catch(NoSuchUser& e) {
        return false;
    }
}

void Client::leaveRoom() {
    room->Leave(name);
    room = nullptr;
}

//------------------------------
//  Main
//------------------------------
int main(int argc, char* argv[]) {
    auto client = make_shared<Client>();
    return client->main(argc, argv);
}