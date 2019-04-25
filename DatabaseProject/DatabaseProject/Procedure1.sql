--DROP PROCEDURE [dbo].[spEcho2]
CREATE PROCEDURE [dbo].[spEcho2]
	@param1 varchar(20) OUTPUT
AS
	UPDATE MyUser
	SET age = age + 1
	WHERE loginName = @param1
