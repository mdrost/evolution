/*#include "collision.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

const int FPS_MAX = 60;
const int FPS_MIN = 15;

const sf::Time UPDATE_TIME = sf::milliseconds(10);

sf::RenderWindow *window;
int loop_counter;

void draw_box(AxisAlignedBoundingBox box)
{
	Vector size = box.max - box.min;
	sf::RectangleShape shape(sf::Vector2f(size.x, size.y));
	shape.setPosition(box.min.x, box.min.y);
	shape.setFillColor(sf::Color::Black);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1);
	window->draw(shape);
}

void draw_circle(Circle circle)
{
	sf::CircleShape shape(circle.radius);
	shape.setPosition(circle.position.x, circle.position.y);
	shape.setFillColor(sf::Color::Black);
	shape.setOutlineColor(sf::Color::Blue);
	shape.setOutlineThickness(1);
	window->draw(shape);
}

void draw_boxes(const std::vector<AxisAlignedBoundingBox> &boxes)
{
	for (auto box : boxes)
	{
		draw_box(box);
	}
}

void draw_circles(const std::vector<Circle> &circles)
{
	for (auto circle : circles)
	{
		draw_circle(circle);
	}
}

std::string fps_str(sf::Time time)
{
	if (time.asMilliseconds()) {
		return std::to_string(1000 / time.asMilliseconds());
	} else {
		return "Inf";
	}
}

sf::Text loop_counter_text;
sf::Text fps_text;
sf::Text real_time_text;
sf::Text game_time_text;

void display(sf::Time real_time, sf::Time game_time, sf::Time elapsed, const std::vector<AxisAlignedBoundingBox> &boxes, const std::vector<Circle> &circles)
{
	real_time_text.setString(std::to_string(real_time.asSeconds()));
	game_time_text.setString(std::to_string(game_time.asSeconds()));
	loop_counter_text.setString(std::to_string(loop_counter));
	fps_text.setString(fps_str(elapsed));

	window->clear();
	draw_boxes(boxes);
	draw_circles(circles);
	window->draw(real_time_text);
	window->draw(game_time_text);
	window->draw(fps_text);
	window->draw(loop_counter_text);
	window->display();
}

void update(const std::vector<AxisAlignedBoundingBox> &boxes, std::vector<Circle> &circles)
{
	for (auto &circle : circles)
	{
		circle.position = circle.position + UPDATE_TIME.asSeconds() * circle.velocity;
	}
}*/

#include "Application.h"

int main()
{
	Application app;
	app.run();

	return 0;
}
