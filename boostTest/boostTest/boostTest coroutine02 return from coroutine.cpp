// https://theboostcpplibraries.com/boost.coroutine
#include <boost/coroutine/all.hpp>
#include <functional>
#include <iostream>

using namespace std::placeholders;
using boost::coroutines::coroutine;

void cooperative(coroutine<int>::push_type &sink, int i)
{
	int j = i;
	sink(++j);
	sink(++j);
	std::cout << "end\n";
}

int main()
{

	coroutine<int>::pull_type source{ std::bind(cooperative, _1, 0) };
	std::cout << source.get() << '\n';
	source();
	std::cout << source.get() << '\n';
	source();
}
