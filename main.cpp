#include "ft_stribog.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <random>

namespace {
    int uniform_random(int min, int max){
        static std::mt19937 prng(std::random_device{}());
        return std::uniform_int_distribution<>(min, max)(prng);
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
        throw std::runtime_error("Usage: <program-name> <arity> <file-name>");

    std::fstream chars_file(argv[2], std::ios::binary | std::ios::in | std::ios::out);
    if(!chars_file.is_open()){
        throw std::runtime_error("Cannot open file!");
    }

    /*
    for (int i = 0; i < (block_size*larity*15)+20; ++i){
        chars_file << ((unsigned char) uniform_random(0, 255));
    }
    */

    std::vector<unsigned char> v{};
    std::stringstream ss;
    ss << chars_file.rdbuf();
    const std::string& s = ss.str();
    v.insert(v.end(), s.begin(), s.end());

    std::cout << "File size in bytes: " << v.size() << ", in bits: " << v.size() * 8 << '\n';
    std::cout << "Vector contains next bytes: \n";
    for(auto x: v){
        ft::hexcout{} << x;
        std::cout << ' ';
    }
    std::cout << "\n------------------------------------------\n";

    ft::fts ft_stribog(std::move(v), 3);
    v = ft_stribog.ft();

    std::cout << "\nFINALLY I'M DONE PLEASE GIVE ME A REST\n";
    for(auto x: v){
        ft::hexcout{} << x;
        std::cout << ' ';
    }

    return 0;
}

