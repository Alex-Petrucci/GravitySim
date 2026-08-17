#pragma once

#include <SFML/Graphics.hpp>
#include "Objects.hpp"
#include "Config.hpp"

class Application
{
public:
    Application();

    void run();

private:

    sf::RenderWindow m_window;
    sf::RenderTexture m_trail_texture{sf::Vector2u{INITIAL_WIDTH, INITIAL_HEIGHT}};
    sf::Clock m_clock;
    Objects m_objects;
    sf::Font m_font{"assets/JetBrainsMono/JetBrainsMono-Regular.ttf"};
    sf::Shader m_fade_shader;
    bool m_show_ui = true;
    float m_gravity = G;
    int m_size = 1;
    int m_size_variance = 1;
    int m_max_start_speed = 25;

    void handle_events(const sf::Event& event, float delta_time);
    void update(float delta_time);
    void render(float delta_time);
};