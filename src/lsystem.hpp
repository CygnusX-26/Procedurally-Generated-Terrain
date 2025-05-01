#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <random>

using namespace std;

class Lsystem {
private:
    std::string lsystem;
    std::map<char, std::string> rules;
public:

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

    static int getBiomeFromChar(char type) {
        switch (type) {
            case 'D':
                return 0;
            case 'F':
                return 1;
            default:
                return 0;
        }
    }
};