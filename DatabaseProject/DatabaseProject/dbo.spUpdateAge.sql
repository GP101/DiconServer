CREATE PROCEDURE dbo.spUpdateAge
	@loginName varchar(40),
	@age int
AS   
    Update MyUser
	Set age = @age
	Where loginName = @loginName