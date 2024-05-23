#include "util/illegal_intercept.h"
#include "cos_sys_config.h"
#include <vector>
using namespace std;
namespace qcloud_cos {

bool IllegalIntercept::ObjectKeySimplifyCheck(const std::string& path) {
    if(!CosSysConfig::GetObjectKeySimplifyCheck()){
        return true;
    }
    auto split = [](const string& s, char delim) -> vector<string> {
        vector<string> ans;
        string cur;
        for (char ch: s) {
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
    for (string& name: names) {
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
        for (string& name: stack) {
            ans += "/" + move(name);
        }
    }
    if ("/"==ans){
        return false;
    }
    return true;
}
} // namespace qcloud_cos