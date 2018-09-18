#ifndef DYNAMO_RETURN_H
#define DYNAMO_RETURN_H

#include <Node.h>

namespace dynamo 
{

class Return : public dynamo::Node
{
public:
	virtual dynamo::NODE_TYPE getType() const { return RETURN; }
};

}

#endif // DYNAMO_RETURN_H
