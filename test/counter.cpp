/*!
 * @file counter.cpp
 * @author tom96da
 * @brief キャリッジリターンを使ったカウンタ
 * @date 2023-04-24
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;

int main()
{
    int n = 10;
    cout << n << "秒カウント" << endl;

    using namespace std::chrono_literals;
    for (int i = 0; i <= 10; i++)
    {
        cout << "\r" << setw(2) << 10 - i << flush;
        std::this_thread::sleep_for(1s);
    }
    cout << endl;
    cout << "完了" << endl;

    return 0;
}