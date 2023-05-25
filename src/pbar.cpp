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

    do
    {
        if (num_done > num_task)
            num_done = num_task;

        percent_ = num_done * 100 / num_task;
        if (percent_ > 100)
            percent_ = 100;
        while (percent_ > step_ * (size(progress_) + 1) - 1)
            progress_ += "#";

        cout << "\r[" << setfill('_') << setw(length_) << left << progress_ << "]"
             << " [" << setfill(' ') << setw(digit_) << right
             << num_done << "/" << num_task << "]"
             << setw(5) << right << percent_ << "%" << flush;
    } while (percent_ != 100);

    cout << "\e[?25h" << flush;
}

/* プログレスバーをクリアする */
void ProgressBar::erase() const { cout << "\r\e[2K" << flush; }
