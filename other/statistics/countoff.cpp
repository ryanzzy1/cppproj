#include <iostream>
#include <vector>
#include <climits> // 用于INT_MAX
#include <algorithm>

using namespace std;

/**
 * @brief 计算满足条件的最小充值金额
 * @param currentBalance 当前账户余额
 * @param monthlyConsumption 每月消费金额
 * @param denominations 可用的充值面额数组
 * @return 最小充值金额。如果无法凑出满足条件的金额，则返回-1。
 */
int findMinRecharge(int currentBalance, int monthlyConsumption, const vector<int>& denominations) {
    // 计算需要满足的余数条件： (当前余额 + 充值金额) % 每月消费额 == 0
    // 即 充值金额 % 每月消费额 ≡ (每月消费额 - 当前余额 % 每月消费额) % 每月消费额
    int requiredRemainder = (monthlyConsumption - (currentBalance % monthlyConsumption)) % monthlyConsumption;

    // 设置一个合理的金额上限，这里设为最大面额的20倍，可以根据实际情况调整
    int maxAmount = *max_element(denominations.begin(), denominations.end()) * 20;

    // dp数组初始化：dp[i]表示凑出金额i所需的最少张数，初始化为一个很大的值（表示无法凑出）
    vector<int> dp(maxAmount + 1, INT_MAX - 1); // INT_MAX-1 防止后续+1溢出
    dp[0] = 0; // 凑出0元需要0张

    // 动态规划过程：完全背包问题的思路
    for (int coin : denominations) {
        for (int i = coin; i <= maxAmount; i++) {
            if (dp[i - coin] != INT_MAX - 1) { // 如果i-coin可以被凑出
                dp[i] = min(dp[i], dp[i - coin] + 1);
            }
        }
    }

    // 寻找满足余数条件的最小金额
    int minRechargeAmount = -1;
    for (int amount = 0; amount <= maxAmount; amount++) {
        if (dp[amount] != INT_MAX - 1) { // 该金额可以被凑出
            if (amount % monthlyConsumption == requiredRemainder) {
                minRechargeAmount = amount;
                break; // 因为金额是从小到大遍历的，第一个找到的就是最小的
            }
        }
    }

    return minRechargeAmount;
}

/**
 * @brief 找出所有满足条件的充值金额及其组合方式（可选，用于展示所有可能方案）
 * @param currentBalance 当前账户余额
 * @param monthlyConsumption 每月消费金额
 * @param denominations 可用的充值面额数组
 * @return 一个向量，包含所有满足条件的充值金额（及其组合方式，这里简化为金额列表）
 */
vector<int> findAllValidRecharges(int currentBalance, int monthlyConsumption, const vector<int>& denominations) {
    int requiredRemainder = (monthlyConsumption - (currentBalance % monthlyConsumption)) % monthlyConsumption;
    int maxAmount = 4000; // 设置一个较大的上限
    vector<int> dp(maxAmount + 1, INT_MAX - 1);
    dp[0] = 0;
    vector<int> validAmounts;

    for (int coin : denominations) {
        for (int i = coin; i <= maxAmount; i++) {
            if (dp[i - coin] != INT_MAX - 1) {
                dp[i] = min(dp[i], dp[i - coin] + 1);
            }
        }
    }

    for (int amount = 0; amount <= maxAmount; amount++) {
        if (dp[amount] != INT_MAX - 1 && amount % monthlyConsumption == requiredRemainder) {
            validAmounts.push_back(amount);
        }
    }
    return validAmounts;
}

int main() {
    // 根据您的问题设置参数
    int currentBalance = 2;
    int monthlyConsumption = 19;
    vector<int> denominations = {20, 50, 100, 200};

    cout << "当前账户余额: " << currentBalance << " 元" << endl;
    cout << "每月消费额: " << monthlyConsumption << " 元" << endl;
    cout << "可用充值面额: ";
    for (int denom : denominations) cout << denom << " ";
    cout << "元" << endl << endl;

    // 计算最小充值金额
    int minRecharge = findMinRecharge(currentBalance, monthlyConsumption, denominations);

    if (minRecharge != -1) {
        int totalBalance = currentBalance + minRecharge;
        int months = totalBalance / monthlyConsumption;
        int remainingBalance = totalBalance % monthlyConsumption; // 应该为0

        cout << ">>> 最优解 <<<" << endl;
        cout << "最小充值金额: " << minRecharge << " 元" << endl;
        cout << "充值后总余额: " << totalBalance << " 元" << endl;
        cout << "可消费月数: " << months << " 个月" << endl;
        cout << "最终剩余余额: " << remainingBalance << " 元 (恰好用完)" << endl;
    } else {
        cout << "在当前设置下，无法找到满足条件的充值金额。" << endl;
    }

    // 可选：列出所有可能的解（在一定范围内）
    cout << endl << ">>> 其他可行解（充值金额小于500元） <<<" << endl;
    vector<int> allSolutions = findAllValidRecharges(currentBalance, monthlyConsumption, denominations);
    int count = 0;
    for (int amount : allSolutions) {
        if (amount > 500) continue; // 只显示较小的解
        count++;
        int totalBalance = currentBalance + amount;
        int months = totalBalance / monthlyConsumption;
        cout << "方案" << count << ": 充值 " << amount << " 元, 总余额 " << totalBalance
             << " 元, 可消费 " << months << " 个月" << endl;
    }

    if (count == 0) {
        cout << "在500元范围内未找到其他可行解。" << endl;
    }

    return 0;
}