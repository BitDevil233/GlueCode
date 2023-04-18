#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <nlohmann/json.hpp>
using namespace std;

// 用于表示一个函数的参数列表
struct FuncParam {
    string type;
    string name;
};

// 用于表示一个函数的所有信息
struct FuncInfo {
    string returnType;
    string name;
    vector<FuncParam> params;
};

// 根据头文件中的函数声明提取出所有函数的信息
vector<FuncInfo> extractFuncInfo(const string& filename) {
    vector<FuncInfo> funcInfos;

    // 打开头文件
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file " << filename << endl;
        return funcInfos;
    }

    // 匹配函数声明的正则表达式
    regex funcRegex("\\s*(\\w+)\\s+(\\w+)\\s*\\((.*)\\)\\s*;");

    // 逐行读取文件内容
    string line;
    while (getline(file, line)) {
        // 匹配函数声明
        smatch match;
        if (regex_match(line, match, funcRegex)) {
            // 解析函数返回值、函数名和参数列表
            FuncInfo funcInfo;
            funcInfo.returnType = match[1].str();
            funcInfo.name = match[2].str();
            string paramsStr = match[3].str();
            regex paramRegex("\\s*(\\w+)\\s+(\\w+)\\s*");
            auto paramsBegin = sregex_iterator(paramsStr.begin(), paramsStr.end(), paramRegex);
            auto paramsEnd = sregex_iterator();
            for (sregex_iterator i = paramsBegin; i != paramsEnd; ++i) {
                FuncParam param;
                param.type = (*i)[1].str();
                param.name = (*i)[2].str();
                funcInfo.params.push_back(param);
            }
            // 将函数信息添加到列表中
            funcInfos.push_back(funcInfo);
        }
    }

    // 关闭文件并返回函数信息列表
    file.close();
    return funcInfos;
}

// 生成将函数参数转换为Json的代码
string generateSerializeCode(const FuncInfo& funcInfo) {
    string code = "void " + funcInfo.name + "_serializer(Json::Value& json";
    for (const auto& param : funcInfo.params) {
        code += ", " + param.type + " " + param.name;
    }
    code += ") {\n";
    code += "    json[\"__function_name__\"] = \"" + funcInfo.name + "\";\n";
    for (const auto& param : funcInfo.params) {
        code += "    json[\"" + param.name + "\"] = " + "Json::Value(" + param.name + ");\n";
    }
    code += "}\n\n";    
    return code;
}

// 生成将函数调用转换为Json的代码
string generateCallCode(const FuncInfo& funcInfo) {
    string code = "Json::Value " + funcInfo.name + "_caller(";
    for (int i = 0; i < funcInfo.params.size(); ++i) {
        const auto& param = funcInfo.params[i];
        code += param.type + " " + param.name;
        if (i != funcInfo.params.size() - 1) {
            code +=", ";
        }
    }
    code += ") {\n";
    code += " Json::Value json;\n";
    code += " " + funcInfo.name + "_serializer(json";
    for (const auto& param : funcInfo.params) {
        code += ", " + param.name;
    }
    code += ");\n";
    code += " return json;\n";
    code += "}\n\n";
    return code;
}

// 生成包含所有函数转换为Json的代码
string generateAllSerializeCode(const vector<FuncInfo>& funcInfos) {
    string code = "// 将函数参数转换为Json\n";
    for (const auto& funcInfo : funcInfos) {
        code += generateSerializeCode(funcInfo);
    }
    code += "\n";
    return code;
}

// 生成包含所有函数调用转换为Json的代码
string generateAllCallCode(const vector<FuncInfo>& funcInfos) {
    string code = "// 将函数调用转换为Json\n";
    for (const auto& funcInfo : funcInfos) {
        code += generateCallCode(funcInfo);
    }
    code += "\n";
    return code;
}

int main() {
    // 提取头文件中的函数信息
    vector<FuncInfo> funcInfos = extractFuncInfo("../test/test.h");
    // 生成将所有函数参数转换为Json的代码
    string serializeCode = generateAllSerializeCode(funcInfos);

    // 生成将所有函数调用转换为Json的代码
    string callCode = generateAllCallCode(funcInfos);

    // 输出生成的代码
    cout << serializeCode << endl;
    cout << callCode << endl;
    return 0;
}
