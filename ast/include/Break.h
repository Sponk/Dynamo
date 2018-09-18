#ifndef DYNAMO_BREAK_H
#define DYNAMO_BREAK_H

#include "Node.h"

namespace dynamo 
{

class Break : public dynamo::Node
{
public:
	virtual dynamo::NODE_TYPE getType() const { return BREAK; }

};

}

#endif // DYNAMO_BREAK_H
