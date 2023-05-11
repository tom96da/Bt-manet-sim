/*!
 * @file size.cpp
 * @author tom96da
 * @brief 配列の大きさの取得方法の違い
 * @date 2023-05-09
 */

#include <iostream>
#include <vector>

using namespace std;

int main(void)
{
    int array[]{0, 1, 2, 3, 4};
    vector<int> vector{0, 1, 2, 3, 4};

    cout << size(array) << endl;
    cout << vector.size() << endl;
    cout << size(vector) << endl;
}