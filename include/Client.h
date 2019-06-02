#ifndef RSO_ICE_CLIENT_H
#define RSO_ICE_CLIENT_H

#include <Ice/Ice.h>
#include <chat.h>
#include <Menu.h>

namespace chat {

class Client : public Ice::Application {
public:
    virtual int run(int argc, char* argv[]) override;

private:
    //------------------------------
    // Init
    //------------------------------
    void connectToChatServer(const std::string& endPoint);
    Ice::ObjectAdapterPtr createAdapter(const std::string& port);
    void createUser(Ice::ObjectAdapterPtr adapter);
    utils::Menu createLobbyMenu();
    utils::Menu createRoomMenu();
    void addGlobalCommands(utils::Menu& menu);
    //------------------------------
    //  Global
    //------------------------------
    void exit();
    //------------------------------
    //  Lobby
    //------------------------------
    RoomList listRooms();
    void createRoom(const std::string& username, const std::string& roomName);
    void joinRoom(const std::string& username, const std::string& roomName);
    //------------------------------
    //  Room
    //------------------------------
    userList listUsers();
    void sendMsg(const std::string& msg);
    void sendPrivateMsg(const std::string& toWho, const std::string& msg);
    void leaveRoom();
    //------------------------------
    //  Variables
    //------------------------------
    std::string name;
    chatServerPrxPtr server;
    chatRoomPrxPtr room; 
    UserPtr user;
    UserPrxPtr userPrx;
};

}

#endif
