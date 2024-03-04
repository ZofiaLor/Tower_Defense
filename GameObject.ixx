export module GameObject;
import <string>;
import <utility>;
import <vector>;
import <SFML/Graphics.hpp>;

export class GameObject {

	sf::Sprite sprite;
	int type;

public:

	void setType(int& t) {
		type = t;
	}

	int getType() {
		return type;
	}

	void setTexture(sf::Texture& t) {
		sprite.setTexture(t);
	}

	void setPosition(float x, float y) {
		sprite.setPosition(x, y);
	}

	void move(float x, float y) {
		sprite.move(x, y);
	}

	bool fadeOut(const float& dt) {
		sf::Color color = sprite.getColor();
		if (color.a - 255.0 * dt <= 0)
			return false;
		color.a -= 255.0 * dt;
		sprite.setColor(color);
		return true;
	}

	virtual void Draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}

	virtual sf::FloatRect getBounds() {
		return sprite.getGlobalBounds();
	}

	virtual bool update(const float& dt, const std::vector<sf::FloatRect> &t, const std::vector<std::tuple<sf::FloatRect, int, int>>& p, const std::vector<std::pair<sf::FloatRect, int>>& e) = 0;

	virtual int getDamage() {
		return 0;
	}

	virtual int getRow() {
		return 0;
	}

	virtual int getDrop() {
		return 0;
	}

	virtual int getEffect() {
		return 0;
	}

	virtual int getIndex() {
		return 0;
	}

};