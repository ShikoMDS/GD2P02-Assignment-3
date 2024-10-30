#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

constexpr int WindowWidth = 1600;
constexpr int WindowHeight = 900;
constexpr int NumWormSegments = 10; 
constexpr float SegmentLength = 50.0f; 
constexpr float WormThickness = 6.0f; 

struct WormSegment
{
    sf::Vector2f Position;
};

void updateWorm(std::vector<WormSegment>& WormSegments, const sf::Vector2f& TargetPosition)
{
    // Set the head of the worm to the target position (mouse cursor)
    WormSegments[0].Position = TargetPosition;

    // Update each segment to follow the previous segment
    for (int I = 1; I < NumWormSegments; ++I)
    {
        sf::Vector2f Direction = WormSegments[I].Position - WormSegments[I - 1].Position;
        const float Distance = std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y);
        if (Distance > SegmentLength)
        {
            Direction /= Distance; // Normalize direction
            WormSegments[I].Position = WormSegments[I - 1].Position + Direction * SegmentLength;
        }
    }
}

void renderWorm(sf::RenderWindow& Window, const std::vector<WormSegment>& WormSegments)
{
    for (int I = 0; I < NumWormSegments - 1; ++I)
    {
        sf::RectangleShape SegmentLine;
        SegmentLine.setPosition(WormSegments[I].Position);
        const sf::Vector2f Direction = WormSegments[I + 1].Position - WormSegments[I].Position;
        const float Length = std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y);
        SegmentLine.setSize(sf::Vector2f(Length, WormThickness));
        SegmentLine.setRotation(std::atan2(Direction.y, Direction.x) * 180.f / 3.14159265f);
        SegmentLine.setFillColor(sf::Color::Red);
        Window.draw(SegmentLine);
    }
}

int main()
{
    sf::RenderWindow Window(sf::VideoMode(WindowWidth, WindowHeight), "Ex 2.1: Unconstrained Worm", sf::Style::Close);

    // Initialize worm segments
    std::vector<WormSegment> WormSegments(NumWormSegments);
    for (int I = 0; I < NumWormSegments; ++I)
    {
        WormSegments[I].Position = sf::Vector2f(WindowWidth / 2.0f, WindowHeight / 2.0f + I * SegmentLength);
    }

    while (Window.isOpen())
    {
        sf::Event Event{};
        while (Window.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
            {
                Window.close();
            }
        }

        // Get mouse position and convert to world coordinates
        sf::Vector2f MousePosition = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));

        // Update worm position to follow mouse
        updateWorm(WormSegments, MousePosition);

        // Render everything
        Window.clear(sf::Color::Black);
        renderWorm(Window, WormSegments);
        Window.display();
    }

    return 0;
}
