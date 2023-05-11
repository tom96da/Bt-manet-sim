/*!
 * @file lambdaex.cpp
 * @author tom96da
 * @brief ラムダ式の使い方
 * @date 2023-04-27
 */

#include <iostream>

using namespace std;

int main(void)
{
    int n = 0,
        m = 0;

    auto f = [&n](int l) -> void
    {
        n += l;
        cout << "n = " << n << endl;
    };

    f(2);

    cout << "m = " << [&m](int k)
    {
        m += k;
        return m;
    }(2) << endl;

    return 0;
}