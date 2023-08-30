/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @details Bluetooth MANET におけるルーティングについて、
 *          接続距離を最大化する提案手法の効果を確認するシミュレーションプログラム
 *          実行時は、オプションに "-std=c++20" を指定する。
 * @date 2023-05-11
 */

#if _WIN32
#include <windows.h>

#endif

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "Device.hpp"
#include "DeviceManager.hpp"
#include "pbar.hpp"

using namespace std;

int main() {
    /* フィールドサイズ */
    const double field_size = 60.0;
    /* ノード数 */
    const int num_node = 100;
    /* 記録ファイル */
    auto files = vector<ofstream>{};
    /* 試行回数 */
    const int num_repeat = 1000;
    /* 結果 */
    vector<tuple<int, double, int64_t, vector<map<int, double>>>>
        result_convetntional, result_proposal;

    /* 座標記録ファイル作成 */
    auto newCsv = [&](MGR *mgr) {
        for (auto id : mgr->getDevicesList()) {
            string fname = "../tmp/position/device" + to_string(id) + ".csv";
            auto &file = files.emplace_back(fname);
            file << "x,y" << endl;
        }
    };

    /* ファイルに座標書き込み */
    auto writeCsv = [&](MGR *mgr) {
        if (files.empty()) {
            newCsv(mgr);
        }

        for (auto id : mgr->getDevicesList()) {
            auto [x, y] = mgr->getPosition(id);
            files[id] << x << ", " << y << endl;
        }
    };

    /* 度数分布を加算する */
    auto addFrequency = [](vector<map<int, double>> frequencys_acc,
                           const vector<map<int, double>> frequencys_elem) {
        if (frequencys_acc.empty()) {
            return frequencys_elem;
        }

        for (int i = 0; auto &frequency_each_zone : frequencys_acc) {
            for (auto [num_hop, num_device] : frequencys_elem[i]) {
                frequency_each_zone[num_hop] += num_device;
            }
            ++i;
        }
        return frequencys_acc;
    };

    /* 度数分布を除算する */
    auto divideFrequency = [](vector<map<int, double>> frequencys,
                              const int divisor) {
        for (auto &frequency_each_zone : frequencys) {
            for (auto [num_hop, _] : frequency_each_zone) {
                frequency_each_zone[num_hop] /= divisor;
            }
        }

        return frequencys;
    };

    /* 結果から平均を得る */
    auto average =
        [&](vector<tuple<int, double, int64_t, vector<map<int, double>>>>
                &result)
        -> tuple<int, double, int64_t, vector<map<int, double>>> {
        /* 結果の総和をとる */
        auto sum = reduce(
            result.begin(), result.end(),
            tuple<int, double, int64_t, vector<map<int, double>>>{
                0, 0.0, 0, {}},
            [&](const auto &acc, const auto &elem)
                -> tuple<int, double, int64_t, vector<map<int, double>>> {
                return {get<0>(acc) + get<0>(elem), get<1>(acc) + get<1>(elem),
                        get<2>(acc) + get<2>(elem),
                        addFrequency(get<3>(acc), get<3>(elem))};
            });

        /* 試行回数で割って返す */
        return {get<0>(sum) / num_repeat, get<1>(sum) / num_repeat,
                get<2>(sum) / num_repeat,
                divideFrequency(get<3>(sum), num_repeat)};
    };

    /* シミュレーション開始 */

    /* パラメータ表示 */
    std::cout << "field size: " << field_size << "x" << field_size << ", "
              << "number of node: " << num_node << ", "
              << "repeat: " << num_repeat << std::endl;

    auto pbar = PBar();
    auto &pb_repeat = pbar.add();
    auto &pb_proposal = pbar.add();
    auto &pb_conventional = pbar.add();
    pb_repeat.set_title("Simulation progress");
    pb_repeat.monitarTime();
    pb_conventional.set_title("CONVENTIONAL");
    pb_proposal.set_title("LONG_CONNECTION");

    int count_repeat = 0;
    pb_repeat.clear();
    pb_repeat.start(num_repeat, count_repeat);

    for (; count_repeat < num_repeat;) {
        pb_conventional.clear();
        pb_proposal.clear();

        /*　マネージャー */
        auto mgr = new MGR{field_size};
        mgr->setSimMode(SIMMODE::CONVENTIONAL);

        /* 孤立した端末がないネットワークを構築する */
        while (true) {
            /* 孤立するノードがないネットワークができるまで繰り返す */
            mgr->deleteDeviceAll();
            mgr->addDevices(num_node);
            mgr->buildNetwork();
            const auto [_, num_member] = mgr->flooding(45);
            if (num_member == num_node) {
                /* 孤立するノードがなければ抜ける */
                break;
            }
        }

        writeCsv(mgr);

        /* 完成するまでテーブルを更新する */
        auto makingTableUntilcomplete =
            [&mgr, num_node](
                std::vector<std::tuple<int, double, int64_t,
                                       vector<map<int, double>>>> &result,
                ProgressBar::BarBody &pb) {
                int num_packet_start = Device::getTotalPacket();
                int num_packet_end = 0;
                int num_done = 0;
                int num_update = 0;
                pb.start(num_node, num_done);

                while (true) {
                    mgr->sendTable();
                    num_done = num_node - mgr->makeTable();
                    if (num_done < num_node) {
                        num_packet_end = Device::getTotalPacket();
                        ++num_update;
                    } else {
                        break;
                    }
                }

                pb.close();
                result.emplace_back(num_packet_end - num_packet_start,
                                    num_update, pb.getTime_millsec(),
                                    mgr->calculateTableFrequency());
            };

        { /* 既存手法 */
            mgr->sendHello();
            mgr->makeMPR();
            // mgr->showMPR(0);

            makingTableUntilcomplete(result_convetntional, pb_conventional);
        }

        mgr->clearDevice();
        mgr->resetNetwork();

        { /* 提案手法 遠距離選択接続 */
            mgr->setSimMode(SIMMODE::PROPOSAL_LONG_CONNECTION);
            mgr->buildNetwork();
            const auto [_, num_member] = mgr->flooding(45);
            if (num_member != num_node) {
                /* 孤立するノードがあれば従来手法の結果を消してループに戻る */
                result_convetntional.pop_back();
                continue;
            }

            mgr->sendHello();
            mgr->makeMPR();
            // mgr->showMPR(0);

            makingTableUntilcomplete(result_proposal, pb_proposal);
        }

        ++count_repeat;

        delete mgr;
    }
    pb_repeat.close();
    pbar.erase();
    auto time = pb_repeat.getTime_sec();

    /* シミュレーションここまで */
    std::cout << "comlete! ";
    if (time < 3600) {
        std::cout << time / 60 << ":" << setfill('0') << setw(2) << right
                  << time % 60;
    } else {
        std::cout << time / 3600 << ":" << setfill('0') << setw(2) << right
                  << time % 3600 / 60 << ":" << setfill('0') << setw(2) << right
                  << time % 60;
    }
    std::cout << std::endl;

    /* 以下結果を集計・記録 */

    /* 従来手法平均 */
    auto average_conventional = average(result_convetntional);
    /* 提案手法平均 */
    auto average_proposal = average(result_proposal);

    auto &file_result = files.emplace_back("../tmp/result.csv");
    /* パラメータ書き込み */
    file_result << "field size;" << field_size << "x" << field_size << ","
                << "number of node;" << num_node << ","
                << "repeat;" << num_repeat << std::endl;
    file_result << "METHOD;packets,update,time[ms]" << std::endl;

    /* 平均書き込み */
    auto writeResult =
        [&](string s,
            tuple<int, double, int64_t, vector<map<int, double>>> &average) {
            file_result << s << ";" << get<0>(average) << "," << get<1>(average)
                        << "," << get<2>(average) << std::endl;
        };

    writeResult("CONVENTIONAL", average_conventional);
    writeResult("PROPOSAL", average_proposal);

    /* 従来手法度数分布 */
    auto &frequency_conventional = get<3>(average_conventional);
    /* 提案手法度数分布 */
    auto &frequency_proposal = get<3>(average_proposal);

    auto &file_frequency = files.emplace_back("../tmp/frequency.csv");
    file_frequency << "field size;" << field_size << "x" << field_size << ","
                   << "number of node;" << num_node << ","
                   << "repeat;" << num_repeat << std::endl;
    file_frequency << ",central,, ,middle,, ,edge," << std::endl;
    file_frequency << "hops,conventional,proposal, ,conventional,"
                      "proposal, ,conventional,proposal"
                   << std::endl;

    /* 度数分布書き込み */
    for (size_t num_hop = 1; true; num_hop++) {
        int count = 0;
        file_frequency << num_hop << ", ";

        for (size_t i = 0; i < 3; i++) {
            auto writeNumHop =
                [&](std::vector<std::map<int, double>> &frequency) {
                    if (frequency[i].count(num_hop)) {
                        file_frequency << frequency[i][num_hop] << ",";
                        ++count;
                    } else {
                        file_frequency << 0 << ",";
                    }
                };

            writeNumHop(frequency_conventional);
            writeNumHop(frequency_proposal);

            if (i != 2) {
                file_frequency << " ,";
            }
        }
        file_frequency << std::endl;

        if (count == 0) {
            break;
        }
    }

    return 0;
}
