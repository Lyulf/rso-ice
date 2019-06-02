#include <Menu.h>
#include <MenuUtils.h>
#include <algorithm>

using namespace std;

namespace utils {

CommandException::CommandException(const string& msg)
    : runtime_error(msg) { }

CommandException::~CommandException() = default;

void CommandException::raise() const {
    throw *this;
}

UnknownCommandException::UnknownCommandException(const string& command, const string& msg)
    : CommandException(msg), command(command) { }

void UnknownCommandException::raise() const {
    throw *this;
}

string UnknownCommandException::getCommand() {
    return command;
}

TooManyMatchingCommandsException::TooManyMatchingCommandsException(
    const vector<string>& commands,
    const string& msg
) : CommandException(msg), commands(commands) { }

void TooManyMatchingCommandsException::raise() const {
    throw *this;
}

vector<string> TooManyMatchingCommandsException::getCommands() const {
    return commands;
}

InvalidNumberOfArgsException::InvalidNumberOfArgsException(
    const size_t numberOfArgs,
    const size_t expectedNumberOfArgs,
    const string& msg)
    : CommandException(msg), numberOfArgs(numberOfArgs), expectedNumberOfArgs(expectedNumberOfArgs) {}

void InvalidNumberOfArgsException::raise() const {
    throw *this;
}

size_t InvalidNumberOfArgsException::getNumberOfArgs() const {
    return numberOfArgs;
}

size_t InvalidNumberOfArgsException::getExpectedNumberOfArgs() const {
    return expectedNumberOfArgs;
}

MenuException::MenuException(const string& msg) 
    : runtime_error(msg) { }

MenuException::~MenuException() = default;

void MenuException::raise() const {
    throw *this;
}

CommandAlreadyExistsException::CommandAlreadyExistsException(
    const string& msg
) : MenuException(msg) { }

void CommandAlreadyExistsException::raise() const {
    throw *this;
}

Command::Command() = default;

Command::Command(
    const string& name, 
    const function<void(const vector<string>& args)>& func,
    const string& desc
    ) : name(name), aliases(), func(func), desc(desc) { }

Command::Command(const Command& other)
    : name(other.name), aliases(other.aliases), func(other.func), desc(other.desc) { }

Command::Command(Command&& other) noexcept
    : Command() {
    swap(*this, other);
}

Command& Command::operator=(const Command& other) {
    Command temp(other);
    swap(*this, temp);
    return *this;
}

bool Command::operator==(const string& command) const {
    if(command == name.substr(0, command.size())) {
        return true;
    }
    return any_of(aliases.begin(), aliases.end(), [&command](auto alias) { return alias.substr(0, command.size()) == command; });
}

void Command::invoke(const vector<string>& args) const {
    func(args);
}

string Command::getName() const {
    return name;
}

string Command::getDesc() const {
    return desc;
}

set<string> Command::getAliases() const {
    return aliases;
}

void Command::setFunc(const function<void(const vector<string>& args)>& func) {
    this->func = func;
}

void Command::setDesc(const string& desc) {
    this->desc = desc;
}

void Command::addAlias(const string& alias) {
    aliases.insert(alias);
}

void Command::removeAlias(const string& alias) {
    aliases.erase(alias);
}

void swap(Command& left, Command& right) {
    swap(left.name, right.name);
    swap(left.aliases, right.aliases);
    swap(left.func, right.func);
    swap(left.desc, right.desc);
}

Menu::Menu()
    :commands() { }

Menu::Menu(const Menu& other)
    : commands(other.commands) { }
    
Menu::Menu(Menu&& other) noexcept
    : Menu() {
    swap(*this, other);
}

Menu& Menu::operator=(const Menu& other) {
    Menu temp(other);
    swap(*this, temp);
    return *this;
}

Menu& Menu::operator=(Menu&& other) noexcept {
    swap(*this, other);
    return *this;
}

void Menu::addCommand(const Command& command) {
// Probably needs cleanup of tabs
    set<string> existingAliases;
    for(auto command : commands) {
        existingAliases.insert(command.getName());
        auto aliases = command.getAliases();
        for(auto alias : aliases) {
            existingAliases.insert(alias);
        }
    }

    if(existingAliases.find(command.getName()) != existingAliases.end()) {
        CommandAlreadyExistsException().raise(); 
    }

    auto aliases = command.getAliases();
    if(any_of(aliases.begin(), aliases.end(), [existingAliases](auto alias) {
        return existingAliases.find(alias) != existingAliases.end();
        }
    )) {
        CommandAlreadyExistsException().raise(); 
    }
    commands.push_back(command);
}

void Menu::invokeCommand(const string& command, const string& args) const {
    auto cmd = getCommand(command);
    auto tokens = splitString(args, ' '); 
    cmd.invoke(tokens);
}

void Menu::removeCommand(const string& command) {
    commands.erase(find(commands.begin(), commands.end(), command));
}

vector<string> Menu::getCommands() const {
    vector<string> result;
    transform(commands.begin(), commands.end(), back_inserter(result), [](auto command) { return command.getName(); });
    return result;
}

Command Menu::getCommand(const string& command) const {
    vector<Command> matchingCommands;
    copy_if(commands.begin(), commands.end(), back_inserter(matchingCommands), [command](auto cmd) { return cmd == command; });
    auto n = matchingCommands.size();
    if(n == 0) {
        UnknownCommandException(command).raise();
    } else if(n > 1) {
        vector<string> commandNames;
        transform(matchingCommands.begin(), matchingCommands.end(), back_inserter(commandNames), [](auto element) { return element.getName(); });
        TooManyMatchingCommandsException(commandNames).raise();
    }
    return matchingCommands.front();
}

void swap(Menu& left, Menu& right) {
    swap(left.commands, right.commands);
}

}