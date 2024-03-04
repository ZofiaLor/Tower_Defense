export module Enemy;
import GameObject;
import <SFML/Graphics.hpp>;
import <string>;

export class Enemy : public GameObject {

	float bounds_left;
	int health, damage, full_damage, row, drop_money;
	double speed, full_speed;
	bool moving, fade_out;
	sf::Clock damage_cooldown, slow_down;

public:

	Enemy(int i, int type, int dmg, int h, double s, int drop, sf::Texture& t) : health(h), damage(dmg), full_damage(dmg), moving(true), fade_out(false), row(i / 8), drop_money(drop), speed(s), full_speed(s) {
		
		setType(type);
		setTexture(t);
		setPosition(1200.f, int(135.0 + (125.0 - getBounds().height) / 2 + float(row) * 125.0));
		bounds_left = getBounds().left;
		
	}

	bool update(const float& dt, const std::vector<sf::FloatRect>& t, const std::vector<std::tuple<sf::FloatRect, int, int>>& p, const std::vector<std::pair<sf::FloatRect, int>>& e) override {
		if (fade_out) {
			return fadeOut(dt);
		}
		for (int i = 0; i < p.size(); i++) {
			if (getBounds().intersects(std::get<0>(p[i]))) {
				health -= std::get<1>(p[i]);
				if (std::get<2>(p[i]) == 2) {
					speed = full_speed * 0.5;
					slow_down.restart();
				}
			}
		}
		moving = true;
		for (int i = 0; i < t.size(); i++) {
			if (getBounds().intersects(t[i])) {
				moving = false;
				if (damage_cooldown.getElapsedTime().asMilliseconds() >= 500) {
					damage = full_damage;
					damage_cooldown.restart();
				}
				else {
					damage = 0;
				}
				break;
			}
		}
		if (slow_down.getElapsedTime().asSeconds() >= 3) {
			speed = full_speed;
		}
		
		if (moving) {
			move(-speed * dt, 0.f);
			bounds_left -= speed * dt;
		}
		if (bounds_left <= 0.0 || health <= 0) {
			damage = 0;
			fade_out = true;
		}
		return true;
	}

	int getDamage() override {
		return damage;
	}

	int getRow() override {
		return row;
	}

	int getDrop() override {
		return drop_money;
	}

};