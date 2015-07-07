
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>

#include <boost/context/all.hpp>

/*
 * grammar:
 *   P ---> E '\0'
 *   E ---> T {('+'|'-') T}
 *   T ---> S {('*'|'/') S}
 *   S ---> digit | '(' E ')'
 */
class Parser{
   char next;
   std::istream& is;
   std::function<void(char)> cb;

   char pull(){
        return std::char_traits<char>::to_char_type(is.get());
   }

   void scan(){
       do{
           next=pull();
       }
       while(isspace(next));
   }

public:
   Parser(std::istream& is_,std::function<void(char)> cb_) :
      next(), is(is_), cb(cb_)
    {}

   void run() {
      scan();
      E();
   }

private:
   void E(){
      T();
      while (next=='+'||next=='-'){
         cb(next); 
         scan();
         T();
      }
   }

   void T(){
      S();
      while (next=='*'||next=='/'){
         cb(next); 
         scan();
         S();
      }
   }

   void S(){
      if (std::isdigit(next)){
         cb(next);
         scan();
      }
      else if(next=='('){
         cb(next); 
         scan();
         E();
         if (next==')'){
             cb(next); 
             scan();
         }else{
             exit(2);
         }
      }
      else{
         exit(3);
      }
   }
};

int main() {
    std::istringstream is("1+1");
    bool done=false;
    char c;

    // create handle to main execution context
    boost::context::execution_context main_ctx(
        boost::context::execution_context::current() );

    // executes parser in new execution context
    boost::context::execution_context parser_ctx(
        boost::context::fixedsize_stack(),
        [&main_ctx,&is,&c,&done](){
            // create parser with callback function
            Parser p( is,
                      [&main_ctx,&c](char ch){
                          c=ch;
                          // resume main execution context
                          main_ctx.resume();
                      });
            // start recursive parsing
            p.run();
            done=true;
            main_ctx.resume();
        });

    // user-code pulls parsed data from parser
    // invert control flow
    parser_ctx.resume();
    do {
        printf("Parsed: %c\n",c);
        parser_ctx.resume();
    } while( ! done);

    std::cout << "main: done" << std::endl;

    return EXIT_SUCCESS;
}
