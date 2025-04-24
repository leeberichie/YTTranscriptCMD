#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // for system()
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>  // Include nlohmann/json

#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

// Function to execute the Python script to get the transcript
std::string getTranscript(const std::string& videoUrl) {
    std::string command = "python src/get_transcript.py " + videoUrl + " > transcript.txt";
    system(command.c_str());  // Runs the Python script and stores output in transcript.txt
    
    // Read the result from transcript.txt
    std::ifstream file("transcript.txt");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to export the transcript, either printing to the console or saving to a .txt file
void exportTranscript(const std::string& transcript, const std::string& exportFile, const std::string& format) {
    if (format == "stdout") {
        // Print to console
        std::cout << "Transcript (printed to console):" << std::endl;
        std::cout << transcript << std::endl;
    } else if (format == "txt") {
        // Save the transcript to a text file
        std::ofstream outFile(exportFile);
        outFile << transcript;
        outFile.close();
        std::cout << "Transcript saved to text file: " << exportFile << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <YouTube URL> [-d <export_location>] [-c <config_file>]" << std::endl;
        return 1;
    }

    std::string videoUrl = argv[1];
    std::string exportLocation = "";
    std::string configFile = "config.json";  // Default config file

    // Check for optional arguments
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "-d" && i + 1 < argc) {
            exportLocation = argv[i + 1];
        } else if (std::string(argv[i]) == "-c" && i + 1 < argc) {
            configFile = argv[i + 1];
        }
    }

    // Load configuration settings from config.json
    json config;
    std::ifstream configStream(configFile);
    if (!configStream) {
        std::cerr << "Can't load configuration file " << configFile << std::endl;
        return 1;
    }
    configStream >> config;

    // Get config options
    std::string format = config["export"].value("format", "txt");  // default is txt
    std::string defaultExportLocation = config["export"].value("default_location", "");
    bool includeTimestamps = config["transcript"].value("timestamps", true);  // default is true

    // If no export location is specified, use the default one from config
    if (exportLocation.empty() && !defaultExportLocation.empty()) {
        exportLocation = defaultExportLocation;
    }

    // Retrieve transcript using Python script
    std::string transcript = getTranscript(videoUrl);

    // If timestamps are disabled, remove timestamps from transcript
    if (!includeTimestamps) {
        std::istringstream iss(transcript);
        std::string line;
        std::string cleanTranscript;
        while (std::getline(iss, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                cleanTranscript += line.substr(pos + 1) + "\n";
            }
        }
        transcript = cleanTranscript;
    }

    // Print transcript to console if no export location is specified
    if (exportLocation.empty()) {
        std::cout << transcript << std::endl;
    } else {
        // Export the transcript to the specified location and format
        exportTranscript(transcript, exportLocation, format);
    }

    return 0;
}
