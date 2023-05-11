/*!
 * @file sleep.cpp
 * @author tom96da
 * @brief sleep の習得
 * @date 2023-04-24
 */

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono_literals;

int main()
{
    cout << "3秒間スリープします" << endl;

    this_thread::sleep_for(3s);

    cout << "3秒間スリープしました" << endl;

    return 0;
}