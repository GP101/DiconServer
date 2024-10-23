#include "KMyUserLayer.h"
#include <iomanip>
#include "KDbLayer.h"
#include "KMyGameServer.h"
#include "KMyUser.h"
#include "KMyUserFsm.h"
#include "KUserManager.h"
#include "KMyUserManager.h"


CONSTRUCTOR KMyUserLayer::KMyUserLayer()
{
}

DESTRUCTOR KMyUserLayer::~KMyUserLayer()
{
}

VIRTUAL bool KMyUserLayer::Initialize( DWORD dwParam_ )
{
    KUserManager::CreateInstance<KMyUserManager>( 0L );
    KUserLayer::Initialize( dwParam_ );

    KUserLayerInitParam* pInitParam = reinterpret_cast<KUserLayerInitParam*>( dwParam_ );
    assert( pInitParam != nullptr );

    return true;
}

VIRTUAL void KMyUserLayer::Finalize()
{
    KUserLayer::Finalize();
}

void KMyUserLayer::Update(DWORD dwElapsedTime_)
{
    KUserLayer::Update(dwElapsedTime_);
}
