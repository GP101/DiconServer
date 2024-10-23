#pragma once
#include "KGen.h"
#include <memory>


class KThread;
typedef KThread* (*CreateDbThreadCallback)( const wchar_t* );

struct KDbLayerInitParam
{
    CreateDbThreadCallback
                        pfnCreateDbThread;
    DWORD               dwNumThreads;
	std::string			strUid;
	std::string			strPwd;
	std::string			strDsn;
};

class KUser;
typedef std::shared_ptr<KUser> (*CreateUserCallback)();
struct KNetLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    DWORD               dwNumIocpThreads;
};

class KFsm;
typedef std::shared_ptr<KFsm> (*CreateFsmCallback)();
struct KUserLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    CreateFsmCallback   pfnCreateFsmCallback;
};
