
#pragma once

#include <iostream>
#include "Parser.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Replies.hpp"

#include <csignal>

extern volatile sig_atomic_t is_running;

void signalHandler(int signal);
