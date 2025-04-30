#include <string>
#include <map>
#include <memory>
#include <iostream>

using namespace std;

class Lsystem {
private:
    std::string lsystem;
    std::map<char, std::string> rules;
public:

    enum BiomeType {
        DESERT,
        FOREST,
        ROCKY,
        SNOWY
    };

    Lsystem(std::string begin, std::map<char, std::string> rules) {
        lsystem = begin;
        this->rules = rules;
    }

    std::string generate(int max_output_val = 64) {
        std::string result = lsystem;
        while (result.size() < max_output_val) {
            std::string tmp;
            for (char c : result) {
                if (rules.count(c)) {
                    tmp.append(rules[c]);
                } else {
                    tmp.push_back(c);
                }
            }
            if (tmp == result) {
                break;
            }
    
            result = std::move(tmp);
        }
        return result.substr(0, max_output_val);
    }
};