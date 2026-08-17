#include "Application.hpp"
#include "Config.hpp"
#include <sstream>
#include <iostream>

Application::Application()
    : m_window{sf::VideoMode{{INITIAL_WIDTH, INITIAL_HEIGHT}}, TITLE}
{
    m_window.setFramerateLimit(FPS_CAP);
    m_window.setMinimumSize(sf::Vector2u{INITIAL_WIDTH, INITIAL_HEIGHT});
    m_window.setMaximumSize(sf::Vector2u{INITIAL_WIDTH, INITIAL_HEIGHT});

    constexpr std::string_view SHADER_SOURCE = R"(
uniform sampler2D texture;
uniform float fade_factor;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 c = texture2D(texture, uv);

    c.rg *= fade_factor;
    c.b = 0;

    if (c.r < 0.05 && c.g < 0.05 && c.b < 0.05)
    c.rgb = vec3(0.0);

    gl_FragColor = c;
}
)";

    if (!m_fade_shader.loadFromMemory(SHADER_SOURCE, sf::Shader::Type::Fragment))
        std::cout << "Fade shader failed to load!\n";
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
        render(delta_time);
    }
}

void Application::handle_events(const sf::Event& event, float delta_time)
{
    if (event.is<sf::Event::Closed>())
    {
        m_window.close();
    }

    if (const auto mouse_event = event.getIf<sf::Event::MouseButtonPressed>())
    {
        m_objects.add_object({
            .x = static_cast<float>(mouse_event->position.x),
            .y = static_cast<float>(mouse_event->position.y),
            .x_velocity = static_cast<float>(rand() % (m_max_start_speed * 2) - m_max_start_speed),
            .y_velocity = static_cast<float>(rand() % (m_max_start_speed * 2) - m_max_start_speed),
            .mass = static_cast<float>(rand() % m_size_variance + 1 + m_size)
        });
    }

    if (const auto key_event = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key_event->code)
        {
        case sf::Keyboard::Key::Grave:
            m_show_ui = !m_show_ui;
            break;
        case sf::Keyboard::Key::R:
            m_objects.reset();
            break;
        case sf::Keyboard::Key::Up:
            m_size += 1;
            break;
        case sf::Keyboard::Key::Down:
            m_size -= 1;
            if (m_size < 1) m_size = 1;
            break;
        case sf::Keyboard::Key::Left:
            m_size_variance -= 1;
            if (m_size_variance < 0) m_size_variance = 0;
            break;
        case sf::Keyboard::Key::Right:
            m_size_variance += 1;
            break;
        case sf::Keyboard::Key::W:
            m_max_start_speed += 1;
            break;
        case sf::Keyboard::Key::S:
            m_max_start_speed -= 1;
            if (m_max_start_speed < 0) m_max_start_speed = 0;
            break;
        }
    }

    if (const auto scroll_event = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        m_gravity += scroll_event->delta * 16.0f;
    }
}

void Application::update(float delta_time)
{
    m_objects.update(delta_time, m_gravity);
}

void Application::render(float delta_time)
{
    m_window.clear();

    m_fade_shader.setUniform("texture", m_trail_texture.getTexture());
    m_fade_shader.setUniform("fade_factor", 0.95f);

    m_trail_texture.draw(sf::Sprite{m_trail_texture.getTexture()}, &m_fade_shader);

    m_objects.render(m_trail_texture);

    m_trail_texture.display();

    m_window.draw(sf::Sprite{m_trail_texture.getTexture()});

    m_objects.render(m_window);

    if (m_show_ui)
    {
        std::stringstream ss;
        ss << "Debug [`]\nSpawn [Left Click]\nReset Simulation [R]\n"
            "Gravity Strength [Scroll]: " << m_gravity
            << "\nSize [Up/Down]: " << m_size
            << "\nSize Variance [Left/Right]: " << m_size_variance
            << "\nMax Start Speed [W/S]: " << m_max_start_speed
            <<"\nObject Count: " << m_objects.get_live_count()
            << "\nMax Object Count: " << m_objects.get_count()
            << "\nFPS: " << std::round(1.0f / delta_time);
        sf::Text text{m_font, ss.view(), 18};
        text.setFillColor(sf::Color::Green);
        m_window.draw(text);
    }

    m_window.display();
}
