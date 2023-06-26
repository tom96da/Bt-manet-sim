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
 *     int task, done;
 *     auto pbar = PBar();
 *     auto pb1 = pbar.add();
 *     pb1.set_title("");
 *     pb1.start(task, done);
 *
 *     // 進捗の処理
 *
 *     auto time1 = pb1.time();
 *     pb1.erase();
 *
 *     return 0;
 * }
 *
 */

#ifndef PBAR_HPP
#define PBAR_HPP

#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

const int PBAR_LENGTH = 20;

/* プログレスバークラス */
class ProgressBar {
   private:
    /* バー本体クラス */
    class BarBody;
    /* バー本体 */
    vector<BarBody> pbars_;

   public:
    ProgressBar();

    BarBody &add(const int layer = 0);
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

    /* かかった時間 */
    int64_t time_;

    /* 並列スレッド */
    thread thread_;

   public:
    BarBody(const int layer);
    void start(const int num_task, int &num_done);
    void close();
    void set_title(const string title);
    void clear() const;
    void erase() const;

    int64_t time();
};

/* プログレスバークラス */
using PBar = ProgressBar;

#include "pbar.cpp"

#endif
