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
             }()},
      num_task_{num_task},
      num_done_{num_done}
{
}

/*!
 * @brief プログレスバーのセット
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 */
void ProgressBar::start()
{
    progress_.clear();

    std::cout << "\e[?25l";
    auto start = chrono::system_clock::now();

    do
    {
        if (num_done_ > num_task_)
            num_done_ = num_task_;

        percent_ = static_cast<int>(num_done_ * 100.0 / num_task_);
        if (percent_ > 100)
            percent_ = 100;
        while (percent_ > step_ * (size(progress_) + 1) - 1)
            progress_ += "#";

        std::cout << "\r";
        if (!title_.empty())
            std::cout << setfill(' ') << setw(28) << left << title_ + ": ";

        std::cout << "[" << setfill('_') << setw(length_) << left << progress_ << "]"
                  << " [" << setfill(' ') << setw(digit_) << right
                  << num_done_ << "/" << num_task_ << "]"
                  << setw(5) << right << percent_ << "%  " << std::flush;
    } while (percent_ != 100);

    auto end = chrono::system_clock::now();
    auto dur = end - start;
    auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
    std::cout << msec << "ms";

    std::cout << "\e[?25h" << std::endl;
}

/*!
 * @brief プログレスバーにタイトルを付ける
 * @param title
 */
void ProgressBar::setTitle(const string title) { title_ = title; }

/* プログレスバーをクリアする */
void ProgressBar::erase() const { std::cout << "\e[1A\e[2K" << std::flush; }
