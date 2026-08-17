#pragma once

#include <SFML/Graphics.hpp>
#include "Objects.hpp"

class Application
{
public:
    Application();

    void run();

private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    Objects m_objects;

    void handle_events(const sf::Event& event, float delta_time);
    void update(float delta_time);
    void render();
};