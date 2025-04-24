#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

class Config {
private:
    std::vector<std::string> preprocessFile(std::string filename) {
        std::ifstream file;
	    file.exceptions(std::ios::badbit | std::ios::failbit);
	    file.open(filename);

	    std::vector<std::string> sv;
        try {
            while (true) {
                sv.push_back(getNextLine(file));
                
            }
        } catch (const std::exception& e) { e; }

        return sv;
    }

    std::string getNextLine(std::istream& istr) {
        const std::string comment = "#";
        std::string line = "";
        while (line == "") {
            std::getline(istr, line);
            auto found = line.find(comment);
            if (found != std::string::npos)
                line = line.substr(0, found);
            line = trim(line);
        }
        return line;
    }

    std::string trim(const std::string& line) {
        const std::string whitespace = " \t\r\n";
        auto first = line.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return "";
        auto last = line.find_last_not_of(whitespace);
        auto range = last - first + 1;
        return line.substr(first, range);
    }
public:
    std::vector<std::string> fileContents;

    Config(std::string fileName) {
        fileContents = preprocessFile(fileName);
    }

    int getConfigPerlin() {
        bool perlin = false;
        for (std::string line : fileContents) {
            if (line == "PERLIN:") {
                perlin = true;
            }
            else if (perlin) {
                return std::stoi(line);
            }
        }
        return 1337; // default seed
    }

    float getConfigPerlinFreq() {
        bool freq = false;
        for (std::string line : fileContents) {
            if (line == "PERLIN_FREQ:") {
                freq = true;
            }
            else if (freq) {
                return std::stof(line);
            }
        }
        return 5.0; // default freq
    }
};
