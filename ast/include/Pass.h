#ifndef DYNAMO_PASS_H
#define DYNAMO_PASS_H

namespace dynamo 
{

class Node;
class Pass
{
public:
	virtual void handleNode(Node* n) = 0;
};

}

#endif // DYNAMO_PASS_H
