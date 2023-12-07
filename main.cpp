#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <random>
#include "random.h"
#include "buttons.h"
#include <fstream>
#include <unordered_map>
#include <chrono>
using namespace std;
using namespace chrono;


// Function to read grid dimensions from the config file
void readConfigFile(int& rows, int& cols, int& totalMines) {
    ifstream configFile("config.cfg");
    if (configFile.is_open()) {
        configFile >> rows >> cols >> totalMines;
        configFile.close();
    } else {
        rows = 10;
        cols = 10;
        totalMines = 10;
    }
}




class Tile {
public:
    Tile():
            position(sf::Vector2f(0.0f, 0.0f)), // Initialize position with a default value
            revealsprite(), // Initialize revealsprite using default constructor
            texture(), // Initialize texture using default constructor
            mine_texture(), // Initialize mine_texture using default constructor
            mine_sprite(), // Initialize mine_sprite using default constructor
            flag_texture(), // Initialize flag_texture using default constructor
            flag_sprite(), // Initialize flag_sprite using default constructor
            num_sprite(), // Initialize num_sprite using default constructor
            flagged(false), // Initialize flagged with a default value
            mine(false), // Initialize mine with a default value
            revealed(false), // Initialize revealed with a default value
            num(false), // Initialize num with a default value
            neighbor_bombs(0), // Initialize neighbor_bombs with a default value
            value(0)
    {}
    sf::Vector2f position;
    sf::Sprite revealsprite;
    sf::Texture texture;
    sf::Texture mine_texture;
    sf::Sprite mine_sprite;
    sf::Texture flag_texture;
    sf::Sprite flag_sprite;
    sf::Sprite num_sprite;


    bool flagged;
    bool mine;
    bool revealed;
    bool num;


    vector<Tile*> neighbors; // adjacent tiles


    int neighbor_bombs;
    int value; // 1 for mine 0 for not a mine


    Tile(sf::Vector2f _position, string _texture){
        texture.loadFromFile(_texture);
        revealsprite.setTexture(texture);
        revealsprite.setPosition(_position.x, _position.y);
        position = _position;




        flagged = false;
        mine = false;
        revealed = false;
        num = false;
        neighbor_bombs = 0;
        value = 0;
    }




    void setmine(string mineTextureFile) {
        mine = true;
        value = 1;
        mine_texture.loadFromFile(mineTextureFile);
        mine_sprite = sf::Sprite(mine_texture);
    }




    void flag(string flagTextureFile) {
        flagged = !flagged;
        flag_texture.loadFromFile(flagTextureFile); // Renamed the variable
        flag_sprite = sf::Sprite(flag_texture);
    }




    void reveal(sf::Texture& _texture) {
        revealed = true;
        revealsprite = sf::Sprite(_texture);
    }


    void setnum(sf::Texture& _texture) {
        num = true;
        num_sprite = sf::Sprite(_texture);
    }
};




class Board {
public:
    int cols;
    int rows;
    int mines;
    vector<vector<Tile>> tiles;


    Board(int _cols, int _rows, int _mines)
            : cols(_cols), rows(_rows), mines(_mines) {
        tiles.resize(cols, vector<Tile>(rows));


        initializeBoard();
    }


    void initializeBoard() {
        int mineCount = 0;


        while (mineCount < mines) {
            int randX = Random::Int(0, cols - 1);
            int randY = Random::Int(0, rows - 1);


            if (!tiles[randX][randY].mine) {
                tiles[randX][randY].setmine("images/mine.png");
                mineCount++;
                cout << "mine have been set" << endl;
            }
        }


        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                determineNeighborMines(i, j);
                cout << "neighboring mines have been determined"<< endl;
            }
        }
    }


    void determineNeighborMines(int x, int y) {
        // Define the boundary limits for neighboring tiles
        int startX = max(0, x - 1);
        int startY = max(0, y - 1);
        int endX = min(cols - 1, x + 1);
        int endY = min(rows - 1, y + 1);


        // Loop through the neighboring tiles within the boundaries
        for (int i = startX; i <= endX; ++i) {
            for (int j = startY; j <= endY; ++j) {
                if (tiles[i][j].mine && (i != x || j != y)) {
                    ++tiles[x][y].neighbor_bombs;
                    cout << "figuring out neighboring mines works" << endl;
                }
            }
        }
    }




    void revealNeighbors(sf::Texture& revealTexture, unordered_map<int, sf::Texture>& numbers, int x, int y) {
        for (int i = x - 1; i <= x + 1; ++i) {
            for (int j = y - 1; j <= y + 1; ++j) {
                if (i >= 0 && i < cols && j >= 0 && j < rows) {
                    Tile& currentTile = tiles[i][j];
                    if (!currentTile.mine && !currentTile.flagged && !currentTile.revealed) {
                        currentTile.reveal(revealTexture);
                        if (currentTile.neighbor_bombs != 0) {
                            currentTile.setnum(numbers[currentTile.neighbor_bombs]);
                        } else {
                            revealNeighbors(revealTexture, numbers, i, j);
                            cout << "reveal neighbors" << endl;
                        }
                    }
                }
            }
        }
    }




    void ResetBoard(const sf::Texture& defaultTexture, int columns, int rows, int mines) {
        cols = columns;
        rows = rows;
        mines = mines;


        // Reset the board state here
        for (auto& col : tiles) {
            for (auto& tile : col) {
                tile.mine = false;
                tile.flagged = false;
                tile.revealed = false;
                tile.num = false;
                tile.neighbor_bombs = 0;
                // Reset textures or other properties as needed
                tile.revealsprite.setTexture(defaultTexture);
                // Reset other sprites or textures if required
                cout << "resetting the board" << endl;
            }
        }


        // Reinitialize the board if needed
        tiles.clear();
        tiles.resize(cols, vector<Tile>(rows));
        initializeBoard();
    }


    bool winnerwinnerchickendinner() {
        for (const auto& col : tiles) {
            for (const auto& tile : col) {
                if (tile.mine && !tile.revealed) {
                    return false;
                }
            }
        }
        return true;
    }
};




void DrawTitle(sf::RenderWindow& window) {
    const float height = 600;
    const float width = 800;


    sf::Font font;
    font.loadFromFile("font.ttf");


    sf::Text title("WELCOME TO MINESWEEPER", font, 24);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);


    sf::FloatRect TitleRect = title.getLocalBounds();
    title.setOrigin(TitleRect.left + TitleRect.width / 2.0f, TitleRect.top + TitleRect.height / 2.0f);
    title.setPosition(width / 2.0f, height / 2.0f - 150);


    window.draw(title);
}


void playername(sf::RenderWindow& window) {
    const float height = 600;
    const float width = 800;


    sf::Font font;
    font.loadFromFile("font.ttf");


    sf::Text prompt("Enter your name:", font, 20);
    prompt.setFillColor(sf::Color::White);
    prompt.setStyle(sf::Text::Bold);


    sf::FloatRect PromptRect = prompt.getLocalBounds();
    prompt.setOrigin(PromptRect.left + PromptRect.width / 2.0f, PromptRect.top + PromptRect.height / 2.0f);
    prompt.setPosition(width / 2.0f, height / 2.0f - 75);


    window.draw(prompt);
}






string capitalizeString(const string& input) {
    string modifiedInput = input;
    if (!modifiedInput.empty()) {
        modifiedInput[0] = toupper(modifiedInput[0]);
        for (size_t i = 1; i < modifiedInput.length(); ++i) {
            if (modifiedInput[i - 1] == ' ') {
                modifiedInput[i] = toupper(modifiedInput[i]);
            } else {
                modifiedInput[i] = tolower(modifiedInput[i]);
            }
        }
    }
    return modifiedInput;
}


string print_leaderboard(const map<string, string>& _players) {
    string output = "";
    int index = 1;


    for (auto iter = _players.begin(); iter != _players.end(); ++iter) {
        output += to_string(index) + ". \t" + iter->first + "\t" + iter->second + "\n\n";
        ++index;
    }


    return output;
}


string fetchLeaderboardContent(const std::vector<std::pair<std::string, std::string>>& players) {
    // Code to fetch and format leaderboard content based on the 'players' vector
    std::string leaderboardContent;


    // Example logic to generate content from 'players'
    for (const auto& player : players) {
        leaderboardContent += player.first + "," + player.second + "\n";
    }


    return leaderboardContent;
}






int main() {
    bool firsttimearound = true;




    const float height = 600;
    const float width = 800;


    unordered_map<string, sf::Texture> textures;
    textures["mine"].loadFromFile("images/mine.png");
    textures["unrevealed"].loadFromFile("images/tile_hidden.png");
    textures["revealed"].loadFromFile("images/tile_revealed.png");
    textures["flag"].loadFromFile("images/flag.png");
    textures["play"].loadFromFile("images/play.png");
    textures["pause"].loadFromFile("images/pause.png");
    textures["win"].loadFromFile("images/face_win.png");
    textures["sad"].loadFromFile("images/face_lose.png");
    textures["happy_face"].loadFromFile("images/face_happy.png");






    unordered_map<int, sf::Texture> numbers;
    numbers[1].loadFromFile("images/number_1.png");
    numbers[2].loadFromFile("images/number_2.png");
    numbers[3].loadFromFile("images/number_3.png");
    numbers[4].loadFromFile("images/number_4.png");
    numbers[5].loadFromFile("images/number_5.png");
    numbers[6].loadFromFile("images/number_6.png");
    numbers[7].loadFromFile("images/number_7.png");
    numbers[8].loadFromFile("images/number_8.png");






    unordered_map<int, sf::Texture> clock;
    clock[0].loadFromFile("images/digits.png", sf::IntRect(0, 0, 21, 32));
    clock[1].loadFromFile("images/digits.png", sf::IntRect(21, 0, 21, 32));
    clock[2].loadFromFile("images/digits.png", sf::IntRect(42, 0, 21, 32));
    clock[3].loadFromFile("images/digits.png", sf::IntRect(63, 0, 21, 32));
    clock[4].loadFromFile("images/digits.png", sf::IntRect(84, 0, 21, 32));
    clock[5].loadFromFile("images/digits.png", sf::IntRect(105, 0, 21, 32));
    clock[6].loadFromFile("images/digits.png", sf::IntRect(126, 0, 21, 32));
    clock[7].loadFromFile("images/digits.png", sf::IntRect(147, 0, 21, 32));
    clock[8].loadFromFile("images/digits.png", sf::IntRect(168, 0, 21, 32));
    clock[9].loadFromFile("images/digits.png", sf::IntRect(189, 0, 21, 32));
    clock[10].loadFromFile("images/digits.png", sf::IntRect(210, 0, 21, 32));




    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);


    //usertext
    sf::Font font;
    font.loadFromFile("font.ttf");


    sf::Text playernam("", font, 18);
    playernam.setOrigin(playernam.getLocalBounds().width / 2.0f, playernam.getLocalBounds().height / 2.0f);
    playernam.setPosition(width / 2.0f, height / 2.0f - 45);
    playernam.setStyle(sf::Text::Bold);
    playernam.setFillColor(sf::Color::Yellow);


    string userinput = playernam.getString();
    if (!userinput.empty()) {
        userinput[0] = toupper(userinput[0]);
    }
    playernam.setString(userinput);


    //cursor stuff
    sf::Text cursor("|", font, 18);
    cursor.setOrigin(cursor.getLocalBounds().width / 2.0f, cursor.getLocalBounds().height / 2.0f);
    cursor.setPosition(width / 2.0f + 5, height / 2.0f - 45);
    cursor.setStyle(sf::Text::Bold);


    string input;
    string final_name;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }


            if (event.type == sf::Event::TextEntered) {
                if (isalpha(event.text.unicode)) {
                    char enteredChar = static_cast<char>(event.text.unicode);
                    if (input.size() < 10) {
                        enteredChar = (input.empty() || input.back() == ' ') ?
                                      toupper(enteredChar) : tolower(enteredChar);
                        input += enteredChar;
                        playernam.setString(input);
                        sf::FloatRect NameRect = playernam.getLocalBounds();
                        playernam.setOrigin(NameRect.left + NameRect.width / 2.0f, NameRect.top + NameRect.height / 2.0f);
                        playernam.setPosition(width / 2.0f, height / 2.0f - 45);
                        cursor.move(6, 0);
                    }
                }
            }


            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!input.empty()) {
                        input.pop_back();
                        playernam.setString(input);
                        sf::FloatRect NameRect = playernam.getLocalBounds();
                        playernam.setOrigin(NameRect.left + NameRect.width / 2.0f,
                                            NameRect.top + NameRect.height / 2.0f);
                        playernam.setPosition(width / 2.0f, height / 2.0f - 45);
                        cursor.move(-6, 0);
                    }
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if (!input.empty()) {
                        window.close();
                    }
                }
            }
        }


        final_name = capitalizeString(input) + "*";
        window.clear(sf::Color::Blue);
        DrawTitle(window);
        playername(window);
        window.draw(playernam);
        window.draw(cursor);
        window.display();
    }


    int bcol, brow, bmine;
    readConfigFile(bcol, brow, bmine);


    // Use the obtained values for your Board initialization and game window dimensions
    Board board(bcol, brow, bmine);
    int gameWidth = bcol * 32;
    int gameHeight = (brow * 32) + 100;


    sf::RenderWindow game(sf::VideoMode(gameWidth, gameHeight), "Minesweeper",sf::Style::Close | sf::Style::Titlebar);


    // Render button sprites
    sf::Texture face_happy1;
    face_happy1.loadFromFile("images/face_happy.png");
    Buttons happy(face_happy1, (((bcol) / 2.0) * 32) - 32, 32 * (brow + 0.5f));


    sf::Texture debug1;
    debug1.loadFromFile("images/debug.png");
    Buttons debug(debug1, (((bcol) * 32) - 304), 32 * (brow + 0.5f));


    sf::Texture pause1;
    pause1.loadFromFile("images/pause.png");
    Buttons pause(pause1, (((bcol) * 32) - 240), 32 * (brow + 0.5f));


    sf::Texture leaderboard1;
    leaderboard1.loadFromFile("images/leaderboard.png");
    Buttons leader_button(leaderboard1, (((bcol) * 32) - 176), 32 * (brow + 0.5f));


    // Load Timer Sprites
    sf::Texture defaultTime;
    defaultTime.loadFromFile("images/digits.png", sf::IntRect(0, 0, 21, 32));


// Position variables
    int minuteCol1PosX = (bcol) * 32 - 97;
    int minuteCol2PosX = (bcol) * 32 - 97 + 21;
    int secCol1PosX = (bcol) * 32 - 54;
    int secCol2PosX = (bcol) * 32 - 54 + 21;
    int timerPosY = 32 * ((brow) + 0.5f) + 16;


    Buttons minuteCol1(defaultTime, minuteCol1PosX, timerPosY);
    Buttons minuteCol2(defaultTime, minuteCol2PosX, timerPosY);
    Buttons secCol1(defaultTime, secCol1PosX, timerPosY);
    Buttons secCol2(defaultTime, secCol2PosX, timerPosY);


// ... (other logic)


// Later in your code, you can use these variables to update positions if needed
// For example:
    //minuteCol1.SetPosition(newPosX, newPosY);




    int counterValue0 = 0, counterValue1 = 0, counterValue2 = 0;
    int digitCount = 0;


    if (bmine >= 0 && bmine < 1000) {
        digitCount = (bmine >= 100) ? 3 : ((bmine >= 10) ? 2 : 1);


        if (digitCount == 1) {
            counterValue2 = bmine;
        } else if (digitCount == 2) {
            counterValue1 = bmine / 10;
            counterValue2 = bmine % 10;
        } else if (digitCount == 3) {
            counterValue0 = bmine / 100;
            counterValue1 = (bmine % 100) / 10;
            counterValue2 = bmine % 10;
        }
    }


    Buttons countButton0(clock[counterValue0], 33, 32 * ((brow) + 0.5f) + 16);
    Buttons countButton1(clock[counterValue1], 54, 32 * ((brow) + 0.5f) + 16);
    Buttons countButton2(clock[counterValue2], 75, 32 * ((brow) + 0.5f) + 16);
    Buttons negativeButton(clock[10], 12, 32 * ((brow) + 0.5f) + 16);




    // board logic starts here
    bool debugbool = false;
    bool won = false;
    bool lost = false;
    bool pausebool = false;
    bool reset = false;
    bool leader = false;


    // flags
    int totalflags = 0;
    int flagsPlaced = 0;


    // mines
    int revealed = 0;
    int counter;
    int sum1;
    int sum2;
    int sum3;
    bool negative;
    bool render;


    // timer
    int minutes;
    int seconds;
    bool havetheywon = false;


    // leaderboard stuff
    ifstream leader_file("leaderboard.txt");
    vector<pair<string, string>> players;


    for (int i = 0; i < 5; ++i) {
        string line;
        getline(leader_file, line);
        string name = line.substr(line.find(",") + 1);
        string time = line.substr(0, line.find(","));
        players.emplace_back(time, name);
    }


    sort(players.begin(), players.end());
    map<string, string> leaderboard;
    for (const auto& player : players) {
        leaderboard.emplace(player.first, player.second);
    }




    auto started = chrono::high_resolution_clock::now();
    auto paused = chrono::high_resolution_clock::time_point();
    auto elapsed_paused_time = 0;


    while (game.isOpen()) {
        auto currenttime = chrono::high_resolution_clock::now();
        auto runtime = chrono::duration_cast<chrono::seconds>(currenttime - started);
        int totaltime = static_cast<int>(runtime.count());


        if (paused != chrono::high_resolution_clock::time_point()) {
            elapsed_paused_time = chrono::duration_cast<chrono::seconds>(currenttime - paused).count();
            paused = chrono::high_resolution_clock::time_point();
        }


        if (elapsed_paused_time > 0) {
            totaltime -= elapsed_paused_time;
            minutes = totaltime / 60;
            seconds = totaltime % 60;
        }


        int tens_minutes = minutes / 10 % 10;
        int ones_minutes = minutes % 10;
        int tens_seconds = seconds / 10 % 10;
        int ones_seconds = seconds % 10;


        // Create a string version of the time
        string minutes_final = (minutes < 10) ? "0" + to_string(minutes) : to_string(minutes);
        string seconds_final = (seconds < 10) ? "0" + to_string(seconds) : to_string(seconds);


        string final_time = minutes_final + ":" + seconds_final;
        game.clear(sf::Color::White);


        if (board.winnerwinnerchickendinner()) {
            won = true;
            havetheywon = true;
        }


        for (int i = 0; i < board.cols; ++i) {
            for (int j = 0; j < board.rows; ++j) {
                auto& currentTile = board.tiles[i][j];
                currentTile.revealsprite.setPosition(32 * i, 32 * j);
                game.draw(currentTile.revealsprite);


                if (paused == chrono::high_resolution_clock::time_point()) {
                    sf::Sprite temp(textures["revealed"]);
                    temp.setPosition(currentTile.revealsprite.getPosition());
                    game.draw(temp);
                }


                bool drawMineSprite = (currentTile.mine && currentTile.revealed && paused != chrono::high_resolution_clock::time_point()) || (debugbool && !won && !lost);
                bool drawFlagSprite = currentTile.flagged || (won && currentTile.mine && !currentTile.flagged);


                if (drawMineSprite || drawFlagSprite) {
                    sf::Sprite& spriteToDraw = drawMineSprite ? currentTile.mine_sprite : currentTile.flag_sprite;
                    spriteToDraw.setPosition(32 * i, 32 * j);
                    game.draw(spriteToDraw);
                }


                if (won || lost) {
                    if (won) {
                        sf::Sprite face_happy;
                        face_happy.setTexture(textures["face_happy"]);


                        if (currentTile.mine && !currentTile.flagged) {
                            currentTile.flag("images/flag.png");
                            currentTile.flag_sprite.setPosition(32 * i, 32 * j);
                            game.draw(currentTile.flag_sprite);
                        }


                        if (havetheywon && firsttimearound) {
                            ofstream leader_file("leaderboard.txt", ios_base::app);
                            leader_file << final_name << "," << final_time << "\n";
                            leader_file.close();


                            if (havetheywon && firsttimearound) {
                                havetheywon = false;
                                firsttimearound = false;
                                players.emplace_back(final_time, final_name);


                                // Append new player data to the leaderboard file
                                ofstream leader_file("leaderboard.txt", ios_base::app); // Append mode
                                leader_file << final_time << "," << final_name << "\n";
                                leader_file.close(); // Close the file after writing the new player data


                                // Display the leaderboard window
                                sf::RenderWindow leaderboard(sf::VideoMode(800, 600), "Leaderboard", sf::Style::Close);
                                sf::Font font; // Load your font if not already loaded
                                // ... (Load font and other necessary textures if not already loaded)


                                sf::Text Leader_title;
                                Leader_title.setFont(font);
                                Leader_title.setCharacterSize(20);
                                Leader_title.setFillColor(sf::Color::White);
                                Leader_title.setString("LEADERBOARD");
                                Leader_title.setStyle(sf::Text::Bold | sf::Text::Underlined);
                                Leader_title.setPosition(350, 50); // Adjust position as needed




                                sf::Text Player_list;
                                Player_list.setFont(font);
                                Player_list.setCharacterSize(18);
                                Player_list.setStyle(sf::Text::Bold);
                                Player_list.setFillColor(sf::Color::White);




                                // Fetch the content of the leaderboard
                                std::string leaderboardContent = fetchLeaderboardContent(players);
                                Player_list.setString(leaderboardContent); // Set string here
                                Player_list.setPosition(100, 100);


                                while (leaderboard.isOpen()) {
                                    sf::Event event;
                                    while (leaderboard.pollEvent(event)) {
                                        if (event.type == sf::Event::Closed) {
                                            leaderboard.close();
                                        }
                                    }


                                    leaderboard.clear(sf::Color::Blue);
                                    leaderboard.draw(Leader_title);
                                    leaderboard.draw(Player_list);
                                    leaderboard.display();
                                }
                            }




                        }
                    } else if (lost) {
                        sf::Sprite face_happy1;
                        face_happy1.setTexture(textures["sad"]);
                        if (currentTile.mine && !currentTile.flagged) {
                            currentTile.revealsprite.setTexture(textures["revealed"]);
                            currentTile.revealsprite.setPosition(32 * i, 32 * j);
                            currentTile.mine_sprite.setPosition(32 * i, 32 * j);
                            game.draw(currentTile.revealsprite);
                            game.draw(currentTile.mine_sprite);
                        }
                    }


                    if (reset) {
                        reset = false;
                        firsttimearound = true;


                        if (won) {
                            won = false;
                            sf::Sprite face_happy1;
                            face_happy1.setTexture(textures["happy_face"]);
                        } else {
                            lost = false;
                            sf::Sprite face_happy1;
                            face_happy1.setTexture(textures["happy_face"]);
                        }
                    }
                }
            }
            sf::Event magicEvent;
            while (game.pollEvent(magicEvent)) {
                if (magicEvent.type == sf::Event::Closed) {
                    game.close();
                }


                if (magicEvent.type == sf::Event::MouseButtonPressed) {
                    sf::Mouse wand;
                    auto spellCoordinates = wand.getPosition(game); // Coordinates of enchantment
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {


                        auto startTime = chrono::high_resolution_clock::now();


                        //If restart button is clicked
                        sf::FloatRect happyRect = happy.open().getGlobalBounds();
                        if (happyRect.contains(spellCoordinates.x, spellCoordinates.y)) {
                            bool paused = false;
                            reset = true;
                            happy.Set_Texture(textures["happy_face"]);
                            board.ResetBoard(textures["unrevealed"], bcol, brow, bmine);


                            flagsPlaced = 0;
                            totalflags = 0;
                            revealed = 0;


                            // Reset timer
                            startTime = chrono::high_resolution_clock::now();


                            // Reset counter
                            negative = false;
                            countButton0.Set_Texture(clock[counterValue0]);
                            countButton1.Set_Texture(clock[counterValue1]);
                            countButton2.Set_Texture(clock[counterValue2]);
                        }


                        // If leaderboard button is clicked
                        auto leaderboardButtonRect = leader_button.open().getGlobalBounds();
                        if (leaderboardButtonRect.contains(spellCoordinates.x, spellCoordinates.y)) {
                            leader = !leader;


                            // Draw revealed tiles if leaderboard is activated
                            if (leader) {
                                for (int i = 0; i < board.cols; i++) {
                                    for (int j = 0; j < board.rows; j++) {
                                        sf::Sprite revealedTile(textures["revealed"]);
                                        revealedTile.setPosition(board.tiles[i][j].revealsprite.getPosition());
                                        game.draw(revealedTile);
                                    }
                                }


                                game.draw(secCol2.open());
                                game.draw(secCol1.open());
                                game.draw(minuteCol2.open());
                                game.draw(minuteCol1.open());


                                if (negative) {
                                    game.draw(negativeButton.open());
                                }
                                game.draw(happy.open());
                                game.draw(pause.open());
                                game.draw(leader_button.open());
                                game.draw(debug.open());
                                game.draw(countButton0.open());
                                game.draw(countButton1.open());
                                game.draw(countButton2.open());


                                game.display();


                                if (leader) {
                                    paused = chrono::high_resolution_clock::now();


                                    // Render the leaderboard window
                                    sf::RenderWindow leaderboardWindow(sf::VideoMode(16 * board.cols, (16 * board.rows) + 50),
                                                                       "Leaderboard", sf::Style::Close | sf::Style::Titlebar);
                                    // ... (leaderboard title and player list setup)
                                    sf::Text Leader_title;
                                    Leader_title.setFont(font);
                                    Leader_title.setCharacterSize(20);
                                    Leader_title.setFillColor(sf::Color::White);
                                    Leader_title.setString("LEADERBOARD");
                                    Leader_title.setStyle(sf::Text::Bold | sf::Text::Underlined);
                                    sf::FloatRect leader_Rect = Leader_title.getLocalBounds();
                                    Leader_title.setOrigin(leader_Rect.left + leader_Rect.width / 2.0f,
                                                           leader_Rect.top + leader_Rect.height / 2.0f);
                                    Leader_title.setPosition(16 * bcol / 2.0f, 16 * brow / 2.0f - 120);




                                    sf::Text Player_list;
                                    Player_list.setFont(font);
                                    Player_list.setCharacterSize(18);
                                    Player_list.setStyle(sf::Text::Bold);
                                    Player_list.setFillColor(sf::Color::White);
                                    //Player_list.setString(print_leaderboard(players));
                                    sf::FloatRect list_Rect = Player_list.getLocalBounds();
                                    Player_list.setOrigin(list_Rect.left + list_Rect.width / 2.0f,
                                                          list_Rect.top + list_Rect.height / 2.0f);
                                    Player_list.setPosition(16 * bcol / 2.0f, 16 * brow / 2.0f + 20);


                                    while (leaderboardWindow.isOpen()) {
                                        sf::Event event;
                                        while (leaderboardWindow.pollEvent(event)) {
                                            if (event.type == sf::Event::Closed) {
                                                leader = false;
                                                leaderboardWindow.close();
                                                auto resumeTime = chrono::high_resolution_clock::now();
                                                elapsed_paused_time += chrono::duration_cast<chrono::seconds>(resumeTime - paused).count();
                                            }
                                        }
                                        
                                        leaderboardWindow.clear(sf::Color::Blue);
                                        leaderboardWindow.draw(Leader_title);
                                        leaderboardWindow.draw(Player_list);
                                        leaderboardWindow.display();
                                    }
                                }
                            }
                            if (paused != chrono::high_resolution_clock::time_point()) {          // checks if game is paused
                                for (int i = 0; i < board.cols; i++) {
                                    for (int j = 0; j < board.rows; j++) {
                                        auto Tile_rect = board.tiles[i][j].revealsprite.getGlobalBounds();         // gets global bounds of an individual tile
                                        if (Tile_rect.contains(spellCoordinates.x, spellCoordinates.y)) {
                                            if (board.tiles[i][j].mine and
                                                !board.tiles[i][j].flagged) {            // checks if a tile is a mine and that if it is flagged
                                                board.tiles[i][j].reveal(textures["revealed"]);              // reveals the tile
                                                lost = true;

                                            } else if (!board.tiles[i][j].flagged) {                     // checks if a tile is flagged
                                                if (board.tiles[i][j].neighbor_bombs != 0) {
                                                    board.tiles[i][j].reveal(textures["revealed"]);
                                                    board.tiles[i][j].setnum(numbers[board.tiles[i][j].neighbor_bombs]);
                                                } else if (board.tiles[i][j].neighbor_bombs == 0) {
                                                    board.revealNeighbors(textures["revealed"], numbers, i, j);
                                                }

                                            }
                                        }
                                    }
                                }

                                // if debug button is clicked
                                auto Debug_Rect = debug.open().getGlobalBounds();
                                if (Debug_Rect.contains(spellCoordinates.x, spellCoordinates.y)) {
                                    debugbool = !debugbool;           // flips the debug boolean value
                                }

                            }
                        }
                    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                        if (!won and !lost) {
                            for (int i = 0; i < board.cols; i++) {
                                for (int j = 0; j < board.rows; j++) {
                                    auto Flag_rect = board.tiles[i][j].revealsprite.getGlobalBounds();
                                    if (Flag_rect.contains(spellCoordinates.x, spellCoordinates.y) and !board.tiles[i][j].revealed) {
                                        if (board.tiles[i][j].flagged){
                                            flagsPlaced--;
                                        }
                                        else{
                                            flagsPlaced++;
                                        }
                                        board.tiles[i][j].flag("image/flag.png");








                                        counter = bmine - flagsPlaced;
                                        if (counter < 0) {
                                            negative = true;
                                            sum1 = -1 * (counter / 100);
                                            sum2 = -1 * (counter / 10);
                                            sum3 = -1 * (counter % 10);
                                        } else {
                                            negative = false;
                                            if (digitCount == 1) {
                                                sum3 = bmine - flagsPlaced;
                                            } else if (digitCount == 2) {
                                                sum2 = counter / 10;
                                                sum3 = counter % 10;








                                            } else if (digitCount == 3) {
                                                sum1 = counter / 100;
                                                sum2 = (counter % 100) / 10;
                                                sum3 = counter % 10;
                                            }








                                        }
                                        // Change the counter texture
                                        countButton0.Set_Texture(clock[sum1]);
                                        countButton1.Set_Texture(clock[sum2]);
                                        countButton2.Set_Texture(clock[sum3]);
                                    }
                                }
                            }
                        }
                    }
                }
            }


            if (!won and !lost) {
                minuteCol1.Set_Texture(clock[tens_minutes]);
                minuteCol2.Set_Texture(clock[ones_minutes]);
                secCol1.Set_Texture(clock[tens_seconds]);
                secCol2.Set_Texture(clock[ones_seconds]);
            }


            game.draw(secCol2.open());
            game.draw(secCol1.open());
            game.draw(minuteCol2.open());
            game.draw(minuteCol1.open());


            if(negative){
                game.draw(negativeButton.open());
            }
            game.draw(happy.open());
            game.draw(pause.open());
            game.draw(leader_button.open());
            game.draw(debug.open());
            game.draw(countButton0.open());
            game.draw(countButton1.open());
            game.draw(countButton2.open());

        };
        game.display();\
    };
    return 0;
};

