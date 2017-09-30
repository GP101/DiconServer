#include <stdio.h>
#include <iostream>
#include <set>
#include <string>

struct KItem
{
    int             m_id;
    std::string     m_name;

    void SetIdName( int id_, const std::string& name_ )
    {
        m_id = id_;
        m_name = name_;
    }

    void Print() const
    {
        std::cout << m_id << " " << m_name.c_str() << std::endl;
    }

    bool operator<( const KItem& rhs_ ) const
    {
        return m_id < rhs_.m_id;
    }
};

void main()
{
    std::set<KItem>     itemSet;
    KItem a;
    a.SetIdName( 1, "First" );
    KItem b;
    b.SetIdName( 2, "Second" );
    itemSet.insert( b );
    itemSet.insert( a );
    for( const KItem& item : itemSet ) {
        item.Print();
    }
}
