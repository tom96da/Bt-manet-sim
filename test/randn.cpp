/*!
 * @file randn.cpp
 * @author tom96da
 * @brief 正規分布による乱数生成
 * @date 2023-05-01
 */

#include <iostream>
#include <fstream>
#include <random>

using namespace std;

int main()
{
    random_device rnd; // 非決定的な乱数生成器
    mt19937 mt(rnd()); // メルセンヌ・ツイスタ

    // 一様実数分布
    // [-1.0, 1.0)の値の範囲で、等確率に実数を生成する
    uniform_real_distribution<> random(-1.0, 1.0);

    // 正規分布
    // 平均1.0、標準偏差0.5で分布させる
    normal_distribution<> randn(1.0, 0.5);

    ofstream file("output/random.csv");
    for (size_t i = 0; i < 10000; ++i)
    {
        // 各分布法に基いて乱数を生成
        double r1 = randn(mt);
        double r2 = randn(mt);

        file << r1 << "," << r2 << "\n";
    }

    return 0;
}