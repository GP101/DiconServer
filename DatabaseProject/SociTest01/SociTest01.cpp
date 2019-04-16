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
	// open Database
	soci::backend_factory const& backEnd = odbc;
	soci::session sql(odbc, "filedsn=./MyCustomer.dsn");

	// Execute Sql
	int count;
	sql << "select count(*) from MyUser", soci::into(count);
	std::cout << count << std::endl;

	// Call Stored Procedure
	std::string in("jintaeks2");
	soci::statement proc = (sql.prepare << "spEcho2 :input", soci::use(in, "input"));
	proc.execute();
	proc.clean_up();

	std::string login;
	int age;
	soci::rowset<soci::row> results = (sql.prepare << "select loginName, age from MyUser");
	for (soci::rowset<soci::row>::const_iterator it = results.begin(); it != results.end(); ++it)
	{
		*it >> login >> age;
		std::cout << "login = " << login << ", age=" << age << std::endl;
	}
	system("pause");
}
