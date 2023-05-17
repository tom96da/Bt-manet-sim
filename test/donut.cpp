/*!
 * @file donut.cpp
 * @author tom96da
 * @brief ドーナツ型確率分布
 * @date 2023-05-12
 */

#include <iostream>
#include <fstream>
#include <random>

using namespace std;

int main()
{
    double x = 0.0;
    double y = 0.0;

    /* メルセンヌ・ツイスタ */
    mt19937 mt{random_device{}()};

    /* 一様実数分布 */
    uniform_real_distribution<> random(0, 2 * M_PI);
    /* 正規分布 */
    normal_distribution<> randn(2.0, 0.7);

    auto f1 = ofstream("tmp/donut.csv");
    auto f2 = ofstream("tmp/donutrun.csv");
    for (size_t i = 0; i < 10000; ++i)
    {
        // 各分布法に基いて乱数を生成
        double radius = randn(mt); // 動経
        double theta = random(mt); // 角度

        x += [&f1, radius, theta]() -> double
        {
            double x_ = radius * cos(theta);
            f1 << x_ << ",";
            return x_;
        }();
        y += [&f1, radius, theta]() -> double
        {
            double y_ = radius * sin(theta);
            f1 << y_ << "\n";
            return y_;
        }();
        f2 << x << "," << y << "\n";
    }

    return 0;
}