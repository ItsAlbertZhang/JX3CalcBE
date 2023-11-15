#include "program/init.h"
#include <fstream>

using namespace ns_program;

// 静态成员变量初始化
fs::path Config::pExeDir; // 可执行文件所在目录的路径
fs::path Config::pUnpack; // 未打包的数据目录
fs::path Config::pJX3;    // 已打包的数据目录

void Init::init(int argc, char *argv[]) { // 初始化所有环境
    Config::pExeDir = fs::absolute(argv[0]).parent_path();
    Config::load();
    Config::save();
}

void Config::load() { // 加载配置文件
    fs::path pathConfig = Config::pExeDir / "config.json";
    // 若配置文件不存在则创建
    if (!fs::exists(pathConfig)) {
        std::ofstream f(pathConfig);
        f << "{}";
        f.close();
    } else {
        std::ifstream fileConfig(pathConfig);
        if (fileConfig.is_open()) {
            fileConfig.seekg(0, std::ios::end); // 定位到文件末尾
            size_t sz = fileConfig.tellg();     // 获取文件大小
            char *content = new char[sz + 1];
            fileConfig.seekg(0, std::ios::beg);
            fileConfig.read(content, sz - 1);
            fileConfig.close();
            const char *jx3 = "JX3Dir";
            const char *unpack = "UnpackDir";
            char *pos1, *pos2;
            if ((pos1 = strstr(content, jx3)) != nullptr) { // 找到 JX3Dir
                pos1 += strlen(jx3) + 1;
                if ((pos1 = strstr(pos1, "\"")) != nullptr) { // 找到第一个双引号
                    pos1++;
                    if ((pos2 = strstr(pos1, "\"")) != nullptr) { // 找到第二个双引号
                        *pos2 = '\0';
                        pJX3 = pos1;
                        *pos2 = '\"';
                    }
                }
            }
            if ((pos1 = strstr(content, unpack)) != nullptr) { // 找到 UnpackDir
                pos1 += strlen(unpack) + 1;
                if ((pos1 = strstr(pos1, "\"")) != nullptr) { // 找到第一个双引号
                    pos1++;
                    if ((pos2 = strstr(pos1, "\"")) != nullptr) { // 找到第二个双引号
                        *pos2 = '\0';
                        pUnpack = pos1;
                        *pos2 = '\"';
                    }
                }
            }
            delete[] content;
        }
    }
}

bool Config::save() {
    bool ret = false;
    fs::path pathConfig = Config::pExeDir / "config.json";
    std::ofstream fileConfig(pathConfig);
    if (fileConfig.is_open()) {
        std::string content = "{\n";
        if (!pJX3.empty()) {
            content += "    \"JX3Dir\": \"" + pJX3.string() + "\",\n";
        }
        if (!pUnpack.empty()) {
            content += "    \"UnpackDir\": \"" + pUnpack.string() + "\"\n";
        }
        content += "}\n";
        fileConfig << content;
        fileConfig.close();
        ret = true;
    }
    return ret;
}