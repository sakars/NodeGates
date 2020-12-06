#include "Node.h"
namespace nd {
	Node::Node(int i, int o) : input{ i }, output{ o }, nodes{}, dirty{}, type{ 0 } {
	}
	Node::Node(int typ) : input{ 1 }, output{ 1 }, nodes{}, dirty{}, type{ typ }{
		if (type == 1) {//and
			input.addNodes(1);
		}
		if (type == 2) {//not
			input.dirty[0] = true;
			output.values[0] = true;
			output.dirty[0] = true;
		}
		if (type == 3) {//or
			input.addNodes(1);
		}
	}
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
	void Node::run() {
		if(type==0){//nested
			std::set<int> newStack;
			for (int n : dirty) {
				nodes[n].run();
				if (!nodes[n].dirty.empty())newStack.insert(n);
				for (int i = 0; i < (int)nodes[n].output.dirty.size(); i++) {
					if (nodes[n].output.dirty[i]) {
						nodes[n].output.dirty[i] = false;
						for (int i2 = 0; i2 < (int)nodes[n].output.destinations[i].size(); i2++) {
							if (nodes[n].output.destinations[i][i2][0] == -2) {
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
		else if (type == 1) {//and
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
		
	}
	
	Input::Input(int count) : values{}, dirty{}, destinations{}{
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
	Output::Output(int count) : values{}, dirty{}, destinations{}{
		for (int i = 0; i < count; i++) {
			values.push_back(false);
			dirty.push_back(false);
			destinations.push_back(std::vector<std::vector<int>>());
		}
	}
}