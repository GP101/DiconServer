#pragma once
#include "KGen.h"
#include <boost/shared_ptr.hpp>


class KUser;
typedef boost::shared_ptr<KUser> (*CreateUserCallback)();
struct KNetLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    DWORD               dwNumIocpThreads;
};
