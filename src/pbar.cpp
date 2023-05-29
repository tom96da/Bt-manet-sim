/*!
 * @file pbar.cpp
 * @author tom96da
 * @brief 並列プログレスバーのソースファイル
 * @date 2023-05-25
 */

#include "pbar.hpp"

/*!
 * @brief コンストラクタ
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 * @param length バーの長さ
 */
ProgressBar::ProgressBar(const int num_task, int &num_done, int length)
    : length_{length},
      step_{100.0 / length_},
      digit_{[&]() -> int
             {
                 string tmp = to_string(num_task);
                 return size(tmp);
             }()}
{
    set(num_task, num_done);
}

/*!
 * @brief プログレスバーのセット
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 */
void ProgressBar::set(const int num_task, int &num_done)
{
    cout << "\e[?25l";
    auto start = chrono::system_clock::now();

    do
    {
        if (num_done > num_task)
            num_done = num_task;

        percent_ = static_cast<int>(num_done * 100.0 / num_task);
        if (percent_ > 100)
            percent_ = 100;
        while (percent_ > step_ * (size(progress_) + 1) - 1)
            progress_ += "#";

        cout << "\r[" << setfill('_') << setw(length_) << left << progress_ << "]"
             << " [" << setfill(' ') << setw(digit_) << right
             << num_done << "/" << num_task << "]"
             << setw(5) << right << percent_ << "%  " << flush;
    } while (percent_ != 100);

    auto end = chrono::system_clock::now();
    auto dur = end - start;
    auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
    cout << msec << "ms" << flush;

    cout << "\e[?25h" << endl;
}

/* プログレスバーをクリアする */
void ProgressBar::erase() const { cout << "\e[1A\e[2K" << flush; }
