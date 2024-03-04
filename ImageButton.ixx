export module ImageButton;
import <SFML/Graphics.hpp>;
import <string>;

export class ImageButton {

	sf::Texture active;
	sf::Texture inactive;
	sf::Sprite button;
	int price;
	bool clicked;

public:

	ImageButton(int type) : clicked (false) {

		active.loadFromFile("./images/buttons/t" + std::to_string(type) + "_active.png");
		inactive.loadFromFile("./images/buttons/t" + std::to_string(type) + "_inactive.png");
		button.setTexture(inactive);
		float mul = 240.0 + 100.0 * float(type);
		button.setPosition({ mul, 35.0 });

		// do zautomatyzowania
		if (type == 1) {
			price = 10;
		}
		else if (type == 2) {
			price = 20;
		}
		else if (type == 3) {
			price = 40;
		}
		else if (type == 4) {
			price = 75;
		}
	}

	void Draw(sf::RenderWindow& window) {
		window.draw(button);
	}

	sf::FloatRect getBounds() {
		return button.getGlobalBounds();
	}

	void deactivate() {
		clicked = false;
		button.setTexture(inactive);
	}

	int click() {
		if (clicked) {
			clicked = false;
			button.setTexture(inactive);
			return 0;
		}
		clicked = true;
		button.setTexture(active);
		return price;
	}

};