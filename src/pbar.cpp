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
ProgressBar::ProgressBar() { std::cout << "\e[?25l" << std::flush; }

/*!
 * @brief デストラクタ
 */
ProgressBar::~ProgressBar() { std::cout << "\e[?25h" << std::flush; }

/*!
 * @brief 新規プログレスバー領域を確保する
 * @return 新規プログレスバーの参照
 */
ProgressBar::BarBody &ProgressBar::add() {
    int index = pbars_.size();
    pbars_.emplace_back(make_unique<BarBody>(index + 1));
    std::cout << std::endl;

    return *(pbars_.back());
}

/* バー本体クラス */

/*!
 * @brief コンストラクタ
 * @param layer 深さ
 */
ProgressBar::BarBody::BarBody(const int layer)
    : layer_{layer},
      length_{PBAR_LENGTH},
      step_{100.0 / length_},
      previous_num_task_{0} {}

/*!
 * @brief プログレスバーのセット
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 */
void ProgressBar::BarBody::start(const int num_task, int &num_done) {
    previous_num_task_ = num_task;
    thread_ = thread([&, num_task]() {
        string progress;
        const int digit = to_string(num_task).size();
        int percent = 0, previous_done = 0;

        auto start = chrono::system_clock::now();

        do {
            if (num_done > num_task) {
                num_done = num_task;
            }

            if (num_done != previous_done) {  // 進捗があれば表示を更新する

                percent = static_cast<int>(num_done * 100.0 / num_task);
                if (percent > 100) {
                    percent = 100;
                }

                while (percent > step_ * (size(progress) + 1) - 1) {
                    progress += "#";
                }

                mutex_.lock();

                std::cout << "\r";
                for (int i = 0; i < layer_; i++) {
                    std::cout << "\e[1A";
                }

                if (!title_.empty()) {
                    std::cout << setfill(' ') << setw(25) << left
                              << title_ + ": ";
                }

                std::cout << "[" << setfill('_') << setw(length_) << left
                          << progress << "]"
                          << " [" << setfill(' ') << setw(digit) << right
                          << num_done << "/" << num_task << "]" << setw(5)
                          << right << percent << "%  ";

                for (int i = 0; i < layer_; i++) {
                    std::cout << "\e[1B";
                }

                std::cout << std::flush;

                mutex_.unlock();

                previous_done = num_done;
            }

        } while (percent != 100);

        auto end = chrono::system_clock::now();
        auto dur = end - start;
        auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();

        mutex_.lock();

        for (int i = 0; i < layer_; i++) {
            std::cout << "\e[1A";
        }

        if (msec < 1000) {
            std::cout << setprecision(3) << msec << "ms";
        } else if (msec < 10000) {
            std::cout << std::fixed << setprecision(2)
                      << static_cast<double>(msec) / 1000 << "s";
        } else {
            std::cout << std::fixed << setprecision(1)
                      << static_cast<double>(msec) / 1000 << "s";
        }

        for (int i = 0; i < layer_; i++) {
            std::cout << "\e[1B\r";
        }

        std::cout << std::flush;
        mutex_.unlock();

        time_ = msec;
    });
}

/*!
 * @brief プログレスバーを閉じる
 */
void ProgressBar::BarBody::close() { thread_.join(); }

/*!
 * @brief プログレスバーにタイトルを付ける
 * @param title
 */
void ProgressBar::BarBody::set_title(const string title) { title_ = title; }

/*!
 * @brief 進捗をクリアする
 */
void ProgressBar::BarBody::clear() const {
    string progress;
    const int digit = to_string(previous_num_task_).size();
    int num_done = 0, percent = 0;

    mutex_.lock();

    std::cout << "\r";
    for (int i = 0; i < layer_; i++) {
        std::cout << "\e[1A";
    }

    if (!title_.empty()) {
        std::cout << setfill(' ') << setw(25) << left << title_ + ": ";
    }

    std::cout << "[" << setfill('_') << setw(length_) << left << progress << "]"
              << " [" << setfill(' ') << setw(digit) << right << num_done << "/"
              << previous_num_task_ << "]" << setw(5) << right << percent
              << "%  ";

    for (int i = 0; i < layer_; i++) {
        std::cout << "\e[1B";
    }

    std::cout << std::flush;

    mutex_.unlock();
}

/* プログレスバーを削除する */
void ProgressBar::BarBody::erase() const {
    std::cout << "\e[1A\e[2K\r" << std::flush;
}

/*!
 * @return かかった時間
 */
int64_t ProgressBar::BarBody::time() { return time_; }
