export module Turret;
import GameObject;
import <SFML/Graphics.hpp>;
import <string>;

export class Turret : public GameObject {

	int health, index;
	bool fade_out;

public:

	Turret(int i, int type, int h, sf::Texture& t) : index(i), health(h), fade_out(false) {
		setType(type);
		setTexture(t);
		float x = 112.5 + float(index % 8) * 125.0;
		float y = 135.0 + (125.0 - getBounds().height) / 2 + float(index / 8) * 125.0;
		setPosition(int(x), int(y));
	}

	bool update(const float& dt, const std::vector<sf::FloatRect>& t, const std::vector<std::tuple<sf::FloatRect, int, int>>& p, const std::vector<std::pair<sf::FloatRect, int>>& e) override {
		if (fade_out) {
			return fadeOut(dt);
		}
		for (int i = 0; i < e.size(); i++) {
			if (getBounds().intersects(e[i].first)) {
				health -= e[i].second;
			}
		}
		if (health <= 0)
			fade_out = true;
		return true;
	}

	int getIndex() override {
		return index;
	}

};