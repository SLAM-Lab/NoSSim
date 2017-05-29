#include <iostream>
#include <vector>
#ifndef VISION_GRAPH__H
#define VISION_GRAPH__H

using namespace std;

// Data structure to hold the global vision graphs information 
class VisionNode;
class VisionEdge {
   public:
	float weight;
	VisionNode* NodeA;
	VisionNode* NodeB; 	
	VisionEdge(){
		this->NodeA=NULL;
		this->NodeB=NULL;
	}
	VisionEdge(VisionNode* NodeA, VisionNode* NodeB, float weight){
		this->NodeA=(NodeA);
		this->NodeB=(NodeB);
		this->weight = weight;
	} 
	
};


class VisionNode {
   public:
	int ID;
	VisionNode (){ this->ID = -1;}
	VisionNode (int ID){ this->ID = ID;}
	bool operator == (const VisionNode & V)
		{return (this->ID == V.ID);}
};

class VisionGraph {
   public:
	vector<VisionNode> NodeList;
	vector<VisionEdge> EdgeList; 
	int size;
	VisionGraph(){
		size=0;
	}

	VisionGraph(int size){
		this-> size=size;
		for(int i=0;i<size;i++){
			NodeList.push_back(VisionNode(i));
		}
	}
	void AddEdge(int A, int B, float weight){
		if(A>=size || B>=size){
			std::cout<< "Array out of range" <<std::endl;				
			return;
		}
		EdgeList.push_back(VisionEdge(&(NodeList[A]), &(NodeList[B]), weight));
	}
};



#endif //VISION_GRAPH___H



