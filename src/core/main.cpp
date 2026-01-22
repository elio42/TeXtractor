#include<iostream>

int main(int argc, char *argv[]){
    std::cout << "hell world\n" << argc;

    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << "\n";
    }
    

    std::cout << "\n";
    return 0;
}