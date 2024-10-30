#include <SFML/Graphics.hpp>
#include <cmath>

// Utility function to compute the squared distance between two points
float distanceSquared(const sf::Vector2f& P1, const sf::Vector2f& P2)
{
	return (P1.x - P2.x) * (P1.x - P2.x) + (P1.y - P2.y) * (P1.y - P2.y);
}

// Utility function to compute the distance between two points
float distance(const sf::Vector2f& P1, const sf::Vector2f& P2)
{
	return std::sqrt(distanceSquared(P1, P2));
}

class Capsule
{
public:
	float Radius;
	sf::Vector2f PointA, PointB; // Endpoints of the capsule
	sf::Vector2f Velocity;
	sf::CircleShape CapA, CapB; // Circles for the capsule's ends
	sf::RectangleShape Body; // Rectangle for the body of the capsule

	Capsule(const float R, const sf::Vector2f StartA, const sf::Vector2f StartB, const sf::Vector2f Vel,
	        const sf::Color& Color)
		: Radius(R), PointA(StartA), PointB(StartB), Velocity(Vel)
	{
		CapA.setRadius(Radius);
		CapB.setRadius(Radius);
		CapA.setOrigin(Radius, Radius);
		CapB.setOrigin(Radius, Radius);
		CapA.setPosition(PointA);
		CapB.setPosition(PointB);
		CapA.setFillColor(Color);
		CapB.setFillColor(Color);

		Body.setSize(sf::Vector2f(distance(PointA, PointB), Radius * 2));
		Body.setOrigin(Body.getSize().x / 2, Radius);
		Body.setPosition((PointA + PointB) / 2.0f);
		Body.setFillColor(Color);
	}

	void update(const float Dt)
	{
		// Move both endpoints
		PointA += Velocity * Dt;
		PointB += Velocity * Dt;

		// Update graphical position
		CapA.setPosition(PointA);
		CapB.setPosition(PointB);
		Body.setPosition((PointA + PointB) / 2.0f);

		// Check for window boundary collisions and reverse velocity if necessary
		if (PointA.x - Radius < 0 || PointB.x + Radius > 1600)
		{
			Velocity.x *= -1;
		}
		if (PointA.y - Radius < 0 || PointA.y + Radius * 2 > 900)
		{
			Velocity.y *= -1;
		}
	}
};

// Helper function to get the closest point on a line segment
sf::Vector2f closestPointOnSegment(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& P)
{
	const sf::Vector2f Ab = B - A;
	const float AbAb = distanceSquared(A, B);
	const float ApAb = (P.x - A.x) * Ab.x + (P.y - A.y) * Ab.y;
	float T = ApAb / AbAb;
	T = std::max(0.f, std::min(1.f, T)); // Clamp t to the range [0, 1]
	return A + Ab * T;
}

// Check if two capsules overlap by checking the closest distance between their segments and their caps
bool capsulesOverlap(const Capsule& C1, const Capsule& C2)
{
	// Check the distance between the body segments of both capsules
	const sf::Vector2f ClosestPointOnC1 = closestPointOnSegment(C1.PointA, C1.PointB, C2.PointA);
	const sf::Vector2f ClosestPointOnC2 = closestPointOnSegment(C2.PointA, C2.PointB, C1.PointA);

	// Check if the closest points between the capsules are within the sum of their radii
	const float Dist = distance(ClosestPointOnC1, ClosestPointOnC2);
	const float TotalRadius = C1.Radius + C2.Radius;
	return Dist < TotalRadius;
}

// Apply a force to separate overlapping capsules and adjust their velocities for bouncing
void applySeparationForce(Capsule& C1, Capsule& C2)
{
	const sf::Vector2f Midpoint1 = (C1.PointA + C1.PointB) / 2.0f;
	const sf::Vector2f Midpoint2 = (C2.PointA + C2.PointB) / 2.0f;

	const sf::Vector2f Direction = Midpoint1 - Midpoint2;
	const float Dist = distance(Midpoint1, Midpoint2);
	[[maybe_unused]] float Overlap = (C1.Radius + C2.Radius) - Dist;

	if (static_cast<int>(Dist) != 0)
	{
		const sf::Vector2f Normal = Direction / Dist;
		const sf::Vector2f RelativeVelocity = C1.Velocity - C2.Velocity;

		// Project the relative velocity onto the collision normal
		const float VelocityAlongNormal = RelativeVelocity.x * Normal.x + RelativeVelocity.y * Normal.y;

		// Apply a strong separation force and bounce effect based on the collision normal
		if (VelocityAlongNormal < 0)
		{
			constexpr float Restitution = 0.9f;
			const float ImpulseMagnitude = -(1 + Restitution) * VelocityAlongNormal / 2.0f;
			// Distribute impulse equally

			const sf::Vector2f Impulse = ImpulseMagnitude * Normal;
			C1.Velocity += Impulse;
			C2.Velocity -= Impulse;
		}
	}
}

int main()
{
	sf::RenderWindow LWindow(sf::VideoMode(1600, 900), "Sub Exercise 003.1 - Capsule Collision");
	LWindow.setFramerateLimit(60);

	// Create two larger horizontal capsules
	Capsule LCapsule1(40.f, sf::Vector2f(200.f, 400.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(150.f, 100.f),
	                  sf::Color::Green);
	Capsule LCapsule2(40.f, sf::Vector2f(500.f, 500.f), sf::Vector2f(600.f, 500.f), sf::Vector2f(-100.f, -80.f),
	                  sf::Color::Blue);

	sf::Clock LClock;
	while (LWindow.isOpen())
	{
		sf::Event LEvent;
		while (LWindow.pollEvent(LEvent))
		{
			if (LEvent.type == sf::Event::Closed)
				LWindow.close();
		}

		float LDeltaTime = LClock.restart().asSeconds();

		// Update capsule positions
		LCapsule1.update(LDeltaTime);
		LCapsule2.update(LDeltaTime);

		// Check for collisions and apply forces
		if (capsulesOverlap(LCapsule1, LCapsule2))
		{
			applySeparationForce(LCapsule1, LCapsule2);
		}

		// Render
		LWindow.clear();
		LWindow.draw(LCapsule1.Body);
		LWindow.draw(LCapsule1.CapA);
		LWindow.draw(LCapsule1.CapB);
		LWindow.draw(LCapsule2.Body);
		LWindow.draw(LCapsule2.CapA);
		LWindow.draw(LCapsule2.CapB);
		LWindow.display();
	}

	return 0;
}
