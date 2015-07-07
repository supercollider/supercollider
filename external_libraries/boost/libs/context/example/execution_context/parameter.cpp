
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include <boost/context/all.hpp>
#include <boost/lexical_cast.hpp>

class X{
private:
    int * inp_;
    std::string outp_;
    std::exception_ptr excptr_;
    boost::context::execution_context caller_;
    boost::context::execution_context callee_;

public:
    X():
        inp_( nullptr),
        outp_(),
        excptr_(),
        caller_(boost::context::execution_context::current()),
        callee_(boost::context::fixedsize_stack(),
             [=](){
                try {
                    int i = * inp_;
                    outp_ = boost::lexical_cast<std::string>(i);
                    caller_.resume();
                } catch (...) {
                    excptr_=std::current_exception();
                }
             })
    {}

    std::string operator()(int i){
        inp_ = & i;
        callee_.resume();
        if(excptr_){
            std::rethrow_exception(excptr_);
        }
        return outp_;
    }
};

int main() {
    X x;
    std::cout<<x(7)<<std::endl;
    std::cout << "done" << std::endl;
}
