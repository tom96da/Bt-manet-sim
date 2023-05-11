/*!
 * @file pbar.cpp
 * @author tom96da
 * @brief キャリッジリターンを使ったプログレスバー
 * @date 2023-04-24
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;

int main(void)
{
    int task = 256;
    int number = 20;
    double step = static_cast<double>(task) / number;
    int percent = 0;
    string progress = "";

    cout << "\e[?25l";

    using namespace std::chrono_literals;
    for (int i = 0; i < task; i++)
    {
        percent = i * 100 / task;
        if (i > step * (size(progress) + 1) || i == task - 1)
            progress += "#";
        if (percent > 100)
            percent = 100;

        cout << "\r[" << setfill('_') << setw(number) << left << progress << "]"
             << " [" << setw(3) << i + 1 << "/" << task << "]"
             << setfill(' ') << setw(5) << right << percent << "%" << flush;

        std::this_thread::sleep_for(30ms);
    }

    cout << endl
         << "finish!" << endl;
    cout << "\e[?25h";

    return 0;
}