#pragma once
#include "KGen.h"
#include <memory>


class KUser;
typedef std::shared_ptr<KUser> (*CreateUserCallback)();
struct KNetLayerInitParam
{
    CreateUserCallback  pfnCreateUserCallback;
    DWORD               dwNumIocpThreads;
};
