#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>

const int N = 40; //number of segments in dragon
const float PI = 3.14159265359f;

class Firework {
public:
    Firework() = default;
    Firework(float radius, sf::Vector2f position, sf::Vector2f velocity, sf::Color color)
        : Radius(radius), Position(position), Velocity(velocity), Color(color) {}

    float Radius = 0.0f;
    sf::Vector2f Position;
    sf::Vector2f Velocity;
    sf::Color Color;
};

// Utility functions
inline float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

inline int randomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

inline sf::Vector2f polarToCartesian(float angleDegrees, float magnitude) {
    float angleRadians = angleDegrees * (PI / 180.0f);
    return sf::Vector2f(std::cos(angleRadians) * magnitude, std::sin(angleRadians) * magnitude);
}

inline void createExplosion(std::vector<Firework>& blast, const Firework& parent) {
    const int count = 20;
    const float minSpeed = 200.0f;
    const float maxSpeed = 450.0f;

    for (int i = 0; i < count; i++) {
        float angle = randomFloat(0.0f, 360.0f);
        float velocity = randomFloat(minSpeed, maxSpeed);
        sf::Vector2f vel = polarToCartesian(angle, velocity);

        blast.emplace_back(
            randomFloat(6.0f, 12.0f),
            parent.Position,
            vel,
            parent.Color
        );
    }
}


// SVG path data structure
struct PathCommand {
    char cmd; // Command type (M: Move, L: Line, Q: Quadratic curve, Z: Close path)
    std::vector<float> params; // Command parameters (coordinates)
};

// class to handle SVG path rendering using SFML
class SVGPath : public sf::Drawable, public sf::Transformable {
private:
    std::vector<sf::VertexArray> triangles;
    sf::Color fillColor;

    void tessellatePath(const std::vector<PathCommand>& commands) {
        std::vector<sf::Vector2f> points;
        sf::Vector2f currentPoint(0, 0);
        // Process each path command
        for (const auto& cmd : commands) {
            switch (cmd.cmd) {
            case 'M':
            case 'L':
                for (size_t i = 0; i < cmd.params.size(); i += 2) {
                    currentPoint = sf::Vector2f(cmd.params[i], cmd.params[i + 1]);
                    points.push_back(currentPoint);
                }
                break;
            case 'Q':
                for (size_t i = 0; i < cmd.params.size(); i += 4) {
                    sf::Vector2f control(cmd.params[i], cmd.params[i + 1]);
                    sf::Vector2f end(cmd.params[i + 2], cmd.params[i + 3]);

                    for (float t = 0; t <= 1; t += 0.1f) {
                        float x = (1 - t) * (1 - t) * currentPoint.x + 2 * (1 - t) * t * control.x + t * t * end.x;
                        float y = (1 - t) * (1 - t) * currentPoint.y + 2 * (1 - t) * t * control.y + t * t * end.y;
                        points.push_back(sf::Vector2f(x, y));
                    }
                    currentPoint = end;
                }
                break;
            case 'Z':
                if (!points.empty()) {
                    points.push_back(points[0]);
                }
                break;
            }
        }

        // Convert points to triangles using fan triangulation
        for (size_t i = 1; i < points.size() - 1; i++) {
            sf::VertexArray triangle(sf::Triangles, 3);
            triangle[0].position = points[0];
            triangle[1].position = points[i];
            triangle[2].position = points[i + 1];

            triangle[0].color = fillColor;
            triangle[1].color = fillColor;
            triangle[2].color = fillColor;

            triangles.push_back(triangle);
        }
    }
    //draw
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        for (const auto& triangle : triangles) {
            target.draw(triangle, states);
        }
    }
    // Constructor initializes path with commands and color
public:
    SVGPath(const std::vector<PathCommand>& commands, const sf::Color& color)
        : fillColor(color) {
        tessellatePath(commands);
    }
};

// Original SVG path data
const std::vector<PathCommand> HEAD_PATH_BLACK = {
    {'M', {-21.05, -8.25}},
    {'Q', {-13.6, -15.95, -1.3, -12.1}},
    {'Q', {-7.85, -8.5, -5.85, -4.35}},
    {'Q', {-2.3, -4.85, 10.5, 0.15}},
    {'Q', {0, 4.35, -5.85, 3.65}},
    {'Q', {-7.85, 7.75, -1.25, 12.45}},
    {'Q', {-13.6, 15.2, -21.05, 7.5}},
    {'Q', {-29.55, 4.05, -30.2, -0.35}},
    {'Q', {-29.55, -4.8, -21.05, -8.25}},
    {'Z', {}}
};


const std::vector<PathCommand> EYE1_PATH = {
    {'M', {-18.35, 1.1}},
    {'Q', {-15.1, 5.45, -9.6, 5.35}},
    {'Q', {-15.1, 9.55, -18.35, 1.1}},
    {'Z', {}}
};

const std::vector<PathCommand> EYE2_PATH = {
    {'M', {-18.35, -1.8}},
    {'Q', {-15.1, -10.3, -9.6, -6.05}},
    {'Q', {-15.1, -6.2, -18.35, -1.8}},
    {'Z', {}}
};

const std::vector<PathCommand> NOSE1_PATH = {
    {'M', {-26.7, -2.95}},
    {'L', {-27.25, -2.95}},
    {'Q', {-28.1, -3.1, -28.55, -1.95}},
    {'L', {-28.9, -1.1}},
    {'Q', {-27.7, -1.65, -26.7, -2.95}},
    {'Z', {}}
};

const std::vector<PathCommand> NOSE2_PATH = {
    {'M', {-28.9, 0.35}},
    {'L', {-28.55, 1.2}},
    {'Q', {-28.1, 2.4, -27.25, 2.25}},
    {'L', {-26.7, 2.2}},
    {'Q', {-27.7, 0.9, -28.9, 0.35}},
    {'Z', {}}
};


const std::vector<PathCommand> SPINE_PATH = {
    {'M', {-18.8, 0}},
    {'Q', {-17.85, -5.7, -12.3, -9.6}},
    {'Q', {-11.2, -5.35, -6.5, -8.25}},
    {'L', {-6.45, -8.2}},
    {'L', {-6.2, -8.3}},
    {'Q', {1.25, -16.25, 6.65, -12.4}},
    {'Q', {0.05, -12.55, 0, -5.95}},
    {'Q', {2.7, -2.4, 7.75, -4.1}},
    {'Q', {18, -1.45, 18.8, 0}},
    {'L', {-18.8, 0}},
    {'M', {18.8, 0}},
    {'Q', {18, 1.45, 7.75, 4.1}},
    {'Q', {2.7, 2.4, 0, 5.95}},
    {'Q', {0.05, 12.55, 6.65, 12.4}},
    {'Q', {1.25, 16.25, -6.2, 8.35}},
    {'Q', {-6.35, 8.25, -6.45, 8.25}},
    {'L', {-6.5, 8.25}},
    {'Q', {-11.2, 5.35, -12.3, 9.6}},
    {'Q', {-17.85, 5.7, -18.8, 0}},
    {'L', {18.8, 0}},
    {'Z', {}}
};

const std::vector<PathCommand> WING1_PATH = {
    {'M', {-18.8, 0}},
    {'Q', {-17.85, -5.7, -12.3, -9.6}},
    {'Q', {-11.2, -5.35, -6.5, -8.25}},
    {'L', {-6.45, -8.2}},
    {'L', {-6.2, -8.3}},
    {'Q', {1, -28, 55, -29}},
    {'L', {17, -11}},
    {'Q', {0.05, -12.55, 0, -5.95}},
    {'Q', {2.7, -2.4, 8, -4}},
    {'Q', {18, -1.45, 18.8, 0}},
    {'L', {-18.8, 0}},
    {'Z', {}}
};

const std::vector<PathCommand> WING2_PATH = {
    {'M', {-18.8, 0}},
    {'Q', {-17.85, 5.7, -12.3, 9.6}},
    {'Q', {-11.2, 5.35, -6.5, 8.25}},
    {'L', {-6.45, 8.2}},
    {'L', {-6.2, 8.3}},
    {'Q', {1, 28, 55, 29}},
    {'L', {17, 11}},
    {'Q', {0.05, 12.55, 0, 5.95}},
    {'Q', {2.7, 2.4, 8, 4}},
    {'Q', {18, 1.45, 18.8, 0}},
    {'L', {-18.8, 0}},
    {'Z', {}}
};

const std::vector<PathCommand> CLAW1 = {
   {'M', {19, -28}},
    {'Q', {34, -29, 25, 2}},
    {'Q', {18, -25, 3, -27}},
    {'Q', {11.2, -24.65, 6.5, -21.75}},
    {'L', {6.45, -21.8}},
    {'L', {6.2, -21.7}},
    {'Q', {-1.25, -13.75, -15, -19}},
    {'Q', {-0.05, -17.45, 0, -24.05}},
    {'Q', {-2.7, -27.6, -8, -27}},
    {'Q', {-18, -28.55, -19, -30}},
    {'Q', {-6.5, -30, 9, -30}},
    {'M', {-19, -30}},
    {'Q', {-18, -31.45, -8, -33}},
    {'Q', {-2.7, -32.4, 0, -35.95}},
    {'Q', {-0.05, -42.55, -13, -41}},
    {'Q', {-1.25, -46.25, 6.2, -38.35}},
    {'Q', {6.35, -38.25, 6.45, -38.25}},
    {'L', {6.5, -38.25}},
    {'Q', {11.2, -35.35, 3, -32}},
    {'Q', {18, -33, 27, -32}},
    {'Q', {26.5, -29.5, 26, -27}},
    {'L', {-17, -30}}
};

const std::vector<PathCommand> CLAW2 = {
   {'M', {19, 28}},
    {'Q', {34, 29, 25, -2}},
    {'Q', {18, 25, 3, 27}},
    {'Q', {11.2, 24.65, 6.5, 21.75}},
    {'L', {6.45, 21.8}},
    {'L', {6.2, 21.7}},
    {'Q', {-1.25, 13.75, -15, 19}},
    {'Q', {-0.05, 17.45, 0, 24.05}},
    {'Q', {-2.7, 27.6, -8, 27}},
    {'Q', {-18, 28.55, -19, 30}},
    {'Q', {-6.5, 30, 9, 30}},
    {'M', {-19, 30}},
    {'Q', {-18, 31.45, -8, 33}},
    {'Q', {-2.7, 32.4, 0, 35.95}},
    {'Q', {-0.05, 42.55, -13, 41}},
    {'Q', {-1.25, 46.25, 6.2, 38.35}},
    {'Q', {6.35, 38.25, 6.45, 38.25}},
    {'L', {6.5, 38.25}},
    {'Q', {11.2, 35.35, 3, 32}},
    {'Q', {18, 33, 27, 32}},
    {'Q', {26.5, 29.5, 26, 27}},
    {'L', {-17, 30}}
};

const std::vector<PathCommand> TAIL = {
   {'M', {27, 0}},
    {'Q', {18, 4, 3, 3}},
    {'Q', {11.2, 5.35, 6.5, 8.25}},
    {'L', {6.45, 8.2}},
    {'L', {6.2, 8.3}},
    {'Q', {-1.25, 16.25, -18, 11}},
    {'Q', {-0.05, 12.55, 0, 5.95}},
    {'Q', {-2.7, 2.4, -7.75, 4.1}},
    {'Q', {-18, 1.45, -22, 0}},
    {'Q', {-6.5, 0, 9, 0}},
    {'M', {-22, 0}},
    {'Q', {-18, -1.45, -7.75, -4.1}},
    {'Q', {-2.7, -2.4, 0, -5.95}},
    {'Q', {-0.05, -12.55, -18, -11}},
    {'Q', {-1.25, -16.25, 6.2, -8.35}},
    {'Q', {6.35, -8.25, 6.45, -8.25}},
    {'L', {6.5, -8.25}},
    {'Q', {11.2, -5.35, 3, -2}},
    {'Q', {18, -3, 27, 0}},
    {'L', {-23, 0}}
};

// class rep a segment of the dragon
class DragonSegment {
public:
    sf::Vector2f position;
    float angle;
    std::string type;

    DragonSegment(const sf::Vector2f& pos, const std::string& t)
        : position(pos), angle(0), type(t) {};
};

class Dragon {
private:
    std::vector<DragonSegment> segments;
    sf::Vector2f target;
    float radius = 0;
    float maxRadius;
    float frame = 0;

    std::unique_ptr<SVGPath> headPathBlack;
    std::unique_ptr<SVGPath> eye1Path;
    std::unique_ptr<SVGPath> eye2Path;
    std::unique_ptr<SVGPath> nose1Path;
    std::unique_ptr<SVGPath> nose2Path;
    std::unique_ptr<SVGPath> spinePath;
    std::unique_ptr<SVGPath> wing1Path;
    std::unique_ptr<SVGPath> wing2Path;
    std::unique_ptr<SVGPath> claw1Path;
    std::unique_ptr<SVGPath> claw2Path;
    std::unique_ptr<SVGPath> tailPath;


    float getWingScale(int segmentIndex) {
        float baseScale = 2.0f;
        if (segmentIndex % 5 == 0) return baseScale;
        if (segmentIndex % 4 == 0) return baseScale * 0.8f;
        if (segmentIndex % 3 == 0) return baseScale * 0.6f;
        return 0.0f; // no wings for other segments
    }
private:
    sf::RenderTexture fadeTexture;
    sf::CircleShape blastBoom;
    std::vector<Firework> Fireworks;
    std::vector<Firework> FireWorksAftermath;
    const float gravity = 500.0f;

public:
    Dragon(float screenWidth, float screenHeight) {
        maxRadius = std::min(screenWidth, screenHeight) - 20;
        target = sf::Vector2f(screenWidth / 2, screenHeight / 2);

        headPathBlack = std::make_unique<SVGPath>(HEAD_PATH_BLACK, sf::Color(88, 158, 125));
        eye1Path = std::make_unique<SVGPath>(EYE1_PATH, sf::Color(254, 210, 105));
        eye2Path = std::make_unique<SVGPath>(EYE2_PATH, sf::Color(254, 210, 105));
        nose1Path = std::make_unique<SVGPath>(NOSE1_PATH, sf::Color(255, 255, 255));
        nose2Path = std::make_unique<SVGPath>(NOSE2_PATH, sf::Color(255, 255, 255));
        spinePath = std::make_unique<SVGPath>(SPINE_PATH, sf::Color(254, 210, 105));
        wing1Path = std::make_unique<SVGPath>(WING1_PATH, sf::Color(64, 136, 97));
        wing2Path = std::make_unique<SVGPath>(WING2_PATH, sf::Color(64, 136, 97));
        claw1Path = std::make_unique<SVGPath>(CLAW1, sf::Color(207, 107, 118));
        claw2Path = std::make_unique<SVGPath>(CLAW2, sf::Color(207, 107, 118));
        tailPath = std::make_unique<SVGPath>(TAIL, sf::Color(88, 158, 125));

        for (int i = 0; i < N - 1; i++) {
            std::string type = "spine";
            if (i == 0) type = "head";
            segments.emplace_back(sf::Vector2f(screenWidth / 2, screenHeight / 2), type);
        }

        fadeTexture.create(screenWidth, screenHeight);
        blastBoom.setRadius(50);
        blastBoom.setPointCount(100);
    }

    void createPaintSplash(const sf::Vector2f& position) {
        Fireworks.emplace_back(
            randomFloat(12.0f, 16.0f),
            position,
            sf::Vector2f{},
            sf::Color((sf::Uint8)randomInt(120, 255), (sf::Uint8)randomInt(120, 255), (sf::Uint8)randomInt(120, 255))
        );
    }

    void update(const sf::Vector2f& mousePos, float dt) {
        target = mousePos;

        for (int i = 0; i < segments.size(); i++) {
            if (i == 0) {
                // Head faces the mouse point
                float dx = target.x - segments[0].position.x;
                float dy = target.y - segments[0].position.y;
                segments[0].angle = atan2(dy, dx) * 180 / PI;
                // Move head directly towards mouse
                segments[0].position += (target - segments[0].position) * 0.1f;
            }
            else {
                auto& curr = segments[i];
                auto& prev = segments[i - 1];

                float dx = curr.position.x - prev.position.x;
                float dy = curr.position.y - prev.position.y;
                float angle = atan2(dy, dx);

                float targetX = prev.position.x + cos(angle) * (100 - i) / 5.0f;
                float targetY = prev.position.y + sin(angle) * (100 - i) / 5.0f;

                curr.position.x += (targetX - curr.position.x) * 0.25f;
                curr.position.y += (targetY - curr.position.y) * 0.25f;
                curr.angle = angle * 180 / PI;
            }
        }
        if (!Fireworks.empty()) {
            for (size_t i = 0; i < Fireworks.size(); i++) {
                auto& current = Fireworks[i];
                current.Velocity.y += gravity * dt;

                if (current.Velocity.y > 0.0f) {
                    createExplosion(FireWorksAftermath, current);
                    Fireworks.erase(Fireworks.begin() + i);
                    i--;
                    continue;
                }

                current.Position += current.Velocity * dt;
                blastBoom.setRadius(current.Radius);
                blastBoom.setFillColor(current.Color);
                blastBoom.setOrigin(sf::Vector2f(current.Radius, current.Radius));
                blastBoom.setPosition(current.Position);
                fadeTexture.draw(blastBoom);
            }
        }

        for (size_t i = 0; i < FireWorksAftermath.size(); i++) {
            auto& current = FireWorksAftermath[i];
            current.Velocity.y += gravity * dt;
            current.Position += current.Velocity * dt;
            current.Radius -= 10.0f * dt;

            if (current.Radius <= 1.0f) {
                FireWorksAftermath.erase(FireWorksAftermath.begin() + i);
                i--;
                continue;
            }

            blastBoom.setRadius(current.Radius);
            blastBoom.setFillColor(current.Color);
            blastBoom.setOrigin(sf::Vector2f(current.Radius, current.Radius));
            blastBoom.setPosition(current.Position);
            fadeTexture.draw(blastBoom);
        }

        fadeTexture.display();
    }
    

    void draw(sf::RenderWindow& window) {
        window.draw(sf::Sprite(fadeTexture.getTexture()));
        int wingSpacing = 5; 
        int nextWingSegment = wingSpacing;  // Track next segment for wing

        for (int i = 0; i < segments.size(); i++) {
            const auto& segment = segments[i];
            sf::Transform transform;
            transform.translate(segment.position);
            transform.rotate(segment.angle);

            if (segment.type == "head") {
                transform.scale(2.0f, 2.0f);
                transform.rotate(180);
                window.draw(*headPathBlack, transform);
                window.draw(*eye1Path, transform);
                window.draw(*eye2Path, transform);
                window.draw(*nose1Path, transform);
                window.draw(*nose2Path, transform);
            }
            else {
                window.draw(*spinePath, transform);
                if (i == 1 ) {
                    sf::Transform clawTransform = transform;
                    clawTransform.scale(1.5f, 1.5f);
                    window.draw(*claw1Path, clawTransform);
                    window.draw(*claw2Path, clawTransform);
                }
                if (i == segments.size() - 1) {
                    sf::Transform tailTransform = transform;
                    tailTransform.scale(1.f, 1.f);
                    window.draw(*tailPath, tailTransform);
                }

                // draw wings right after head
                if (i == 2) {
                    sf::Transform wingTransform = transform;
                    float scale = 2.5f;
                    wingTransform.scale(scale, scale);
                    window.draw(*wing1Path, wingTransform);
                    window.draw(*wing2Path, wingTransform);
                }

                // con8intue with regular wing pattern
                if (i == nextWingSegment && i > 1) {
                    sf::Transform wingTransform = transform;
                    float scale = 2.5f * (1.0f - (float)i / segments.size());
                    wingTransform.scale(scale, scale);
                    window.draw(*wing1Path, wingTransform);
                    window.draw(*wing2Path, wingTransform);

                    wingSpacing = std::max(2, wingSpacing - 1);
                    nextWingSegment += wingSpacing;
                }
            }
        }
    }
    void clearEffects() {
        Fireworks.clear();
        FireWorksAftermath.clear();
        fadeTexture.clear(sf::Color::White);  
        fadeTexture.display();
    }
    sf::Vector2f getHeadPosition() const {
        if (!segments.empty()) {
            return segments[0].position;
        }
        return sf::Vector2f{ 0, 0 };
    }



};
