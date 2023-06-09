/*!
 * @file iota.cpp
 * @author tom96da
 * @brief 配列の生成とシャッフル
 * @date 2023-06-09
 */

#include <iostream>
#include <vector>
#include <numeric>   // std::iota
#include <random>    // std::random_device, std::mt19937
#include <algorithm> // std::shuffle

int main()
{
    std::vector<int> a0(10);
    std::iota(a0.begin(), a0.end(), 0);
    auto a1 = a0;
    std::mt19937 mt{std::random_device{}()};
    std::shuffle(a0.begin(), a0.end(), mt);

    for (const auto &e0 : a0)
    {
        std::shuffle(a1.begin(), a1.end(), mt);
        for (const auto &e1 : a1)
            std::cout << e0 << "-" << e1 << " ";
    }
    std::cout << std::endl;

    return 0;
}
