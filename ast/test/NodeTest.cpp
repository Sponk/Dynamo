#include <gtest/gtest.h>
#include <VariableScope.h>

using namespace dynamo;

TEST(VariableScope, pushpop)
{
	VariableScope scope;
	
	scope.set("T", (Node*) 1);
	scope.set("U", (Node*) 1);
	
	scope.push();
	
	scope.set("V", (Node*) 2);
	scope.set("W", (Node*) 2);
	scope.set("U", (Node*) 2);
	
	EXPECT_EQ(2, (long) scope.get("V"));
	EXPECT_EQ(2, (long) scope.get("W"));
	EXPECT_EQ(2, (long) scope.get("U"));
	
	scope.pop();
	
	EXPECT_EQ(1, (long) scope.get("T"));
	EXPECT_EQ(1, (long) scope.get("U"));
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

