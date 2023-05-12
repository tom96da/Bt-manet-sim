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
    double x = 0.0;
    double y = 0.0;

    random_device rnd; // 非決定的な乱数生成器
    mt19937 mt(rnd()); // メルセンヌ・ツイスタ

    // 正規分布
    // 平均0.0、標準偏差1.8で分布させる
    normal_distribution<> randn(0.0, 1.8);

    auto f1 = ofstream("out/randn.csv");
    auto f2 = ofstream("out/randnrun.csv");
    for (size_t i = 0; i < 10000; ++i)
    {
        // 各分布法に基いて乱数を生成
        x += [&f1, &randn, &mt]() -> double
        {
            double x_ = randn(mt);
            f1 << x_ << ",";
            return x_;
        }();
        y += [&f1, &randn, &mt]() -> double
        {
            double y_ = randn(mt);
            f1 << y_ << "\n";
            return y_;
        }();
        f2 << x << "," << y << "\n";
    }

    return 0;
}