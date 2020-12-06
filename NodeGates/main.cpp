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
namespace win {
	void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom, sf::View &v)
	{
		const sf::Vector2f beforeCoord(window.mapPixelToCoords(pixel));
		v.zoom(zoom);
		window.setView(v);
		const sf::Vector2f afterCoord(window.mapPixelToCoords(pixel));
		const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
		v.move(offsetCoords);
	}
}

int main()
{
    con::HideConsole();

	//temporary drawables
	std::vector<std::vector<sf::Vertex>> shapes = {
		{	sf::Vertex(sf::Vector2f(100.0f, 100.0f), sf::Color(24, 143, 123)),
			sf::Vertex(sf::Vector2f(240.0f, 100.0f), sf::Color(24, 143, 123)),
			sf::Vertex(sf::Vector2f(240.0f, 180.0f), sf::Color(24, 143, 123)),
			sf::Vertex(sf::Vector2f(100.0f, 180.0f), sf::Color(24, 143, 123))
		},
		{
			sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(143, 24, 74)),
			sf::Vertex(sf::Vector2f(140.0f, 0.0f), sf::Color(143, 24, 74)),
			sf::Vertex(sf::Vector2f(140.0f, 80.0f), sf::Color(143, 24, 74)),
			sf::Vertex(sf::Vector2f(0.0f, 80.0f), sf::Color(143, 24, 74))
		},
		{
			sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(114, 163, 39)),
			sf::Vertex(sf::Vector2f(140.0f, 0.0f), sf::Color(114, 163, 39)),
			sf::Vertex(sf::Vector2f(140.0f, 80.0f), sf::Color(114, 163, 39)),
			sf::Vertex(sf::Vector2f(0.0f, 80.0f), sf::Color(114, 163, 39))
		}
	};
	std::vector<std::vector<int>> counts = {
		{0, 4, 2},
		{2, 2, 1}
	};
	std::vector<std::vector<int>> nodeConnections = {};

	//for drag
	int dragRelative[4] = {-1, 0, 0, -1};
	int nodeDraw[5] = {-1, 0, 0, 0, 0}; //on/off x y ind1 ind2

	//for camera
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "NodeGates", sf::Style::Close | sf::Style::Resize);
	sf::VertexArray va(sf::Quads);
	sf::VertexArray nodes(sf::Lines);
	sf::View view = window.getView();
	sf::Vector2f mousePosO;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
			else if (event.type == sf::Event::Resized) {
				view.setSize(event.size.width, event.size.height);
			}
			//zoom
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					if (event.mouseWheelScroll.delta > 0) win::zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, (0.8f), view);
					else if (event.mouseWheelScroll.delta < 0) win::zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, 1.25f, view);
				}
			}
        }

		//reset
		va.clear();
		nodes.clear();
		window.clear(sf::Color(47, 61, 59));

		//pan
		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
			sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			if (dragRelative[3] == -1) {
				dragRelative[3] = 1;
				mousePosO = mousePos;
			}
			sf::Vector2f pos((float)mousePosO.x - mousePos.x, (float)mousePosO.y - mousePos.y);
			view.move(pos.x, pos.y);
			mousePosO = mousePos;
		}
		else {
			dragRelative[3] = -1;
		}

		//mouse i
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			if(dragRelative[0] == -1){
				//node io drag start
				if (nodeDraw[0] == -1) {
					for (int i = 0; i < shapes.size(); i++) {
						for (int k = 0; k < counts[1][i]; k++) {
							if (shapes[i][1].position.x - 7 < mousePos.x &&
								shapes[i][1].position.x + 7 > mousePos.x &&
								shapes[i][0].position.y + 32 + k * 25 < mousePos.y &&
								shapes[i][0].position.y + 32 + k * 25 + 14 > mousePos.y
							) {
								nodeDraw[0] = 1;
								nodeDraw[1] = shapes[i][1].position.x;
								nodeDraw[2] = shapes[i][0].position.y + 39 + k * 25;
								nodeDraw[3] = i;
								nodeDraw[4] = k;
								break;
							}
						}
					}
				//node drag start
					if (nodeDraw[0] == -1) {
						for (int i = 0; i < shapes.size(); i++) {
							if (shapes[i][0].position.x < mousePos.x &&
								shapes[i][1].position.x > mousePos.x&&
								shapes[i][0].position.y < mousePos.y &&
								shapes[i][2].position.y > mousePos.y
								) {
								dragRelative[0] = i;
								dragRelative[1] = mousePos.x - shapes[i][0].position.x;
								dragRelative[2] = mousePos.y - shapes[i][0].position.y;
								break;
							}
						}
					}
				}
				else {
					nodes.append(sf::Vertex(sf::Vector2f(nodeDraw[1], nodeDraw[2])));
					nodes.append(sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y)));
				}
			}
			else {
				//node drag movement
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = i * 2 + j;
						shapes[dragRelative[0]][ci].position.x = mousePos.x - dragRelative[1] + 140.0f * floor((ci%3 + 1)/2);
						shapes[dragRelative[0]][ci].position.y = mousePos.y - dragRelative[2] + (80.0f + (std::max(counts[0][dragRelative[0]], counts[1][dragRelative[0]]) - 1)*25) * (ci/2);
					}
				}
			}
		}
		else {
			//end drag
			dragRelative[0] = -1;
			if (nodeDraw[0] != -1) {
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				bool found = false;
				for (int i = 0; i < shapes.size(); i++) {
					for (int k = 0; k < counts[0][i]; k++) {
						if (shapes[i][0].position.x - 7 < mousePos.x &&
							shapes[i][0].position.x + 7 > mousePos.x &&
							shapes[i][0].position.y + 32 + k * 25 < mousePos.y &&
							shapes[i][0].position.y + 32 + k * 25 + 14 > mousePos.y
						) {
							nodeConnections.push_back(std::vector<int>());
							nodeConnections[nodeConnections.size() - 1].push_back(i);
							nodeConnections[nodeConnections.size() - 1].push_back(k);
							found = true;
							i = shapes.size();
							break;
						}
					}
				}
				if (found) {
					nodeConnections[nodeConnections.size() - 1].push_back(nodeDraw[3]);
					nodeConnections[nodeConnections.size() - 1].push_back(nodeDraw[4]);
				}
				nodeDraw[0] = -1;
			}
		}

		//node io points
		for (int i = 0; i < shapes.size(); i++) {
			for (int j = 0; j < 4; j++) va.append(shapes[i][j]);
			for (int k = 0; k < counts[0][i]; k++)
			{
				for (int o = 0; o < 2; o++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = o * 2 + j;
						sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
						currentCorner.position.x = shapes[i][0].position.x - 7 + 14 * floor((ci % 3 + 1) / 2);
						currentCorner.position.y = shapes[i][0].position.y + 32 + k * 25 + 14 * (ci / 2);
						va.append(currentCorner);
					}
				}
			}
			for (int k = 0; k < counts[1][i]; k++)
			{
				for (int o = 0; o < 2; o++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = o * 2 + j;
						sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
						currentCorner.position.x = shapes[i][1].position.x - 7 + 14 * floor((ci % 3 + 1) / 2);
						currentCorner.position.y = shapes[i][0].position.y + 32 + k * 25 + 14 * (ci / 2);
						va.append(currentCorner);
					}
				}
			}
		}

		//mode io connections
		for (int i = 0; i < nodeConnections.size(); i++)
		{
			nodes.append(sf::Vertex(sf::Vector2f(shapes[nodeConnections[i][0]][0].position.x, shapes[nodeConnections[i][0]][1].position.y + 32 + 7 + nodeConnections[i][1] * 25)));
			nodes.append(sf::Vertex(sf::Vector2f(shapes[nodeConnections[i][2]][1].position.x, shapes[nodeConnections[i][2]][1].position.y + 32 + 7 + nodeConnections[i][3] * 25)));
		}


		window.setView(view);
		window.draw(va);
		window.draw(nodes);
        window.display();
    }

    return 0;
}

