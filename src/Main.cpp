#include <game/ClientGame.h>
#include <game/ServerGame.h>
#include <string>
#include <util/Config.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <Windows.h>
#include <constants/error_vals.h>

// Lobby setup executable
std::string EXEC_FILE = "LobbySetup.exe"; 

// Config loader variables
// Declared here because static variables need to be declared outside of class
std::unordered_map<std::string, std::string>* Config::client_vars;
std::unordered_map<std::string, std::string>* Config::server_vars;
std::unordered_map<std::string, std::string>* Config::updated_client_vars;
std::unordered_map<std::string, std::string>* Config::updated_server_vars;

void spawnLobbySetup(int exitCode) {

	std::string err;
	switch (exitCode) {
		case ERR_BADCONNECT:
			err = ERR_BADCONNECT_STR;
			break;
		default:
			err = "";
			break;
	}

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, strdup(std::string(EXEC_FILE + " " + err).c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

// ./game [client / server]
int main(int argc, char * argv[])
{
	srand(time(NULL));
	
	// Load config data
	Config::load();

	// Should not get here - no port nor IP
	// If we do get here, it indicates that LobbySetup failed to prevent this
	if (argc < 3) {
		return -1;
	}

	// Host and client - get port argument
	int port = atoi(argv[2]);

	// Client spawn only - get host argument
	std::string host("localhost");
	if (argc > 3) host = argv[3];
	
	// Used to store exit codes
	int exitCode = 0;

	// Determine if running server/client
	std::string option = argv[1];
	if (option == "server")
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		ServerGame game(port);
	}
	else if (option == "client")
	{
		std::cerr << "Attempting to connect to " << host << std::endl;
		ClientGame game(host, port);
		exitCode = game.runGame();
	}

	// Unload config data
	Config::unload();

	// Spawn lobby setup at the end
	spawnLobbySetup(exitCode);
}


