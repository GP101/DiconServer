#pragma once


template<typename T>
class KGlobalSingleton
{
public:
    inline static T&    Singleton() { return KSingletonCreator::Access(); }
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

        inline static T& Access()
        {
            // when an static object is declared inside in a inline function.
            // more than two objects can be created.
            // to fix this problem, we need to access non-inline fuction that has a static instance inside it.
            static T& ref = _CreateUniqueInstance();
            return ref;
        }//Access()

    private:
        inline void _ToGuaranteeInstanceCreationBeforeMainFunction() {}

        __declspec( noinline ) static T& _CreateUniqueInstance()
        {
            static T obj;
            // by calling meaningless member function, 'ms_kSingletonCreator' must be created.
            // it means than 'obj' of type 'T' will be created before main().
            ms_kSingletonCreator._ToGuaranteeInstanceCreationBeforeMainFunction();
            return obj;
        }

    private:
        static KSingletonCreator    ms_kSingletonCreator;
    };//struct KSingletonCreator
};

template<typename T>
/*static*/ typename KGlobalSingleton<T>::KSingletonCreator  KGlobalSingleton<T>::KSingletonCreator::ms_kSingletonCreator;
