#ifndef RSO_ICE_CLIENT_H
#define RSO_ICE_CLIENT_H

#include <memory>

namespace chat {

class Client {
public:
    Client();
    ~Client();
    
    virtual int main(int argc, char* argv[]);
    
private:
    struct ClientImpl;
    std::unique_ptr<ClientImpl> impl;
};

}

#endif
