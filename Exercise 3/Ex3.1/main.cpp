#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

constexpr int WindowWidth = 1600;
constexpr int WindowHeight = 900;
constexpr int CurvePoints = 1000;

sf::Vector2f bezierPoint(const sf::Vector2f& P0, const sf::Vector2f& P1, const sf::Vector2f& P2, const sf::Vector2f& P3, const float T)
{
	const float U = 1 - T;
	const float TT = T * T;
	const float UU = U * U;
    float UUU = UU * U;
    float TTT = TT * T;

    sf::Vector2f Point = UUU * P0; // (1-T)^3 * P0
    Point += 3 * UU * T * P1;     // 3 * (1-T)^2 * T * P1
    Point += 3 * U * TT * P2;      // 3 * (1-T) * T^2 * P2
    Point += TTT * P3;             // T^3 * P3

    return Point;
}

int main()
{
    sf::RenderWindow Window(sf::VideoMode(WindowWidth, WindowHeight), "Ex 3.1: Cubic Bezier Curve", sf::Style::Close);

    // Define anchor and control points
    sf::Vector2f AnchorLeft(0.0f, WindowHeight / 2.0f);
    sf::Vector2f AnchorRight(WindowWidth, WindowHeight / 2.0f);
    sf::Vector2f ControlLeft(400.0f, WindowHeight / 3.0f);
    sf::Vector2f ControlRight(1200.0f, 2 * WindowHeight / 3.0f);

    bool MovingControlLeft = false;
    bool MovingControlRight = false;

    while (Window.isOpen())
    {
        sf::Event Event{};
        while (Window.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
            {
                Window.close();
            }
            else if (Event.type == sf::Event::MouseButtonPressed)
            {
                if (Event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f MousePosition = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
                    if (std::hypot(MousePosition.x - ControlLeft.x, MousePosition.y - ControlLeft.y) < 10.0f)
                    {
                        MovingControlLeft = true;
                    }
                }
                else if (Event.mouseButton.button == sf::Mouse::Right)
                {
                    sf::Vector2f MousePosition = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
                    if (std::hypot(MousePosition.x - ControlRight.x, MousePosition.y - ControlRight.y) < 10.0f)
                    {
                        MovingControlRight = true;
                    }
                }
            }
            else if (Event.type == sf::Event::MouseButtonReleased)
            {
                if (Event.mouseButton.button == sf::Mouse::Left)
                {
                    MovingControlLeft = false;
                }
                else if (Event.mouseButton.button == sf::Mouse::Right)
                {
                    MovingControlRight = false;
                }
            }
        }

        // Update control point positions if being dragged
        if (MovingControlLeft)
        {
            ControlLeft = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
        }
        if (MovingControlRight)
        {
            ControlRight = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
        }

        // Draw the Bezier curve
        std::vector<sf::Vertex> CurveVertices;
        for (int I = 0; I <= CurvePoints; ++I)
        {
            float T = static_cast<float>(I) / CurvePoints;
            sf::Vector2f Point = bezierPoint(AnchorLeft, ControlLeft, ControlRight, AnchorRight, T);
            CurveVertices.emplace_back(Point, sf::Color::Green);
        }

        // Render everything
        Window.clear(sf::Color::Black);

        // Draw bezier curve
        if (!CurveVertices.empty())
        {
            Window.draw(CurveVertices.data(), CurveVertices.size(), sf::LineStrip);
        }

        // Draw control points
        sf::CircleShape ControlPointShape(10.0f);
        ControlPointShape.setFillColor(sf::Color::Red); // Left point, left mouse button (LMB, MB1)

        ControlPointShape.setPosition(ControlLeft.x - 10.0f, ControlLeft.y - 10.0f);
        Window.draw(ControlPointShape);

        // Change color for the right control point
        ControlPointShape.setFillColor(sf::Color::Blue); // Right point, right mouse button (RMB, MB2)

        ControlPointShape.setPosition(ControlRight.x - 10.0f, ControlRight.y - 10.0f);
        Window.draw(ControlPointShape);

        Window.display();
    }

    return 0;
}
