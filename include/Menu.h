#ifndef LYULF_UTILS_MENU_H
#define LYULF_UTILS_MENU_H

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>

#define UNUSED(x) (void)(x)

namespace utils {
class CommandException : public std::runtime_error {
public:
    CommandException(const std::string& msg = "");
    virtual ~CommandException();
    virtual void raise() const;
};

class UnknownCommandException : public CommandException {
public:
    UnknownCommandException(const std::string& msg = "");
    virtual void raise() const override;
};

class TooManyMatchingCommandsException : public CommandException {
public:
    TooManyMatchingCommandsException(const std::vector<std::string>& commands, const std::string& msg = "");
    virtual void raise() const override;
    std::vector<std::string> getCommands() const;
private:
    std::vector<std::string> commands;
};

class MenuException : public std::runtime_error {
public:
    MenuException(const std::string& msg);
    virtual ~MenuException();
    virtual void raise() const;
};

class CommandAlreadyExistsException : public MenuException {
public:
    CommandAlreadyExistsException(const std::string& msg);
    virtual void raise() const override;
};

class Command {
    Command();
public:
    Command(
        const std::string& name, 
        const std::function<void(const std::string& args)>& func,
        const std::string& desc = ""
        );
    Command(const Command& other);
    Command(Command&& other);

    Command& operator=(const Command& other);
    bool operator==(const std::string& command) const;

    void invoke(const std::string& args) const;
    std::string getName() const;
    std::string getDesc() const;
    std::set<std::string> getAliases() const;
    void setFunc(const std::function<void(const std::string& args)>& func);
    void setDesc(const std::string& desc);
    void addAlias(const std::string& alias);
    void removeAlias(const std::string& alias);

private:
    friend void swap(Command& left, Command& right);

    std::string name;
    std::set<std::string> aliases;
    std::function<void(const std::string&)> func;
    std::string desc;
};

class Menu {
public:
    Menu();
    Menu(const Menu& other);
    Menu(Menu&& other);
    Menu& operator=(const Menu& other);
    void addCommand(const Command& command);
    void invokeCommand(const std::string& command, const std::string& args) const;
    void removeCommand(const std::string& command);
    std::vector<std::string> getCommands() const;
    Command getCommand(const std::string& command) const;

private:
    friend void swap(Menu& left, Menu& right);

    std::vector<Command> commands;
    // std::shared_ptr<T> data;
};

}


#endif