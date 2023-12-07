#ifndef PROJECT3_BUTTONS_H
#define PROJECT3_BUTTONS_H
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
using namespace std;


class Buttons {
private:
    sf::Texture texture;
    int x_cord;
    int y_cord;

public:
    Buttons(sf::Texture  _texture, int x, int y);
    sf::Sprite open();
    void Set_Texture(sf::Texture &_texture);

};


#endif //PROJECT3_BUTTONS_H