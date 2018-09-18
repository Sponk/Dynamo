#ifndef DYNAMO_PASS_H
#define DYNAMO_PASS_H

#include <Node.h>

namespace dynamo 
{

class Node;
class Pass
{
public:
	virtual void handleNode(NodeRef n) = 0;
};

}

#endif // DYNAMO_PASS_H
