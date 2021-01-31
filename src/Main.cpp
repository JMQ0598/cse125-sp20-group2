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
std::unordered_set<std::string>* Config::updated_vars;
std::unordered_map<std::string, std::string>* Config::dm_vars;
std::unordered_map<std::string, std::string>* Config::km_vars;
std::unordered_map<std::string, std::string>* Config::lm_vars;

/**
 * Spawns the lobby setup process. Meant to be called exclusively
 * at the end of main's lifespan.
 * 
 * Handles exit codes (as defined by constants/error_vals.h) and sends
 * error strings to lobby setup for handling.
 */
void spawnLobbySetup(int exitCode) {

	// Determine the error string (if any)
	std::string err;
	switch (exitCode) {
		case ERR_BADHOST:
			err = ERR_BADHOST_STR;
			break;
		case ERR_FULLSERVER:
			err = ERR_FULLSERVER_STR;
			break;
		default:
			err = "";
			break;
	}

	// Process/startup information setup, as required
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	// Create the lobby setup process
	CreateProcess(NULL, strdup((EXEC_FILE + " " + err).c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

/**
 * Cleans up the config file (as well as writing as needed)
 * Returns to the lobby setup process.
 */
void cleanup(int exitCode) {

	// Unload config data and write as needed
	Config::unload();

	// Return to lobby setup with no errors.
	spawnLobbySetup(exitCode);
}

/**
 * This callback performs cleanup upon abrupt program termination.
 */
void signal_callback_handler(int signum) {
	cleanup(ERR_NONE);
}

/**
 * Game entry point. Determines whether to spawn a client or
 * a server host. Sets up random number gen seed as well as config.
 * Returns to the lobby setup process upon normal termination.
 * 
 * Command line usage: ./game [client / server] [port] [IP]
 */
int main(int argc, char * argv[])
{
	// Perform cleanup even when the program is abruptly terminated. (does not work unless -mwindows is disabled)
	signal(SIGBREAK, &signal_callback_handler);
	
	// Set the seed for random number generation.
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

	///TODO: Also need to receive map file names as args in the future.
	Config::readMapFile("maps/BasicDungeon.dm", DM_VAL);
	Config::readMapFile("maps/BasicKitchen.km", KM_VAL);
	Config::readMapFile("maps/BasicLobby.lm", LM_VAL);

	// Determine if running server/client
	std::string option = argv[1];
	if (option == "server")
	{
		ServerGame game(port);
	}
	else if (option == "client")
	{
		std::cout << "Attempting to connect to " << host << std::endl;
		ClientGame game(host, port);
		FreeConsole();
		exitCode = game.runGame();
	}

	// Clean up config and go back to lobby process
	cleanup(exitCode);
}

