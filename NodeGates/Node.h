#include <vector>
#include <set>
#pragma once
namespace nd {
	class Input {
	public:
		Input(int count);
		std::vector<bool> values;
		std::vector<bool> dirty;
		std::vector<std::vector<std::vector<int>>> destinations;
		void SetNode(int id, bool value);
		void addNodes(int count);
	};
	class Output {
	public:
		Output(int count);
		std::vector<bool> values;
		std::vector<bool> dirty;
		std::vector<std::vector<std::vector<int>>> destinations;
	};
	class Node
	{
	public:
		Node(int i, int o);
		Node(int type);
		Input input;
		std::vector<Node> nodes;
		Output output;
		int type;//0-nested 1-and 2-not
		void run();
		void connect(int nodeId1, int pin1, int nodeId2, int pin2);
		std::set<int> dirty;
		void add(Node node);
		void set(int i, bool value);
	};
	
	
}


