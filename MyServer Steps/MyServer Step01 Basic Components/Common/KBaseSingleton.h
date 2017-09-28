#pragma once
#include <boost/shared_ptr.hpp>


template<typename T>
class KBaseSingleton
{
public:
    template<typename U>
    static void         CreateInstance( DWORD dwParam ) { ms_instance.reset( new U() ); }

protected:
    static boost::shared_ptr<T>
                        ms_instance;

public:
    static boost::shared_ptr<T>
                        Instance() { return ms_instance; }
};

template<typename T>
boost::shared_ptr<T> KBaseSingleton<T>::ms_instance = nullptr;
