// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/03/17
// Description:

#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H
#pragma once

namespace qcloud_cos {

class NonCopyable {
 protected:
  NonCopyable() {}
  ~NonCopyable() {}

 private:  // emphasize the following members are private
  NonCopyable(const NonCopyable&);
  const NonCopyable& operator=(const NonCopyable&);
};
}  // namespace qcloud_cos

#endif  // NONCOPYABLE_H
