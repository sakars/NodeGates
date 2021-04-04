#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#pragma once
namespace nd {
	class NodeSave {
	public:
		struct dests {
			int nodeId = 0;
			int destPin = 0;
		};
		struct nestNode
		{
			int type = 0;
			std::vector<std::vector<dests>> destinations = {};
		};
		struct nodeData
		{
			int inS = 0;
			int outS = 0;
			std::vector<nestNode> nodes = {};
			std::vector<std::vector<dests>> inputDestinations = {};
		};
		void Load();


		std::vector<nodeData> loaded;
	};
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
		void addNodes(int count);
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
		void save(int type);//type >= 10
		std::string toString();
		static NodeSave templates;
	};
	
	
}


