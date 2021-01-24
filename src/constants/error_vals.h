#pragma once
#include <string>

const int ERR_NONE = 0;         // No error has occurred. Exit normally.
const int ERR_BADHOST = 1;      // Error - client tried to connect to a bad host.
const int ERR_FULLSERVER = 2;   // Error - client tried to connect to a valid but full server.

const std::string ERR_BADHOST_STR = "Unable to connect - invalid IP or port, or server offline. Please try again.";
const std::string ERR_FULLSERVER_STR = "Unable to connect - server full.";