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

const int MAX_CONNECTIONS = 6;

class Device
{
private:
    const int id_;
    const string name_;
    const int max_connections_;

public:
    Device(int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    string getName() const;

    void hello() const;
};

#endif