//
// Created by banani on 25/12/25.
//

#include "Translator.h"

int main() {
    std::cout
    << "Welcome to cool well made console app, enter expression:\n";
    std::string s;
    std::getline(std::cin, s);




    std::cout <<
        "Unreal result: "
    <<
        solve
    (
        parse_into_tokens(
            s
            )
        )
    <<
            '\n'
    ;
}