/*!
 * @file threadpbar.cpp
 * @author tom96da
 * @brief 並列処理でプログレスバーをタスクから分離
 * @date 2023-04-26
 */

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono_literals;

void pbar(int task, int &done);

int main()
{
    int task = 1024;
    int done = 0;

    auto t = thread([task, &done]()
                    { pbar(task, done); });

    for (int i = 0; i < task; i++)
    {
        this_thread::sleep_for(1ms);
        done = i + 1;
    }

    t.join();

    return 0;
}

void pbar(const int task, int &done)
{
    const int number = 20;
    double percent = 0;
    string progress = "";

    const int digit = [task]() -> int
    {
        string tmp = to_string(task);
        return size(tmp);
    }();
    const double step = 100.0 / number;

    cout << "\e[?25l";

    do
    {
        this_thread::sleep_for(50ms);

        if (done > task)
            done = task;

        percent = done * 100 / task;
        if (percent > 100)
            percent = 100;
        while (percent > step * (size(progress) + 1) - 1)
            progress += "#";

        cout << "\r[" << setfill('_') << setw(number) << left << progress << "]"
             << " [" << setfill(' ') << setw(digit) << right << done << "/" << task << "]"
             << setw(5) << right << percent << "%" << flush;
    } while (percent != 100);

    cout << "\e[?25h" << endl
         << "finish!" << endl;
}