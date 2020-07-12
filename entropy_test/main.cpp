#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <bit>
#include <cmath>
#include <filesystem>

template <typename... Ts>
std::string format(Ts&&... args)
{
    std::ostringstream ss;
    (ss << ... << std::forward<Ts>(args));
    return ss.str();
}

int main(int argc, char **argv)
{
    if (argc != 2)
        throw std::runtime_error(format("Usage: ", argv[0], "<input-file>"));

    std::fstream in(argv[1], std::ios::binary | std::ios::in);
    if(!in.is_open())
        throw std::runtime_error(format("File with name ", argv[1], " can't be opened!"));


    std::uint64_t zeros = 0, ones = 0, pos = 0,
                  size = std::filesystem::file_size(argv[1]);

    while(size-pos > 0){
        unsigned char n;
        in >> n;
        ones += std::popcount(n);
        pos += 1;
    }

    zeros = (size*8)-ones;

    std::cout << "File " << argv[1] << " size: " << size << " bytes"
              << "\nNumber of one bits: " << ones
              << "\nNumber of zero bits: " << zeros << std::endl;

    double pones = static_cast<double>(ones) / (size*8);
    double pzeros = static_cast<double>(zeros) / (size*8);
    std::cout << "P(one) = " << pones
              << "\nP(zero) = " << pzeros
              << "\nP(one)+P(zero) = " << pones+pzeros << std::endl;
    double entropy = -(pones*log2(pones) + pzeros*log2(pzeros));
    std::cout << "Entropy (base 2) = " << entropy << std::endl;

    return 0;
}
