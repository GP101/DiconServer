#include <cereal/archives/binary.hpp>
#include <sstream>

struct MyData
{
    int m_x;
    int m_y;
    int m_z;

    void Set(int x, int y, int z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    void Print() {
        printf("%i,%i,%i\r\n", m_x, m_y, m_z );
    }

    const int& operator[](int i) const
    {
        if (i == 0)
            return m_x;
        else if (i == 1)
            return m_y;
        else
            return m_z;
    }
};

int main()
{
    MyData m;
    m.Set(1, 2, 3);
    printf("%i,%i,%i\r\n", m[0], m[1], m[2]);
    //m[0] = 4;
    //m[1] = 5;
    //m[2] = 6;
    m.Print();
}
