#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "Node.h"
#include <iostream>
namespace con {//functions for console
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

namespace win {//functions for window
	void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom, sf::View &v)
	{
		const sf::Vector2f beforeCoord(window.mapPixelToCoords(pixel));
		v.zoom(zoom);
		window.setView(v);
		const sf::Vector2f afterCoord(window.mapPixelToCoords(pixel));
		const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
		v.move(offsetCoords);
	}
	void nodeMain() {

	}
}

int main()
{
    con::HideConsole();

	//master IO field
	nd::Node master(2, 2);


	//top bar
	struct preset
	{
		sf::String name;
		int node;
	};
	std::vector<preset> presetStorage = {
		{"AND", 1}, {"NOT", 2}, {"OR", 3}
	};
	preset tempCreate = { //for placement
		"", -1
	};

	//click
	int clickitey = -1;

	//drag
	int dragRelative[4] = {-1, 0, 0, -1};
	int nodeDraw[5] = {-1, 0, 0, 0, 0}; //on/off x y ind1 ind2
	sf::Vector2f mousePosO;

	//camera
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "NodeGates", sf::Style::Close | sf::Style::Resize);
	sf::VertexArray va(sf::Quads);
	sf::VertexArray nodes(sf::Lines);
	sf::View view = window.getView();
	view.move(sf::Vector2f(-75.f, -8.f));
	
	//font
	sf::Font font;
	if (!font.loadFromFile("./FuturaNowText-Bd.otf")) {}

    while (window.isOpen())
    {
		//events
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

		//mouse I
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			if(dragRelative[0] == -1){
				if (nodeDraw[0] == -1) {
					//pin connection drag start
					for (int i = 0; i < master.nodes.size(); i++) {
						for (int k = 0; k < master.nodes[i].output.values.size(); k++) {
							if (master.nodes[i].x + 140.f - 7 < mousePos.x &&
								master.nodes[i].x + 140.f + 7 > mousePos.x &&
								master.nodes[i].y + 32 + k * 25 < mousePos.y &&
								master.nodes[i].y + 32 + k * 25 + 14 > mousePos.y
							) {
								nodeDraw[0] = 1;
								nodeDraw[1] = master.nodes[i].x + 140.f;
								nodeDraw[2] = master.nodes[i].y + 39 + k * 25;
								nodeDraw[3] = i;
								nodeDraw[4] = k;
								break;
							}
						}
					}
					if (nodeDraw[0] == -1) {
						//I settings
						for (int k = 0; k < master.input.values.size(); k++) {
							if (master.input.x + 60 - 7 - 20 < mousePos.x &&
								master.input.x + 60 + 7 - 20 > mousePos.x&&
								master.input.y + 25 + k * 25 < mousePos.y &&
								master.input.y + 25 + k * 25 + 14 > mousePos.y
								) {
								clickitey = k;
								break;
							}
						}
						//create new node from master
						if (-20 < mousePos.x &&
							-70 > mousePos.x &&
							50 < mousePos.y &&
							0 > mousePos.y
							) {
							win::nodeMain();
						}
						if (clickitey == -1) {
							//I pin connection drag start
							for (int k = 0; k < master.input.values.size(); k++) {
								if (master.input.x + 60.f - 7 < mousePos.x &&
									master.input.x + 60.f + 7 > mousePos.x&&
									master.input.y + 32 + k * 25 < mousePos.y &&
									master.input.y + 32 + k * 25 + 14 > mousePos.y
									) {
									nodeDraw[0] = 1;
									nodeDraw[1] = master.input.x + 60.f;
									nodeDraw[2] = master.input.y + 39 + k * 25;
									nodeDraw[3] = -1;
									nodeDraw[4] = k;
									break;
								}
							}
							if (nodeDraw[0] == -1) {
								//node drag start
								for (int i = 0; i < master.nodes.size(); i++) {
									if (master.nodes[i].x < mousePos.x &&
										master.nodes[i].x + 140.f > mousePos.x&&
										master.nodes[i].y < mousePos.y &&
										master.nodes[i].y + 55.f + std::max(master.nodes[i].input.values.size() * 25, master.nodes[i].output.values.size() * 25) > mousePos.y
										) {
										dragRelative[0] = i;
										dragRelative[1] = mousePos.x - master.nodes[i].x;
										dragRelative[2] = mousePos.y - master.nodes[i].y;
										break;
									}
								}
								if (dragRelative[0] == -1) {
									//IO node drag start
									if (master.output.x < mousePos.x &&
										master.output.x + 140.0f > mousePos.x&&
										master.output.y < mousePos.y &&
										master.output.y + 55.f + master.output.values.size() * 25> mousePos.y
										) {
										dragRelative[0] = -2;
										dragRelative[1] = mousePos.x - master.output.x;
										dragRelative[2] = mousePos.y - master.output.y;
									}
									else if (master.input.x < mousePos.x &&
										master.input.x + 140.0f > mousePos.x&&
										master.input.y < mousePos.y &&
										master.input.y + 55.0f + master.input.values.size() * 25 > mousePos.y
										) {
										dragRelative[0] = -3;
										dragRelative[1] = mousePos.x - master.input.x;
										dragRelative[2] = mousePos.y - master.input.y;
									}
									else if (tempCreate.node == -1) {
										//spawn node from preset
										for (int i = 0; i < presetStorage.size(); i++) {
											preset c = presetStorage[i];
											if (i * 100 < mousePos.x &&
												i * 100 + 100 > mousePos.x &&
												0 < mousePos.y &&
												50 > mousePos.y
												) {
												tempCreate = c;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
				else {
				//while dragging pin connection
					nodes.append(sf::Vertex(sf::Vector2f(nodeDraw[1], nodeDraw[2])));
					nodes.append(sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y)));
				}
			}
			else {
				//while dragging node
				if (dragRelative[0] == -3) {
					master.input.x = mousePos.x - dragRelative[1];
					master.input.y = mousePos.y - dragRelative[2];
				}
				else if (dragRelative[0] == -2) {
					master.output.x = mousePos.x - dragRelative[1];
					master.output.y = mousePos.y - dragRelative[2];
				}
				else {
					master.nodes[dragRelative[0]].x = mousePos.x - dragRelative[1];
					master.nodes[dragRelative[0]].y = mousePos.y - dragRelative[2];
				}
			}
		}
		else {
			//set value of I
			if (clickitey != -1) {
				master.set(clickitey, !master.input.values[clickitey]);
				clickitey = -1;
			}
			else {
				dragRelative[0] = -1;
				if (nodeDraw[0] != -1) {
					//connect nodes
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					bool found = false;
					std::vector<int> h = {};
					for (int i = 0; i < master.nodes.size(); i++) {
						for (int k = 0; k < master.nodes[i].input.values.size(); k++) {
							if (master.nodes[i].x - 7 < mousePos.x &&
								master.nodes[i].x + 7 > mousePos.x&&
								master.nodes[i].y + 32 + k * 25 < mousePos.y &&
								master.nodes[i].y + 32 + k * 25 + 14 > mousePos.y
								) {
								h.push_back(i);
								h.push_back(k);
								found = true;
								i = master.nodes.size();
								break;
							}
						}
					}
					if (found) {
						master.connect(nodeDraw[3], nodeDraw[4], h[0], h[1]);
					}
					else {
						for (int k = 0; k < master.output.values.size(); k++) {
							if (master.output.x - 7 < mousePos.x &&
								master.output.x + 7 > mousePos.x&&
								master.output.y + 32 + k * 25 < mousePos.y &&
								master.output.y + 32 + k * 25 + 14 > mousePos.y
								) {
								h.push_back(-2);
								h.push_back(k);
								found = true;
								break;
							}
						}
						if (found) {
							master.connect(nodeDraw[3], nodeDraw[4], h[0], h[1]);
						}
					}
					nodeDraw[0] = -1;
				}
				if (tempCreate.node != -1) {
					master.add(nd::Node(tempCreate.node));
					tempCreate.node = -1;
				}
			}
		}

		//main draw
		for (int i = 0; i < master.nodes.size(); i++) {
			//nodes
			for (int o = 0; o < 2; o++)
			{
				for (int j = 0; j < 2; j++)
				{
					int ci = o * 2 + j;
					sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f), sf::Color(222, 104, 64));
					currentCorner.position.x = master.nodes[i].x + 140.f * floor((ci % 3 + 1) / 2);
					currentCorner.position.y = master.nodes[i].y + (55.f + std::max(master.nodes[i].input.values.size(), master.nodes[i].output.values.size()) * 25) * (ci / 2);
					va.append(currentCorner);
				}
			}
			//pins
			for (int k = 0; k < master.nodes[i].input.values.size(); k++)
			{
				for (int o = 0; o < 2; o++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = o * 2 + j;
						sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
						currentCorner.position.x = master.nodes[i].x - 7 + 14 * floor((ci % 3 + 1) / 2);
						currentCorner.position.y = master.nodes[i].y + 32 + k * 25 + 14 * (ci / 2);
						va.append(currentCorner);
					}
				}
			}
			for (int k = 0; k < master.nodes[i].output.values.size(); k++)
			{
				for (int o = 0; o < 2; o++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = o * 2 + j;
						sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
						currentCorner.position.x = master.nodes[i].x + 140 - 7 + 14 * floor((ci % 3 + 1) / 2);
						currentCorner.position.y = master.nodes[i].y + 32 + k * 25 + 14 * (ci / 2);
						va.append(currentCorner);
					}
				}
			}
		}

		//main IO nodes
		for (int o = 0; o < 2; o++)
		{
			for (int j = 0; j < 2; j++)
			{
				int ci = o * 2 + j;
				sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f), sf::Color(71, 214, 193));
				currentCorner.position.x = master.output.x + 60.f * floor((ci % 3 + 1) / 2);
				currentCorner.position.y = master.output.y + (55.f + master.output.values.size() * 25) * (ci / 2);
				va.append(currentCorner);
			}
		}
		for (int o = 0; o < 2; o++)
		{
			for (int j = 0; j < 2; j++)
			{
				int ci = o * 2 + j;
				sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f), sf::Color(71, 214, 193));
				currentCorner.position.x = master.input.x + 60 * floor((ci % 3 + 1) / 2);
				currentCorner.position.y = master.input.y + (55 + master.input.values.size() * 25) * (ci / 2);
				va.append(currentCorner);
			}
		}
		//main IO pins
		for (int k = 0; k < master.input.values.size(); k++)
		{
			bool yes = master.input.values[k];
			for (int l = 0; l < 2; l++)
			{
				for (int o = 0; o < 2; o++)
				{
					for (int j = 0; j < 2; j++)
					{
						int ci = o * 2 + j;
						float times = yes && (l == 0) ? 1.5 : 1;
						sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
						currentCorner.position.x = master.input.x + 60 - 15 * (1 - l) - (int)7 * (((float)l + 1) / 2) * times + 14 * (((float)l + 1) / 2) * times * floor((ci % 3 + 1) / 2);
						currentCorner.position.y = master.input.y + 32 + k * 25 + 7 * ((float)(1 - l) / 2) - times * 3 + 14 * (((float)l + 1) / 2) * times * (ci / 2);
						va.append(currentCorner);
					}
				}
			}
		}
		for (int k = 0; k < master.output.values.size(); k++)
		{
			for (int o = 0; o < 2; o++)
			{
				for (int j = 0; j < 2; j++)
				{
					int ci = o * 2 + j;
					sf::Vertex currentCorner(sf::Vector2f(0.0f, 0.0f));
					currentCorner.position.x = master.output.x - 7 + 14 * floor((ci % 3 + 1) / 2);
					currentCorner.position.y = master.output.y + 32 + k * 25 + 14 * (ci / 2);
					va.append(currentCorner);
				}
			}
		}

		//node io connections
		for (int i = 0; i < master.nodes.size(); i++) {
			nd::Node c = master.nodes[i];
			for (int j = 0; j < c.output.destinations.size(); j++)
			{
				for (int k = 0; k < c.output.destinations[j].size(); k++)
				{
					sf::Color colorCurrent = c.output.values[j] ? sf::Color::White : sf::Color(102, 122, 119);
					nodes.append(sf::Vertex(sf::Vector2f((float)c.x + 140.f, (float)c.y + 32 + 7 + j * 25), colorCurrent));
					if (c.output.destinations[j][k][0] != -2) nodes.append(sf::Vertex(sf::Vector2f((float)master.nodes[c.output.destinations[j][k][0]].x, (float)master.nodes[c.output.destinations[j][k][0]].y + 32 + 7 + c.output.destinations[j][k][1] * 25), colorCurrent));
					else nodes.append(sf::Vertex(sf::Vector2f((float)master.output.x, (float)master.output.y + 32 + 7 + c.output.destinations[j][k][1] * 25), colorCurrent));
				}
			}
		}
		for (int j = 0; j < master.input.destinations.size(); j++)
		{
			for (int k = 0; k < master.input.destinations[j].size(); k++)
			{
				sf::Color colorCurrent = master.input.values[j] ? sf::Color::White : sf::Color(102, 122, 119);
				nodes.append(sf::Vertex(sf::Vector2f((float)master.input.x + 60.f, (float)master.input.y + 32 + 7 + j * 25), colorCurrent));
				nodes.append(sf::Vertex(sf::Vector2f((float)master.nodes[master.input.destinations[j][k][0]].x, (float)master.nodes[master.input.destinations[j][k][0]].y + 32 + 7 + master.input.destinations[j][k][1] * 25), colorCurrent));
			}
		}

		//run nodes
		master.run();

		//texts of selection
		sf::String endDisplay = " ";
		for (int i = 0; i < presetStorage.size(); i++) {
			preset c = presetStorage[i];
			endDisplay += c.name;
			for (int i = 0; i < 12 - c.name.getSize(); i++) {
				endDisplay += " ";
			}
			nodes.append(sf::Vertex(sf::Vector2f(i * 100, 0)));
			nodes.append(sf::Vertex(sf::Vector2f(i * 100, 50)));
		}
		sf::Text text(endDisplay, font);
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);

		//plus
		va.append(sf::Vertex(sf::Vector2f(-30 - 20, 10)));
		va.append(sf::Vertex(sf::Vector2f(-20 - 20, 10)));
		va.append(sf::Vertex(sf::Vector2f(-20 - 20, 40)));
		va.append(sf::Vertex(sf::Vector2f(-30 - 20, 40)));

		va.append(sf::Vertex(sf::Vector2f(-20 - 40, 20)));
		va.append(sf::Vertex(sf::Vector2f(-20 - 10, 20)));
		va.append(sf::Vertex(sf::Vector2f(-20 - 10, 30)));
		va.append(sf::Vertex(sf::Vector2f(-20 - 40, 30)));

		//main draw calls
		window.setView(view);
		window.draw(text);
		window.draw(va);
		window.draw(nodes);

		//texts of nodes
		for (int i = 0; i < master.nodes.size(); i++)
		{
			sf::Text visualName(master.nodes[i].name, font);
			visualName.setCharacterSize(20);
			visualName.setFillColor(sf::Color::White);
			visualName.setPosition(master.nodes[i].x + 5.f, master.nodes[i].y);
			window.draw(visualName);
		}
		sf::Text inVisual("I", font);
		inVisual.setCharacterSize(50);
		inVisual.setFillColor(sf::Color::White);
		inVisual.setPosition(master.input.x + 5.f, master.input.y);
		window.draw(inVisual);
		sf::Text outVisual("O", font);
		outVisual.setCharacterSize(50);
		outVisual.setFillColor(sf::Color::White);
		outVisual.setPosition(master.output.x + 10.f, master.output.y);
		window.draw(outVisual);
		
		//yes
        window.display();
    }

    return 0;
}

