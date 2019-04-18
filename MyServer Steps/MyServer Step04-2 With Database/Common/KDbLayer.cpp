#include "KDbLayer.h"


CONSTRUCTOR KDbLayer::KDbLayer()
    : m_pfnCreateThread( nullptr )
{
}

DESTRUCTOR KDbLayer::~KDbLayer() noexcept(true)
{
}

bool KDbLayer::Initialize( DWORD dwParam_ ) 
{
    KDbLayerInitParam* pInitParam = reinterpret_cast<KDbLayerInitParam*>( dwParam_ );
    BOOST_ASSERT( pInitParam != nullptr );

    m_pfnCreateThread = pInitParam->pfnCreateDbThread;

	otl_connect::otl_initialize(); // initialize ODBC environment
	try {
		char buff[1024];
		snprintf(buff, _countof(buff), "UID=%s;PWD=%s;DSN=%s"
			, pInitParam->strUid.c_str()
			, pInitParam->strPwd.c_str()
			, pInitParam->strDsn.c_str());
		//db.rlogon("UID=sa;PWD=1234;DSN=MySqlExpress"); // connect to ODBC
		m_odbc.rlogon(buff); // connect to ODBC

		// Odbc connection test. 20190418_jintaeks
		{
			otl_stream i(50, // buffer size
				"select loginName from MyUser"
				, m_odbc
			);

			// create select stream
			char f2[31];
			while (!i.eof()) { // while not end-of-data
				i >> f2;
				std::cout << "f2=" << f2 << std::endl;
			}
		}
	}
	catch (otl_exception& p) { // intercept OTL exceptions
		std::cerr << p.msg << std::endl; // print out error message
		std::cerr << p.stm_text << std::endl; // print out SQL that caused the error
		std::cerr << p.sqlstate << std::endl; // print out SQLSTATE message
		std::cerr << p.var_info << std::endl; // print out the variable that caused the error
	}

    const bool bIsInitialized = KThreadManager::Initialize( pInitParam->dwNumThreads );
    if( bIsInitialized == true )
        KThreadManager::BeginAllThread();

    return bIsInitialized;
}

void KDbLayer::Finalize()
{
    KThreadManager::EndAllThread();
    VIRTUAL KThreadManager::Finalize();

	m_odbc.logoff(); // disconnect from Oracle
}

void KDbLayer::Dump()
{
}

VIRTUAL KThread* KDbLayer::CreateThread()
{
    if( m_pfnCreateThread != nullptr ) {
        return m_pfnCreateThread( m_strFileDSN.c_str() );
    }
    return nullptr;
}
