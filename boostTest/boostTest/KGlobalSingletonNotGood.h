#pragma once

template<typename T>
class KGlobalSingleton
{
public:
    typedef T           Type;
    inline static Type& Singleton() { return KSingletonCreator::Access(); }
    inline              KGlobalSingleton()
                        {
                        }

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

        void Dummy()
        {
        }

        __declspec(noinline) static Type& Access( )
        {
            static Type ref;
            ms_kSingletonCreator.Dummy();
            return ref;
        }//Access()

    private:
        static KSingletonCreator    ms_kSingletonCreator;
    };//class KSingletonCreator
};

template <typename T>
/*static*/ typename KGlobalSingleton<T>::KSingletonCreator
    KGlobalSingleton<T>::KSingletonCreator::ms_kSingletonCreator;
