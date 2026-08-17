#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Objects
{
public:
    friend class ObjectView;

    Objects() = default;

    void update(float delta_time);
    void render(sf::RenderWindow& window);

    [[nodiscard]] size_t get_count() const;

    void add_object(const class ObjectDescription& desc);
    void remove_object(size_t index);

private:
    std::vector<float> m_x_positions;
    std::vector<float> m_y_positions;
    std::vector<float> m_x_velocities;
    std::vector<float> m_y_velocities;
    std::vector<float> m_masses;
    std::vector<char>  m_is_alive; // char is being used instead of bool because vector<bool> is scuffed

    std::vector<size_t> m_free_stack;
    std::vector<ObjectDescription> m_pending_adds;
    std::vector<size_t> m_pending_removes;
};

class ObjectView
{
public:
    ObjectView(Objects& objects, size_t index)
        : m_objects{objects}
        , m_index{index}
    {}

    [[nodiscard]] float get_x() const { return m_objects.m_x_positions[m_index]; }
    [[nodiscard]] float get_y() const { return m_objects.m_y_positions[m_index]; }
    [[nodiscard]] float get_x_velocity() const { return m_objects.m_x_velocities[m_index]; }
    [[nodiscard]] float get_y_velocity() const { return m_objects.m_y_velocities[m_index]; }
    [[nodiscard]] float get_mass() const { return m_objects.m_masses[m_index]; }
    [[nodiscard]] bool get_is_alive() const { return static_cast<bool>(m_objects.m_is_alive[m_index]);}
    [[nodiscard]] sf::Vector2f get_position() const { return {get_x(), get_y()}; }
    [[nodiscard]] sf::Vector2f get_velocity() const { return {get_x_velocity(), get_y_velocity()}; }
    [[nodiscard]] size_t get_index() const { return m_index; }

    void set_x(float x) const { m_objects.m_x_positions[m_index] = x; }
    void set_y(float y) const { m_objects.m_y_positions[m_index] = y; }
    void set_x_velocity(float velocity) const { m_objects.m_x_velocities[m_index] = velocity; }
    void set_y_velocity(float velocity) const { m_objects.m_y_velocities[m_index] = velocity; }
    void set_mass(float mass) const { m_objects.m_masses[m_index] = mass; }
    void set_is_alive(bool status) const { m_objects.m_is_alive[m_index] = static_cast<char>(status); }
    void set_position(sf::Vector2f position) const { set_x(position.x); set_y(position.y); }
    void set_velocity(sf::Vector2f velocity) const { set_x_velocity(velocity.x); set_y_velocity(velocity.y); }

private:
    Objects& m_objects;
    size_t m_index;
};

class ObjectsViewer
{
public:
    class Iterator
    {
    public:
        Iterator(Objects& objects, size_t index);

        [[nodiscard]] ObjectView operator*() const;
        Iterator operator++();
        bool operator==(Iterator other) const;

    private:
        Objects& m_objects;
        size_t m_index;
    };

    explicit ObjectsViewer(Objects& objects);

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;

    [[nodiscard]] ObjectView operator[](size_t index) const;

private:
    Objects& m_objects;
};

struct ObjectDescription
{
    float x;
    float y;
    float x_velocity;
    float y_velocity;
    float mass;
};