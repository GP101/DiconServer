#include <iostream>
#include <stdio.h>
#define OTL_ODBC // Compile OTL 4.0/ODBC
#include "otlv4.h" // include the OTL 4.0 header file

otl_connect db; // connect object

void select()
{
    otl_stream i(50, // buffer size
        "select loginName from MyUser",
        // SELECT statement
        db // connect object
    );

    // create select stream
    char f2[31];
    while (!i.eof()) { // while not end-of-data
        i >> f2;
        std::cout << "f2=" << f2 << std::endl;
    }
}

int main()
{
    otl_connect::otl_initialize(); // initialize ODBC environment
    try {
        db.rlogon("UID=sa;PWD=1234;DSN=MySqlExpress"); // connect to ODBC
        select(); // select records from table
    }
    catch (otl_exception& p) { // intercept OTL exceptions
        std::cerr << p.msg << std::endl; // print out error message
        std::cerr << p.stm_text << std::endl; // print out SQL that caused the error
        std::cerr << p.sqlstate << std::endl; // print out SQLSTATE message
        std::cerr << p.var_info << std::endl; // print out the variable that caused the error
    }

    db.logoff(); // disconnect from Oracle

    return 0;
}
