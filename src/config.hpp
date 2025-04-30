#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <map>

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

    int getConfigTree() {
        bool tree = false;
        for (std::string line : fileContents) {
            if (line == "TREE:") {
                tree = true;
            }
            else if (tree) {
                return std::stoi(line);
            }
        }
        return 1337; // default seed
    }

    std::vector<float> getConfigTreeProp() {
        std::vector<float> treePropVec; // default configuration
        bool treeProps = false;
        for (std::string line : fileContents) {
            if (line == "TREE_PROPS:") {
                treeProps = true;
            }
            else if (treeProps) {
                std::stringstream ss(line);
                std::string item;

                while (std::getline(ss, item, ',')) {
                    treePropVec.push_back(std::stof(trim(item)));
                }
                
                return treePropVec;
            }
        }
        return std::vector<float>({3.0f, 2.0f, 7.0f, 6.0f});
    }

    float getConfigTreeFreq() {
        bool freq = false;
        for (std::string line : fileContents) {
            if (line == "TREE_FREQ:") {
                freq = true;
            }
            else if (freq) {
                return std::stof(line);
            }
        }
        return 0.01; // default freq
    }

    int getConfigRock() {
        bool rock = false;
        for (std::string line : fileContents) {
            if (line == "ROCK:") {
                rock = true;
            }
            else if (rock) {
                return std::stoi(line);
            }
        }
        return 1337; // default seed
    }

    float getConfigRockFreq() {
        bool freq = false;
        for (std::string line : fileContents) {
            if (line == "ROCK_FREQ:") {
                freq = true;
            }
            else if (freq) {
                return std::stof(line);
            }
        }
        return 0.01; // default freq
    }

    std::vector<float> getConfigRockProp() {
        std::vector<float> rockPropVec; // default configuration
        bool rockProps = false;
        for (std::string line : fileContents) {
            if (line == "ROCK_PROPS:") {
                rockProps = true;
            }
            else if (rockProps) {
                std::stringstream ss(line);
                std::string item;

                while (std::getline(ss, item, ',')) {
                    rockPropVec.push_back(std::stof(trim(item)));
                }
                
                return rockPropVec;
            }
        }
        return std::vector<float>({2.0f, 0.2f, 1337.0f});
    }

    /**
     * bruh why wont this stupid c++ syntax highlighter highlight me...
     */
    std::pair<std::string, std::map<char, std::string>> getConfigLsystem() {
        std::string axiom;
        std::map<char, std::string> rules;
        bool lsystem = false;
        int numRules = -1;
        for (const std::string& line : fileContents) {
            if (line == "BIOME_SYSTEM:") {
                lsystem = true;
            } 
            else if (lsystem) {
                if (axiom.empty()) {
                    axiom = line;
                } else {
                    if (numRules < 0) {
                        numRules = std::stoi(line);
                    }
                    else if (numRules == 0) {
                        break;
                    }
                    else {
                        std::stringstream ss(line);
                        size_t colonPos = line.find(':');
                        if (colonPos != std::string::npos) {
                            char key = line[0];
                            std::string replacement = line.substr(colonPos + 2);
                            rules[key] = replacement;
                        }
                        numRules--;
                    }
                    
                }
            }
        }
    
        return std::make_pair(axiom, rules);
    }

};
