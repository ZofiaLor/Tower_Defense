export module TextButton;
import <SFML/Graphics.hpp>;
import <string>;

export class TextButton {

	sf::RectangleShape shape;
	sf::Text text;
	sf::Font font;
	float width, height, x, y;

public:

	TextButton(float w, float h, float x, float y, std::string msg, sf::Font& f) : font(f), width(w), height(h), x(x), y(y) {
		shape.setSize({ width, height });
		shape.setPosition({ x, y });
		shape.setFillColor(sf::Color(81, 131, 207));
		text.setFont(font);
		
		text.setCharacterSize(int(height / 2));
		//text.setPosition(width - text.getGlobalBounds().width / 2 - x, height - text.getGlobalBounds().height / 2 - y);
		setText(msg);
	}

	void Draw(sf::RenderWindow& window) {
		window.draw(shape);
		window.draw(text);
	}

	std::string click() {
		return text.getString();
	}

	sf::FloatRect getBounds() {
		return shape.getGlobalBounds();
	}

	void setPosition(const float& x, const float& y) {
		shape.setPosition({ x, y });
		text.setPosition(x + width / 2.0, y + height / 2.0);
	}

	void setText(const std::string& txt) {
		text.setString(txt);
		sf::FloatRect textRect = text.getLocalBounds();
		int s = int(height / 2);
		while (textRect.width > width) {
			s -= 10;
			text.setCharacterSize(s);
			textRect = text.getLocalBounds();
		}
		text.setOrigin(int(textRect.left + textRect.width / 2.0f),
			int(textRect.top + textRect.height / 2.0f));
		text.setPosition(x + int(width / 2.0), y + int(height / 2.0));
	}

};