#include "ft_stribog.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <charconv>
#include <optional>

namespace {
template<typename T>
std::optional<T> from_chars(std::string_view sv_) noexcept
{
    T out;
    auto end = sv_.data() + sv_.size();
    auto res = std::from_chars(sv_.data(), end, out);
    if(res.ec==std::errc{}&&res.ptr==end)
        return out;
    return {};
}
}

int main(int argc, char **argv)
{
    if (argc < 3)
        throw std::runtime_error("Usage: <program-name> <arity> <file-name>\n");

    auto l_arity = from_chars<std::size_t>(argv[1]);
    if(!l_arity || *l_arity<2){
        throw std::runtime_error("set arity >= 2\n");
    }

    std::fstream chars_file(argv[2], std::ios::binary | std::ios::in | std::ios::out);
    if(!chars_file.is_open()){
        throw std::runtime_error("Cannot open file!\n");
    }

    std::vector<unsigned char> v{};
    std::stringstream ss;
    ss << chars_file.rdbuf();
    const std::string& s = ss.str();
    v.insert(v.end(), s.begin(), s.end());

    std::cout << "File size in bytes: " << v.size() << ", in bits: " << v.size() * 8 << '\n';
    /*std::cout << "Vector contains next bytes: \n";
    for(auto x: v){
        ft::hexcout{} << x;
        std::cout << ' ';
    }
    std::cout << "\n------------------------------------------\n";
    */
    ft::fts ft_stribog(std::move(v), *l_arity);
    v = ft_stribog.ft();

    std::cout << "\nFINALLY I'M DONE PLEASE GIVE ME A REST\n";
    for(auto x: v){
        ft::hexcout{} << x;
        std::cout << ' ';
    }

    return 0;
}

