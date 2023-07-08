/*!
 * @file pbar.hpp
 * @author tom96da
 * @brief 並列プログレスバーのヘッダファイル
 * @date 2023-05-25
 */

/**
 * 想定する使い方
 *
 * int main()
 * {
 *     auto pbar = PBar();
 *     auto &pb_sample = pbar.add();
 *     pb_sample.set_title("Sample");
 *
 *     int task, done;
 *     pb_sample.start(task, done);
 *
 *     // 進捗の処理
 *
 *     pb_sample.close();
 *     pb_sample.erase();
 *     auto time = pb_sample.getTime();
 *
 *     return 0;
 * }
 *
 */

#ifndef PBAR_HPP
#define PBAR_HPP

#include <string>
#include <thread>
#include <vector>

#if __linux__
#include <mutex>

#elif _WIN32

#endif

using namespace std;

const int PBAR_LENGTH = 20;

#if __linux__
std::mutex mutex_;

#elif _WIN32

#endif

/* プログレスバークラス */
class ProgressBar {
   public:
    /* バー本体クラス */
    class BarBody;

   private:
    /* バー本体 */
    vector<std::unique_ptr<BarBody>> pbars_;

   public:
    ProgressBar();
    ~ProgressBar();

    BarBody &add();

    void erase() const;
};

/* バー本体 */
class ProgressBar::BarBody {
   private:
    /* レイヤー */
    const int layer_;
    /* バーの長さ */
    const int length_;
    /* 1ステップの大きさ */
    const double step_;

    /* バータイトル */
    string title_;
    /* タスク数の履歴 */
    int previous_num_task_;
    /* かかった時間[ms] */
    int64_t time_millsec_;
    /* かかった時間[s] */
    int64_t time_sec_;

    /* 経過時間の表示 */
    bool monitars_time_;

#if __linux__
    /* 並列スレッド */
    thread thread_;

#elif _WIN32

#endif

   public:
    BarBody(const int index);
    ~BarBody();

    void start(const int num_task, int &num_done);
    void close();
    void set_title(const string title);
    void monitarTime();
    void clear() const;
    void erase() const;

    int64_t getTime_millsec();
    int64_t getTime_sec();
};

/* プログレスバークラス */
using PBar = ProgressBar;

#include "pbar.cpp"

#endif
