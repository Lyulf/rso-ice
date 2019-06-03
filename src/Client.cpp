#include <Client.h>
#include <ChatUtils.h>
#include <UserI.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <csignal>

using namespace std;
using namespace utils;
using namespace chat;


int Client::run(int argc, char* argv[]) {
    try {
        if(argc < 2) {
            throw "Invalid number of arguments\n";
        }
        callbackOnInterrupt();
        string endPoint = "Server:default -p 10000";
        connectToChatServer(endPoint);
        auto adapter = createAdapter(argv[1]);
        if(!adapter) {
            throw "Failed to create adapter\n";
        }
        createUser(adapter);
        adapter->activate();
        auto lobbyMenu = createLobbyMenu();
        auto roomMenu = createRoomMenu();

        cout << "Connected to server\n\n";
        cout << "Write help to get list of commands\n";
        cout << "Write help <command> to get command description\n";
        while(true) {
            string line;
            try {
                if(room) {
                    try {
                        getline(cin, line);
                        if(line.front() != '/') {
                            sendMsg(line);
                            continue;
                        }
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
                    } catch(Ice::ConnectionRefusedException& e) {
                        cerr << "Lost connection to the chat room\n";
                        room.reset();
                    }
                } else {
                    try {
                        cout << endl << ">>";
                        getline(cin, line);
                        cout << endl;
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
                    } catch( Ice::ConnectionRefusedException& e) {
                        cerr << "Lost connection to the server\n"
                                "Exiting\n";
                        exit(EXIT_FAILURE);
                    }
                }
            } catch(UnknownCommandException& e) {
                cout << "Unknown command \"" << e.getCommand() << "\"\n";
            } catch(TooManyMatchingCommandsException& e) {
                cout << "Too many matching commands\n"
                        "\n"
                        "Candidates are:\n";
                for(auto command : e.getCommands()) {
                    cout << command << ", ";
                }
                removeNCharacters(cout, 2) << endl;
            } catch(InvalidNumberOfArgsException& e) {
                ostringstream ss;
                cout << "Invalid number of arguments\n"
                        "\n";
                cout << "got: " << e.getNumberOfArgs() << endl;
                cout << "expected: " << e.getExpectedNumberOfArgs() << endl;
            }
        }
    } catch(exception& e) {
        cerr << e.what() << endl;
    }
    return 0;
}

void Client::interruptCallback(int signal) {
    exit(signal);
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
    addGlobalCommands(menu);

    Command listRooms( "list",
        [this](auto args) {
            UNUSED(args);
            auto rooms = this->listRooms();
            cout << "Current rooms:\n";
            for_each(rooms.begin(), rooms.end(), [](auto room) { cout << room.name << ": " << room.activeUsers << endl; });
        },
        "list: Lists current rooms\n");
    listRooms.addAlias("rooms");
    listRooms.addAlias("ls");
    menu.addCommand(listRooms);

    Command createRoom(
        "create",
        [this](auto args) {
            if(auto size = args.size() != 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto roomName = args[0];
            auto nick = args[1];
            try {
                this->createRoom(nick, roomName);
                if(room) {
                    cout << "Created room " << roomName << endl;
                    cout << endl;
                    cout << "To write a command start the line with '/'\n";
                } else {
                    cout << "Failed to create the room\n";
                }
            } catch(NameAlreadyExists& e) {
                cout << "Room " << roomName << " already exists\n";
            }
        },
        "create <room_name> <nick>: Creates new room if it doesnt exist as <nick>\n"
    );
    menu.addCommand(createRoom);

    Command joinRoom(
        "join",
        [this](auto args) {
            if(auto size = args.size() != 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto roomName = args[0];
            auto nick = args[1];
            try {
                this->joinRoom(nick, roomName);
                cout << "Joined room " << roomName << endl;
                cout << endl;
                cout << "To write a command start the line with '/'\n";
            } catch(NoSuchRoom& e) {
                cout << "Room " << roomName << " doesn't exist\n";
            } catch(NickNotAvailable& e) {
                cout << "Nick " << nick << " is not available\n";
            }
        },
        "join <room_name> <nick>: Joins room if it exists as <nick>\n"
    );
    menu.addCommand(joinRoom);

    return menu;
}

Menu Client::createRoomMenu() {
    Menu menu;
    addGlobalCommands(menu);

    Command listUsers(
        "list",
        [this](auto args) {
            UNUSED(args);
            auto users = this->listUsers();
            cout << "Current users:\n";
            for(auto user : users) {
                cout << user << endl;
            }
        },
        "list: Lists current users\n"
    );
    listUsers.addAlias("users");
    listUsers.addAlias("ls");
    menu.addCommand(listUsers);

    Command sendPrivateMsg (
        "private",
        [this](auto args) {
            if(auto size = args.size() < 2) {
                InvalidNumberOfArgsException(size, 2).raise();
            }
            auto toWho = args[0];
            stringstream ss;
            for_each(args.begin() + 1, args.end(),
                [&ss](auto arg) { ss << arg << " "; });
            removeNCharacters(ss, 1) << '\n';
            try {
                this->sendPrivateMsg(toWho, ss.str());
                cout << "Message sent\n";
            } catch(NoSuchUser& e) {
                cout << "User " << toWho << " doesn't exist\n";
            }
        },
        "private <nick>: Sends private message to <nick>\n"
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
        "leave: Leaves current room\n"
    );
    menu.addCommand(leave);

    return menu;
}

void Client::addGlobalCommands(Menu& menu) {
   Command help (
        "help",
        [&menu](auto args) {
            if(args.empty()) {
                auto commandList = menu.getCommands();
                if(!commandList.empty()) {
                    cout << "Commands:\n";
                    for_each(commandList.begin(), commandList.end(), [](auto command) { cout << command << ", "; });
                    removeNCharacters(cout, 2) << endl;
                } else {
                    cout << "No commands available\n";
                }
            } else if(args.size() == 1) {
                auto command = menu.getCommand(args.front());
                cout << "Command: " << command.getName() << endl;
                auto description = command.getDesc();
                if(!description.empty()) {
                    cout << endl;
                    cout << description;
                }
                auto aliases = command.getAliases();
                if(!aliases.empty()) {
                    cout << "\nAliases:\n";
                    for(auto alias : aliases) {
                        cout << alias << ", ";
                    }
                    removeNCharacters(cout, 2) << endl;
                }
            } else {
                InvalidNumberOfArgsException(args.size(), 1).raise();
            }
        },
        "help: Displays list of commands\n" 
        "help <command>: Displays info about command\n"
    );
    menu.addCommand(help);

    Command exit(
        "exit",
        [this] (auto args) {
            UNUSED(args);
            cout << "You are about to exit program\n";
            cout << "Are you sure:[y/n]\n";
            do {
                string input;
                getline(cin, input);
                if(input == "y") {
                    this->exit(EXIT_SUCCESS);
                } else if(input == "n") {
                    break;
                } else {
                    cout << "Invalid input\n";
                    cout << "Please write 'y' or 'n'\n";
                }
            } while(true);
        },
        "exit: Closes the application\n"
    );
    menu.addCommand(exit);
}

//------------------------------
//  Global
//------------------------------

void Client::exit(int status) {
    leaveRoom();
    auto ic = communicator();
    if(ic) {
        ic->destroy();
    }
    std::exit(status);
}

//------------------------------
//  Lobby
//------------------------------

RoomList Client::listRooms() {
    return server->getRooms();
}

void Client::createRoom(const std::string& username, const std::string& roomName) {
    room = server->newChatRoom(roomName);
    if(room) {
        name = username;
        room->join(name, userPrx);
    }
}

void Client::joinRoom(const std::string& username, const std::string& roomName)
{
    room = server->getRoom(roomName);
    if(room) {
        name = username;
        room->join(name, userPrx);
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

void Client::sendPrivateMsg(const std::string& toWho, const std::string& msg) {
    auto user = room->getUser(toWho);
    user->sendPrivateMessage(msg, name);
}

void Client::leaveRoom() {
    if(room) {
        room->Leave(name);
    }
    room.reset();
}

//------------------------------
//  Main
//------------------------------
int main(int argc, char* argv[]) {
    auto client = make_shared<Client>();
    return client->main(argc, argv);
}