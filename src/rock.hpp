#include <random>

class RockNoise {
private:
    std::minstd_rand0 mt;
    std::uniform_real_distribution<> distrib;

public:
    RockNoise(unsigned int seed = 0) {
        mt = std::minstd_rand0(seed);
        distrib = std::uniform_real_distribution<>(0, 100);
    }

    bool hasRock(double height, double upperT, double lowerT, double frequency=0.1) {
        float num = distrib(mt);
        if (num >= (1 - frequency) * 100.0 && height < upperT && height > lowerT) {
            return true;
        }
        return false;
    }
};
