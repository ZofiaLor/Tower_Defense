#include <SFML/Graphics.hpp>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
import Game;

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Tower Defense", sf::Style::Close);
    Game game;
    game.run(window);

}

