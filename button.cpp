#include "buttons.h"


Buttons::Buttons(sf::Texture _texture, int x, int y) {
    this->texture = _texture;
    this->x_cord = x;
    this->y_cord = y;
}

sf::Sprite Buttons::open() {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(x_cord, y_cord);
    return sprite;
}

void Buttons::Set_Texture(sf::Texture &_texture) {
    texture = _texture;
}
//
// Created by diksh on 11/29/2023.
//
