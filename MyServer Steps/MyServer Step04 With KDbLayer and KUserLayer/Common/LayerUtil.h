#pragma once
#include "KGen.h"
#include <boost/shared_ptr.hpp>


class KThread;
typedef KThread* (*CreateDbThreadCallback)( const wchar_t* );

struct KDbLayerInitParam
{
    CreateDbThreadCallback
                        pfnCreateDbThread;
    DWORD               dwNumThreads;
};

class KUser;
typedef boost::shared_ptr<KUser> (*CreateUserCallback)();
struct KNetLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    DWORD               dwNumIocpThreads;
};

class KFsm;
typedef boost::shared_ptr<KFsm> (*CreateFsmCallback)();
struct KUserLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    CreateFsmCallback   pfnCreateFsmCallback;
};
