#include "util/illegal_intercept.h"
#include <vector>
#include "cos_sys_config.h"

using namespace std;
namespace qcloud_cos {

bool IllegalIntercept::ObjectKeySimplifyCheck(const std::string& path) {
    if(!CosSysConfig::GetObjectKeySimplifyCheck()) {
        return true;
    }
    auto split = [](const string& s, char delim) -> vector<string> {
        vector<string> ans;
        string cur;
        for (char ch : s) {
            if (ch == delim) {
                ans.push_back(move(cur));
                cur.clear();
            }
            else {
                cur += ch;
            }
        }
        ans.push_back(move(cur));
        return ans;
    };

    vector<string> names = split(path, '/');
    vector<string> stack;
    for (string& name : names) {
        if (name == "..") {
            if (!stack.empty()) {
                stack.pop_back();
            }
        }
        else if (!name.empty() && name != ".") {
            stack.push_back(move(name));
        }
    }
    string ans;
    if (stack.empty()) {
        ans = "/";
    }
    else {
        for (string& name : stack) {
            ans += "/" + move(name);
        }
    }
    if ("/" == ans) {
        return false;
    }
    return true;
}

bool IllegalIntercept::isAlnum(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

bool MatchesPattern2(const std::string& str) {
    if (str.size() != 1 || !IllegalIntercept::isAlnum(str[0])) {
        return false;
    }
    return true;
}

bool MatchesPattern1(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    // 检查第一个字符是否为字母或数字
    if (!IllegalIntercept::isAlnum(str.front())) {
        return false;
    }

    // 检查最后一个字符是否为字母或数字
    if (!IllegalIntercept::isAlnum(str.back())) {
        return false;
    }

    // 检查中间字符是否为字母、数字或短横线
    for (size_t i = 1; i < str.size() - 1; ++i) {
        char c = str[i];
        if (!IllegalIntercept::isAlnum(c) && c != '-') {
            return false;
        }
    }

    return true;
}

bool IllegalIntercept::CheckBucket(const std::string& bucket) {
    if (MatchesPattern1(bucket)) {
        return true;
    }
    if (MatchesPattern2(bucket)) {
        return true;
    }
    return false;
}
} // namespace qcloud_cos