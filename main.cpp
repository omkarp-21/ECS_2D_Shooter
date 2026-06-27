#include <SFML/Graphics.hpp>
#include "Game.hpp"

int main()
{
    srand(time(0));
    Game game("config.txt");
    game.run();
    return 0;
}