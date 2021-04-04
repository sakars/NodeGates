#include "Node.h"
#include <fstream>
#include <iostream>
#include <string>
namespace nd {

	Node::Node(int i, int o) : input{ i }, output{ o }, nodes{}, dirty{}, type{ 0 }, x{ 200 }, y{ 200 }, name{ "BLANK" } {
	}

	Node::Node(int typ) : input{ 1 }, output{ 1 }, nodes{}, dirty{}, type{ typ }, x{ 200 }, y{ 200 }, name{ "BLANK" }{
		if (type == 1) {//and
			input.addNodes(1);
			name = "AND";
		}else if (type == 2) {//not
			input.dirty[0] = true;
			output.values[0] = true;
			output.dirty[0] = true;
			name = "NOT";
		}else if (type == 3) {//or
			input.addNodes(1);
			name = "OR";
		}else {//custom node-parse from save
			//configure I/O size
			input.addNodes(templates.loaded[typ - 10].inS - 1);
			output.addNodes(templates.loaded[typ - 10].outS - 1);
			//add the nodes
			for (int i = 0; i < (int)templates.loaded[typ - 10].nodes.size(); i++) {
				add(Node(templates.loaded[typ - 10].nodes[i].type));
			}
			//connect nodes
			for (int i = 0; i < (int)templates.loaded[typ - 10].nodes.size(); i++) {
				add(templates.loaded[typ - 10].nodes[i].type);
				for (int i2 = 0; i2 < (int)templates.loaded[typ - 10].nodes[i].destinations.size(); i2++) {
					for (int i3 = 0; i3 < (int)templates.loaded[typ - 10].nodes[i].destinations[i2].size(); i3++) {
						connect(i, i2, templates.loaded[typ - 10].nodes[i].destinations[i2][i3].nodeId, templates.loaded[typ - 10].nodes[i].destinations[i2][i3].destPin);
					}
				}
			}
			//connect inputs
			for (int i = 0; i < (int)templates.loaded[typ - 10].inputDestinations.size(); i++) {
				for (int i2 = 0; i2 < (int)templates.loaded[typ - 10].inputDestinations[i].size(); i2++) {
					connect(-1, i, templates.loaded[typ - 10].inputDestinations[i][i2].nodeId, templates.loaded[typ - 10].inputDestinations[i][i2].destPin);
				}
			}
		}
	}
	NodeSave Node::templates=NodeSave();
	void Node::add(Node node) {
		nodes.push_back(node);
		dirty.insert(nodes.size()-1);
	}
	void Node::set(int i, bool value) {
		input.dirty[i] = true;
		input.values[i] = value;
		for (int i2 = 0; i2 < (int)input.destinations[i].size(); i2++) {
			//nodes[input.destinations[i][i2][0]].input.values[input.destinations[i][i2][1]] = value;
			//nodes[input.destinations[i][i2][0]].input.dirty[input.destinations[i][i2][1]] = true;
			nodes[input.destinations[i][i2][0]].set(input.destinations[i][i2][1], value);
			dirty.insert(input.destinations[i][i2][0]);
		}
	}
	void Node::connect(int nodeId1, int pin1, int nodeId2, int pin2) {
		std::vector<int> dest = {nodeId2,pin2};
		switch (nodeId1)
		{
		case -1://input
			input.destinations[pin1].push_back(dest);
			break;
		case -2://output
			output.destinations[pin1].push_back(dest);
			break;
		default:
			nodes[nodeId1].output.destinations[pin1].push_back(dest);
			break;
		}
	}
	void Node::disconnect(int nodeId1,int pin1,int nodeId2,int pin2) {
		std::vector<int> dest = { nodeId2,pin2 };
		switch (nodeId1)
		{
		case -1://input
			remove(input.destinations[pin1].begin(), input.destinations[pin1].end(),dest);
			break;
		case -2://output
			remove(output.destinations[pin1].begin(), output.destinations[pin1].end(), dest);
			break;
		default:
			remove(nodes[nodeId1].output.destinations[pin1].begin(), nodes[nodeId1].output.destinations[pin1].end(), dest);
			break;
		}
	}
	void Node::run() {
		if (type == 1) {//and
			if (input.dirty[0] || input.dirty[1]) {
				input.dirty[0] = false;
				input.dirty[1] = false;
				output.dirty[0] = output.values[0] != (input.values[0] && input.values[1]);
				output.values[0] = input.values[0] && input.values[1];
			}
		}
		else if (type == 2) {//not
			if (input.dirty[0]) {
				input.dirty[0] = false;
				output.values[0] = !input.values[0];
				output.dirty[0] = true;
			}
		}
		else if (type == 3) {//or
			if (input.dirty[0] || input.dirty[1]) {
				input.dirty[0] = false;
				input.dirty[1] = false;
				output.dirty[0] = output.values[0] != (input.values[0] || input.values[1]);
				output.values[0] = input.values[0] || input.values[1];
			}
		}
		else {//nested
			std::set<int> newStack;
			for (int n : dirty) {
				nodes[n].run();
				if (!nodes[n].dirty.empty())newStack.insert(n);
				for (int i = 0; i < (int)nodes[n].output.dirty.size(); i++) {
					if (nodes[n].output.dirty[i]) {
						nodes[n].output.dirty[i] = false;
						for (int i2 = 0; i2 < (int)nodes[n].output.destinations[i].size(); i2++) {
							if (nodes[n].output.destinations[i][i2][0] == -2) {//output handling
								output.dirty[nodes[n].output.destinations[i][i2][1]] = true;
								output.values[nodes[n].output.destinations[i][i2][1]] = nodes[n].output.values[i];
							}
							else {
								newStack.insert(nodes[n].output.destinations[i][i2][0]);
								nodes[nodes[n].output.destinations[i][i2][0]].set(nodes[n].output.destinations[i][i2][1], nodes[n].output.values[i]);
							}

							/*nodes[n.output.destinations[i][i2][0]].input.values[n.output.destinations[i][i2][1]] = n.output.values[i];
							nodes[n.output.destinations[i][i2][0]].input.dirty[n.output.destinations[i][i2][1]] = true;*/
						}
					}
				}
			}
			dirty = newStack;
		}
		
	}
	std::string Node::toString() {//input_size, output_size, nodes<node_type, node_outputs<each_output_pin<destinations>>>, each_input_pin<each_pin_dest<node_id,dest_pin>>
		std::string ins="";
		for (std::vector<std::vector<int>> a : input.destinations) {
			for (std::vector<int> b : a) {
				ins += std::to_string(b[0]) + " " + std::to_string(b[1]) + " ";
				//ins += "b ";
			}
			ins += "b ";
		}
		ins += "b";
		std::string ns = "";
		for (Node a : nodes) {
			ns += std::to_string(a.type) + " ";
			for (int i = 0; i < (int)a.output.destinations.size(); i++) {
				for (int i2 = 0; i2 < (int)a.output.destinations[i].size();i2++) {
					ns += std::to_string(a.output.destinations[i][i2][0]) + " " + std::to_string(a.output.destinations[i][i2][1]) + " ";
					//ns += "b ";
				}
				ns += "b ";
			}
			ns += "b ";
		}
		ns += "b";
		return std::to_string(input.values.size()) + " " + std::to_string(output.values.size()) + " " + ns + " " + ins;
	}
	void Node::save(int type) {
		this->type = type;
		std::string Line;
		//std::ofstream Overwriter("SavedNodes.txt2");
		//std::ifstream Reader("SavedNodes.txt");
		std::ofstream Overwriter("SavedNodes.txt2");
		std::ifstream Reader("SavedNodes.txt");
		int lineNum = 10;
		// Use a while loop together with the getline() function to read the file line by line
		while (getline(Reader, Line)) {
			if (lineNum != type) {
				Overwriter << Line << std::endl;
			}
			else {
				Overwriter << this->toString() << std::endl;
			}
			
			lineNum++;
		}
		for (; lineNum < type; lineNum++) {
			Overwriter << std::endl;
		}
		if (lineNum == type) {
			Overwriter << this->toString() << std::endl;
		}
		// Close the file
		Reader.close();
		Overwriter.close();
		remove("SavedNodes.txt");
		if (rename("SavedNodes.txt2", "SavedNodes.txt") != 0) {
			throw "Oh no!";
		}
	}
	
	Input::Input(int count) : values{}, dirty{}, destinations{}, x{ 0 }, y{ 200 }{
		for (int i = 0; i < count; i++) {
			values.push_back(false);
			dirty.push_back(false);
			destinations.push_back(std::vector<std::vector<int>>());
		}
	}
	void Input::addNodes(int count) {
		for (int i = 0; i < count; i++) {
			values.push_back(false);
			dirty.push_back(false);
			destinations.push_back(std::vector<std::vector<int>>());
		}
	}
	Output::Output(int count) : values{}, dirty{}, destinations{}, x{ 500 }, y{ 200 }{
		for (int i = 0; i < count; i++) {
			values.push_back(false);
			dirty.push_back(false);
			destinations.push_back(std::vector<std::vector<int>>());
		}
	}
	void Output::addNodes(int count) {
		for (int i = 0; i < count; i++) {
			values.push_back(false);
			dirty.push_back(false);
			destinations.push_back(std::vector<std::vector<int>>());
		}
	}
	void NodeSave::Load(){//input_size, output_size, nodes<node_type, node_outputs<each_output_pin<node_id,dest_pin>>>, each_input_pin<each_pin_dest<node_id,dest_pin>>
		std::string Line;
		std::string ob;
		std::ifstream Reader("SavedNodes.txt");
		int stage = 0;/*
					  0-input_size
					  1-output_size
					  2-node_type	  !-->  (5)
					  3-node_id		  !-->  (2)					
					  4-dest_pin	   --> (2+1)
					  5-node_node_pin !-->  (5) // special case (when 2 breaks in a row the whole text ends)
					  6-node_dest_pin  --> (4+1)
					  */

					  /*
						0-input_size
						1-output_size
						2-node_type	  !-->  (6)
						3-node_id		  !-->  (2)
						4-dest_pin	
						5-node_id2	!--> (3) // --> (3+1)
						6-node_node_pin !-->  (6) // special case (when 2 breaks in a row the whole text ends)
						7-node_dest_pin  --> (5+1)
						*/
		loaded = {};
		while (std::getline(Reader, Line)) {   // get a whole line
			std::stringstream ss(Line);
			loaded.push_back(nodeData());
			stage = 0;
			while (std::getline(ss, ob, ' ')) {
				if (ob == "b") {
					switch (stage)
					{
					case 2:
						loaded.back().inputDestinations.push_back(std::vector<dests>());//create dest array for pin 0
						stage = 6;
						break;
					case 3:
						stage = 2;
						break;
					case 5:
						stage = 3;
						break;
					case 6:
						loaded.back().inputDestinations.push_back(std::vector<dests>());//create dest array for pin N
						stage = 6;
						break;
					
					}
				}
				else {
					switch (stage) {
					case 0:
						loaded.back().inS = std::stoi(ob);
						break;
					case 1:
						loaded.back().outS = std::stoi(ob);
						break;
					case 2:
						loaded.back().nodes.push_back(nestNode());
						loaded.back().nodes.back().type = std::stoi(ob);
						loaded.back().nodes.back().destinations.push_back(std::vector<dests>());
						break;
					case 3:
						loaded.back().nodes.back().destinations.back().push_back(dests());
						loaded.back().nodes.back().destinations.back().back().nodeId = std::stoi(ob);
						break;
					case 4:
						loaded.back().nodes.back().destinations.back().back().destPin = std::stoi(ob);
						break;
					case 5:
						loaded.back().nodes.back().destinations.back().push_back(dests());
						loaded.back().nodes.back().destinations.back().back().nodeId = std::stoi(ob);
						stage = 3;
						break;
					case 6:
						loaded.back().inputDestinations.back().push_back(dests());
						loaded.back().inputDestinations.back().back().nodeId = std::stoi(ob);
						break;
					case 7:
						loaded.back().inputDestinations.back().back().destPin = std::stoi(ob);
						stage = 5;
						break;
					}
					stage++;
				}
			}
		}
		// Close the file
		Reader.close();
	}
	
}