import <SFML/Graphics.hpp>;
export module Tile;

export class Tile {

	int x, y;
	int index;
	sf::RectangleShape tile;
	bool turret_placed;
	

public:

	Tile(int i) : index(i), turret_placed(false) {
		tile.setSize({ 125.f, 125.f });
		if ((index % 2 == 0 && index / 8 % 2 == 0) || (index % 2 == 1 && index / 8 % 2 == 1))
			tile.setFillColor(sf::Color(135, 222, 135));
		else
			tile.setFillColor(sf::Color(184, 229, 162));
		x = 100 + (index % 8) * 125;
		y = 135 + (index / 8) * 125;
		tile.setPosition( x, y );
	}

	void Draw(sf::RenderWindow& window) {
		window.draw(tile);
	}

	sf::FloatRect getBounds() {
		return tile.getGlobalBounds();
	}

	bool getTurretPlaced() {
		return turret_placed;
	}

	void setTurretPlaced(const bool& tp) {
		turret_placed = tp;
	}

};