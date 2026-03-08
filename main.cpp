#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>
#include <algorithm> // this is required for tearing the cloth 

// 1. THE PARTICLE SYSTEM
struct Point {
    sf::Vector2f position;
    sf::Vector2f previous_position;
    bool locked;

    Point(float x, float y, bool is_locked = false) {
        position = sf::Vector2f(x, y);
        previous_position = sf::Vector2f(x, y);
        locked = is_locked;
    }
};

// 2. THE CONSTRAINT
struct Stick {
    Point* pA;
    Point* pB;
    float length;

    Stick(Point* p1, Point* p2, float len) {
        pA = p1;
        pB = p2;
        length = len;
    }
};

// 3. THE PHYSICS ENGINE
class PhysicsEngine {
public:
    std::vector<Point> points;
    std::vector<Stick> sticks;
    sf::Vector2f gravity = sf::Vector2f(0.0f, 981.0f); 

    void update(float dt) {
        // move the points
        for (auto& p : points) {
            if (!p.locked) {
                sf::Vector2f position_before_update = p.position;
                p.position += (p.position - p.previous_position) + gravity * (dt * dt);
                p.previous_position = position_before_update;
            }
        }

        // keep them constrained
        for (int i = 0; i < 5; i++) {
            for (auto& stick : sticks) {
                sf::Vector2f stickCenter = stick.pB->position - stick.pA->position;
                float currentLength = std::sqrt(stickCenter.x * stickCenter.x + stickCenter.y * stickCenter.y);
                float difference = (stick.length - currentLength) / currentLength;
                
                sf::Vector2f offset = stickCenter * difference * 0.5f;

                if (!stick.pA->locked) stick.pA->position -= offset;
                if (!stick.pB->locked) stick.pB->position += offset;
            }
        }
    }
};

// 4. THE MAIN RUN LOOP
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Interactive Verlet Cloth");
    window.setFramerateLimit(60); 

    PhysicsEngine engine;

    // the cloth Generator
    int rows = 20;
    int cols = 20;
    float spacing = 15.0f; 
    float startX = 250.0f; 
    float startY = 50.0f;

    engine.points.reserve(rows * cols);

    // this is where we generate the points
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            bool isLocked = (y == 0);
            engine.points.push_back(Point(startX + x * spacing, startY + y * spacing, isLocked));
        }
    }

    // this is where we generate the sticks
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int i = y * cols + x; 
            if (x < cols - 1) {
                engine.sticks.push_back(Stick(&engine.points[i], &engine.points[i + 1], spacing));
            }
            if (y < rows - 1) {
                engine.sticks.push_back(Stick(&engine.points[i], &engine.points[i + cols], spacing));
            }
        }
    }

    // TRACKING FOR MOUSE **why my code broke
    Point* draggedPoint = nullptr; 
    bool wasLocked = false;

    // the game loop
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- MOUSE INTERACTION LOGIC ---
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);

        // LEFT CLICK: DRAG CLOTH
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (draggedPoint == nullptr) {
                float min_dist = 30.0f; // Increased grab radius
                for (auto& p : engine.points) {
                    float dx = mousePos.x - p.position.x;
                    float dy = mousePos.y - p.position.y;
                    float dist = std::sqrt(dx*dx + dy*dy);
                    if (dist < min_dist) {
                        min_dist = dist;
                        draggedPoint = &p;
                        wasLocked = p.locked; // remember if it was a ceiling nail
                    }
                }
            }
            
            if (draggedPoint != nullptr) {
                draggedPoint->locked = true; // lock it so the engine doesn't fight the mouse
                draggedPoint->position = mousePos;
                draggedPoint->previous_position = mousePos;
            }
        } else {
            // let go when mouse is released
            if (draggedPoint != nullptr) {
                draggedPoint->locked = wasLocked; // return it to normal
                draggedPoint = nullptr;
            }
        }

        // RIGHT CLICK: TEAR CLOTH
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            engine.sticks.erase(std::remove_if(engine.sticks.begin(), engine.sticks.end(),
                [&](const Stick& stick) {
                    sf::Vector2f stickCenter = (stick.pA->position + stick.pB->position) / 2.0f;
                    float dx = mousePos.x - stickCenter.x;
                    float dy = mousePos.y - stickCenter.y;
                    return std::sqrt(dx*dx + dy*dy) < 20.0f; // increased tear radius
                }), engine.sticks.end());
        }
        // -------------------------------

        engine.update(1.0f / 60.0f);
        window.clear(sf::Color::Black);

        for (auto& stick : engine.sticks) {
            sf::Vertex line[] = {
                sf::Vertex{stick.pA->position, sf::Color::White},
                sf::Vertex{stick.pB->position, sf::Color::White}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (auto& p: engine.points) {
            sf::CircleShape circle(2.0f); 
            circle.setFillColor(sf::Color::Red);
            circle.setOrigin({2.0f, 2.0f});
            circle.setPosition(p.position);
            window.draw(circle);
        }

        window.display();
    }
    
    return 0;
}