export module Projectile;
import GameObject;
import <SFML/Graphics.hpp>;
import <string>;

export class Projectile : public GameObject {

	float bounds_right;
	int damage, full_damage, effect;
	double speed;
	bool fade_out;
	int fo;

public:

	Projectile(int i, int type, int dmg, double s, int e, sf::Texture& t) : fo(0), damage(dmg), full_damage(dmg), speed(s), effect(e), fade_out(false) {
		setType(type);
		setTexture(t);
		float x = 222.5 + float(i % 8) * 125.0;
		float y = 135.0 + (125.0 - getBounds().height) / 2 + float(i / 8) * 125.0;
		setPosition(int(x), int(y));
		bounds_right = getBounds().left + getBounds().width;
	}

	
	bool update(const float& dt, const std::vector<sf::FloatRect>& t, const std::vector<std::tuple<sf::FloatRect, int, int>>& p, const std::vector<std::pair<sf::FloatRect, int>>& e) override {
		
		if (fade_out) {
			if (getType() == 4) {
				fo++;
				if (fo >= 5) {
					fo = 0;
					return fadeOut(dt);
				}
				return true;
			}
			return fadeOut(dt);
		}
		for (int i = 0; i < e.size(); i++) {
			if (getBounds().intersects(e[i].first)) {
				damage = 0;
				fade_out = true;
				break;
			}
		}
		move(speed * dt, 0.f);
		bounds_right += speed * dt;
		if (bounds_right >= 1200.0)
			fade_out = true;
		return true;
	}

	int getDamage() override {
		return damage;
	}

	int getEffect() override {
		return effect;
	}

};