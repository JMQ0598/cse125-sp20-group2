#include <game/ClientGame.h>
#include <string>
#include <util/Config.h>

// Config loader variables
// Declared here because static variables need to be declared outside of class
std::unordered_map<std::string, std::string>* Config::vars;

// ./game [client / server]
int main(int argc, char * argv[])
{
	// Load config data
	Config::vars = new std::unordered_map<std::string, std::string>();
	Config::load();

	// Run game
	ClientGame game(0);
	game.runGame();

	// Unload config data
	Config::unload();
}


