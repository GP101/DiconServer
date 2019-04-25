#include <iostream>
#include <sstream>

void main()
{
	std::stringstream ss;
	std::uint64_t n1 = 1234567890;

	ss.write((char const*)&n1, sizeof(n1)); // sizeof(n1) gives the number of char needed

	std::uint64_t n2;
	ss.read((char*)&n2, sizeof(n2));

	std::cout << n2 << '\n';
}//main()
