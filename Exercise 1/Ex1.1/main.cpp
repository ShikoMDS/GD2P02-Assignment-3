#include <chrono>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

constexpr int WindowWidth = 1600;
constexpr int WindowHeight = 900;
constexpr int NumGrassBlades = 500;
constexpr int GrassSegments = 5;
constexpr float BaseGrassLength = 150.0f;
constexpr float WindSwayFrequencyBase = 0.2f;  
constexpr float WindSwayAmplitude = 20.0f;
constexpr float DampingFactor = 0.7f;  

struct GrassBlade
{
    std::vector<sf::Vector2f> Segments;
    float LengthVariance;
    float PhaseOffset;
    float FrequencyVariance;  
};

void initializeGrass(std::vector<GrassBlade>& GrassBlades)
{
    // Seed the random number generator with the current time for more randomness
    std::default_random_engine Generator(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution LengthDist(1.0f, 1.3f);
    std::uniform_real_distribution PhaseDist(0.0f, 3.14f);
    std::uniform_real_distribution XPosDist(0.0f, static_cast<float>(WindowWidth));
    std::uniform_real_distribution FrequencyDist(0.8f, 1.2f);  // Variance for swaying frequency

    for (auto& Blade : GrassBlades)
    {
        Blade.LengthVariance = LengthDist(Generator);
        Blade.PhaseOffset = PhaseDist(Generator);
        Blade.FrequencyVariance = FrequencyDist(Generator);

        // Initialize the position of the grass blades along the entire width of the window
        const float StartX = XPosDist(Generator);
        Blade.Segments.resize(GrassSegments + 1);
        Blade.Segments[0] = sf::Vector2f(StartX, WindowHeight); // Grass blade base at bottom of the window
    }
}

void updateGrass(std::vector<GrassBlade>& GrassBlades, const float Time)
{
    for (auto& Blade : GrassBlades)
    {
        // Adjust the frequency based on a variance to make blades move uniquely
        const float EffectiveFrequency = WindSwayFrequencyBase * Blade.FrequencyVariance;
        const float Sway = std::sin(Time * EffectiveFrequency + Blade.PhaseOffset) * WindSwayAmplitude * DampingFactor;

        for (int I = 1; I <= GrassSegments; ++I)
        {
            const float SegmentLength = BaseGrassLength * Blade.LengthVariance / GrassSegments;
            const float Angle = Sway * (I / static_cast<float>(GrassSegments));
            Blade.Segments[I] = Blade.Segments[I - 1] + sf::Vector2f(std::sin(Angle) * SegmentLength, -std::cos(Angle) * SegmentLength);
        }
    }
}

void renderGrass(sf::RenderWindow& Window, const std::vector<GrassBlade>& GrassBlades)
{
    for (const auto& Blade : GrassBlades)
    {
        for (int I = 0; I < GrassSegments; ++I)
        {
            const float Thickness = 3.0f - static_cast<float>(I) * 0.4f; // Thicker at the base, thinner towards the tip
            sf::RectangleShape SegmentLine;
            SegmentLine.setPosition(Blade.Segments[I]);
            const sf::Vector2f Direction = Blade.Segments[I + 1] - Blade.Segments[I];
            const float Length = std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y);
            SegmentLine.setSize(sf::Vector2f(Length, Thickness));
            SegmentLine.setRotation(std::atan2(Direction.y, Direction.x) * 180.f / 3.14159265f);
            SegmentLine.setFillColor(sf::Color(34, 139, 34));

            Window.draw(SegmentLine);
        }
    }
}

int main()
{
    sf::RenderWindow Window(sf::VideoMode(WindowWidth, WindowHeight), "Ex 1.1: Grass Simulation", sf::Style::Close);

    std::vector<GrassBlade> GrassBlades(NumGrassBlades);
    initializeGrass(GrassBlades);

    sf::Clock Clock;

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

        float ElapsedTime = Clock.getElapsedTime().asSeconds();

        // Update grass animation based on elapsed time
        updateGrass(GrassBlades, ElapsedTime);

        // Render everything
        Window.clear(sf::Color::Cyan);
        renderGrass(Window, GrassBlades);
        Window.display();
    }

    return 0;
}
