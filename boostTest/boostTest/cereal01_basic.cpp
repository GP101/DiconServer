#include <cereal/archives/binary.hpp>
#include <sstream>

struct MyData
{
    int m_x;
    int m_y;
    int m_z;

    void Set(int x, int y, int z) {
        m_x = x; m_y = y; m_z = z;
    }

    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(m_x, m_y, m_z); // serialize things by passing them to the archive
    }
};

int main()
{
    std::stringstream ss; // any stream can be used

    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
        MyData m1, m2, m3;
        m1.Set(1, 2, 3);
        m2.Set(4, 5, 6);
        m3.Set(7, 8, 9);
        oarchive(m1, m2, m3); // Write the data to the archive
    } // archive goes out of scope, ensuring all contents are flushed

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        MyData m1, m2, m3;
        iarchive(m1, m2, m3); // Read the data from the archive
    }
}
