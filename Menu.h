#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class UpgradeManager;

enum class MenuState {
    MainMenu,
    HowToPlay,
    CharacterSelect,
    UpgradeScreen,
    SaveLoadScreen,
    StartGame,
    GameOverScreen,
    VictoryStory,
    Congratulation
};

class Menu {
private:
    MenuState currentState;
    int selectedCharacter;
    bool isCharConfirmed;
    int currentSlot;

    sf::Font font;
    sf::Text titleText;
    sf::Text storyText;
    sf::RectangleShape storyBackground;

    std::vector<std::string> mainMenuOptions;
    std::vector<sf::Text> mainMenuTexts;

    sf::Texture charTextures[3];
    std::vector<sf::Sprite> charSprites;
    sf::Text confirmCharText;

    sf::Text upgradeTitleText;
    sf::Text pointsText;
    std::vector<sf::Text> upgradeItemTexts;
    std::vector<sf::RectangleShape> upgradePlusButtons;
    sf::Text resetText;
    sf::Text confirmUpgradeText;

    std::vector<sf::RectangleShape> saveSlots;
    std::vector<sf::Text> slotTexts;
    std::vector<sf::Text> saveActionTexts;
    sf::Text backToMenuFromSaveText;

    sf::RectangleShape pauseOverlay;
    sf::Text pauseTitleText;
    std::vector<sf::Text> pauseOptions;

    sf::Text gameOverText;
    sf::Text restartText;

    sf::Texture victoryBgTexture;
    sf::Sprite victoryBgSprite;
    sf::RectangleShape victoryMask;
    sf::RectangleShape victoryTextBackground;
    sf::Text victoryStoryText;
    sf::Text congratulationText;
    sf::Text victoryBackText;

    sf::Texture gameOverBgTexture;
    sf::Sprite gameOverBgSprite;

    sf::Texture startBgTexture;
    sf::Sprite startBgSprite;

    void initText(sf::Text& text, const std::string& str, sf::Vector2f pos, unsigned int size, sf::Color color = sf::Color::White);
    void executeSave(int slot, const UpgradeManager& upg);
    void executeLoad(int slot, UpgradeManager& upg);
    void executeDelete(int slot);

public:
    Menu(sf::RenderWindow& window);
    ~Menu() = default;

    MenuState getCurrentState() const { return currentState; }
    void setCurrentState(MenuState state) { currentState = state; }
    int getSelectedCharacter() const { return selectedCharacter; }

    // 🛠️ 關鍵對齊：確保與 Menu.cpp 的參數特徵 100% 相同，徹底打破 LNK2019 外部符號魔咒
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, UpgradeManager& upg, bool& isPaused);
    void updateUpgradeStrings(const UpgradeManager& upg);
    void draw(sf::RenderWindow& window, const UpgradeManager& upg, bool isPaused);

    sf::Texture heavyBulletTexture;
    sf::Texture sniperBulletTexture;
    sf::Texture scatterBulletTexture;

    sf::Texture bossSpadeTexture;
    sf::Texture bossHeartTexture;
    sf::Texture bossDiamondTexture;
    sf::Texture bossClubTexture;

    sf::Texture gardenBgTexture;
};