#include "ft_stribog.hpp"
#include "stribog.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>

namespace ft {

fts::fts(const std::vector<unsigned char>& vec, std::size_t l_arity) :
    l_arity_(l_arity),
    sblock_index_{0x80, 0x00},
    v_(vec)
{
    sblock_index_[block_size-1] = 0x01;
}

fts::fts(std::vector<unsigned char>&& vec, std::size_t l_arity) :
    l_arity_(l_arity),
    sblock_index_{0x80, 0x00},
    v_(vec)
{
    sblock_index_[block_size-1] = 0x01;
}

std::vector<unsigned char> fts::ft()
{
    first_stage();
    second_stage();

    return std::vector<unsigned char>{};
}

void fts::first_stage()
{
    //1.1. 1 bit addition
    this->v_.insert(this->v_.begin(), (unsigned char) 0x01);
    std::cout << "After 1 bit addition size in bytes: " << this->v_.size() << ", in bits: " << this->v_.size() * 8 << '\n';

    //1.2 zero bit padding
    if(this->v_.size()%block_size > 0)
        this->v_.insert(this->v_.begin(), block_size - (this->v_.size()%block_size), (unsigned char) 0x00);
    std::cout << "After zero padding size in bytes: " << this->v_.size() << ", in bits: " << this->v_.size() * 8 << '\n';
    std::cout << "Number of blocks: " << this->v_.size() / block_size << '\n';

    //1.3 super block enumeration
    add_sblock_indexes(this->v_);
}

void fts::second_stage()
{
    //2.1 find tau
    std::size_t super_block_number = 1 + ((this->v_.size()/block_size)-1)/(this->l_arity_+1);
    std::size_t tau = 1;
    std::size_t l_power = this->l_arity_;
    while(super_block_number >= l_power){
        l_power *= this->l_arity_;
        ++tau;
    }
    --tau;
    l_power /= this->l_arity_;

    std::cout << "tau: " << tau << '\n';
    if(super_block_number == l_power){
        //do nothing
        ;
    } else {
        std::size_t delta = 1+((super_block_number-l_power)-1)/(this->l_arity_-1);
        std::cout << "delta: " << delta << '\n';
        std::size_t left_half_block_number = super_block_number - l_power + delta;
        std::size_t right_half_block_number = l_power - delta;

        g_map(left_half_block_number, v_.begin(), v_.end()-(right_half_block_number*block_size*(l_arity_+1)));
    }

}

std::vector<unsigned char> fts::g_map(std::size_t sblock_number, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end)
{
    std::size_t in_sblock_size = block_size * (this->l_arity_+1);
    std::size_t out_sblock_size = in_sblock_size * this->l_arity_;
    std::size_t full_sblock_number = sblock_number / this->l_arity_;
    std::size_t incomplete_sblock_number = (sblock_number % this->l_arity_) ? 1 : 0;
    std::size_t out_sblock_number = full_sblock_number + incomplete_sblock_number;

    std::vector<unsigned char> out;
    std::cout << "out size: " << out.size() << '\n';
    unsigned char tmp[block_size];

    for(std::size_t i = 1; i < out_sblock_number; ++i){
        std::cout << "out sblock " << i << '\n';
        for(std::size_t j = 1; j <= l_arity_; ++j){
            std::cout << "inner block " << j << '\n';
            std::vector<unsigned char>::const_iterator pos = end - (out_sblock_size*i) + (in_sblock_size * (l_arity_-j));
            for(auto p = pos; p != pos+in_sblock_size; ++p){
                ft::hexcout{} << *p;
                std::cout << ' ';
            }
            std::cout << '\n';
            hash_512(&(*pos), in_sblock_size, tmp);
            out.insert(out.begin(), tmp, tmp+block_size);
        }
    }

    //last block should be hadled carefully because it can be not full
    std::cout << "outer block " << out_sblock_number << '\n';
    auto end_pos = end - full_sblock_number * out_sblock_size;
    auto size = end_pos - begin;
    auto sbn = size / in_sblock_size;
    for(std::size_t i = 1; i <= sbn; ++i){
        std::cout << "inner block " << i << '\n';
        auto pos = end_pos - i*in_sblock_size;
        for(auto p = pos; p != pos+in_sblock_size; ++p){
            hexcout{} << *p;
            std::cout << ' ';
        }
        std::cout << '\n';
        hash_512(&(*pos), in_sblock_size, tmp);
        out.insert(out.begin(), tmp, tmp+block_size);
    }
    std::cout << "inner block" << sbn + 1 << '\n';
    for(auto p = begin; p < end_pos - sbn*in_sblock_size; ++p){
        hexcout{} << *p;
        std::cout << ' ';
    }
    std::cout << '\n';
    hash_512(&(*begin), end-sbn*in_sblock_size-begin, tmp);
    out.insert(out.begin(), tmp, tmp+block_size);

    std::cout << "Out vector: size - " << out.size() << "\n";
    for(auto x: out){
        hexcout{} << x;
        std::cout << ' ';
    }
    std::cout << '\n';

    std::cout << "add indexes:\n\n";
    add_sblock_indexes(out);

    return out;
}

void fts::add_sblock_indexes(std::vector<unsigned char>& v)
{
    std::size_t block_number = v.size() / block_size;
    std::size_t full_super_block_number = block_number / this->l_arity_;
    std::size_t incomplete_super_block_number = (block_number / this->l_arity_ > 0) ? 1 : 0;
    std::size_t incomplete_super_block_size = block_number % this->l_arity_;
    std::size_t super_block_number = full_super_block_number + incomplete_super_block_number;

    for(std::size_t i = 1; i <= full_super_block_number; ++i){
        std::vector<unsigned char>::const_iterator pos = v.begin() + (block_number%this->l_arity_)*block_size
                                                         + (full_super_block_number-i)*this->l_arity_*block_size;
        v.insert(pos, this->sblock_index_.begin(), this->sblock_index_.end());
        ++this->sblock_index_;
    }

    std::size_t t = (incomplete_super_block_size + 1) * block_size;
    if(incomplete_super_block_number){
        v.insert(v.begin(), this->sblock_index_.begin(), this->sblock_index_.end());
        ++this->sblock_index_;
        std::cout  << "Super block number " << super_block_number << '\n';
        for(auto i = v.begin(); i < v.begin()+ t; ++i) {
            ft::hexcout{} << *i;
            std::cout << ' ';
        }
        std::cout <<  "\n\n";
    }

    for(std::size_t i = 0; i < full_super_block_number; ++i){
        std::vector<unsigned char>::const_iterator pos = v.begin() + t
                                                         + i*(this->l_arity_+1)*block_size;
        std::cout << "Super block number " << full_super_block_number - i << '\n';
        for(auto j = pos; j < pos + (this->l_arity_+1)*block_size; ++j){
            ft::hexcout{} << *j;
            std::cout << ' ';
        }
        std::cout << "\n\n";
    }

}

uint_512& operator ++(uint_512& n){
    unsigned char cf = 1;
    for(auto i = n.rbegin(); i < n.rend() && cf; ++i){
        *i += cf;
        cf = *i < cf;
    }
    return n;
}
}
