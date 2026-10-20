#pragma once

#include "Client.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Replies.hpp"
#include "ToolsHelper.hpp"


#define COMMAND_COUNT 9

class CommandDispatcher
{
    public :
        CommandDispatcher();
        CommandDispatcher(const CommandDispatcher &other);
        CommandDispatcher &operator=(const CommandDispatcher &other);
        ~CommandDispatcher();

        void execute(Client &client, Server &server, const IrcMessage &message);
    
        //registration commands
        void executePASS(Client &client, Server &server, const IrcMessage &message);
        void executeNICK(Client &client, Server &server, const IrcMessage &message);
        void executeUSER(Client &client, Server &server, const IrcMessage &message);

        //check registration status
        void tryRegister(Client &client, Server &server);

        void executeJOIN(Client &client, Server &server, const IrcMessage &message);
        void executePRIVMSG(Client &client, Server &server, const IrcMessage &message);
        void executeKICK(Client &client, Server &server, const IrcMessage &message);
        void executeINVITE(Client &client, Server &server, const IrcMessage &message);
        void executeTOPIC(Client &client, Server &server, const IrcMessage &message);
        void executeMODE(Client &client, Server &server, const IrcMessage &message);
};
