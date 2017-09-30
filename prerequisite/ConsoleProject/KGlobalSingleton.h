#pragma once

#define IMPLEMENT_KGLOBALSINGLETON( T ) \
    public: \
        inline static T& SingletonT() { return KSingletonCreator::Access(); } \
    private: \
        friend class KGlobalSingleton<T>::KSingletonCreator; \
        T( const T& ); \
        T& operator=( const T& );

template<typename T>
class KGlobalSingleton
{
public:
    typedef T           Type;
    inline static Type& Singleton() { return KSingletonCreator::Access(); }
    inline              KGlobalSingleton() {}

private:
                        KGlobalSingleton( const KGlobalSingleton& );
                        KGlobalSingleton& operator=( const KGlobalSingleton& );

protected:
    class KSingletonCreator
    {
    public:
        inline KSingletonCreator()
        {
            Access();
        }//KSingletonCreator()

        inline static Type&    Access()
        {
            static  Type&  ref = _NoInlineAccess();
            return  ref;
        }//Access()

    private:
        inline void _DoNothing() {}

        __declspec( noinline ) static Type& _NoInlineAccess()
        {
            static Type obj;
            ms_kSingletonCreator._DoNothing();
            return obj;
        }//_NoInlineAccess()

    private:
        static KSingletonCreator    ms_kSingletonCreator;
    };//struct KSingletonCreator
};//template<typename T>

template <typename T>
/*static*/ typename KGlobalSingleton<T>::KSingletonCreator  KGlobalSingleton<T>::KSingletonCreator::ms_kSingletonCreator;
