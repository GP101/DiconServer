#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <iostream>

static auto const flags = boost::archive::no_header | boost::archive::no_tracking;

template <typename T>
std::string direct(T const& v) {
	std::ostringstream oss;
	{
		boost::archive::binary_oarchive oa(oss, flags);
		oa << v;
	}
	return oss.str();
}

template <typename T>
std::string as_pod_array(T const& v) {
	std::ostringstream oss;
	{
		boost::archive::binary_oarchive oa(oss, flags);
		oa << v.size() << boost::serialization::make_array(v.data(), v.size());
	}
	return oss.str();
}

int main() {
	std::vector<int> i(100);
	std::cout << "direct: " << direct(i).size() << "\n";
	std::cout << "as_pod_array: " << as_pod_array(i).size() << "\n";
}