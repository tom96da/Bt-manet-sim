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
#include <vector>
#include <any>
using namespace std;

/* 最大接続数 */
const int MAX_CONNECTIONS = 6;

/* パケットクラスの前方宣言 */
template <typename T>
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
    map<int, Device *> paired_devices_;
    /* 接続中デバイス */
    set<int> connected_devices_;
    /* メモリー */
    map<size_t, any> memory_;
    /* ルーティングテーブル */
    Table table_;

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

    bool isPaired(const int another_device_id) const;
    bool isConnected(const int another_device_id) const;

    void pairing(Device &another_device);
    void unpairing(const int another_device_id);
    void connect(const int another_device_id);
    void disconnect(const int another_device_id);

    void sendMessage(const int receiver_id, string message);
    void receiveMessage(const int sender_id, string message);

    template <typename T>
    Packet<T> makePacket(T data) const;
    template <typename T>
    Packet<T> makePacket(pair<size_t, T> idata, const bool flood_flag = false) const;
    template <typename T>
    void sendPacket(const int receiver_id, const Packet<T> &packet);
    template <typename T>
    void receivePacket(const int sender_id, const Packet<T> &packet);

    template <typename T>
    size_t flooding();
    template <typename T>
    void hopping(const pair<size_t, T> idata, const int sender_id);

    void sendHello();

private:
    Device *getPairedDevice(const int id);

    bool isSelf(const int another_device_id) const;

    template <typename T>
    pair<size_t, T> makeSendData(const T data, const bool flood_flag = false) const;
};

/* パケットクラス */
template <typename T>
class Packet
{
private:
    /* 送信元デバイスのID */
    const int sender_id_;
    /* パケットID */
    const int packet_id_;
    /* シーケンスナンバー */
    const int seq_num_;
    /* データ識別子 */
    // const int data_id_;
    /* 送信するデータ */
    const pair<size_t, T> data_;
    /* フラッディングフラグ */
    const bool flood_flag_;

public:
    Packet(const int sender_id, const int packet_id, const int seq_num,
           const pair<size_t, T> data, const bool flood_flag = false);

    int getSenderId() const;
    int getPacketId() const;
    int getSeqNum() const;
    int getDataId() const;

    pair<size_t, T> getData() const;

    bool isFloodFlag() const;
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
