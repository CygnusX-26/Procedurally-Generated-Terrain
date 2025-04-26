#include <random>

class TreeNoise {
private:
    std::mt19937 mt;
    std::uniform_real_distribution<> distrib;

public:
    TreeNoise(unsigned int seed = 0) {
        mt = std::mt19937(seed);
        distrib = std::uniform_real_distribution<>(0, 100);
    }

    bool hasTree(double height, double upperT, double lowerT, double frequency=0.1) {
        float num = distrib(mt);
        if (num >= (1 - frequency) * 100.0 && height < upperT && height > lowerT) {
            return true;
        }
        return false;
    }

    int treeHeight() {
        float num = distrib(mt);
        if (num >= 90) {
            return 5;
        }
        if (num >= 50) {
            return 3;
        }
        return 2;
    }
};
