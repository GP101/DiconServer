#include <boost/coroutine/all.hpp>
#include <iostream>

using namespace boost::coroutines;

void cooperative(coroutine<void>::push_type &sink)
{
	std::cout << "Hello" << std::endl;
	sink();
	std::cout << "world" << std::endl;
	sink();
	std::cout << "Last" << std::endl;
}

int main()
{
	coroutine<void>::pull_type source{ cooperative };
	std::cout << "main1" << std::endl;
	source();
	std::cout << "main2" << std::endl;
	source();
}
/** output:
	Hello
	main1
	world
	main2
	Last
*/