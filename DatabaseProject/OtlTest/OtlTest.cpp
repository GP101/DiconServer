#include <iostream>
#include <stdio.h>
#define OTL_ODBC // Compile OTL 4.0/ODBC
#include "otlv4.h" // include the OTL 4.0 header file

otl_connect db; // connect object

// invoking stored procedure
void stored_proc(void)
{
	/** CreateUser stored procedure:
		USE [Customer]
		GO
		SET ANSI_NULLS ON
		GO
		SET QUOTED_IDENTIFIER ON
		GO
		ALTER Procedure[dbo].[CreateUser]
			@loginName varchar(50),
			@id bigint OUTPUT
		As
			Set NOCOUNT On;
			if EXISTS(SELECT * From User2 Where loginName = @loginName)
			begin
				Select id from User2 Where loginName = @loginName;
				Return 0 -- return false, loginName is already exist
			end
			else
			begin
				Insert into User2(loginName, creationDate)
					values(@loginName, getdate());
				Select @id = id from User2 Where loginName = @loginName;
				Return 1 -- return true, loginName is created
			end
	*/
	otl_stream o(1, // buffer size should be equal to 1 in case of stored procedure call
		"{call CreateUser("
		" :A<char[50],in>, "
		" :B<int,out>"
		")}",
		// stored procedure call
		db // connect object
	);

	o.set_commit(0); // set stream auto-commit off since
					 // the stream does not generate transaction
	std::string strLogin = "jintaeks5";
	o << strLogin.c_str(); // assigning :1 = 1, :3 = "Test String1"

	int id;
	o >> id;
	std::cout << "A=" << strLogin.c_str() << ", B=" << id << std::endl;
}

void select()
{
	/** User2 table structure, jintaeks on 2019/04/24 14:25
		loginName : nvarchar(50), nullable
		nickName : nvarchar(50), nullable
		age : int, nullable
		creationDate : datetime, nullable
		id : biting, identity
	 */
    otl_stream i(50, // buffer size
        "select loginName from User2",
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
		stored_proc(); // stored procedure test, 20190424_jintaeks
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
