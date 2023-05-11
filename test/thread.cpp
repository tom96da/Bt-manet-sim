/*!
 * @file thread.cpp
 * @author tom96da
 * @brief いろんな並列処理の呼び出し方と参照によるフラグの監視
 * @date 2023-04-26
 */

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono_literals;

void task(int &i, int &flag, string str);

int main()
{
    int i = 0;
    int flag = 0;

    int n = 4;

    // 一時オブジェクトを用いた初期化(ラムダ式) ☆★★
    auto t1 = thread([&i, &flag]()
                     { task(i, flag, "t1"); });

    for (i = 0; i < n; i++)
        this_thread::sleep_for(0.3s);
    flag = 1;
    t1.join();

    flag = 0;

    // 一時オブジェクトを用いた初期化(ref()) ★★★
    auto t2 = thread(task, ref(i), ref(flag), "t2");

    for (i = 0; i < n; i++)
        this_thread::sleep_for(0.3s);
    flag = 1;
    t2.join();

    flag = 0;

    // 直接初期化(ref()) ☆★★
    thread t3(task, ref(i), ref(flag), "t3");

    for (i = 0; i < n; i++)
        this_thread::sleep_for(0.3s);
    flag = 1;
    t3.join();

    flag = 0;

    // 直接初期化(ラムダ式) ☆☆★
    thread t4([&i, &flag]()
              { task(i, flag, "t4"); });
    for (i = 0; i < n; i++)

        this_thread::sleep_for(0.3s);
    flag = 1;
    t4.join();

    return 0;
}

void task(int &i, int &flag, string str)
{
    while (!flag)
    {
        cout << "\r" << str << ":" << i << " " << flush;
        this_thread::sleep_for(0.1s);
    }
    cout << endl;
}
