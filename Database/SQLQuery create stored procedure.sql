USE Customer;
GO  
CREATE PROCEDURE dbo.spGetAllUser
AS   
    SET NOCOUNT ON;  
	SELECT [loginName]
      ,[nickName]
      ,[age]
      ,[creationDate]
	FROM [Customer].[dbo].[MyUser]
GO  
