/*!
 * @file Device.hpp
 * @author tom96da
 * @brief Device クラスの定義
 * @date 2023-05-11
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "routingTable.hpp"
#include <string>
#include <map>
#include <set>
#include <variant>
using namespace std;
using Var = variant<int, double, string, Table>;

/* 最大接続数 */
const int MAX_CONNECTIONS = 5;

/* パケットクラスの前方宣言 */
class Packet;

/* Bluetooth デバイスクラス */
class Device
{
private:
    /* デバイスID */
    const int id_;
    /* 最大接続台数 */
    const int max_connections_;

    /* 累計パケット生成数 */
    mutable int num_packet_made_;
    /* ペアリング登録済みデバイス */
    map<int, Device &> paired_devices_;
    /* 接続中デバイス */
    set<int> connected_devices_;
    /* ルーティングテーブル */
    Table table_;

    /* メモリセルクラス */
    class Sell;
    /* メモリ */
    map<size_t, Sell> memory_;

public:
    Device(int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    string getName() const;

    int getNumPaired() const;
    int getNumConnected() const;
    set<int> getPairedDeviceId() const;
    set<int> getConnectedDeviceId() const;
    int getNumPacket() const;
    int getNewPacketId() const;
    Sell &getSellData(const size_t data_id);
    pair<size_t, Var> loadData(const size_t data_id) const;

    bool isPaired(const int another_device_id) const;
    bool isConnected(const int another_device_id) const;
    bool hasData(const size_t data_id) const;

    void pairing(Device &another_device);
    void unpairing(const int another_device_id);
    bool connect(const int another_device_id);
    void disconnect(const int another_device_id);
    void saveData(pair<size_t, Var> idata,
                  bool flood_flag = false, int flood_step = 0);
    void saveData(const Packet &packet);

    void sendMessage(const int receiver_id, string message);
    void receiveMessage(const int sender_id, string message);

    Packet makePacket(Var data) const;
    Packet makePacket(pair<size_t, Var> idata,
                      const bool flood_flag = false, const int flood_step = 0) const;
    void sendPacket(const int receiver_id, const Packet &packet);
    void receivePacket(const Packet &packet);

    size_t makeFloodData();
    void flooding(const int flag = false);

    void sendHello();

private:
    Device &getPairedDevice(const int id);

    bool isSelf(const int another_device_id) const;

    pair<size_t, Var> assignIdToData(const Var data, const bool flood_flag = false,
                                     size_t data_id = 0) const;
};

/* メモリセルクラス */
class Device::Sell
{
private:
    /* 送信元デバイスのID */
    const int sender_id_;
    /* 識別子付きデータ */
    const pair<size_t, Var> idata_;
    /* フラッディングホップ数 */
    const int flood_step_;
    /* フラッディングフラグ */
    bool flood_flag_;

public:
    Sell(const int sender_id, const pair<size_t, Var> idata,
         bool flood_flag, const int flood_step);

    int getSenderId() const;
    size_t getDataId() const;
    pair<size_t, Var> getData() const;
    int getFloodStep() const;

    bool isFloodFlag() const;

    void markFlagValid();
    void markFlagInvalid();
};

/* パケットクラス */
class Packet
{
private:
    /* 送信元デバイスのID */
    const int sender_id_;
    /* パケットID */
    const int packet_id_;
    /* シーケンスナンバー */
    const int seq_num_;
    /* 識別子付きデータ */
    const pair<size_t, Var> idata_;
    /* フラッディングフラグ */
    const bool flood_flag_;
    /* フラッディングホップ数 */
    const int flood_step_;

public:
    Packet(const int sender_id, const int packet_id,
           const int seq_num, const pair<size_t, Var> data,
           const bool flood_flag = false, const int flood_step = 0);

    int getSenderId() const;
    int getPacketId() const;
    int getSeqNum() const;

    pair<size_t, Var> getData() const;

    bool isFloodFlag() const;
    int getFloodStep() const;
};

/* パケットカウンタ */
namespace PacketCounter
{
    /* 累計パケット数 */
    extern int num_total_packe;

    int getTotalPacket();
    int getNewSequenceNum();
};

namespace pcnt = PacketCounter;

#include "Device.cpp"

#endif // DEVICE_HPP
