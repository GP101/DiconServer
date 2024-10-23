#pragma once
#include <memory>


template<typename T>
class KBaseSingleton
{
public:
    template<typename U>
    static void         CreateInstance( DWORD dwParam ) { ms_instance.reset( new U() ); }

protected:
    static std::shared_ptr<T>
                        ms_instance;

public:
    static std::shared_ptr<T>
                        Instance() { return ms_instance; }
};

template<typename T>
std::shared_ptr<T> KBaseSingleton<T>::ms_instance = nullptr;
