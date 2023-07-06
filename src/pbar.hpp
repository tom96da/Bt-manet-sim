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
 *     auto &pb1 = pbar.add();
 *     pb1.set_title("Sample");
 *
 *     int task, done;
 *     pb1.start(task, done);
 *
 *     // 進捗の処理
 *
 *     pb1.close();
 *     pb1.erase();
 *     auto time1 = pb1.time();
 *
 *     return 0;
 * }
 *
 */

#ifndef PBAR_HPP
#define PBAR_HPP

#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace std;

const int PBAR_LENGTH = 20;

std::mutex mutex_;

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
    bool monitor_time_;

    /* 並列スレッド */
    thread thread_;

   public:
    BarBody(const int index);
    void start(const int num_task, int &num_done);
    void close();
    void set_title(const string title);
    void monitor_time();
    void clear() const;
    void erase() const;

    int64_t time_millsec();
    int64_t time_sec();
};

/* プログレスバークラス */
using PBar = ProgressBar;

#include "pbar.cpp"

#endif
