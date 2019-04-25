--select * from MyUser
DECLARE @param1 varchar(20)
DECLARE @result varchar(20)

exec spEcho @param1, 'hello';
select @result = @param1