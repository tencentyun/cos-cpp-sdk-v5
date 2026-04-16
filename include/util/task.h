#pragma once

#include <cstdint>

// 任务状态枚举
typedef enum {
  TASK_IDLE = 0,      // 任务空闲，可分配新任务
  TASK_RUNNING = 1,   // 任务正在执行中
  TASK_COMPLETED = 2  // 任务已完成
} TaskStatus;

// 任务相关信息结构体
class TaskInfo {
public:
  TaskStatus status;
  uint64_t sequence;

  TaskInfo() : status(TASK_IDLE), sequence(0) {}

  explicit TaskInfo(TaskStatus status, uint64_t sequence) : status(status), sequence(sequence) {}
};
