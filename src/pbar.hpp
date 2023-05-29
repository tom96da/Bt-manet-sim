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
#include <thread>
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

    /* バー本体 */
    string progress_;
    /* 完了パーセント */
    int percent_;

    void set(const int num_task, int &num_done);

public:
    ProgressBar(const int num_task, int &num_done, int length = PBAR_LENGTH);
    void erase() const;
};

/* プログレスバークラス */
using PBar = ProgressBar;

#endif
