#include "firework.h"


int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "rayquaraz hehe");
    window.setFramerateLimit(60);

    Dragon dragon(1600, 900);
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    float dt = 1.0f / 60.0f;
    unsigned int frameCount = 0;
    std::string baseTitle = "Click to spew! Spacebar to reset! - FPS: ";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                dragon.createPaintSplash(dragon.getHeadPosition());
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                dragon.clearEffects();

            }
        }

        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            float fps = static_cast<float>(frameCount) / fpsClock.restart().asSeconds();
            window.setTitle(baseTitle + std::to_string(static_cast<int>(fps)));
            frameCount = 0;
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        dragon.update(sf::Vector2f(mousePos), dt);

        window.clear(sf::Color::White);  
        dragon.draw(window);
        window.display();

        dt = deltaClock.restart().asSeconds();
    }

    return 0;
}