#include "renderer.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>
#include <ctime>
#include <cmath>

Renderer::Renderer(int width, int height, int numCircles, const std::string& title) : 
    width(width), height(height), numCircles(numCircles), GRID_SIZE(4), 
    image(sf::Image(sf::Vector2u(width, height), sf::Color::Transparent)), 
    texture(sf::Texture(image)),
    sprite(sf::Sprite(texture)) {
}

sf::Color Renderer::blend(sf::Color& background, sf::Color& color) {
    float srcAlpha = color.a / 255.0f;  // Alpha del nuovo colore (cerchio)
    float destAlpha = background.a / 255.0f;  // Alpha del pixel esistente
    float outAlpha = srcAlpha + destAlpha * (1 - srcAlpha);  // Alpha risultante

    if (outAlpha == 0) return sf::Color::Transparent;  // Se alpha è 0, il colore è trasparente

    return sf::Color(
        static_cast<unsigned char>((color.r * srcAlpha + background.r * destAlpha * (1 - srcAlpha)) / outAlpha),
        static_cast<unsigned char>((color.g * srcAlpha + background.g * destAlpha * (1 - srcAlpha)) / outAlpha),
        static_cast<unsigned char>((color.b * srcAlpha + background.b * destAlpha * (1 - srcAlpha)) / outAlpha),
        static_cast<unsigned char>(outAlpha * 255)
    );
}