/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

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
    const double field_size = 60;
    /* ノード数 */
    const int num_node = 100;
    /* 記録ファイル */
    auto files = vector<ofstream>{};

    /* ファイル作成 */
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

    /* シミュレーション開始 */
    std::cout << "field size: " << field_size << "x" << field_size << ", "
              << "number of node: " << num_node << std::endl;

    /*　マネージャー */
    auto mgr = new MGR{field_size};
    mgr->setSimMode(SIMMODE::EXITING);

    // 孤立しないネットワークを構築する
    while (true) {
        // 孤立するノードがないネットワークができるまで繰り返す
        mgr->deleteDeviceAll();
        mgr->addDevices(num_node);
        mgr->buildNetwork();
        const auto [_, num_member] = mgr->flooding(45);
        if (num_member == num_node) {
            // 孤立するノードがなければ抜ける
            break;
        }
    }

    writeCsv(mgr);
    int id_central = mgr->getCentralDevice();

    {  // 既存手法
        mgr->sendHello();
        mgr->makeMPR();
        mgr->showMPR(id_central);
    }

    mgr->clearDevice();
    mgr->resetNetwork();

    {  // 提案手法 遠距離選択接続
        mgr->setSimMode(SIMMODE::PROPOSAL_LONG_CONNECTION);
        mgr->buildNetwork();
        const auto [_, num_member] = mgr->flooding(45);
        if (num_member != num_node) {
            // 孤立するノードがあれば
            return 0;
        }

        mgr->sendHello();
        mgr->makeMPR();
        mgr->showMPR(id_central);
    }

    delete mgr;

    /* シミュレーションここまで */
    std::cout << "comlete! " << std::endl;

    return 0;
}
