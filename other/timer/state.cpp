
#include <iostream>


class CanSignalJumpDetector {
private:
    CanSignalConfig signal_cfg;   // 目标信号配置
    JumpCondition jump_condition; // 跳变条件
    float last_valid_val;         // 上一次有效的信号值
    float current_val;            // 当前信号值
    bool is_initialized;          // 是否完成初始化（避免首次比较误判）
    std::function<void(float, float)> on_jump; // 跳变触发回调（参数：上一值，当前值）

    // 解析CAN报文中的原始信号值（根据字节序和长度）
    int32_t parseRawValue(const uint8_t* data, uint8_t data_len) {
        int32_t raw_val = 0;
        // 简化实现：假设数据长度足够，根据起始位和长度提取原始值
        // （实际需处理跨字节、字节序转换，可参考CANoe/CANalyzer的解析逻辑）
        for (uint8_t i = 0; i < signal_cfg.signal_length; i++) {
            uint8_t bit_pos = signal_cfg.start_bit + i;
            uint8_t byte_idx = bit_pos / 8;
            uint8_t bit_in_byte = bit_pos % 8;
            if (byte_idx >= data_len) break; // 报文长度不足，视为无效
            raw_val |= ((data[byte_idx] >> bit_in_byte) & 0x01) << i;
        }
        // 处理有符号数
        if (signal_cfg.is_signed && (raw_val & (1 << (signal_cfg.signal_length - 1)))) {
            raw_val -= (1 << signal_cfg.signal_length);
        }
        return raw_val;
    }

    // 检查信号值是否在有效范围
    bool isValidValue(float val) {
        return (val >= signal_cfg.min_val) && (val <= signal_cfg.max_val);
    }

    // 判定是否满足跳变条件
    bool checkJumpCondition(float prev, float curr) {
        switch (jump_condition.type) {
            case JUMP_ANY:
                return (prev != curr); // 任意值变化均视为跳变
            case JUMP_FROM_TO:
                return (prev == jump_condition.from_val) && (curr == jump_condition.to_val);
            case JUMP_RANGE_TO_RANGE:
                bool from_in_range = (prev >= jump_condition.from_min) && (prev <= jump_condition.from_max);
                bool to_in_range = (curr >= jump_condition.to_min) && (curr <= jump_condition.to_max);
                return from_in_range && to_in_range;
            default:
                return false;
        }
    }

public:
    CanSignalJumpDetector(const CanSignalConfig& cfg, const JumpCondition& cond)
        : signal_cfg(cfg), jump_condition(cond), last_valid_val(0), current_val(0), is_initialized(false) {}

    // 设置跳变触发回调函数
    void setJumpCallback(std::function<void(float, float)> callback) {
        on_jump = callback;
    }

    // 处理接收到的CAN报文（由CAN接收中断或定时任务调用）
    void processCanMessage(uint32_t can_id, const uint8_t* data, uint8_t data_len) {
        // 仅处理目标ID的报文
        if (can_id != signal_cfg.can_id) return;

        // 解析原始值并转换为物理值
        int32_t raw_val = parseRawValue(data, data_len);
        current_val = raw_val * signal_cfg.factor + signal_cfg.offset;

        // 过滤无效值（如超出范围的信号，可能是总线错误）
        if (!isValidValue(current_val)) return;

        // 初始化：首次收到有效值，不触发跳变
        if (!is_initialized) {
            last_valid_val = current_val;
            is_initialized = true;
            return;
        }

        // 检测跳变
        if (checkJumpCondition(last_valid_val, current_val)) {
            if (on_jump) {
                on_jump(last_valid_val, current_val); // 触发回调
            }
        }

        // 更新上一次有效值
        last_valid_val = current_val;
    }
};