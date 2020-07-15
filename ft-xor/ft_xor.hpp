#ifndef FT_XOR_HPP
#define FT_XOR_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <span>

template<typename... Ts>
std::string format(Ts&&... args)
{
    std::ostringstream out;
    (out << ... << std::forward<Ts>(args));
    return out.str();
}

namespace ft {

constexpr int block_size = 64;
typedef std::array<unsigned char, block_size> uint_512;
uint_512& operator ++(uint_512& n);

class ft_xor
{
public:
    ft_xor(const std::vector<unsigned char>& vec, std::size_t l_arity, std::string_view out_prefix);
    ft_xor(std::vector<unsigned char>&& vec, std::size_t l_arity, std::string_view out_prefix);

    ~ft_xor() = default;
    ft_xor& operator=(const ft_xor& other) = delete;
    ft_xor& operator=(ft_xor&& other) = delete;

    //main function to call
    std::vector<unsigned char> ft(); //3rd stage

private:
    void first_stage();
    void second_stage();

    std::vector<unsigned char> g_map(std::size_t sblock_number, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end, bool to_write = 0);
    void add_sblock_indexes(std::vector<unsigned char>& v);

    std::vector<unsigned char> xor_512(std::span<unsigned char> data);

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

#endif // FT_XOR_HPP
