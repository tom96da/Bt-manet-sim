/*!
 * @file pbar.cpp
 * @author tom96da
 * @brief 並列プログレスバーのソースファイル
 * @date 2023-05-25
 */

#include "pbar.hpp"

/* プログレスバークラス */

/*!
 * @brief コンストラクタ
 */
ProgressBar::ProgressBar()
{
}

ProgressBar::BarBody &ProgressBar::add(const int layer)
{
    pbars_.emplace_back(layer);
    return pbars_.at(layer);
}

/* バー本体クラス */

/*!
 * @brief コンストラクタ
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 * @param length バーの長さ
 */
ProgressBar::BarBody::BarBody(const int layer)
    : layer_{layer},
      length_{PBAR_LENGTH},
      step_{100.0 / length_} //,
//   digit_{[&]() -> int
//          {
//              string tmp = to_string(num_task);
//              return size(tmp);
//          }()},
//   num_task_{num_task},
//   num_done_{num_done}
{
}

/*!
 * @brief プログレスバーのセット
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 */
void ProgressBar::BarBody::start(const int num_task, int &num_done)
{
    thread_ = thread(
        [&, num_task]()
        {
            string progress;
            const int digit = to_string(num_task).size();
            int percent = 0;

            std::cout << "\e[?25l";
            auto start = chrono::system_clock::now();

            do
            {
                if (num_done > num_task)
                    num_done = num_task;

                percent = static_cast<int>(num_done * 100.0 / num_task);
                if (percent > 100)
                    percent = 100;
                while (percent > step_ * (size(progress) + 1) - 1)
                    progress += "#";

                std::cout << "\r";
                if (!title_.empty())
                    std::cout << setfill(' ') << setw(25) << left << title_ + ": ";

                std::cout << "[" << setfill('_') << setw(length_) << left << progress << "]"
                          << " [" << setfill(' ') << setw(digit) << right
                          << num_done << "/" << num_task << "]"
                          << setw(5) << right << percent << "%  " << std::flush;
            } while (percent != 100);

            auto end = chrono::system_clock::now();
            auto dur = end - start;
            auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
            if (msec < 1000)
                std::cout << msec << "ms";
            else if (msec < 10000)
                std::cout << setprecision(3) << static_cast<double>(msec) / 1000 << "s";
            else
            {
                auto sec = chrono::duration_cast<chrono::seconds>(dur).count();
                std::cout << sec << "s";
            }
            std::cout << "\e[?25h" << std::flush;

            time_ = msec;
        });
}

void ProgressBar::BarBody::close()
{
    thread_.join();
}

/*!
 * @brief プログレスバーにタイトルを付ける
 * @param title
 */
void ProgressBar::BarBody::set_title(const string title) { title_ = title; }

/* プログレスバーをクリアする */
void ProgressBar::BarBody::erase() const { std::cout << "\r\e[2K" << std::flush; }

/*!
 * @return かかった時間
 */
int64_t ProgressBar::BarBody::time()
{
    return time_;
}
