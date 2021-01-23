#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <cstdio>
#include <glm/gtx/string_cast.hpp>

const static std::string CLIENT_CONFIG_PATH = "config/client_config.txt";
const static std::string SERVER_CONFIG_PATH = "config/server_config.txt";
const static std::string TEMP_FILE_PATH = "config/temp.txt";

class Config {
public:
    
    // Config variables
    static std::unordered_map<std::string, std::string>* client_vars;
    static std::unordered_map<std::string, std::string>* server_vars;
    static std::unordered_map<std::string, std::string>* updated_client_vars;
    static std::unordered_map<std::string, std::string>* updated_server_vars;

    /**
     * Set a config variable (used by other functions)
     * */
    static void set(std::string key, std::string value) {

        // Where is the value located (if it exists)?
        switch (Config::hasKey(key)) {
            case 1:
                (*client_vars)[key] = value;
                (*updated_client_vars)[key] = value;
                break;
            case 2:
                (*server_vars)[key] = value;
                (*updated_server_vars)[key] = value;
                break;
        }
    }

    /**
     * Set an int variable
     * */
    static void setInt(std::string key, int value) {

        // Delegate to set
        set(key, std::to_string(value));
    }

    /**
     * Set a float variable
     * */
    static void setFloat(std::string key, int value) {

        // Delegate to set
        set(key, std::to_string(value));
    }

    /**
     * Sets a vec3 variable in the format
     * x,y,z .
     * */
    static void setVec3(std::string key, glm::vec3 value) {

        // The string to insert
        std::string newVal = "";
        
        // Parse vector coordinates
        newVal += std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z);

        // Delegate to set
        set(key, newVal);
    }

    /**
     * Sets a vec4 variable in the format
     * x,y,z,w .
     * */
    static void setVec4(std::string key, glm::vec4 value) {

        // The string to insert
        std::string newVal = "";
        
        // Parse vector coordinates
        newVal += std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z) + "," + std::to_string(value.w);

        // Delegate to set
        set(key, newVal);
    }

    /**
     * Get a config variable (used by other functions)
     * Can also be called directly if need to obtain a string value
     * */
    static std::string get(std::string key) {

        // Where is the value located (if it exists)?
        switch (Config::hasKey(key)) {
            case 1:
                return client_vars->at(key);
                break;
            case 2:
                return server_vars->at(key);
                break;
            default:
                return "";
        }
    }

    /**
     * Parses an int from the config file
     * */
    static int getInt(std::string key) {

        // Get value or return default
        std::string val = get(key);
        if (key == "")  return -1;
        else            return std::stoi(val);
    }

    /**
     * Parses a float from the config file
     * */
    static float getFloat(std::string key) {

        // Get value or return default
        std::string val = get(key);
        if (key == "")  return -1;
        else            return std::stof(val);
    }

    /**
     * Parses a vec3 from the config file in the format
     * x,y,z .
     * */
    static glm::vec3 getVec3(std::string key) {

        // Get value or return default
        std::string val = get(key);
        if (key == "")  return glm::vec3(0);
        
        // Parse vector coordinate x
        int pos = val.find(',');
        float x = std::stof(val.substr(0, pos));
        val.erase(0, pos + 1);

        // Parse vector coordinate y
        pos = val.find(',');
        float y = std::stof(val.substr(0, pos));
        val.erase(0, pos + 1);

        // Parse remainder of string, assumed to be z
        float z = std::stof(val);

        // Return the full vector
        return glm::vec3(x, y, z);
    }

    /**
     * Parses a vec4 from the config file in the format
     * x,y,z,w .
     * */
    static glm::vec4 getVec4(std::string key) {

        // Get value or return default
        std::string val = get(key);
        if (key == "")  return glm::vec4(0);
        
        // Parse vector coordinate x
        int pos = val.find(',');
        float x = std::stof(val.substr(0, pos));
        val.erase(0, pos + 1);

        // Parse vector coordinate y
        pos = val.find(',');
        float y = std::stof(val.substr(0, pos));
        val.erase(0, pos + 1);

        // Parse vector coordinate z
        pos = val.find(',');
        float z = std::stof(val.substr(0, pos));
        val.erase(0, pos + 1);

        // Parse remainder of the string, assumed to be w
        float w = std::stof(val);

        // Return the full vector
        return glm::vec4(x, y, z, w);
    }

    /**
     * Initialize variable maps and read config values from files.
     * */
    static void load() 
    {
        // Init
        client_vars = new std::unordered_map<std::string, std::string>();
        server_vars = new std::unordered_map<std::string, std::string>();
        updated_client_vars = new std::unordered_map<std::string, std::string>();
        updated_server_vars = new std::unordered_map<std::string, std::string>();

        // Read from known config paths
        readConfigFile(CLIENT_CONFIG_PATH, client_vars);
        readConfigFile(SERVER_CONFIG_PATH, server_vars);
    }

    /**
     * Helper method to read from individual config files.
     * */
    static void readConfigFile(std::string filepath, 
            std::unordered_map<std::string, std::string>* vars) {

        // To read from the file
        std::ifstream configFile(filepath);

        // Full line of text to be read
        std::string line;

        // Read lines of the config file
        while (std::getline(configFile, line))
        {
            // Skip comments and empty lines
            if (line.length() == 0 || line.at(0) == '/') continue;

            // Read string
            std::istringstream lineStream(line);

            // Key/value pair to be extracted
            std::string key, value;

            // Get next key/value pair
            std::getline(lineStream, key, '=');
            std::getline(lineStream, value);

            // Store data
            (*vars)[key] = value;
        }
    }

    /**
     * Deallocate map of config variables. Write any updated config values to files.
     * */
    static void unload()
    {
        // Write to files
        writeConfigFile(CLIENT_CONFIG_PATH, updated_client_vars);
        writeConfigFile(SERVER_CONFIG_PATH, updated_server_vars);

        // Cleanup
        delete client_vars;
        delete server_vars;
        delete updated_client_vars;
        delete updated_server_vars;
    }

    /**
     * Helper method to write to individual config files.
     * */
    static void writeConfigFile(std::string filepath, 
            std::unordered_map<std::string, std::string>* vars) {

        // To read from the file
        std::ifstream configFile(filepath);

        // Temp file that will replace the old one
        std::ofstream tempFile(TEMP_FILE_PATH);

        // Full line of text to be read
        std::string line;

        // Read lines of the config file
        while (std::getline(configFile, line))
        {
            // Get key to check
            std::string key = line.substr(0, line.find('='));

            // If key needs to be updated, write new value instead
            if (vars->find(key) != vars->end()) tempFile << key << '=' << (*vars)[key] << std::endl;
            
            // Store data in temp file
            else                                tempFile << line << std::endl;
        }

        // Close files
        configFile.close();
        tempFile.close();

        // Replace old file
        remove(filepath.c_str());
        rename(TEMP_FILE_PATH.c_str(), filepath.c_str());
    }

    /**
     * Helper method to check if something exists in either config
     * Returns 0 if not found.
     * Returns 1 if found in client config.
     * Returns 2 if found in server config.
     * */
    static int hasKey(std::string key) {
        
        // Does it exist in one of the maps?
        int ret = 0;
        
        // Check client config
        if (client_vars->find(key) != client_vars->end())       ret = 1;

        // Check server config
        else if (server_vars->find(key) != server_vars->end())  ret = 2;

        // Debug
        if (!ret)   std::cerr << "Cannot find key: " << key << " in config." << std::endl;
        
        // Return
        return ret;
    }

    /**
     * Helper method to check if a key is already bound in
     * the config
     * */
    static bool noConflictingKey(int newKey) {
        if (getInt("Walk_Forward") == newKey)       return false;
        else if (getInt("Walk_Backward") == newKey) return false;
        else if (getInt("Turn_Left") == newKey)     return false;
        else if (getInt("Turn_Right") == newKey)    return false;
        else if (getInt("Action") == newKey)        return false;
        else if (getInt("Ready") == newKey)         return false;
        else if (getInt("Open_Settings") == newKey) return false;
        return true;
    }
};