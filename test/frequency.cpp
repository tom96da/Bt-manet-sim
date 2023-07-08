/*!
 * @file frequency.cpp
 * @author tom96da
 * @brief 度数分布の作成と集計
 * @date 2023-07-03
 */

#include <iostream>
#include <map>
#include <vector>

std::map<int, int> calculateFrequency(const std::vector<int>& data) {
    std::map<int, int> frequencyMap;

    // データの個数を数える
    for (int num : data) {
        frequencyMap[num]++;
    }

    return frequencyMap;
}

void printFrequency(const std::map<int, int>& frequencyMap) {
    for (const auto& [v, n] : frequencyMap) {
        std::cout << v << ": " << n << std::endl;
    }
}

void mergeFrequency(std::map<int, int>& frequency_original,
                    std::map<int, int> frequency_device) {
    for (auto [num_hop, num_device] : frequency_device) {
        frequency_original[num_hop] += num_device;
    }
};

int main() {
    std::map<int, int> sum;
    std::vector<int> data = {1, 2, 3, 1, 2, 4, 5, 2, 3, 4, 1, 3};

    std::map<int, int> frequencyMap = calculateFrequency(data);
    mergeFrequency(sum, frequencyMap);
    mergeFrequency(sum, frequencyMap);

    printFrequency(frequencyMap);
    printFrequency(sum);

    return 0;
}
