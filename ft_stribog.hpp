#ifndef FT_STRIBOG_HPP
#define FT_STRIBOG_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <string_view>

template<typename... Ts>
std::string format(Ts&&... args)
{
    std::ostringstream out;
    (out << ... << std::forward<Ts>(args));
    return out.str();
}

namespace ft {
    constexpr int block_size = 64; //char8*64 = 512 bits

    typedef std::array<unsigned char, block_size> uint_512;
    uint_512& operator ++(uint_512& n);

    class fts {
    public:
        fts(const std::vector<unsigned char>& vec, std::size_t l_arity, std::string_view out_prefix);
        fts(std::vector<unsigned char>&& vec, std::size_t l_arity, std::string_view out_prefix);

        ~fts() = default;
        fts& operator=(const fts& other) = delete;
        fts& operator=(fts&& other) = delete;

        //main function to call
        std::vector<unsigned char> ft();

    private:
        void first_stage();
        void second_stage();
        std::vector<unsigned char> g_map(std::size_t sblock_number, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end, bool to_write = 0);
        void add_sblock_indexes(std::vector<unsigned char>& v);

        std::size_t l_arity_;
        std::size_t tau_;
        std::size_t sblock_number_;
        uint_512 sblock_index_;
        std::vector<unsigned char> v_;

        //for output
        std::string out_prefix_;
        int layer_index_;
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
