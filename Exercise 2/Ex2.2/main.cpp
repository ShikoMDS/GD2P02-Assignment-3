#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

constexpr int WindowWidth = 1600;
constexpr int WindowHeight = 900;
constexpr int NumArmSegments = 10; 
constexpr float SegmentLength = 50.0f;
constexpr float ArmThickness = 6.0f;

struct ArmSegment
{
    sf::Vector2f Position;
};

void updateArm(std::vector<ArmSegment>& ArmSegments, const sf::Vector2f& TargetPosition)
{
    // Set the end effector (last segment) to the target position (mouse cursor)
    ArmSegments.back().Position = TargetPosition;

    // Update each segment to follow the next segment towards the constrained base
    for (int I = NumArmSegments - 2; I >= 0; --I)
    {
        sf::Vector2f Direction = ArmSegments[I].Position - ArmSegments[I + 1].Position;
        const float Distance = std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y);
        if (Distance > SegmentLength)
        {
            Direction /= Distance; // Normalize direction
            ArmSegments[I].Position = ArmSegments[I + 1].Position + Direction * SegmentLength;
        }
    }

    // Constrain the base of the arm to the center of the window
    ArmSegments[0].Position = sf::Vector2f(WindowWidth / 2.0f, WindowHeight / 2.0f);
}

void renderArm(sf::RenderWindow& Window, const std::vector<ArmSegment>& ArmSegments)
{
    for (int I = 0; I < NumArmSegments - 1; ++I)
    {
        sf::RectangleShape SegmentLine;
        SegmentLine.setPosition(ArmSegments[I].Position);
        const sf::Vector2f Direction = ArmSegments[I + 1].Position - ArmSegments[I].Position;
        const float Length = std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y);
        SegmentLine.setSize(sf::Vector2f(Length, ArmThickness)); 
        SegmentLine.setRotation(std::atan2(Direction.y, Direction.x) * 180.f / 3.14159265f);
        SegmentLine.setFillColor(sf::Color::Blue);
        Window.draw(SegmentLine);
    }
}

int main()
{
    sf::RenderWindow Window(sf::VideoMode(WindowWidth, WindowHeight), "Ex 2.2: Constrained Arm", sf::Style::Close);

    // Initialize arm segments
    std::vector<ArmSegment> ArmSegments(NumArmSegments);
    for (int I = 0; I < NumArmSegments; ++I)
    {
        ArmSegments[I].Position = sf::Vector2f(WindowWidth / 2.0f, WindowHeight / 2.0f + I * SegmentLength);
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

        // Update arm position to follow mouse
        updateArm(ArmSegments, MousePosition);

        // Render everything
        Window.clear(sf::Color::Black);
        renderArm(Window, ArmSegments);
        Window.display();
    }

    return 0;
}
