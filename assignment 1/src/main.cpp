#include "parser.hpp"
#include "tokenizer.hpp"
#include "linker.hpp"

using namespace std;

int main (int argc, char** argv) {

    Tokenizer tokenizer(argv[1]);
    Parser parser;
    Linker linker(parser);
    linker.pass1(tokenizer);
    tokenizer.reset();
    linker.pass2(tokenizer);
    linker.printWarnings();
    return 0;
}
