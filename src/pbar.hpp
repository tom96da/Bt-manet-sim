/*!
 * @file pbar.hpp
 * @author tom96da
 * @brief 並列プログレスバーのヘッダファイル
 * @date 2023-05-25
 */

#ifndef PBAR_HPP
#define PBAR_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

using namespace std;

const int PBAR_LENGTH = 20;

/* プログレスバークラス */
class ProgressBar
{
private:
    /* バーの長さ */
    const int length_;
    /* 1ステップの大きさ */
    const double step_;
    /* タスクの桁数 */
    const int digit_;
    /* 総タスク数 */
    const int num_task_;

    /* 完了タスク数 */
    int &num_done_;

    /* バー進捗 */
    string progress_;
    /* バータイトル */
    string title_;
    /* 完了パーセント */
    int percent_;

    /* かかった時間 */
    int64_t time_;

public:
    ProgressBar(const int num_task, int &num_done, int length = PBAR_LENGTH);
    void start();
    void setTitle(const string title);
    void erase() const;

    int64_t getTime() const;
};

/* プログレスバークラス */
using PBar = ProgressBar;

#include "pbar.cpp"

#endif
