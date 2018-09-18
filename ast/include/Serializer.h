#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <Module.h>
#include <ostream>

namespace dynamo
{

// The backend needs to implement this!
void serialize(const Module& module, std::ostream& out);

}

#endif
