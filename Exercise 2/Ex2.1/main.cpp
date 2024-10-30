#include <SFML/Graphics.hpp>
#include <vector>

class Sphere
{
public:
	float Mass;
	sf::CircleShape Shape;
	sf::Vector2f Position;
	sf::Vector2f Velocity;
	sf::Vector2f Acceleration;

	Sphere(const float Radius, const float Mass, const sf::Vector2f StartPosition, const sf::Color& Color)
		: Mass(Mass), Position(StartPosition), Velocity(0.f, 0.f), Acceleration(0.f, 0.f)
	{
		Shape.setRadius(Radius);
		Shape.setFillColor(Color);
		Shape.setPosition(Position);
	}

	void applyForce(const sf::Vector2f& Force)
	{
		Acceleration += Force / Mass;
	}

	void update(const float DeltaTime)
	{
		Velocity += Acceleration * DeltaTime;
		Position += Velocity * DeltaTime;
		Shape.setPosition(Position);
		Acceleration = sf::Vector2f(0.f, 0.f); // Reset acceleration after applying
	}

	void bounceOffEdges(const sf::RenderWindow& Window)
	{
		if (Position.x < 0 || Position.x + Shape.getRadius() * 2 > static_cast<float>(Window.getSize().x))
		{
			Velocity.x *= -1;
		}
		if (Position.y < 0 || Position.y + Shape.getRadius() * 2 > static_cast<float>(Window.getSize().y))
		{
			Velocity.y *= -1;
		}
	}
};

int main()
{
	sf::RenderWindow LWindow(sf::VideoMode(1600, 900), "Exercise 002.1 - C++ Euler Integration");
	LWindow.setFramerateLimit(60);

	std::vector<Sphere> LSpheres;
	LSpheres.emplace_back(20.f, 5.f, sf::Vector2f(300.0f, 50.f), sf::Color::Red);
	LSpheres.emplace_back(30.f, 10.f, sf::Vector2f(600.0f, 150.f), sf::Color::Yellow);
	LSpheres.emplace_back(40.f, 15.f, sf::Vector2f(900.0f, 250.f), sf::Color::Green);

	const sf::Vector2f LGravity(0.f, 500.f);
	bool LApplyingWind = false;
	sf::Vector2f LWindForce(500.f, 0.f);

	sf::Clock LClock;
	float LTimeStepMultiplier = 2.0f; // Scale deltaTime to speed up the simulation

	while (LWindow.isOpen())
	{
		sf::Event LEvent;
		while (LWindow.pollEvent(LEvent))
		{
			if (LEvent.type == sf::Event::Closed)
				LWindow.close();
			if (LEvent.type == sf::Event::MouseButtonPressed && LEvent.mouseButton.button == sf::Mouse::Left)
				LApplyingWind = true;
			if (LEvent.type == sf::Event::MouseButtonReleased && LEvent.mouseButton.button == sf::Mouse::Left)
				LApplyingWind = false;
		}

		float LDeltaTime = LClock.restart().asSeconds() * LTimeStepMultiplier;

		// Update sphere positions
		for (auto& Sphere : LSpheres)
		{
			Sphere.applyForce(LGravity);

			// If left mouse button is held, apply wind force
			if (LApplyingWind)
			{
				Sphere.applyForce(LWindForce);
			}

			Sphere.update(LDeltaTime);
			Sphere.bounceOffEdges(LWindow);
		}

		// Rendering
		LWindow.clear();
		for (const auto& Sphere : LSpheres)
		{
			LWindow.draw(Sphere.Shape);
		}
		LWindow.display();
	}

	return 0;
}
