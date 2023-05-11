/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include "Device.hpp"

using namespace std;

int main(void)
{
    int id = 0;
    auto d = Device(id);

    d.hello();

    return 0;
}
