#include "ft_xor.hpp"
#include <iostream>
#include <fstream>

namespace ft{

ft_xor::ft_xor(const std::vector<unsigned char>& vec, std::size_t l_arity, std::string_view out_prefix) :
    l_arity_(l_arity),
    tau_(0),
    sblock_number_{0},
    sblock_index_{0x80, 0x00},
    v_(vec),
    out_prefix_(out_prefix),
    layer_index_(1)
{
    sblock_index_[block_size-1] = 0x01;
}

ft_xor::ft_xor(std::vector<unsigned char>&& vec, std::size_t l_arity, std::string_view out_prefix) :
    l_arity_(l_arity),
    tau_(0),
    sblock_number_{0},
    sblock_index_{0x80, 0x00},
    v_(vec),
    out_prefix_(out_prefix),
    layer_index_(1)
{
    sblock_index_[block_size-1] = 0x01;
}

std::vector<unsigned char> ft_xor::xor_512(std::span<unsigned char> data)
{
    std::vector<unsigned char> res;
    res.resize(block_size, 0);
    std::size_t index = 0;
    for(auto x: data){
        res[index] ^= x;
        index = (index+1) % block_size;
    }

    return res;
}

std::vector<unsigned char> ft_xor::ft()
{
    std::cout << "-------------------------\nFIRST STAGE \n";
    first_stage();
    std::cout << "-------------------------\nSECOND STAGE \n";
    second_stage();

    //3rd stage
    std::cout << "-------------------------\nTHIRD STAGE \n";
    for(std::size_t i = 1; i < tau_+1; ++i){
        std::cout << "processing layer " << layer_index_ << '\n';
        std::cout << "in vector size in bytes: " << v_.size() << '\n';
        this->v_ = g_map(this->sblock_number_, this->v_.begin(), this->v_.end(), true);
        this->sblock_number_ /= this->l_arity_;
    }

    std::cout << "processing layer " << layer_index_ << '\n';
    std::cout << "in vector size in bytes: " << v_.size() << '\n';

    this->v_ = xor_512(std::span<unsigned char>(v_.data(), v_.size()));

    std::string file_name(format(this->out_prefix_, "_layer_", this->layer_index_++));
    std::fstream out_file(file_name, std::ios::binary | std::ios::out);
    if(!out_file.is_open()){
        throw std::runtime_error(format("Cannot open file ", file_name, " \n"));
    }
    for(auto x:this->v_)
        out_file << x;

    std::cout << "layer output is written to " << file_name << '\n';

//        for(auto x: this->v_){
//            ft::hexcout{} << x;
//            std::cout << ' ';
//        }

    return this->v_;
}


void ft_xor::first_stage()
{
    //1.1. 1 bit addition
    this->v_.insert(this->v_.begin(), (unsigned char) 0x01);
    std::cout << "After 1 bit added vector size in bytes: " << this->v_.size() << ", in bits: " << this->v_.size() * 8 << '\n';

    //1.2 zero bit padding
    if(this->v_.size()%block_size > 0)
        this->v_.insert(this->v_.begin(), block_size - (this->v_.size()%block_size), (unsigned char) 0x00);
    std::cout << "After zero padding vector size in bytes: " << this->v_.size() << ", in bits: " << this->v_.size() * 8 << '\n';
    std::cout << "Number of 512-bit blocks: " << this->v_.size() / block_size << '\n';

    //1.3 super block enumeration
    add_sblock_indexes(this->v_);
}

void ft_xor::second_stage()
{
    //2.1 find this->tau_
    std::size_t super_block_number = 1 + ((this->v_.size()/block_size)-1)/(this->l_arity_+1);
    this->tau_ = 1;
    std::size_t l_power = this->l_arity_;
    while(super_block_number >= l_power){
        l_power *= this->l_arity_;
        ++this->tau_;
    }
    --this->tau_;
    l_power /= this->l_arity_;

    std::cout << "this->tau_: " << this->tau_ << '\n';
    std::cout << "layers to output: " << this->tau_+1 << '\n';
    if(super_block_number != l_power){
        std::cout << "Formating our message\n";
        std::size_t delta = 1+((super_block_number-l_power)-1)/(this->l_arity_-1);
        std::cout << "delta: " << delta << '\n';
        std::size_t left_half_block_number = super_block_number - l_power + delta;
        std::size_t right_half_block_number = l_power - delta;

        std::cout << "g_map left part of our vector\n";
        std::vector<unsigned char> leftvec = g_map(left_half_block_number, v_.begin(), v_.end()-(right_half_block_number*block_size*(l_arity_+1)));
        auto rightvec_pos = this->v_.end()-(right_half_block_number)*(l_arity_+1)*(block_size);
        leftvec.insert(leftvec.end(), rightvec_pos, this->v_.end());
        this->v_ = std::move(leftvec);
        std::cout << "g_mapped left part and original right part merged\n";
        this->sblock_number_ = right_half_block_number + 1 + (left_half_block_number-1)/l_arity_;
    } else {
        this->sblock_number_ = super_block_number;
        std::cout << "Formating is not required, super block number = l^tau\n";
    }
}

std::vector<unsigned char> ft_xor::g_map(std::size_t sblock_number, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end, bool to_write)
{
    //    std::cout << "g_map:\n";
    std::size_t in_sblock_size = block_size * (this->l_arity_+1);
    std::size_t out_sblock_size = in_sblock_size * this->l_arity_;
    std::size_t full_sblock_number = sblock_number / this->l_arity_;
    std::size_t incomplete_sblock_number = (sblock_number % this->l_arity_) ? 1 : 0;
    std::size_t out_sblock_number = full_sblock_number + incomplete_sblock_number;

    std::vector<unsigned char> out;
    //    std::cout << "out size: " << out.size() << '\n';
    std::vector<unsigned char> tmp;

    for(std::size_t i = 1; i < out_sblock_number; ++i){
        //        std::cout << "out sblock " << i << '\n';
        for(std::size_t j = 1; j <= l_arity_; ++j){
            //            std::cout << "inner block " << j << '\n';
            std::vector<unsigned char>::iterator pos = end - (out_sblock_size*i) + (in_sblock_size * (l_arity_-j));
            //            for(auto p = pos; p != pos+in_sblock_size; ++p){
            //                ft::hexcout{} << *p;
            //                std::cout << ' ';
            //            }
            //            std::cout << '\n';

            tmp = xor_512(std::span<unsigned char>(&(*pos), in_sblock_size));
            out.insert(out.begin(), tmp.begin(), tmp.end());
        }
    }

    //last block should be hadled carefully because it can be not full
    //    std::cout << "outer block " << out_sblock_number << '\n';
    auto end_pos = end - (out_sblock_number-1) * out_sblock_size;
    auto size = end_pos - begin;
    auto sbn = size / in_sblock_size;
    for(std::size_t i = 1; i <= sbn; ++i){
        //        std::cout << "inner block " << i << '\n';
        auto pos = end_pos - i*in_sblock_size;
        //        for(auto p = pos; p != pos+in_sblock_size; ++p){
        //            hexcout{} << *p;
        //            std::cout << ' ';
        //        }
        //        std::cout << '\n';
        tmp = xor_512(std::span<unsigned char>(&(*pos), in_sblock_size));
        out.insert(out.begin(), tmp.begin(), tmp.end());
    }

    if(size % in_sblock_size){
        //        std::cout << "inner block" << sbn + 1 << '\n';
        //        for(auto p = begin; p < end_pos - sbn*in_sblock_size; ++p){
        //            hexcout{} << *p;
        //            std::cout << ' ';
        //        }
        //        std::cout << '\n';
        tmp = xor_512(std::span<unsigned char>(&(*begin), end-sbn*in_sblock_size-begin));
        out.insert(out.begin(), tmp.begin(), tmp.end());
    }

    std::cout << "g_map out vector size in bytes: " << out.size() << "\n";
    //    for(auto x: out){
    //        hexcout{} << x;
    //        std::cout << ' ';
    //    }
    //    std::cout << '\n';

    if(to_write){
        //Print layer state
        std::string file_name(format(this->out_prefix_, "_layer_", this->layer_index_++));
        std::fstream out_file(file_name, std::ios::binary | std::ios::out);
        if(!out_file.is_open()){
            throw std::runtime_error(format("Cannot open file ", file_name, " \n"));
        }
        for(auto x:out)
            out_file << x;
        std::cout << "layer output is written to " << file_name << '\n';
    }

    add_sblock_indexes(out);

    return out;
}

void ft_xor::add_sblock_indexes(std::vector<unsigned char>& v)
{
    //    std::cout << "add_sblock_indexes: \n";
    std::size_t block_number = v.size() / block_size;
    std::size_t full_super_block_number = block_number / this->l_arity_;
    std::size_t incomplete_super_block_number = (block_number % this->l_arity_ > 0) ? 1 : 0;
    std::size_t incomplete_super_block_size = block_number % this->l_arity_;
    std::size_t super_block_number = full_super_block_number + incomplete_super_block_number;

    for(std::size_t i = 1; i <= full_super_block_number; ++i){
        std::vector<unsigned char>::const_iterator pos = v.begin() + incomplete_super_block_size*block_size
                                                         + (full_super_block_number-i)*this->l_arity_*block_size;
        v.insert(pos, this->sblock_index_.begin(), this->sblock_index_.end());
        ++this->sblock_index_;
    }

    //    std::size_t t = 0;
    if(incomplete_super_block_number){
        v.insert(v.begin(), this->sblock_index_.begin(), this->sblock_index_.end());
        ++this->sblock_index_;
        //        t = (incomplete_super_block_size + 1) * block_size;
        //        std::cout  << "Super block number " << super_block_number << " contains:\n";
        //        for(auto i = v.begin(); i < v.begin()+ t; ++i) {
        //            ft::hexcout{} << *i;
        //            std::cout << ' ';
        //        }
        //        std::cout <<  "\n";
    }

    //    for(std::size_t i = 0; i < full_super_block_number; ++i){
    //        std::vector<unsigned char>::const_iterator pos = v.begin() + t
    //                                                         + i*(this->l_arity_+1)*block_size;
    //        std::cout << "Super block number " << full_super_block_number - i << " contains:\n";
    //        for(auto j = pos; j < pos + (this->l_arity_+1)*block_size; ++j){
    //            ft::hexcout{} << *j;
    //            std::cout << ' ';
    //        }
    //        std::cout << "\n";
    //    }

    std::cout << "After super block index insertion size of vector in bytes: " << v.size() << '\n';

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
