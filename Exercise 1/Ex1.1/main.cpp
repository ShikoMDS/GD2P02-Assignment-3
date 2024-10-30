#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

// Function to check if the circle is within leniency of the plane
bool isCircleOnPlaneWithTolerance(const sf::Vector2f& PlanePoint, const sf::Vector2f& Normal,
                                  const sf::Vector2f& CircleCenter, const float CircleRadius, const float Tolerance)
{
	// Vector from the plane point to the circle center
	const sf::Vector2f Diff = CircleCenter - PlanePoint;

	// Project the Diff vector onto the plane normal to get the perpendicular distance
	const float Distance = std::abs(Diff.x * Normal.x + Diff.y * Normal.y) / std::sqrt(
		Normal.x * Normal.x + Normal.y * Normal.y);

	// If the distance is less than or equal to the circle radius plus tolerance, consider it "on the plane"
	return Distance <= CircleRadius + Tolerance;
}

int main()
{
	sf::RenderWindow Window(sf::VideoMode(1600, 900), "Sub Exercise 001.1 - Plane vs Point Function", sf::Style::Close);

	bool DrawingLine = false;
	bool LineDrawn = false;
	sf::Vector2f Start;
	sf::Vector2f End;
	sf::Vector2f Normal;

	constexpr float PlaneThickness = 5.0f; // Reduced thickness for both visual and calculation
	constexpr float CircleRadius = 5.0f;
	constexpr float Tolerance = 2.0f; // Leniency for determining "on the plane"

	// Create the plane as a rectangle shape
	sf::RectangleShape Plane(sf::Vector2f(0.0f, PlaneThickness));
	Plane.setFillColor(sf::Color::Blue);
	Plane.setOrigin(0.0f, PlaneThickness / 2.0f); // Correct the center of the plane

	sf::CircleShape Dot(CircleRadius);
	Dot.setFillColor(sf::Color::Yellow);
	bool PointPlaced = false;

	while (Window.isOpen())
	{
		sf::Event Event{};
		while (Window.pollEvent(Event))
		{
			// Handle window close event
			if (Event.type == sf::Event::Closed)
			{
				Window.close();
			}
			// Handle mouse button press events
			else if (Event.type == sf::Event::MouseButtonPressed)
			{
				if (Event.mouseButton.button == sf::Mouse::Left)
				{
					// Start drawing the line on left-click
					DrawingLine = true;
					Start = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
					Plane.setPosition(Start);
				}
				else if (Event.mouseButton.button == sf::Mouse::Right && LineDrawn)
				{
					// Place the point (circle shape) on right-click
					sf::Vector2f CircleCenter = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
					Dot.setPosition(CircleCenter.x - CircleRadius, CircleCenter.y - CircleRadius);
					// Adjust for circle origin
					PointPlaced = true;

					// Check if the circle is "on the plane" with tolerance
					if (isCircleOnPlaneWithTolerance(Start, Normal, CircleCenter, CircleRadius, Tolerance))
					{
						std::cout << "Point is: On Plane" << '\n';
					}
					else
					{
						float DotProduct = (CircleCenter.x - Start.x) * Normal.x + (CircleCenter.y - Start.y) * Normal.
							y;
						std::cout << "Point is: " << (DotProduct > 0 ? "In Front" : "Behind") << '\n';
					}
				}
			}
			// Handle mouse button release event
			else if (Event.type == sf::Event::MouseButtonReleased)
			{
				if (Event.mouseButton.button == sf::Mouse::Left && DrawingLine)
				{
					// Finish drawing the line on release
					DrawingLine = false;
					LineDrawn = true;
					End = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));

					// Calculate the normal vector for the plane
					Normal = sf::Vector2f(End.y - Start.y, Start.x - End.x);

					// Set the plane length and rotation
					float Length = std::sqrt(std::pow(End.x - Start.x, 2.0f) + std::pow(End.y - Start.y, 2.0f));
					Plane.setSize(sf::Vector2f(Length, PlaneThickness));
					float Angle = std::atan2(End.y - Start.y, End.x - Start.x) * 180.0f / 3.14159265f;
					Plane.setRotation(Angle);
				}
			}
			// Handle mouse movement event
			else if (Event.type == sf::Event::MouseMoved)
			{
				if (DrawingLine)
				{
					// Update the line's end point while drawing
					sf::Vector2f CurrentPos = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
					float Length = std::sqrt(
						std::pow(CurrentPos.x - Start.x, 2.0f) + std::pow(CurrentPos.y - Start.y, 2.0f));
					Plane.setSize(sf::Vector2f(Length, PlaneThickness));
					float Angle = std::atan2(CurrentPos.y - Start.y, CurrentPos.x - Start.x) * 180.0f / 3.14159265f;
					Plane.setRotation(Angle);
				}
			}
		}

		Window.clear();

		// Draw the plane line if it is being drawn or has been drawn
		if (DrawingLine || LineDrawn)
		{
			Window.draw(Plane);
		}

		// Draw the point (circle) if it has been placed
		if (PointPlaced)
		{
			Window.draw(Dot);
		}

		Window.display();
	}

	return 0;
}
