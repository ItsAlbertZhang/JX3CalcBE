#ifndef PROGRAM_LOG_H_
#define PROGRAM_LOG_H_

#include <format>
#include <string>

namespace ns_program {

class Log {
public:
    static Log &getInstance();                  // 获取单例
    void operator()(const std::string &format); // 重载()运算符

private:
    Log();                       // private 构造函数
    ~Log();                      // private 析构函数
    Log(const Log &);            // 阻止复制
    Log &operator=(const Log &); // 阻止赋值
    char *data = nullptr;
    char *curr = nullptr;
    int page = 0;
};

extern Log &log;

} // namespace ns_program

#endif // PROGRAM_LOG_H_