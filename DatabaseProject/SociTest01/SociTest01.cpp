#include "soci/soci.h"
#include "soci/odbc/soci-odbc.h"
#include <iostream>
#include <string>

#pragma comment(lib,"libsoci_core_4_0.lib")
#pragma comment(lib,"libsoci_odbc_4_0.lib")

using namespace soci;

void main()
{
	std::cout << "Hello" << std::endl;
	soci::backend_factory const& backEnd = odbc;
	soci::session sql(odbc, "filedsn=./MyCustomer.dsn");
	int count;
	sql << "select count(*) from MyUser", soci::into(count);
	std::cout << count << std::endl;
}
