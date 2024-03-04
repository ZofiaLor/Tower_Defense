import <SFML/Graphics.hpp>;
import <vector>;
import <string>;
import <map>;
import <utility>;
import <filesystem>;
import <fstream>;
import <regex>;
import <ranges>;
export module Game;
import Tile;
import Turret;
import Projectile;
import Enemy;
import GameObject;
import GameObjectManager;
import ImageButton;
import TextButton;
import User;

export class Game {

    static const enum GameStateType { MainMenu, Level, GameOver, LevelFinished, LogIn, SignIn, TitleScreen, GameFinished };
    GameStateType game_state;
    std::vector<Tile*> tiles;
    sf::RectangleShape shop_bar;
    GameObjectManager gom;
    std::vector<ImageButton*> img_buttons;
    std::map<GameStateType, std::vector<TextButton*>> txt_buttons;
    int buying, price, money, enemies_on_map, current_level;
    sf::Font font;
    sf::Text money_display, level_display;
    sf::Text big_text;
    std::map<std::string, std::vector<std::string>> level_info;
    std::vector<int> level_data;
    int level_progress;
    sf::Clock game_clock;
    sf::Clock delta_time;
    float dt;
    std::regex input_regex;
    sf::Text input_text;
    User user;

public:

    Game() : money(100), level_progress(0), buying(0), price(0), enemies_on_map(0), current_level(1), game_state(TitleScreen), dt(0.0005) {

        font.loadFromFile("text/bahnschrift.ttf");

        for (int i = 1; i < 5; i++) {
            img_buttons.push_back(new ImageButton(i));
        }

        input_regex = "[a-zA-Z0-9]+";

        txt_buttons[MainMenu].push_back(new TextButton(800.0, 120.0, 200.0, 300.0, "START", font));
        txt_buttons[MainMenu].push_back(new TextButton(800.0, 120.0, 200.0, 440.0, "LOG OUT", font));
        txt_buttons[MainMenu].push_back(new TextButton(800.0, 120.0, 200.0, 580.0, "DELETE USER", font));
        txt_buttons[TitleScreen].push_back(new TextButton(800.0, 200.0, 200.0, 300.0, "LOG IN", font));
        txt_buttons[TitleScreen].push_back(new TextButton(800.0, 200.0, 200.0, 525.0, "EXIT", font));
        txt_buttons[LevelFinished].push_back(new TextButton(400.0, 200.0, 150.0, 500.0, "NEXT LEVEL", font));
        txt_buttons[LevelFinished].push_back(new TextButton(400.0, 200.0, 650.0, 500.0, "RETURN TO MENU", font));
        txt_buttons[GameOver].push_back(new TextButton(400.0, 200.0, 150.0, 500.0, "RESTART LEVEL", font));
        txt_buttons[GameOver].push_back(new TextButton(400.0, 200.0, 650.0, 500.0, "RETURN TO MENU", font));
        txt_buttons[SignIn].push_back(new TextButton(400.0, 200.0, 150.0, 500.0, "CONFIRM", font));
        txt_buttons[SignIn].push_back(new TextButton(400.0, 200.0, 650.0, 500.0, "CANCEL", font));
        txt_buttons[GameFinished].push_back(new TextButton(800.0, 200.0, 200.0, 300.0, "START OVER FROM LEVEL 1", font));
        txt_buttons[GameFinished].push_back(new TextButton(800.0, 200.0, 200.0, 525.0, "LOG OUT", font));
        txt_buttons[Level].push_back(new TextButton(135.0, 75.0, 965.0, 30.0, "MENU", font));

        for (auto& itr : std::filesystem::directory_iterator("./text/users")) {
            txt_buttons[LogIn].push_back(new TextButton(800.0, 100.0, 200.0, 75.0, itr.path().stem().string(), font));
        }
        std::ranges::sort(txt_buttons[LogIn], std::ranges::less{}, [](auto const& b) {
            std::string s = b->click();
            std::ranges::transform(s, begin(s), [](unsigned char c) {return std::toupper(c); });
            return s; });
        for (int i = 0; i < txt_buttons[LogIn].size(); i++) {
            txt_buttons[LogIn][i]->setPosition(200.0, 75.0 + 125.0 * i);
        }
        if (txt_buttons[LogIn].size() < 5)
            txt_buttons[LogIn].push_back(new TextButton(800.0, 100.0, 200.0, 75.0 + 125.0 * txt_buttons[LogIn].size(), "ADD NEW USER", font));

        shop_bar.setSize({ 850.f, 75.f });
        shop_bar.setFillColor(sf::Color(81, 131, 207));
        shop_bar.setPosition({ 100.0, 30.0 });

        input_text.setFont(font);
        centerText("", input_text, 350.0, 60);

        money_display.setString("$" + std::to_string(money));
        money_display.setPosition({ 110.0, 40.0 });
        money_display.setFont(font);
        money_display.setCharacterSize(40);

        level_display.setString("LVL 1");
        level_display.setPosition({ 15.0, 760.0 });
        level_display.setFont(font);
        level_display.setFillColor(sf::Color(81, 131, 207));
        level_display.setCharacterSize(25);
        
        big_text.setFont(font);
        centerText("TOWER DEFENSE", big_text, 200.0, 100);

        for (int i = 0; i < 40; i++) {
            tiles.push_back(new Tile(i));
        }

        
        for (auto& itr : std::filesystem::directory_iterator("./text/levels")) {
            std::ifstream file(itr.path().string());
            if (file) {
                std::vector<std::string> v;
                std::string ts;
                while (std::getline(file, ts)) {
                    v.push_back(ts);
                }
                level_info[itr.path().stem().string()] = v;
            }
            file.close();
        }

    }

    ~Game() {
        for (int i = 0; i < tiles.size(); i++) {
            delete tiles[i];
        }
        for (int i = 0; i < img_buttons.size(); i++) {
            delete img_buttons[i];
        }
        for (auto& el : txt_buttons) {
            for (int i = 0; i < el.second.size(); i++) {
                delete el.second[i];
            }
        }
    }

	void run(sf::RenderWindow& window) {
        

        while (window.isOpen())
        {
            switch (game_state) {
            case Level:
                dt = delta_time.restart().asSeconds();
                runLevel(window);
                break;
            case SignIn:
                runSignIn(window);
                break;
            default:
                runMenu(window);
                break;
            }

        }

        
	}

    void runMenu(sf::RenderWindow& window) {
        while (game_state != Level && game_state != SignIn && window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::MouseButtonPressed) {

                    auto mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    for (int i = 0; i < txt_buttons[game_state].size(); i++) {
                        if (txt_buttons[game_state][i]->getBounds().contains(mouse_pos)) {
                            reactToButtonPress(window, txt_buttons[game_state][i]->click());
                            break;
                        }
                    }
                }
            }

            window.clear(sf::Color(175, 198, 233));
            if (game_state != LogIn)
                window.draw(big_text);

            for (int i = 0; i < txt_buttons[game_state].size(); i++) {
                txt_buttons[game_state][i]->Draw(window);
            }

            window.display();
        }
    }

    void reactToButtonPress(sf::RenderWindow& window, const std::string& function) {
        if (game_state != LogIn) {
            //MainMenu
            if (function == "START" || function == "RESTART LEVEL" || function == "NEXT LEVEL") {
                setUpLevel();
                game_state = Level;
            }
            else if (function == "LOG OUT") {
                user.logOut();
                centerText("TOWER DEFENSE", big_text, 200.0, 100);
                game_state = TitleScreen;
            }
            else if (function == "DELETE USER") {
                std::string username = user.getUsername();
                centerText("TOWER DEFENSE", big_text, 200.0, 100);
                game_state = TitleScreen;
                for (int i = 0; i < txt_buttons[LogIn].size() - 1; i++) {
                    if (username == txt_buttons[LogIn][i]->click()) {
                        delete txt_buttons[LogIn][i];
                        txt_buttons[LogIn].erase(txt_buttons[LogIn].begin() + i);
                        for (int j = i; j < txt_buttons[LogIn].size(); j++) {
                            txt_buttons[LogIn][j]->setPosition(200.0, 75.0 + 125.0 * j);
                        }
                        if (txt_buttons[LogIn].size() == 4 && txt_buttons[LogIn][3]->click() != "ADD NEW USER") {
                            txt_buttons[LogIn].push_back(new TextButton(800.0, 100.0, 200.0, 75.0 + 125.0 * 4, "ADD NEW USER", font));
                        }
                        break;
                    }
                }
                user.logOutDeleteUser();
            }
            //TitleScreen
            else if (function == "LOG IN") {
                game_state = LogIn;
            }
            else if (function == "EXIT") {
                window.close();
            }
            //LevelFinished
            else if (function == "RETURN TO MENU") {
                game_state = MainMenu;
                centerText("WELCOME " + user.getUsername(), big_text, 200.0, 100);
            }
            else if (function == "START OVER FROM LEVEL 1") {
                current_level = 1;
                user.startOver();
                setUpLevel();
                game_state = Level;
            }
        }
        //LogIn
        else {
            if (function == "ADD NEW USER") {
                centerText("ENTER NEW USERNAME", big_text, 200.0, 100);
                game_state = SignIn;
            }
            else {
                user.logIn(function);
                centerText("WELCOME " + function, big_text, 200.0, 100);
                current_level = user.getProgress();
                if (current_level > level_info.size()) {
                    current_level = 1;
                    user.startOver();
                }

                game_state = MainMenu;
            }
        }
    }

    void runLevel(sf::RenderWindow& window) {
        manageLevel();
        

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                auto mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (txt_buttons[Level][0]->getBounds().contains(mouse_pos)) {
                    game_state = MainMenu;
                    centerText("WELCOME " + user.getUsername(), big_text, 200.0, 100);
                    break;
                }

                for (int i = 0; i < img_buttons.size(); i++) {
                    if (img_buttons[i]->getBounds().contains(mouse_pos)) {
                        price = img_buttons[i]->click();
                        if (price > 0 && price <= money) {
                            if (buying && buying != i + 1) {
                                img_buttons[buying - 1]->deactivate();
                            }
                            buying = i + 1;
                        }
                        else {
                            img_buttons[i]->deactivate();
                            buying = 0;
                        }
                        break;
                    }
                }
                if (buying) {
                    for (int j = 0; j < 40; j++) {
                        if (tiles[j]->getBounds().contains(mouse_pos) && !tiles[j]->getTurretPlaced()) {
                            gom.addTurret(buying, j);
                            tiles[j]->setTurretPlaced(true);
                            money -= price;

                            img_buttons[buying - 1]->deactivate();
                            buying = 0;
                            break;
                        }
                    }
                }

            }
        }

        window.clear(sf::Color(175, 198, 233));
        window.draw(shop_bar);
        for (int i = 0; i < img_buttons.size(); i++) {
            img_buttons[i]->Draw(window);
        }
        for (int i = 0; i < 40; i++) {
            tiles[i]->Draw(window);
        }
        txt_buttons[Level][0]->Draw(window);
        gom.drawAll(window);
        if (!gom.updateAll(dt, money, enemies_on_map, tiles)) {
            game_state = GameOver;
            
            centerText("GAME OVER", big_text, 200.0, 100);
        }
        money_display.setString("$" + std::to_string(money));

        window.draw(money_display);
        window.draw(level_display);


        window.display();
    }

    

    void runSignIn(sf::RenderWindow& window) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                auto mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                for (int i = 0; i < txt_buttons[SignIn].size(); i++) {
                    if (txt_buttons[SignIn][i]->getBounds().contains(mouse_pos)) {
                        std::string function = txt_buttons[SignIn][i]->click();
                        if (function == "CONFIRM") {
                            addNewUser(input_text.getString());


                        }
                        else if (function == "CANCEL") {
                            game_state = LogIn;
                            input_text.setString("");
                        }
                    }
                }

            }
            else if (event.type == sf::Event::TextEntered) {
                std::string input = input_text.getString();
                if (input.size() > 0 && event.text.unicode == '\b') {
                    input = input.substr(0, input.size() - 1);
                }
                else if (event.text.unicode == 13) {

                    addNewUser(input_text.getString());
                }
                else if (input.size() < 30) {
                    input += (char)event.text.unicode;
                    
                }
                if (input.size() == 0 || std::regex_match(input, input_regex)) {
                    centerText(input, input_text, 350.0, 60);
                }
            }
        }

        window.clear(sf::Color(175, 198, 233));
        window.draw(big_text);
        window.draw(input_text);
        for (int i = 0; i < txt_buttons[SignIn].size(); i++) {
            txt_buttons[SignIn][i]->Draw(window);
        }

        window.display();
    }

    void setUpLevel() {
        level_display.setString("LVL " + std::to_string(current_level));
        gom.clearAll(tiles);
        enemies_on_map = 0;
        level_data.clear();
        std::regex reg("[0-9]+");

        for (std::string line : level_info["level_" + std::to_string(current_level)]) {
            if (std::regex_match(line, reg)) {
                level_data.push_back(std::stoi(line));
            }
        }

        money = level_data[0];
        level_progress = 1;
        game_clock.restart();

    }

    void manageLevel() {
        if (level_progress == level_data.size() && enemies_on_map == 0) {
            if (current_level == level_info.size()) {
                game_state = GameFinished;
                centerText("CONGRATULATONS!", big_text, 200.0, 100);
            }
            else {
                game_state = LevelFinished;
                centerText("LEVEL " + std::to_string(current_level) + " FINISHED", big_text, 200.0, 100);
            }
            
            user.nextLevel();
            current_level++;
            level_display.setString("LVL " + std::to_string(current_level));
            gom.clearAll(tiles);
        }
        else if (level_progress < level_data.size() && game_clock.getElapsedTime().asSeconds() >= level_data[level_progress + 2]) {
            gom.addEnemy(level_data[level_progress], level_data[level_progress + 1] * 8 - 1);
            enemies_on_map++;
            level_progress += 3;
            game_clock.restart();
        }
    }

    void centerText(const std::string& txt, sf::Text& text, const float& y_pos, int size) {
        text.setCharacterSize(size);
        text.setString(txt);
        sf::FloatRect textRect = text.getLocalBounds();
        while (textRect.width >= 1200.f) {
            size -= 10;
            text.setCharacterSize(size);
            textRect = text.getLocalBounds();
        }
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(600.0, y_pos);
    }

    void addNewUser(std::string text) {

        if (text == "") {
            return;
        }
        for (int i = 0; i < txt_buttons[LogIn].size(); i++) {
            if (text == txt_buttons[LogIn][i]->click()) {
                return;
            }
        }

        game_state = MainMenu;
        current_level = 1;

        user.signIn(text);
        centerText("WELCOME " + user.getUsername(), big_text, 200.0, 100);

        

        txt_buttons[LogIn][txt_buttons[LogIn].size() - 1]->setText(text);
        // Dodatkowa instrukcja transform - wielkie litery maja mniejsza wartosc kodu ASCII 'Z' < 'a'
        std::ranges::sort(txt_buttons[LogIn], std::ranges::less{}, [](auto const& b) {
            std::string s = b->click();
            std::ranges::transform(s, begin(s), [](unsigned char c) {return std::toupper(c); });
            return s; });
        for (int i = 0; i < txt_buttons[LogIn].size(); i++) {
            txt_buttons[LogIn][i]->setPosition(200.0, 75.0 + 125.0 * i);
        }
        if (txt_buttons[LogIn].size() < 5)
            txt_buttons[LogIn].push_back(new TextButton(800.0, 100.0, 200.0, 75.0 + 125.0 * txt_buttons[LogIn].size(), "ADD NEW USER", font));

        input_text.setString("");
    }

};