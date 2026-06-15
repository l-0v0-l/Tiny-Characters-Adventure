#pragma once

class UpgradeManager {
private:
    int hpLevel = 1;
    int atkLevel = 1;
    int speedLevel = 1;
    int points = 3; // 🛠️ 修正：初始點數改為 3

public:
    UpgradeManager() : hpLevel(1), atkLevel(1), speedLevel(1), points(3) {}

    void addPoints(int amount) { points += amount; }
    int getPoints() const { return points; }
    void setPoints(int p) { points = p; }

    int getHpLevel() const { return hpLevel; }
    int getAtkLevel() const { return atkLevel; }
    int getSpeedLevel() const { return speedLevel; }

    void upgradeHp() { if (points > 0 && hpLevel < 10) { hpLevel++; points--; } }
    void upgradeAtk() { if (points > 0 && atkLevel < 10) { atkLevel++; points--; } }
    void upgradeSpeed() { if (points > 0 && speedLevel < 4) { speedLevel++; points--; } }

    void resetUpgrades() {
        points += (hpLevel - 1) + (atkLevel - 1) + (speedLevel - 1);
        hpLevel = 1; atkLevel = 1; speedLevel = 1;
    }

    void forceSetLevels(int h, int a, int s, int p) {
        hpLevel = h; atkLevel = a; speedLevel = s; points = p;
    }
};