Use Customer
Go
Alter Procedure dbo.CreateUser
    @loginName varchar(50),
    @id bigint OUTPUT
As
    Set NOCOUNT On;
    if EXISTS (SELECT * From User2 Where loginName = @loginName)
    begin
        Select id from User2 Where loginName = @loginName;
        Return 0 -- return false, loginName is already exist
    end
    else
    begin
        Insert into User2(loginName, creationDate) 
            values (@loginName, getdate());
        Select @id = id from User2 Where loginName = @loginName;
        Return 1 -- return true, loginName is created
    end