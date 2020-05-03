#ifndef FT_STRIBOG_HPP
#define FT_STRIBOG_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>

namespace ft {
    constexpr int block_size = 64; //char8*64 = 512 bits

    typedef std::array<unsigned char, block_size> uint_512;
    uint_512& operator ++(uint_512& n);

    void ft_stribog(std::vector<unsigned char>& vec, std::size_t larity = 3);
    void first_stage(std::vector<unsigned char>& vec, uint_512& sblock_index, std::size_t larity = 3);
    void second_stage(std::vector<unsigned char>& vec, uint_512& sblock_index, std::size_t larity = 3);
    void g_map(std::vector<unsigned char>& vec, uint_512& sblock_start_index);

    class fts {
    public:
        fts(const std::vector<unsigned char>& vec, std::size_t l_arity);
        fts(std::vector<unsigned char>&& vec, std::size_t l_arity);
        ~fts() = default;

        std::vector<unsigned char> ft();

    private:
        void first_stage();
        void second_stage();
        void g_map(std::size_t sblock_number, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end);
        void add_sblock_indexes(std::vector<unsigned char>& v);

        std::size_t l_arity_;
        uint_512 sblock_index_;
        std::vector<unsigned char> v_;
    };

    class hexcout {
    public:
        hexcout& operator<<(const unsigned char& s){
            auto flags = std::cout.flags();
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (0xFF & (unsigned int) s);
            std::cout.flags(flags);
            return *this;
        }
    };

}


#endif // FT_STRIBOG_HPP
