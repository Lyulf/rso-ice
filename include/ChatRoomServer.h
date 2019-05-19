#ifndef RSO_ICE_CHATROOM_SERVER_H
#define RSO_ICE_CHATROOM_SERVER_H

#include <memory>

namespace chat {

class ChatRoomServer : public Ice::Application {
public:
	ChatRoomServer();
	~ChatRoomServer();
	
	virtual int run(int argc, char* argv[]);
};

}

#endif
