#include <Menu.h>
#include <algorithm>

using namespace std;

namespace utils {

CommandException::CommandException(const string& msg)
    : runtime_error(msg) { }

CommandException::~CommandException() = default;

void CommandException::raise() const {
    throw *this;
}

UnknownCommandException::UnknownCommandException(const string& msg)
    : CommandException(msg) { }

void UnknownCommandException::raise() const {
    throw *this;
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
    const std::string& name, 
    const std::function<void(const std::string& args)>& func,
    const std::string& desc
    ) : name(name), aliases(), func(func), desc(desc) { }

Command::Command(const Command& other)
    : name(other.name), aliases(other.aliases), func(other.func), desc(other.desc) { }

Command::Command(Command&& other)
    : Command() {
    swap(*this, other);
}

Command& Command::operator=(const Command& other) {
    Command temp(other);
    swap(*this, temp);
    return *this;
}

bool Command::operator==(const std::string& command) const {
    if(command == name.substr(0, command.size())) {
        return true;
    }
    return std::any_of(aliases.begin(), aliases.end(), [&command](auto alias) { return alias.substr(0, command.size()) == command; });
}

void Command::invoke(const std::string& args) const {
    func(args);
}

std::string Command::getName() const {
    return name;
}

std::string Command::getDesc() const {
    return desc;
}

std::set<std::string> Command::getAliases() const {
    return aliases;
}

void Command::setFunc(const std::function<void(const std::string& args)>& func) {
    this->func = func;
}

void Command::setDesc(const std::string& desc) {
    this->desc = desc;
}

void Command::addAlias(const std::string& alias) {
    aliases.insert(alias);
}

void Command::removeAlias(const std::string& alias) {
    aliases.erase(alias);
}

void swap(Command& left, Command& right) {
    std::swap(left.name, right.name);
    std::swap(left.aliases, right.aliases);
    std::swap(left.func, right.func);
    std::swap(left.desc, right.desc);
}

Menu::Menu() {
   Command help (
        "help",
        [this](const std::string& args) {
            auto clearNCharacters = [](std::size_t n) {
                for(std::size_t i = 0; i < n; i++) {
                    std::cout << "\b \b";
                }
            };
            if(args.empty()) {
                auto commandList = getCommands();
                std::cout << "Commands:\n";
                std::for_each(commandList.begin(), commandList.end(), [](auto command) { std::cout << command << ", "; });
                clearNCharacters(2);
                std::cout << std::endl;
            } else {
                try {
                    auto command = getCommand(args);
                    std::cout << std::endl;
                    std::cout << command.getDesc() << std::endl;
                } catch(UnknownCommandException& e) {
                    std::cout << "Error: Command \"" << args << "\" is unknown\n";
                } catch(TooManyMatchingCommandsException& e) {
                    std::cout << "Error: Too many matching commands\n";
                    std::cout << "Match commands: \n";
                    auto commandList = e.getCommands();
                    std::for_each(commandList.begin(), commandList.end(), [](auto command) { std::cout << command << ", "; });
                    std::cout << "\b\b  \n";
                }
            }
        },
        "help: Displays list of commands\n" 
        "help <command>: Displays info about command\n"
    );
    addCommand(help);
}

Menu::Menu(const Menu& other)
    : commands(other.commands) { }
    
Menu::Menu(Menu&& other)
    : Menu() {
    swap(*this, other);
}

Menu& Menu::operator=(const Menu& other) {
    Menu temp(other);
    swap(*this, temp);
    return *this;
}

void Menu::addCommand(const Command& command) {
    if(std::any_of(commands.begin(), commands.end(), [&command](auto element) { return element.getName() == command.getName(); })) {
        return;
    }
    commands.push_back(command);
}

void Menu::invokeCommand(const std::string& command, const std::string& args) const {
    auto cmd = getCommand(command);
    cmd.invoke(args);
}

void Menu::removeCommand(const std::string& command) {
    commands.erase(std::find(commands.begin(), commands.end(), command));
}

std::vector<std::string> Menu::getCommands() const {
    std::vector<std::string> result;
    std::transform(commands.begin(), commands.end(), back_inserter(result), [](auto command) { return command.getName(); });
    return result;
}

Command Menu::getCommand(const std::string& command) const {
    std::vector<Command> matchingCommands;
    std::copy_if(commands.begin(), commands.end(), back_inserter(matchingCommands), [&command](auto cmd) { return cmd == command; });
    auto n = matchingCommands.size();
    if(n == 0) {
        UnknownCommandException().raise();
    } else if(n > 1) {
        std::vector<std::string> commandNames;
        std::transform(matchingCommands.begin(), matchingCommands.end(), back_inserter(commandNames), [](auto element) { return element.getName(); });
        TooManyMatchingCommandsException(commandNames).raise();
    }
    return matchingCommands.front();
}

void swap(Menu& left, Menu& right) {
    std::swap(left.commands, right.commands);
}

}