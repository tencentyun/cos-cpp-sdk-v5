#pragma once

#include <functional>

#include "Poco/Task.h"
#include "Poco/TaskManager.h"

namespace qcloud_cos {

using TaskFunc = std::function<void()>;

class AsyncTask : public Poco::Task {
 public:
  AsyncTask(TaskFunc &&f) : Task("AsyncTask"), _f(f) {}
  ~AsyncTask() {}

  void runTask() { _f(); }

 private:
  TaskFunc _f;
};

}  // namespace qcloud_cos