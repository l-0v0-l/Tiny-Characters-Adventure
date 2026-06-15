#include "Menu.h"
#include "UpgradeManager.h"
#include "AssetManager.h" 
#include <iostream>
#include <fstream>

// initText：標準隱式轉換與置中對齊
void Menu::initText(sf::Text& text, const std::string& str, sf::Vector2f pos, unsigned int size, sf::Color color) {
    text.setFont(font);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    text.setFillColor(color);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
    text.setPosition(pos);
}

// 建構子
Menu::Menu(sf::RenderWindow& window)
    : currentState(MenuState::MainMenu), selectedCharacter(1), isCharConfirmed(false), currentSlot(1),
    titleText(font), storyText(font), confirmCharText(font), backToMenuFromSaveText(font),
    upgradeTitleText(font), pointsText(font), confirmUpgradeText(font), resetText(font),
    pauseTitleText(font), gameOverText(font), restartText(font), victoryStoryText(font),
    congratulationText(font), victoryBackText(font),
    victoryBgSprite(victoryBgTexture),
    gameOverBgSprite(gameOverBgTexture),
    startBgSprite(startBgTexture)
{
    if (!font.openFromFile("C:/Windows/Fonts/msjh.ttc")) {
        std::cerr << "Cannot load font: msjh.ttc" << std::endl;
    }

    sf::Vector2f center{ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f };

    // === 主選單文字 ===
    initText(titleText, "Tiny  Characters' Adventure", { center.x, center.y - 250.f }, 52, sf::Color(255, 236, 139));

    mainMenuOptions = { "Game Start", "Upgrade", "Save Load", "How to Play", "Quit Game" };
    mainMenuTexts.clear();
    for (size_t i = 0; i < mainMenuOptions.size(); ++i) {
        sf::Text text(font);
        initText(text, mainMenuOptions[i], { center.x, center.y - 60.f + i * 75.f }, 30);
        mainMenuTexts.push_back(text);
    }

    // === 故事背景與操作說明 ===
    std::string storyStr = R"(【Story Background】
In 2026, an unknown mothership invaded Earth...
As the last ace pilot, you must stand up and fight!

【Controls】
Move: WASD  |  Attack: SPACE  |  Skill: K
Anytime press [ ESC ] back to Main Menu.

- Click mouse or press any key to return -)";

    storyText.setFont(font);
    storyText.setString(sf::String::fromUtf8(storyStr.begin(), storyStr.end()));
    storyText.setCharacterSize(22);
    storyText.setFillColor(sf::Color::White);
    sf::FloatRect sBounds = storyText.getLocalBounds();
    storyText.setOrigin({ sBounds.size.x / 2.0f, sBounds.size.y / 2.0f });
    storyText.setPosition(center);

    storyBackground.setSize({ sBounds.size.x + 50.f, sBounds.size.y + 50.f });
    storyBackground.setOrigin(sf::Vector2f(storyBackground.getSize().x / 2.f, storyBackground.getSize().y / 2.f));
    storyBackground.setPosition(center);
    storyBackground.setFillColor(sf::Color(0, 0, 0, 195));
    storyBackground.setOutlineThickness(2.0f);
    storyBackground.setOutlineColor(sf::Color(0, 255, 255, 120));

    // === 結算與通關文字 ===
    initText(gameOverText, "GAME OVER", { center.x, center.y - 40.f }, 50, sf::Color::Red);
    initText(restartText, "Press ENTER to Return to Station", { center.x, center.y + 40.f }, 24, sf::Color::Yellow);
    initText(congratulationText, "VICTORY! PEACE HAS RETURNED!", { center.x, center.y - 60.f }, 46, sf::Color::Magenta);
    initText(victoryBackText, "Press ENTER to Return to Menu", { center.x, center.y + 40.f }, 24, sf::Color::Yellow);

    // === 預加載快取貼圖 ===
    heavyBulletTexture = AssetManager::getInstance().getTexture("assets/heavy_bullet.png");
    sniperBulletTexture = AssetManager::getInstance().getTexture("assets/bullet_laser.png");
    scatterBulletTexture = AssetManager::getInstance().getTexture("assets/scatter_bullet.png");

    bossSpadeTexture = AssetManager::getInstance().getTexture("assets/spade.png");
    bossHeartTexture = AssetManager::getInstance().getTexture("assets/heart.png");
    bossDiamondTexture = AssetManager::getInstance().getTexture("assets/diamond.png");
    bossClubTexture = AssetManager::getInstance().getTexture("assets/club.png");

    gardenBgTexture = AssetManager::getInstance().getTexture("assets/garden_bg.png");

    // 加載選角精靈
    std::string charFiles[3] = { "assets/heavy.png", "assets/sniper.png", "assets/scatter.png" };
    charSprites.clear();
    for (int i = 0; i < 3; ++i) {
        charTextures[i] = AssetManager::getInstance().getTexture(charFiles[i]);
        if (charTextures[i].getSize().x == 0) {
            charTextures[i] = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/" + charFiles[i]);
        }

        sf::Sprite spr(charTextures[i]);
        sf::FloatRect imgBounds = spr.getLocalBounds();
        spr.setOrigin({ imgBounds.size.x / 2.f, imgBounds.size.y / 2.f });
        spr.setScale({ 0.45f, 0.45f });
        spr.setPosition({ center.x - 320.f + i * 320.f, center.y - 40.f });
        spr.setColor(sf::Color(100, 100, 100));
        charSprites.push_back(spr);
    }

    if (!charSprites.empty()) {
        charSprites[selectedCharacter - 1].setColor(sf::Color::White);
    }

    initText(confirmCharText, "Confirm Character (ENTER)", { center.x, center.y + 260.f }, 32, sf::Color::Green);

    // === 升級車站介面 UI ===
    initText(upgradeTitleText, "UPGRADE STATION", { center.x, center.y - 220.f }, 42, sf::Color::Yellow);
    initText(pointsText, "Available Points: 3", { center.x, center.y - 150.f }, 26, sf::Color::White);

    std::vector<std::string> items = { "Armor HP Level", "Weapon ATK Level", "Engine SPEED Level" };
    upgradeItemTexts.clear();
    upgradePlusButtons.clear();
    for (size_t i = 0; i < items.size(); ++i) {
        sf::Text t(font);
        initText(t, items[i], { center.x - 50.f, center.y - 30.f + i * 70.f }, 24, sf::Color::White);
        upgradeItemTexts.push_back(t);

        sf::RectangleShape btn({ 35.f, 35.f });
        btn.setFillColor(sf::Color(50, 200, 50));
        btn.setOrigin({ 17.5f, 17.5f });
        btn.setPosition({ center.x + 200.f, center.y - 30.f + i * 70.f });
        upgradePlusButtons.push_back(btn);
    }

    initText(resetText, "[ Reset Points ]", { center.x - 140.f, center.y + 180.f }, 24, sf::Color(200, 100, 100));
    initText(confirmUpgradeText, "[ Back to Menu ]", { center.x + 140.f, center.y + 180.f }, 24, sf::Color::Green);

    // === 存檔槽介面 UI ===
    saveSlots.clear();
    slotTexts.clear();
    saveActionTexts.clear();
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape slot({ 600.f, 90.f });
        slot.setFillColor(sf::Color(30, 30, 40));
        slot.setOutlineThickness(2.f);
        slot.setOutlineColor(sf::Color(70, 70, 90));
        slot.setOrigin({ 300.f, 45.f });
        slot.setPosition({ center.x, center.y - 130.f + i * 130.f });
        saveSlots.push_back(slot);

        sf::Text st(font);
        initText(st, "Slot " + std::to_string(i + 1) + " : Empty Record", { center.x - 100.f, center.y - 130.f + i * 130.f }, 22, sf::Color::White);
        slotTexts.push_back(st);

        sf::Text act(font);
        initText(act, "[SAVE]  [LOAD]  [DEL]", { center.x + 170.f, center.y - 130.f + i * 130.f }, 18, sf::Color::Cyan);
        saveActionTexts.push_back(act);
    }

    initText(backToMenuFromSaveText, "Back to Main Menu", { center.x, center.y + 240.f }, 26, sf::Color::Yellow);

    // === 暫停介面配置 ===
    pauseOverlay.setSize({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    initText(pauseTitleText, "GAME PAUSED", { center.x, center.y - 100.f }, 46, sf::Color::Yellow);

    std::vector<std::string> pOpts = { "Resume Game", "Quit to Menu" };
    pauseOptions.clear();
    for (size_t i = 0; i < pOpts.size(); ++i) {
        sf::Text pt(font);
        initText(pt, pOpts[i], { center.x, center.y + 20.f + i * 80.f }, 30, sf::Color::White);
        pauseOptions.push_back(pt);
    }

    // === 勝利故事面板 ===
    std::string vStr = "MISSION ACCOMPLISHED!\n\nThe alien core mothership exploded completely.\nPeace has returned to the galaxy once again.\n\nThank you for playing our OOP Shooting Game!\n- Click anywhere to return to base -";

    victoryStoryText.setFont(font);
    victoryStoryText.setString(sf::String::fromUtf8(vStr.begin(), vStr.end()));
    victoryStoryText.setCharacterSize(20);
    victoryStoryText.setFillColor(sf::Color::White);
    sf::FloatRect vBounds = victoryStoryText.getLocalBounds();
    victoryStoryText.setOrigin({ vBounds.size.x / 2.0f, vBounds.size.y / 2.0f });
    victoryStoryText.setPosition({ center.x, center.y + 150.f });

    victoryTextBackground.setSize({ 680.f, 220.f });
    victoryTextBackground.setFillColor(sf::Color(0, 0, 0, 200));
    victoryTextBackground.setOutlineThickness(2.f);
    victoryTextBackground.setOutlineColor(sf::Color::Yellow);
    victoryTextBackground.setOrigin({ 340.f, 110.f });
    victoryTextBackground.setPosition({ center.x, center.y + 150.f });
}

void Menu::updateUpgradeStrings(const UpgradeManager& upg) {
    std::string pStr = "Available Points: " + std::to_string(upg.getPoints());
    pointsText.setString(sf::String::fromUtf8(pStr.begin(), pStr.end()));

    if (upgradeItemTexts.size() >= 3) {
        std::string hStr = "HP Level : " + std::to_string(upg.getHpLevel()) + " / 10";
        upgradeItemTexts[0].setString(sf::String::fromUtf8(hStr.begin(), hStr.end()));
        sf::FloatRect b0 = upgradeItemTexts[0].getLocalBounds();
        upgradeItemTexts[0].setOrigin({ b0.size.x / 2.f, b0.size.y / 2.f });

        std::string aStr = "Weapon ATK Level : " + std::to_string(upg.getAtkLevel()) + " / 10";
        upgradeItemTexts[1].setString(sf::String::fromUtf8(aStr.begin(), aStr.end()));
        sf::FloatRect b1 = upgradeItemTexts[1].getLocalBounds();
        upgradeItemTexts[1].setOrigin({ b1.size.x / 2.f, b1.size.y / 2.f });

        std::string sStr = "SPEED Level : " + std::to_string(upg.getSpeedLevel()) + " / 4";
        upgradeItemTexts[2].setString(sf::String::fromUtf8(sStr.begin(), sStr.end()));
        sf::FloatRect b2 = upgradeItemTexts[2].getLocalBounds();
        upgradeItemTexts[2].setOrigin({ b2.size.x / 2.f, b2.size.y / 2.f });
    }
}

// 🛠️ 核心修正：與 Menu.h 參數特徵完全對齊 (const sf::Event& event)
void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window, UpgradeManager& upg, bool& isPaused) {
    sf::Vector2f center{ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f };

    if (currentState != MenuState::MainMenu && currentState != MenuState::StartGame) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                currentState = MenuState::MainMenu;
                selectedCharacter = 1;
                return;
            }
        }
    }

    if (currentState == MenuState::MainMenu) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::W || keyPressed->code == sf::Keyboard::Key::Up) {
                selectedCharacter = (selectedCharacter == 1) ? 5 : selectedCharacter - 1;
            }
            if (keyPressed->code == sf::Keyboard::Key::S || keyPressed->code == sf::Keyboard::Key::Down) {
                selectedCharacter = (selectedCharacter == 5) ? 1 : selectedCharacter + 1;
            }
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (selectedCharacter == 1) { currentState = MenuState::CharacterSelect; selectedCharacter = 1; }
                else if (selectedCharacter == 2) { currentState = MenuState::UpgradeScreen; updateUpgradeStrings(upg); }
                else if (selectedCharacter == 3) { currentState = MenuState::SaveLoadScreen; }
                else if (selectedCharacter == 4) { currentState = MenuState::HowToPlay; }
                else if (selectedCharacter == 5) { window.close(); }
            }
        }

        if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePixelPos{ mouseButtonPressed->position.x, mouseButtonPressed->position.y };
                sf::Vector2f mPos = window.mapPixelToCoords(mousePixelPos);

                for (size_t i = 0; i < mainMenuTexts.size(); ++i) {
                    if (mainMenuTexts[i].getGlobalBounds().contains(mPos)) {
                        if (i == 0) { currentState = MenuState::CharacterSelect; selectedCharacter = 1; }
                        else if (i == 1) { currentState = MenuState::UpgradeScreen; updateUpgradeStrings(upg); }
                        else if (i == 2) { currentState = MenuState::SaveLoadScreen; }
                        else if (i == 3) { currentState = MenuState::HowToPlay; }
                        else if (i == 4) { window.close(); }
                    }
                }
            }
        }

        for (size_t i = 0; i < mainMenuTexts.size(); ++i) {
            if (static_cast<size_t>(selectedCharacter - 1) == i) mainMenuTexts[i].setFillColor(sf::Color::Cyan);
            else mainMenuTexts[i].setFillColor(sf::Color::White);
        }
    }
    else if (currentState == MenuState::HowToPlay) {
        if (event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::KeyPressed>()) {
            currentState = MenuState::MainMenu;
            selectedCharacter = 1;
        }
    }
    else if (currentState == MenuState::CharacterSelect) {
        if (selectedCharacter > 3) selectedCharacter = 1;

        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::A || keyPressed->code == sf::Keyboard::Key::Left) {
                selectedCharacter = (selectedCharacter == 1) ? 3 : selectedCharacter - 1;
            }
            if (keyPressed->code == sf::Keyboard::Key::D || keyPressed->code == sf::Keyboard::Key::Right) {
                selectedCharacter = (selectedCharacter == 3) ? 1 : selectedCharacter + 1;
            }
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                isCharConfirmed = true;
                currentState = MenuState::StartGame;
            }
        }

        if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePixelPos{ mouseButtonPressed->position.x, mouseButtonPressed->position.y };
                sf::Vector2f mPos = window.mapPixelToCoords(mousePixelPos);

                for (size_t i = 0; i < charSprites.size(); ++i) {
                    if (charSprites[i].getGlobalBounds().contains(mPos)) {
                        selectedCharacter = static_cast<int>(i + 1);
                    }
                }

                if (confirmCharText.getGlobalBounds().contains(mPos)) {
                    isCharConfirmed = true;
                    currentState = MenuState::StartGame;
                }
            }
        }

        for (size_t i = 0; i < charSprites.size(); ++i) {
            if (static_cast<size_t>(selectedCharacter - 1) == i) {
                charSprites[i].setColor(sf::Color::White);
            }
            else {
                charSprites[i].setColor(sf::Color(100, 100, 100));
            }
        }
    }
    else if (currentState == MenuState::UpgradeScreen) {
        if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePixelPos{ mouseButtonPressed->position.x, mouseButtonPressed->position.y };
                sf::Vector2f mPos = window.mapPixelToCoords(mousePixelPos);

                for (size_t i = 0; i < upgradePlusButtons.size(); ++i) {
                    if (upgradePlusButtons[i].getGlobalBounds().contains(mPos)) {
                        if (i == 0) upg.upgradeHp();
                        if (i == 1) upg.upgradeAtk();
                        if (i == 2) upg.upgradeSpeed();
                        updateUpgradeStrings(upg);
                    }
                }
                if (resetText.getGlobalBounds().contains(mPos)) { upg.resetUpgrades(); updateUpgradeStrings(upg); }
                if (confirmUpgradeText.getGlobalBounds().contains(mPos)) { currentState = MenuState::MainMenu; selectedCharacter = 1; }
            }
        }
    }
    else if (currentState == MenuState::SaveLoadScreen) {
        if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePixelPos{ mouseButtonPressed->position.x, mouseButtonPressed->position.y };
                sf::Vector2f mPos = window.mapPixelToCoords(mousePixelPos);

                for (size_t i = 0; i < saveSlots.size(); ++i) {
                    sf::FloatRect baseRect = saveSlots[i].getGlobalBounds();
                    if (baseRect.contains(mPos)) {
                        float localX = mPos.x - baseRect.position.x;
                        if (localX > 320.f && localX < 400.f) { executeSave(static_cast<int>(i + 1), upg); }
                        else if (localX > 410.f && localX < 500.f) { executeLoad(static_cast<int>(i + 1), upg); updateUpgradeStrings(upg); }
                        else if (localX > 510.f && localX < 580.f) { executeDelete(static_cast<int>(i + 1)); }
                    }
                }
                if (backToMenuFromSaveText.getGlobalBounds().contains(mPos)) { currentState = MenuState::MainMenu; selectedCharacter = 1; }
            }
        }
    }
}

// 順序移置上方：全域共享 start_bg 加載函式
void drawSharedStartBg(sf::RenderWindow& window, sf::Texture& startBgTexture, sf::Sprite& startBgSprite) {
    startBgTexture = AssetManager::getInstance().getTexture("assets/start_bg.png");
    if (startBgTexture.getSize().x == 0) {
        startBgTexture = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/assets/start_bg.png");
    }
    startBgSprite.setTexture(startBgTexture, true);
    sf::FloatRect stBounds = startBgSprite.getLocalBounds();
    if (stBounds.size.x > 0 && stBounds.size.y > 0) {
        startBgSprite.setScale({
            static_cast<float>(window.getSize().x) / stBounds.size.x,
            static_cast<float>(window.getSize().y) / stBounds.size.y
            });
        window.draw(startBgSprite);
    }
    else {
        sf::RectangleShape fallbackMenuBg({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
        fallbackMenuBg.setFillColor(sf::Color(12, 18, 38, 255));
        window.draw(fallbackMenuBg);
    }
}

void Menu::draw(sf::RenderWindow& window, const UpgradeManager& upg, bool isPaused) {
    sf::Vector2f center{ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f };

    if (currentState == MenuState::MainMenu ||
        currentState == MenuState::HowToPlay ||
        currentState == MenuState::CharacterSelect ||
        currentState == MenuState::UpgradeScreen ||
        currentState == MenuState::SaveLoadScreen)
    {
        drawSharedStartBg(window, startBgTexture, startBgSprite);
    }

    if (currentState == MenuState::MainMenu) {
        sf::FloatRect titleBounds = titleText.getGlobalBounds();
        sf::RectangleShape titleBox({ titleBounds.size.x + 80.f, titleBounds.size.y + 30.f });
        titleBox.setOrigin({ titleBox.getSize().x / 2.f, titleBox.getSize().y / 2.f });
        titleBox.setPosition(titleText.getPosition());
        titleBox.setFillColor(sf::Color(0, 0, 0, 160));
        titleBox.setOutlineThickness(2.f);
        titleBox.setOutlineColor(sf::Color(0, 255, 255, 100));
        window.draw(titleBox);
        window.draw(titleText);

        for (size_t i = 0; i < mainMenuTexts.size(); ++i) {
            sf::FloatRect textBounds = mainMenuTexts[i].getGlobalBounds();
            sf::RectangleShape btnBox({ textBounds.size.x + 50.f, textBounds.size.y + 16.f });
            btnBox.setOrigin({ btnBox.getSize().x / 2.f, btnBox.getSize().y / 2.f });
            btnBox.setPosition(mainMenuTexts[i].getPosition());

            if (static_cast<size_t>(selectedCharacter - 1) == i) {
                btnBox.setFillColor(sf::Color(0, 255, 255, 45));
                btnBox.setOutlineThickness(2.f);
                btnBox.setOutlineColor(sf::Color(0, 255, 255, 180));
            }
            else {
                btnBox.setFillColor(sf::Color(0, 0, 0, 140));
                btnBox.setOutlineThickness(1.5f);
                btnBox.setOutlineColor(sf::Color(255, 255, 255, 40));
            }
            window.draw(btnBox);
            window.draw(mainMenuTexts[i]);
        }
    }
    else if (currentState == MenuState::HowToPlay) {
        window.draw(storyBackground);
        window.draw(storyText);
    }
    else if (currentState == MenuState::CharacterSelect) {
        for (size_t i = 0; i < charSprites.size(); ++i) {
            window.draw(charSprites[i]);
        }
        window.draw(confirmCharText);
    }
    else if (currentState == MenuState::UpgradeScreen) {
        sf::RectangleShape upgradeBox({ 650.f, 340.f });
        upgradeBox.setOrigin({ 325.f, 170.f });
        upgradeBox.setPosition({ center.x, center.y + 10.f });
        upgradeBox.setFillColor(sf::Color(0, 0, 0, 170));
        upgradeBox.setOutlineThickness(2.f);
        upgradeBox.setOutlineColor(sf::Color(255, 215, 0, 120));
        window.draw(upgradeBox);

        window.draw(upgradeTitleText);
        window.draw(pointsText);
        for (auto& t : upgradeItemTexts) window.draw(t);
        for (auto& b : upgradePlusButtons) window.draw(b);
        window.draw(resetText);
        window.draw(confirmUpgradeText);
    }
    else if (currentState == MenuState::SaveLoadScreen) {
        for (auto& s : saveSlots) window.draw(s);
        for (auto& t : slotTexts) window.draw(t);
        for (auto& a : saveActionTexts) window.draw(a);
        window.draw(backToMenuFromSaveText);
    }
    else if (isPaused) {
        window.draw(pauseOverlay); window.draw(pauseTitleText);
        if (pauseOptions.size() >= 2) {
            window.draw(pauseOptions[0]); window.draw(pauseOptions[1]);
        }
    }
    else if (currentState == MenuState::GameOverScreen) {
        gameOverBgTexture = AssetManager::getInstance().getTexture("assets/loss.png");
        if (gameOverBgTexture.getSize().x == 0) {
            gameOverBgTexture = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/assets/loss.png");
        }

        gameOverBgSprite.setTexture(gameOverBgTexture, true);
        sf::FloatRect goBounds = gameOverBgSprite.getLocalBounds();

        if (goBounds.size.x == 0 || goBounds.size.y == 0) {
            sf::RectangleShape fallbackMask({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
            fallbackMask.setFillColor(sf::Color(60, 10, 10, 255));
            window.draw(fallbackMask);
        }
        else {
            gameOverBgSprite.setScale({
                static_cast<float>(window.getSize().x) / goBounds.size.x,
                static_cast<float>(window.getSize().y) / goBounds.size.y
                });
            window.draw(gameOverBgSprite);
        }

        sf::RectangleShape lossBox({ 550.f, 200.f });
        lossBox.setFillColor(sf::Color(0, 0, 0, 180));
        lossBox.setOutlineThickness(2.f);
        lossBox.setOutlineColor(sf::Color(255, 0, 0, 180));
        lossBox.setOrigin({ 275.f, 100.f });
        lossBox.setPosition(center);

        window.draw(lossBox);
        window.draw(gameOverText);
        window.draw(restartText);
    }
    else if (currentState == MenuState::VictoryStory) {
        victoryBgTexture = AssetManager::getInstance().getTexture("assets/victory_bg1.png");
        if (victoryBgTexture.getSize().x == 0) {
            victoryBgTexture = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/assets/victory_bg1.png");
        }
        victoryBgSprite.setTexture(victoryBgTexture, true);
        sf::FloatRect vicBounds = victoryBgSprite.getLocalBounds();
        if (vicBounds.size.x > 0 && vicBounds.size.y > 0) {
            victoryBgSprite.setScale({
                static_cast<float>(window.getSize().x) / vicBounds.size.x,
                static_cast<float>(window.getSize().y) / vicBounds.size.y
                });
            window.draw(victoryBgSprite);
        }
        else {
            sf::RectangleShape fallbackMask({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
            fallbackMask.setFillColor(sf::Color(10, 15, 40, 255));
            window.draw(fallbackMask);
        }

        window.draw(victoryMask);
        window.draw(victoryTextBackground);
        window.draw(victoryStoryText);
    }
    else if (currentState == MenuState::Congratulation) {
        sf::Texture localVicBgTex = AssetManager::getInstance().getTexture("assets/victory_bg.png");
        if (localVicBgTex.getSize().x == 0) {
            localVicBgTex = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/assets/victory_bg.png");
        }
        sf::Sprite localVicBgSprite(localVicBgTex);

        sf::FloatRect vicBounds = localVicBgSprite.getLocalBounds();
        if (vicBounds.size.x > 0 && vicBounds.size.y > 0) {
            localVicBgSprite.setScale({
                static_cast<float>(window.getSize().x) / vicBounds.size.x,
                static_cast<float>(window.getSize().y) / vicBounds.size.y
                });
            window.draw(localVicBgSprite);
        }
        else {
            sf::RectangleShape fallbackMask({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
            fallbackMask.setFillColor(sf::Color(10, 15, 40, 255));
            window.draw(fallbackMask);
        }

        sf::RectangleShape congratsBox({ 720.f, 200.f });
        congratsBox.setOrigin({ 360.f, 100.f });
        congratsBox.setPosition({ center.x, center.y - 10.f });
        congratsBox.setFillColor(sf::Color(0, 0, 0, 160));
        congratsBox.setOutlineThickness(2.f);
        congratsBox.setOutlineColor(sf::Color(255, 0, 255, 150));
        window.draw(congratsBox);

        window.draw(congratulationText);
        window.draw(victoryBackText);
    }
}

void Menu::executeSave(int slot, const UpgradeManager& upg) {
    std::ofstream f("save_slot_" + std::to_string(slot) + ".dat", std::ios::binary);
    if (f) {
        int hp = upg.getHpLevel(), atk = upg.getAtkLevel(), spd = upg.getSpeedLevel(), pts = upg.getPoints();
        f.write((char*)&hp, sizeof(hp)); f.write((char*)&atk, sizeof(atk));
        f.write((char*)&spd, sizeof(spd)); f.write((char*)&pts, sizeof(pts));
    }
}

void Menu::executeLoad(int slot, UpgradeManager& upg) {
    std::ifstream f("save_slot_" + std::to_string(slot) + ".dat", std::ios::binary);
    if (f) {
        int hp, atk, spd, pts;
        f.read((char*)&hp, sizeof(hp)); f.read((char*)&atk, sizeof(atk));
        f.read((char*)&spd, sizeof(spd)); f.read((char*)&pts, sizeof(pts));
        upg.forceSetLevels(hp, atk, spd, pts);
    }
}

void Menu::executeDelete(int slot) {
    std::remove(("save_slot_" + std::to_string(slot) + ".dat").c_str());
}