#pragma once

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "KGlobalSingleton.h"
#include "KGen.h"

class KLog : public KGlobalSingleton<KLog>
{
    typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

public:
    KLog()
    {
        Initialize();
    }

    ~KLog()
    {
        Finalize();
    }

    void Initialize()
    {
        sink = boost::make_shared<text_sink>();
        stream.reset( &std::clog, boost::empty_deleter{} );
        sink->locked_backend( )->add_stream( stream );
        boost::log::core::get( )->add_sink( sink );
    }

    void Finalize()
    {
        sink->flush( );
    }

    boost::log::sources::logger lg;

private:
    boost::shared_ptr<text_sink> sink;
    boost::shared_ptr<std::ostream> stream;
};

#define _KLog           KLog::Singleton()
#define KLOG            BOOST_LOG( _KLog.lg )
#define KLOG_NAME(a)    VAR_NAME( a ) << "=" << a
