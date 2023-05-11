/*!
 * @file version.cpp
 * @author tom96da
 * @brief c++標準コンパイラのバージョンを表示する
 * @date 2023-04-24
 */

#include <iostream>

int main()
{
    if (__cplusplus == 202002L)
        std::cout << "C++20" << std::endl;
    else if (__cplusplus == 201703L)
        std::cout << "C++17" << std::endl;
    else if (__cplusplus == 201402L)
        std::cout << "C++14" << std::endl;
    else if (__cplusplus == 201103L)
        std::cout << "C++11" << std::endl;
    else if (__cplusplus == 199711L)
        std::cout << "C++98" << std::endl;
    else
        std::cout << "pre-standard C++" << std::endl;
}