/*!
 * @file Device.hpp
 * @author tom96da
 * @brief Device クラスの定義
 * @date 2023-05-11
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include <vector>
using namespace std;

/* 最大接続数 */
const int MAX_CONNECTIONS = 6;

/* Bluetooth デバイス */
class Device
{
private:
    /* デバイスID */
    const int id_;
    /* デバイス名 */
    const string name_;
    /* 最大接続台数 */
    const int max_connections_;

    /* ペアリング登録済みデバイス */
    vector<Device *> paired_devices_;
    /* 接続中デバイス */
    vector<const int *> connected_devices_;

    class Packet
    {
    private:
        const int id_;
        const size_t seq_;
    };

public:
    Device(int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    const int *getIdPtr() const;
    string getName() const;

    int getNumPaired() const;
    vector<int> getPairedDeviceId() const;

    int getNumConnected() const;
    vector<int> getConnectedDeviceId() const;

    void hello() const;

    bool isSelf(const Device &another_device) const;

    bool isPaired(const Device &another_device) const;
    void pairing(Device &another_device);
    void removePairing(Device &another_device);

    bool isConnected(const Device &another_device) const;
    void connect(Device &another_device);
    void disconnect(Device &another_device);

    void sendMessage(Device &receiver, string message);
    void receiveMessage(Device &sender, string message);

    Device *getPairedDevice(const int id);
};

#endif
