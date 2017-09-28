#pragma once
#include <boost/pool/object_pool.hpp>

template<typename T>
class KPool
{
public:
    void*               operator new(size_t iBlockSize_)
                        {
                            return (void*)m_pool.malloc();
                        }

    void                operator delete(void* pBlock_)
                        {
                            m_pool.destroy( (T*)pBlock_ );
                        }

private:
    static boost::object_pool<T>
                        m_pool;
};

template<typename T>
boost::object_pool<T>   KPool<T>::m_pool;
