#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "Node.h"
#include <iostream>
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
    //con::ShowConsole();
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    /*nd::Node test(2, 1);
    test.add(nd::Node(2, 1));
    test.add(nd::Node(1));
    test.connect(-1, 0, 0, 0);
    test.connect(-1, 1, 0, 1);
    test.connect(0, 0, 1, 0);
    test.connect(-1, 1, 1, 1);
    test.connect(1, 0, -2, 0);
    test.nodes[0].add(nd::Node(1));
    test.nodes[0].add(nd::Node(2));
    test.nodes[0].connect(-1, 0, 0, 0);
    test.nodes[0].connect(-1, 1, 0, 1);
    test.nodes[0].connect(0, 0, 1, 0);
    test.nodes[0].connect(1, 0, -2, 0);
    test.set(0, false);
    test.set(1, false);
    test.run();
    test.run();
    test.run();
    std::cout << test.output.values[0] << std::endl;
    test.set(0, true);
    test.set(1, false);
    test.run();
    test.run();
    test.run();
    std::cout << test.output.values[0] << std::endl;
    test.set(0, false);
    test.set(1, true);
    test.run();
    test.run();
    test.run();
    std::cout << test.output.values[0] << std::endl;
    test.set(0, true);
    test.set(1, true);
    test.run();
    test.run();
    test.run();
    std::cout << test.output.values[0] << std::endl;*/
    //std::cout << test.input.values[0];// << test.input.values[1] << std::endl;
    //std::cout << test.nodes[0].input.values[0];// << test.nodes[0].input.values[1] << std::endl;
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

