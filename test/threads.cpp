/*!
 * @file threads.cpp
 * @author tom96da
 * @brief 互いに干渉しない複数の処理を並列化
 * @date 2023-04-26
 */

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void myfunc(int i, int *v)
{
    v[i]++;
}

int main()
{
    int v[] = {0, 1, 2, 3, 4, 5, 6, 7};

    vector<thread> threads;

    // コンストラクタを用いた初期化
    for (int i = 0; i < 2; ++i)
        threads.emplace_back([i, &v]()
                             { myfunc(i, v); });

    for (int i = 2; i < 4; i++)
        threads.emplace_back(myfunc, i, ref(v));

    // コピーコンストラクタを用いた初期化
    for (int i = 4; i < 6; i++)
        threads.push_back(thread(myfunc, i, ref(v)));

    for (int i = 6; i < 8; i++)
        threads.push_back(thread([i, &v]()
                                 { myfunc(i, v); }));

    // 全スレッドの同期待ち
    for (auto &t : threads)
        t.join();

    for (auto &vi : v)
        cout << vi << endl;

    return 0;
}