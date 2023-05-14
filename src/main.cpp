/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include <iostream>
#include <vector>
#include "Device.hpp"

using namespace std;

int main(void)
{
    int num = 4;
    vector<Device> d;
    for (int id = 0; id < num; id++)
        d.emplace_back(id);

    d[0].pairing(d[1]);
    d[0].pairing(d[2]);
    d[0].pairing(d[3]);
    d[0].pairing(d[0]);
    d[0].connect(d[1]);
    d[0].connect(d[3]);

    cout << d[0].isPaired(d[1]) << " ";
    cout << d[1].isPaired(d[0]) << endl;
    cout << d[0].isConnected(d[1]) << " ";
    cout << d[1].isConnected(d[0]) << endl;

    vector<int> ids = d[0].getPairedDeviceId();
    for (int id : ids)
    {
        cout << id << " " << flush;
    }
    cout << endl;
    ids = d[0].getConnectedDeviceId();
    for (int id : ids)
    {
        cout << id << " " << flush;
    }
    cout << endl;

    return 0;
}
