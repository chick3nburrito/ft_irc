#include "../include/CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher() {}
CommandDispatcher::CommandDispatcher(const CommandDispatcher &other) {(void)other;}
CommandDispatcher &CommandDispatcher::operator=(const CommandDispatcher &other) {(void)other;return (*this);}
CommandDispatcher::~CommandDispatcher() {}

void CommandDispatcher::execute(Client &client, Server &server, const IrcMessage &message)
{
    static const std::string commands[COMMAND_COUNT] = {"PASS", "NICK", "USER", 
                                                "JOIN", "PRIVMSG", "KICK", 
                                                "INVITE", "TOPIC", "MODE"};
    static void (CommandDispatcher::*func[COMMAND_COUNT])(Client &, Server &, const IrcMessage &) = {
                        &CommandDispatcher::executePASS, &CommandDispatcher::executeNICK,
                        &CommandDispatcher::executeUSER, &CommandDispatcher::executeJOIN,
                        &CommandDispatcher::executePRIVMSG, &CommandDispatcher::executeKICK, 
                        &CommandDispatcher::executeINVITE, &CommandDispatcher::executeTOPIC, 
                        &CommandDispatcher::executeMODE};
    for (int i = 0; i < COMMAND_COUNT; i++)
    {
        if (message.command == commands[i])
        {
            (this->*func[i])(client, server, message);
            return;
        }
    }
    if(client.getRegistrationStatus())
    {
        sendReply(client, server, ERR_UNKNOWNCOMMAND, message.command + " :Unknown command");
    }
}
void CommandDispatcher::tryRegister(Client &client, Server &server)
{
    if (client.getRegistrationStatus())
        return;

    if (client.getNickName().empty())
        return;

    if (client.getUserName().empty())
        return;

    if (client.getPassword().empty())
        return;

    if (client.getPassword() != server.getPassword())
    {
        sendReply(client, server, ERR_PASSWDMISMATCH, ":Password incorrect");
        return;
    }

    client.setRegistered();
    sendReply(client, server, RPL_WELCOME, "Welcome to the IRC server, " + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostname());
}
