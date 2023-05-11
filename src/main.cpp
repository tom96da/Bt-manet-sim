/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include <iostream>
#include "Device.hpp"

using namespace std;

int main(void)
{
    int id = 0;
    auto d = Device(id);
    cout << d.getId() << endl;

    return 0;
}
