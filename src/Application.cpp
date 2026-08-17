#include "Application.hpp"
#include "config.hpp"
#include "../dep/SFML-3.1.0/src/SFML/Window/InputImpl.hpp"

Application::Application()
    : m_window{sf::VideoMode{{INITIAL_WIDTH, INITIAL_HEIGHT}}, TITLE}
{
    m_window.setFramerateLimit(FPS_CAP);
}

void Application::run()
{
    while (m_window.isOpen())
    {
        const float delta_time = m_clock.restart().asSeconds();

        while (const std::optional event = m_window.pollEvent())
        {
            handle_events(*event, delta_time);
        }

        update(delta_time);
        render();
    }
}

void Application::handle_events(const sf::Event& event, float delta_time)
{
    if (event.is<sf::Event::Closed>())
    {
        m_window.close();
    }

    if (event.is<sf::Event::MouseButtonPressed>())
    {
        const auto mouse_pos = sf::Mouse::getPosition(m_window);
        m_objects.add_object({
            .x = static_cast<float>(mouse_pos.x),
            .y = static_cast<float>(mouse_pos.y),
            .x_velocity = static_cast<float>(rand() % 50 - 25),
            .y_velocity = static_cast<float>(rand() % 50 - 25),
            .mass = 1.0f
        });
    }
}

void Application::update(float delta_time)
{
    m_objects.update(delta_time);
}

void Application::render()
{
    m_window.clear();

    m_objects.render(m_window);

    m_window.display();
}
