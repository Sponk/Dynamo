#ifndef DYNAMO_PASS_H
#define DYNAMO_PASS_H

#include <Node.h>

namespace dynamo 
{

class Node;
class Pass
{
protected:
	void check(bool b, const char* message)
	{
		if(!b)
			throw std::runtime_error(message);
	}
public:
	virtual void handleNode(NodeRef n) = 0;
};

}

#endif // DYNAMO_PASS_H
