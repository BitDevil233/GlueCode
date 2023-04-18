#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <experimental/filesystem>

using json = nlohmann::json;    // 引入JSON库的命名空间
namespace fs = std::experimental::filesystem; // 引入filesystem库的命名空间

// 定义一个结构体
struct MyStruct
{
    int x;
    std::string y;
};

// 定义一个模板函数，用于将参数转换为JSON格式
template <typename T>
json toJson(const T &value)
{
    if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string>)
    {
        // 如果参数是数字或字符串类型，则直接转换为JSON格式
        return value;
    }
    else if constexpr (std::is_same_v<T, MyStruct>)
    {
        // 如果参数是结构体类型，则递归地将结构体中的成员转换为JSON格式
        json jsonObj;
        jsonObj["x"] = toJson(value.x);
        jsonObj["y"] = toJson(value.y);
        return jsonObj;
    }
    else
    {
        // 如果参数是其他类型，则输出错误信息
        std::cerr << "Error: unsupported parameter type: " << typeid(T).name() << std::endl;
        return json(nullptr);
    }
}

// 定义一个模板函数，用于将函数的参数序列化为JSON格式
template <typename... Args>
json serializeParams(const Args &...args)
{
    // 将参数序列化为JSON格式
    json jsonObj = json::array({toJson(args)...});

    return jsonObj;
}

// 定义一个函数，用于从文件中读取内容
std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// 定义一个函数，用于从函数声明中解析参数列表
std::vector<std::string> parseParams(const std::string &decl)
{
    std::vector<std::string> params;
    std::regex re("\\((.*)\\)"); // 匹配括号中的内容
    std::smatch match;
    if (std::regex_search(decl, match, re) && match.size() > 1)
    {
        std::string paramStr = match[1].str();
        std::regex re2("(\\w+\\s*\\**)(\\w+)"); // 匹配参数类型和参数名
        std::sregex_iterator it(paramStr.begin(), paramStr.end(), re2);
        std::sregex_iterator end;
        for (; it != end; ++it)
        {
            params.push_back(it->str());
        }
    }
    return params;
}

// 定义一个函数，用于将cpp头文件中的函数的参数序列化为JSON格式
json serializeHeader(const std::string &headerFile)
{
    // 读取文件内容
    std::string content = readFile(headerFile);

    // 定义正则表达式，用于
    // 从文件内容中解析函数声明
    std::regex re("(\w+)\s+(\w+)\s*\((.*)\)"); // 匹配函数返回值、函数名和参数列表
    std::sregex_iterator it(content.begin(), content.end(), re);
    std::sregex_iterator end;
    if (it != end)
    {
        // 只处理第一个函数声明
        std::string decl = it->str();
        // 解析参数列表
        std::vector<std::string> params = parseParams(decl);

        // 将参数序列化为JSON格式
        json jsonObj = serializeParams(params);

        return jsonObj;
    }
    else
    {
        // 没有找到函数声明
        std::cerr << "Error: no function declaration found in file " << headerFile << std::endl;
        return json(nullptr);
    }
}

int main()
{
    // 定义一个结构体
    MyStruct myStruct = {1, "hello"};
    // 将结构体转换为JSON格式并输出
    std::cout << toJson(myStruct).dump() << std::endl;

    // 从cpp头文件中解析函数的参数并将参数序列化为JSON格式
    json jsonObj = serializeHeader("../test/test.h");

    // 输出JSON格式的参数
    std::cout << jsonObj.dump(4) << std::endl;

    return 0;
}

// 上面的代码首先定义了一个结构体 `MyStruct`，并定义了一个模板函数 `toJson`，用于将不同类型的参数转换为JSON格式。然后定义了一个模板函数 `serializeParams`，用于将函数的参数序列化为JSON格式。

// 接下来，定义了一个函数 `readFile`，用于从文件中读取内容，以及一个函数 `parseParams`，用于从函数声明中解析参数列表。

// 最后，定义了一个函数 `serializeHeader`，用于将cpp头文件中的函数的参数序列化为JSON格式。这个函数首先读取文件内容，然后使用正则表达式从文件内容中解析函数声明，并使用 `parseParams` 函数解析参数列表。最后，使用 `serializeParams` 函数将参数序列化为JSON格式。

// 在 `main` 函数中，我们首先将结构体 `MyStruct` 转换为JSON格式并输出。然后调用 `serializeHeader` 函数，将cpp头文件中的函数的参数序列化为JSON格式，并输出结果。
