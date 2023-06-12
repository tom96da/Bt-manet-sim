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
#include <chrono>

int main()
{
    std::vector<int> a0(1000);
    std::iota(a0.begin(), a0.end(), 0);

    std::mt19937 mt{std::random_device{}()};
    std::shuffle(a0.begin(), a0.end(), mt);

    auto a1 = a0;

    auto start = std::chrono::system_clock::now();

    for (const auto &e0 : a0)
    {
        for (const auto &e1 : a1)
            // std::cout << e0 << "-" << e1 << " ";
            ;
        auto it = std::find(a1.begin(), a1.end(), e0);
        a1.erase(it);
        std::shuffle(a1.begin(), a1.end(), mt);
    }

    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << msec << "ms" << std::endl;

    return 0;
}
