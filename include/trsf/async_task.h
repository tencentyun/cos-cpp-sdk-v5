#pragma once

#include "Poco/TaskManager.h"
#include "Poco/Task.h"

namespace qcloud_cos {

using TaskFunc = std::function<void()>;

class AsyncTask : public Poco::Task {
  public:
    AsyncTask(TaskFunc &&f) :  Task("AsyncTask"), _f(f) {}
    ~AsyncTask() {}

  void runTask() {
    _f();
  }

  private:
    TaskFunc _f;
};

}  // namespace qcloud_cos