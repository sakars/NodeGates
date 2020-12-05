#include <SFML/Graphics.hpp>
#include <Windows.h>
namespace con {
    void HideConsole()
    {
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }

    void ShowConsole()
    {
        ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    }

    bool IsConsoleVisible()
    {
        return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
    }
}

int main()
{
    con::HideConsole();
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}

