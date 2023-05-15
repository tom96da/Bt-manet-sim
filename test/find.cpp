/*!
 * @file finde.cpp
 * @author tom96da
 * @brief find()のテスト
 * @date 2023-05-13
 */

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v{1, 2, 9, 7};
    int erase_num = 7;

    auto it = std::find(v.begin(), v.end(), erase_num);
    if (it == v.end())
    {
        std::cout << "not found." << std::endl;
        return 0;
    }
    std::cout << "found: " << *it << std::endl;
    std::cout << "position: "
              << std::distance(v.begin(), it) << std::endl;

    std::cout << "erase: " << *it << std::endl;
    v.erase(it);

    it = std::find(v.begin(), v.end(), erase_num);
    if (it == v.end())
    {
        std::cout << "not found." << std::endl;
        return 0;
    }

    std::cout << "found: " << *it << std::endl;
    std::cout << "position: "
              << std::distance(v.begin(), it) << std::endl;

    return 0;
}
