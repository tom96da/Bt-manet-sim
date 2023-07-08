/*!
 * @file pbar.cpp
 * @author tom96da
 * @brief 並列プログレスバーのソースファイル
 * @date 2023-05-25
 */

#include "pbar.hpp"

#include <iomanip>
#include <iostream>

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
    std::cout << std::endl;

    return *(pbars_.emplace_back(make_unique<BarBody>(index + 1)));
}

/*!
 * @brief すべてのプログレスバーを削除する
 */
void ProgressBar::erase() const {
    for (auto &pbar : pbars_) {
        pbar->erase();
    }
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
      previous_num_task_{0},
      monitars_time_{false} {}

/*!
 * @brief デストラクタ
 */
ProgressBar::BarBody::~BarBody() {
#if __linux__
    if (thread_.joinable()) {
        thread_.join();
    }

#elif _WIN32

#endif
}

/*!
 * @brief プログレスバーのセット
 * @param num_task タスク数
 * @param num_done 終了済みタスク数
 */
void ProgressBar::BarBody::start(const int num_task, int &num_done) {
#if __linux__
    thread_ = thread([&, num_task]() {  // 並列処理開始
        string progress;
        const int digit = to_string(num_task).size();
        int percent = 0, previous_done = 0;

        auto start = chrono::system_clock::now();

        do {
            if (num_done > num_task) {
                num_done = num_task;
            }

            if (num_done == previous_done) {  // 進捗がなければ表示を更新しない
                continue;
            }

            previous_done = num_done;

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
                std::cout << setfill(' ') << setw(25) << left << title_ + ": ";
            }

            std::cout << "[" << setfill('_') << setw(length_) << left
                      << progress << "] "
                      << "[" << setfill(' ') << setw(digit) << right << num_done
                      << "/" << num_task << "]" << setw(5) << right << percent
                      << "%  ";

            /* モニターが有効なら経過時間と推定残り時間を表示する */
            if (monitars_time_ == true) {
                auto now = chrono::system_clock::now();
                auto pass = now - start;
                auto sec = chrono::duration_cast<chrono::seconds>(pass).count();
                if (sec < 3600) {
                    std::cout << sec / 60 << ":" << setfill('0') << setw(2)
                              << right << sec % 60;
                } else {
                    std::cout << sec / 3600 << ":" << setfill('0') << setw(2)
                              << right << sec % 3600 / 60 << ":" << setfill('0')
                              << setw(2) << right << sec % 60;
                }

                auto remain = static_cast<int64_t>(
                    sec * (static_cast<double>(num_task) / num_done - 1));
                std::cout << "\e[0K";
                if (remain > 0) {
                    std::cout << "  remaining: ";
                    if (remain < 3600) {
                        std::cout << remain / 60 << ":" << setfill('0')
                                  << setw(2) << right << remain % 60;
                    } else {
                        std::cout << remain / 3600 << ":" << setfill('0')
                                  << setw(2) << right << remain % 3600 / 60
                                  << ":" << setfill('0') << setw(2) << right
                                  << remain % 60;
                    }
                }
            }

            for (int i = 0; i < layer_; i++) {
                std::cout << "\e[1B";
            }

            std::cout << std::flush;

            mutex_.unlock();

        } while (percent != 100);

        auto end = chrono::system_clock::now();
        auto dur = end - start;
        auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
        auto sec = chrono::duration_cast<chrono::seconds>(dur).count();

        time_millsec_ = msec;
        time_sec_ = sec;

        mutex_.lock();

        /* モニターが無効なら経過時間と推定残り時間を表示する */
        if (!monitars_time_) {
            for (int i = 0; i < layer_; i++) {
                std::cout << "\e[1A";
            }

            if (msec < 1000) {
                std::cout << setprecision(3) << msec << "ms";
            } else if (msec < 10000) {
                std::cout << std::fixed << setprecision(2)
                          << static_cast<double>(msec) / 1000 << "s";
            } else if (msec < 60000) {
                std::cout << std::fixed << setprecision(1)
                          << static_cast<double>(msec) / 1000 << "s";
            } else {
                std::cout << sec / 60 << ":" << setfill('0') << setw(2) << right
                          << sec % 60;
            }

            for (int i = 0; i < layer_; i++) {
                std::cout << "\e[1B";
            }
        }

        std::cout << "\r" << std::flush;
        mutex_.unlock();

    });

#elif _WIN32

#endif

    previous_num_task_ = num_task;
}

/*!
 * @brief プログレスバーを閉じる
 */
void ProgressBar::BarBody::close() {
#if __linux__
    thread_.join();

#elif _WIN32

#endif
}

/*!
 * @brief プログレスバーにタイトルを付ける
 * @param title
 */
void ProgressBar::BarBody::set_title(const string title) { title_ = title; }

/*!
 * @brief 経過時間の表示を有効にする
 */
void ProgressBar::BarBody::monitarTime() { monitars_time_ = true; }

/*!
 * @brief 進捗をクリアする
 */
void ProgressBar::BarBody::clear() const {
    string progress;
    const int digit = to_string(previous_num_task_).size();
    int num_done = 0, percent = 0;

#if __linux__
    mutex_.lock();

#elif _WIN32

#endif

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

    if (monitars_time_ == true) {
        std::cout << "0:00";
    }

    for (int i = 0; i < layer_; i++) {
        std::cout << "\e[1B";
    }

    std::cout << std::flush;

#if __linux__
    mutex_.unlock();

#elif _WIN32

#endif
}

/* プログレスバーを削除する */
void ProgressBar::BarBody::erase() const {
    std::cout << "\e[1A\e[2K\r" << std::flush;
}

/*!
 * @return かかった時間[ms]
 */
int64_t ProgressBar::BarBody::getTime_millsec() { return time_millsec_; }

/*!
 * @return かかった時間[s]
 */
int64_t ProgressBar::BarBody::getTime_sec() { return time_sec_; }
