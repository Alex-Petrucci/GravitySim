#include "Objects.hpp"
#include "Config.hpp"

ObjectsViewer::Iterator::Iterator(Objects& objects, size_t index)
    : m_objects{objects}
    , m_index{index}
{}

ObjectView ObjectsViewer::Iterator::operator*() const
{
    return {m_objects, m_index};
}

ObjectsViewer::Iterator ObjectsViewer::Iterator::operator++()
{
    ++m_index;
    return *this;
}

bool ObjectsViewer::Iterator::operator==(Iterator other) const
{
    return m_index == other.m_index;
}

ObjectsViewer::ObjectsViewer(Objects& objects)
    : m_objects{objects}
{}

ObjectsViewer::Iterator ObjectsViewer::begin() const
{
    return {m_objects, 0};
}

ObjectsViewer::Iterator ObjectsViewer::end() const
{
    return {m_objects, m_objects.get_count()};
}

ObjectView ObjectsViewer::operator[](size_t index) const
{
    return {m_objects, index};
}

void Objects::update(float delta_time)
{
    for (auto object : ObjectsViewer{*this})
    {
        if (!object.get_is_alive()) continue;

        const auto pos = object.get_position();
        auto vel = object.get_velocity();

        sf::Vector2f acceleration{0.0f, 0.0f};

        for (auto other : ObjectsViewer{*this})
        {
            if (object.get_index() == other.get_index()) continue;
            if (!other.get_is_alive()) continue;

            const auto distance_squared = (pos.x - other.get_x()) * (pos.x - other.get_x()) // A^2
                + (pos.y - other.get_y()) * (pos.y - other.get_y()); // B^2
            const auto direction = (other.get_position() - pos).normalized();

            acceleration += direction * G * other.get_mass() / (distance_squared + SOFTENING);
        }

        if (acceleration.length() > C)
        {
            acceleration *= C / acceleration.length();
        }

        vel += acceleration * delta_time;
        object.set_velocity(vel);
    }

    for (auto object : ObjectsViewer{*this})
    {
        object.set_position(object.get_position() + object.get_velocity() * delta_time);
    }

    for (auto object : ObjectsViewer{*this})
    {
        if (!object.get_is_alive()) continue;
        for (auto other : ObjectsViewer{*this})
        {
            if (object.get_index() <= other.get_index()) continue; // <= also checks for ones already tested
            if (!other.get_is_alive()) continue;

            const auto dist = (object.get_position() - other.get_position()).length();

            if (dist <= object.get_mass() + other.get_mass())
            {
                const auto new_pos = (object.get_position() * object.get_mass()
                    + other.get_position() * other.get_mass())
                    / (object.get_mass() + other.get_mass());

                const auto new_vel = (object.get_velocity() * object.get_mass()
                    + other.get_velocity() * other.get_mass())
                    / (object.get_mass() + other.get_mass());

                m_pending_adds.emplace_back(
                    new_pos.x,
                    new_pos.y,
                    new_vel.x,
                    new_vel.y,
                    object.get_mass() + other.get_mass()
                );

                // this kills them immediately to prevent triple merging which causes bugs
                object.set_is_alive(false);
                other.set_is_alive(false);

                m_pending_removes.push_back(object.get_index());
                m_pending_removes.push_back(other.get_index());
            }
        }
    }

    for (auto index : m_pending_removes)
    {
        remove_object(index);
    }
    m_pending_removes.clear();

    for (auto desc : m_pending_adds)
    {
        add_object(desc);
    }
    m_pending_adds.clear();
}

void Objects::render(sf::RenderWindow& window)
{
    for (auto object : ObjectsViewer{*this})
    {
        if (!object.get_is_alive()) continue;

        const auto mass = object.get_mass();

        sf::CircleShape circle{mass};
        circle.setPosition(object.get_position() - sf::Vector2f{mass, mass});

        window.draw(circle);
    }
}

size_t Objects::get_count() const
{
    return m_x_positions.size();
}

void Objects::add_object(const ObjectDescription& desc)
{
    if (m_free_stack.empty())
    {
        m_x_positions.push_back(desc.x);
        m_y_positions.push_back(desc.y);
        m_x_velocities.push_back(desc.x_velocity);
        m_y_velocities.push_back(desc.y_velocity);
        m_masses.push_back(desc.mass);
        m_is_alive.push_back(true);
    }
    else
    {
        const size_t index = m_free_stack.back();
        m_free_stack.pop_back();

        m_x_positions[index] = desc.x;
        m_y_positions[index] = desc.y;
        m_x_velocities[index] = desc.x_velocity;
        m_y_velocities[index] = desc.y_velocity;
        m_masses[index] = desc.mass;
        m_is_alive[index] = true;
    }
}

void Objects::remove_object(size_t index)
{
    m_free_stack.push_back(index);
    m_is_alive[index] = false;
}
