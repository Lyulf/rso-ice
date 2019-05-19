#include <Server.h>
#include <Ice/Ice.h>

namespace chat {

struct Server::ServerImpl : public virtual ::Ice::Application {
    virtual int run(int argc, char* argv[]) {
        (void) argc;
        (void) argv;
        return 0;
    }
};

Server::Server()
    : impl(std::make_unique<ServerImpl>()) { }
    
Server::~Server() = default;

int Server::main(int argc, char* argv[]) {
    return impl->main(argc, argv);
}

}
