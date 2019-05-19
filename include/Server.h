#ifndef RSO_ICE_SERVER_H
#define RSO_ICE_SERVER_H

#include <memory>

namespace chat {

class Server {
public:
    Server();
    ~Server();
    
    virtual int main(int argc, char* argv[]);

private:
    struct ServerImpl;
    std::unique_ptr<ServerImpl> impl;
};

}

#endif
