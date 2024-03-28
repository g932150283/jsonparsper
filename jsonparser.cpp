#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>
namespace json
{

    // 定义Node结构前向声明。
    struct Node;

    // 定义空类型。
    using Null = std::monostate;

    // 定义布尔类型。
    using Bool = bool;

    // 定义整数类型。
    using Int = int64_t;

    // 定义浮点数类型。
    using Float = double;

    // 定义字符串类型。
    using String = std::string;

    // 定义数组类型，其中每个元素都是Node类型。
    using Array = std::vector<Node>;

    // 定义对象类型，是一个键值对集合，键为字符串，值为Node类型。
    using Object = std::map<std::string, Node>;

    // 定义Value为以上类型的变体，能够存储任何一种类型的值。
    using Value = std::variant<Null, Bool, Int, Float, String, Array, Object>;

    /**
     * @brief Node结构用于存储不同类型的数据，支持空类型、布尔、整数、浮点数、字符串、数组和对象。
     */
    struct Node
    {
        Value value; ///< 存储节点值的变体。

        /**
         * @brief 构造函数，初始化Node为特定值。
         *
         * @param _value 要存储在节点中的值。
         */
        Node(Value _value) : value(_value) {}

        /**
         * @brief 默认构造函数，初始化Node为Null。
         */
        Node() : value(Null{}) {}

        /**
         * @brief 通过键访问对象类型的值。
         *
         * @param key 对象中的键。
         * @return 对象中键对应的值的引用。
         * @throws std::runtime_error 如果当前Node不是对象类型。
         */
        auto &operator[](const std::string &key)
        {
            if (auto object = std::get_if<Object>(&value))
            {
                return (*object)[key];
            }
            throw std::runtime_error("not an object");
        }

        /**
         * @brief 通过索引访问数组类型的值。
         *
         * @param index 数组中的索引。
         * @return 数组中索引对应的值的副本。
         * @throws std::runtime_error 如果当前Node不是数组类型。
         */
        auto operator[](size_t index)
        {
            if (auto array = std::get_if<Array>(&value))
            {
                return array->at(index);
            }
            throw std::runtime_error("not an array");
        }

        /**
         * @brief 向数组类型的Node添加一个新的Node元素。
         *
         * @param rhs 要添加到数组中的Node。
         * @throws std::runtime_error 如果当前Node不是数组类型。
         */
        void push(const Node &rhs)
        {
            if (auto array = std::get_if<Array>(&value))
            {
                array->push_back(rhs);
            }
        }
    };

    struct JsonParser
    {
        /* 解析 JSON 字符串的视图 */
        std::string_view json_str;
        /* 当前解析位置 */
        size_t pos = 0;

        /* 函数名称: parse_whitespace
         * 功能描述: 跳过 JSON 字符串中的空白字符，包括空格、制表符等。
         * 参数:
         *     - 无（使用成员变量 json_str 和 pos）
         * 返回值:
         *     - void（不返回任何值，直接修改 pos 的值）
         */
        void parse_whitespace()
        {
            // 循环条件：当前位置小于 JSON 字符串的长度，并且当前字符是空白字符
            while (pos < json_str.size() && std::isspace(json_str[pos]))
            {
                ++pos; // 如果是空白字符，则 pos 自增，跳过该字符
            }
        }

        /* 函数名称: parse_null
         * 功能描述: 从当前位置开始解析 JSON 字符串中的 null 值。
         * 参数:
         *     - 无（使用成员变量 json_str 和 pos）
         * 返回值:
         *     - std::optional<Value>（解析成功时返回 Null 类型的值，失败时返回空 optional）
         */
        auto parse_null() -> std::optional<Value>
        {
            // 判断从当前位置开始的 4 个字符是否为 "null"
            if (json_str.substr(pos, 4) == "null")
            {
                pos += 4;      // 如果是，则 pos 增加 4，跳过 "null"
                return Null{}; // 并返回 Null 类型的值
            }
            return {}; // 如果不是 "null"，返回空 optional
        }

        /* 函数名称: parse_true
         * 功能描述: 从当前位置开始解析 JSON 字符串中的 true 值。
         * 参数:
         *     - 无（使用成员变量 json_str 和 pos）
         * 返回值:
         *     - std::optional<Value>（解析成功时返回 true，失败时返回空 optional）
         */
        auto parse_true() -> std::optional<Value>
        {
            // 判断从当前位置开始的 4 个字符是否为 "true"
            if (json_str.substr(pos, 4) == "true")
            {
                pos += 4;    // 如果是，则 pos 增加 4，跳过 "true"
                return true; // 并返回 true
            }
            return {}; // 如果不是 "true"，返回空 optional
        }

        /* 函数名称: parse_false
         * 功能描述: 从当前位置开始解析 JSON 字符串中的 false 值。
         * 参数:
         *     - 无（使用成员变量 json_str 和 pos）
         * 返回值:
         *     - std::optional<Value>（解析成功时返回 false，失败时返回空 optional）
         */
        auto parse_false() -> std::optional<Value>
        {
            // 判断从当前位置开始的 5 个字符是否为 "false"
            if (json_str.substr(pos, 5) == "false")
            {
                pos += 5;     // 如果是，则 pos 增加 5，跳过 "false"
                return false; // 并返回 false
            }
            return {}; // 如果不是 "false"，返回空 optional
        }

        /**
         * 函数名称: parse_number
         * 功能描述:
         *     解析 JSON 字符串中的数字。这个函数尝试从当前位置（pos）开始解析一个数字，
         *     并根据数字的格式（整数或浮点数）来返回相应的值。如果遇到非数字字符，则停止解析。
         *     函数支持识别标准的 JSON 数字格式，包括指数表示法。
         * 参数: 无
         * 返回值:
         *     std::optional<Value> - 如果解析成功，返回一个包含数字（整数或浮点数）的 Value 对象；
         *                            如果解析失败或遇到异常，则返回一个空的 optional 对象。
         */
        auto parse_number() -> std::optional<Value>
        {
            size_t endpos = pos; // 初始化结束位置为当前解析的起始位置
            // 循环查找数字的结束位置，包括数字、指数表示符'e'和小数点
            while (endpos < json_str.size() && (std::isdigit(json_str[endpos]) ||
                                                json_str[endpos] == 'e' ||
                                                json_str[endpos] == '.'))
            {
                endpos++; // 移动结束位置指针
            }
            // 根据起始位置和结束位置截取数字字符串
            std::string number = std::string{json_str.substr(pos, endpos - pos)};
            pos = endpos; // 更新解析器的当前位置为数字的结束位置

            // 定义一个 lambda 函数，用于判断字符串是否表示浮点数（包含'.'或'e'）
            static auto is_Float = [](std::string &number)
            {
                return number.find('.') != number.npos || number.find('e') != number.npos;
            };

            if (is_Float(number)) // 如果是浮点数
            {
                try
                {
                    Float ret = std::stod(number); // 尝试将字符串转换为浮点数
                    return ret;                    // 返回转换后的浮点数
                }
                catch (...) // 捕获所有类型的异常
                {
                    return {}; // 发生异常，返回空的 optional 对象
                }
            }
            else // 如果是整数
            {
                try
                {
                    Int ret = std::stoi(number); // 尝试将字符串转换为整数
                    return ret;                  // 返回转换后的整数
                }
                catch (...) // 捕获所有类型的异常
                {
                    return {}; // 发生异常，返回空的 optional 对象
                }
            }
        }

        /**
         * 函数名称: parse_string
         * 功能描述:
         *     从当前位置开始解析 JSON 字符串中的字符串值。这个函数寻找以双引号(")开始和结束的文本，
         *     并将其内容作为字符串值返回。
         * 参数: 无
         * 返回值:
         *     std::optional<Value> - 如果成功解析字符串，返回包含该字符串的 Value 对象；
         *                            如果失败，则返回空的 optional 对象。
         */
        auto parse_string() -> std::optional<Value>
        {
            pos++; // 跳过开始的双引号(")
            size_t endpos = pos;
            // 查找字符串的结束双引号(")，标记字符串结束位置
            while (pos < json_str.size() && json_str[endpos] != '"')
            {
                endpos++; // 移动结束位置指针
            }
            // 根据起始位置和结束位置截取字符串
            std::string str = std::string{json_str.substr(pos, endpos - pos)};
            pos = endpos + 1; // 更新解析器位置到结束双引号之后
            return str;       // 返回解析得到的字符串
        }

        /**
         * 函数名称: parse_array
         * 功能描述:
         *     从当前位置开始解析 JSON 字符串中的数组。这个函数处理以方括号([)开始和结束的数组，
         *     解析数组中的每个元素，并将它们作为一个 Array 返回。
         * 参数: 无
         * 返回值:
         *     std::optional<Value> - 如果成功解析数组，返回包含数组元素的 Value 对象（一个 Array 类型）；
         *                            如果解析过程中遇到任何错误，则返回空的 optional 对象。
         */
        auto parse_array() -> std::optional<Value>
        {
            pos++;     // 跳过开始的方括号([)
            Array arr; // 初始化空数组
            // 循环解析数组元素，直到遇到结束的方括号(])
            while (pos < json_str.size() && json_str[pos] != ']')
            {
                auto value = parse_value();   // 解析当前元素
                arr.push_back(value.value()); // 将解析得到的元素加入数组中
                parse_whitespace();           // 解析并跳过任何空白字符
                if (pos < json_str.size() && json_str[pos] == ',')
                {
                    pos++; // 跳过元素间的逗号(,)
                }
                parse_whitespace(); // 再次解析并跳过任何空白字符
            }
            pos++;      // 跳过结束的方括号(])
            return arr; // 返回解析得到的数组
        }

        /**
         * 函数名称: parse_object
         * 功能描述:
         *     从当前位置开始解析 JSON 字符串中的对象。这个函数处理以大括号({)开始和结束的对象，
         *     解析对象中的每个键值对，并将它们作为一个 Object 返回。
         * 参数: 无
         * 返回值:
         *     std::optional<Value> - 如果成功解析对象，返回包含对象键值对的 Value 对象（一个 Object 类型）；
         *                            如果解析过程中遇到任何错误，例如键不是字符串类型或格式不正确，则返回空的 optional 对象。
         */
        auto parse_object() -> std::optional<Value>
        {
            pos++;      // 跳过开始的大括号({
            Object obj; // 初始化空对象
            // 循环解析键值对，直到遇到结束的大括号(})
            while (pos < json_str.size() && json_str[pos] != '}')
            {
                auto key = parse_value(); // 解析键
                parse_whitespace();       // 解析并跳过任何空白字符
                // 检查解析得到的键是否为字符串类型
                if (!std::holds_alternative<String>(key.value()))
                {
                    return {}; // 如果键不是字符串，返回空的 optional 对象
                }
                // 确认键值对中的分隔符为冒号(:)
                if (pos < json_str.size() && json_str[pos] == ':')
                {
                    pos++; // 跳过冒号(:)
                }
                parse_whitespace();       // 解析并跳过任何空白字符
                auto val = parse_value(); // 解析值
                // 将键和值添加到对象中
                obj[std::get<String>(key.value())] = val.value();
                parse_whitespace(); // 解析并跳过任何空白字符
                // 如果遇到逗号(,)，表示后面还有键值对
                if (pos < json_str.size() && json_str[pos] == ',')
                {
                    pos++; // 跳过逗号(,)
                }
                parse_whitespace(); // 再次解析并跳过任何空白字符
            }
            pos++;      // 跳过结束的大括号(})
            return obj; // 返回解析得到的对象
        }

        /**
         * 函数名称: parse_value
         * 功能描述:
         *     根据当前解析位置的字符决定并调用相应的解析函数。该函数是一个分发中心，
         *     根据当前字符来解析 JSON 支持的不同类型的值：null, true, false, 数字, 字符串, 数组, 和对象。
         * 参数: 无
         * 返回值:
         *     std::optional<Value> - 根据解析到的值的类型返回一个相应的 Value 对象；
         *                            如果没有匹配的类型或解析过程中发生错误，则返回空的 optional 对象。
         */
        auto parse_value() -> std::optional<Value>
        {
            parse_whitespace(); // 首先解析并跳过任何空白字符
            // 根据当前字符决定下一步解析哪种类型的值
            switch (json_str[pos])
            {
            case 'n': // 如果是 'n'，尝试解析 null
                return parse_null();
            case 't': // 如果是 't'，尝试解析 true
                return parse_true();
            case 'f': // 如果是 'f'，尝试解析 false
                return parse_false();
            case '"': // 如果是 '"'，解析字符串
                return parse_string();
            case '[': // 如果是 '['，解析数组
                return parse_array();
            case '{': // 如果是 '{'，解析对象
                return parse_object();
            default: // 默认情况下，尝试解析数字
                return parse_number();
            }
        }

        /**
         * 函数名称: parse
         * 功能描述:
         *     这是解析 JSON 文本的高层次入口点函数。它首先跳过任何前导的空白字符，
         *     然后调用 parse_value 来解析 JSON 文本中的第一个值。解析成功后，这个值被用来构造一个 Node 对象。
         * 参数: 无
         * 返回值:
         *     std::optional<Node> - 如果解析成功，返回一个包含解析到的值的 Node 对象；
         *                            如果解析过程中遇到错误，则返回空的 optional 对象。
         */
        auto parse() -> std::optional<Node>
        {
            parse_whitespace();         // 解析并跳过 JSON 文本前的任何空白字符
            auto value = parse_value(); // 尝试解析 JSON 文本中的值
            if (!value)                 // 如果解析值失败
            {
                return {}; // 返回空的 optional 对象，表示解析失败
            }
            return Node{*value}; // 使用解析到的值构造一个 Node 对象并返回
        }
    };

    /*
     * 函数名: parser
     * 参数: json_str - 包含 JSON 数据的 std::string_view
     * 返回值: std::optional<Node>，一个可能包含解析后 JSON 数据的节点的可选对象
     * 描述: 此函数接受一个 JSON 字符串，并尝试解析它。如果解析成功，则返回一个包含解析结果的节点；如果解析失败，则返回空的 std::optional。
     */
    auto parser(std::string_view json_str) -> std::optional<Node>
    {
        // 创建 JsonParser 对象，并传入要解析的 JSON 字符串
        JsonParser p{json_str};

        // 调用 JsonParser 的 parse() 方法进行解析，并返回解析结果
        return p.parse();
    }

    /*
     * 类名: JsonGenerator
     * 描述: 此类提供了生成 JSON 字符串的方法，根据输入的节点生成相应的 JSON 格式字符串。
     */
    class JsonGenerator
    {
    public:
        /*
         * 函数名: generate
         * 参数: node - 要生成 JSON 字符串的节点
         * 返回值: 一个字符串，包含了生成的 JSON 数据
         * 描述: 根据节点生成相应的 JSON 字符串。支持各种类型的节点，如 Null、Bool、Int、Float、String、Array 和 Object。
         */
        static auto generate(const Node &node) -> std::string
        {
            // 使用 std::visit 遍历节点的值，并根据值的类型选择适当的生成函数
            return std::visit(
                [](auto &&arg) -> std::string // 使用 Lambda 表达式处理不同类型的节点值
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, Null>)
                    {
                        return "null"; // 如果节点值是 Null 类型，返回 "null"
                    }
                    else if constexpr (std::is_same_v<T, Bool>)
                    {
                        return arg ? "true" : "false"; // 如果节点值是 Bool 类型，返回 "true" 或 "false"
                    }
                    else if constexpr (std::is_same_v<T, Int>)
                    {
                        return std::to_string(arg); // 如果节点值是 Int 类型，将其转换为字符串并返回
                    }
                    else if constexpr (std::is_same_v<T, Float>)
                    {
                        return std::to_string(arg); // 如果节点值是 Float 类型，将其转换为字符串并返回
                    }
                    else if constexpr (std::is_same_v<T, String>)
                    {
                        return generate_string(arg); // 如果节点值是 String 类型，调用 generate_string 函数生成 JSON 字符串
                    }
                    else if constexpr (std::is_same_v<T, Array>)
                    {
                        return generate_array(arg); // 如果节点值是 Array 类型，调用 generate_array 函数生成 JSON 数组字符串
                    }
                    else if constexpr (std::is_same_v<T, Object>)
                    {
                        return generate_object(arg); // 如果节点值是 Object 类型，调用 generate_object 函数生成 JSON 对象字符串
                    }
                },
                node.value);
        }

        /*
         * 函数名: generate_string
         * 参数: str - 要生成 JSON 字符串的字符串
         * 返回值: 一个字符串，包含了生成的 JSON 字符串
         * 描述: 根据输入的字符串生成相应的 JSON 字符串。字符串会被包裹在双引号中。
         */
        static auto generate_string(const String &str) -> std::string
        {
            std::string json_str = "\""; // 在字符串前后添加双引号
            json_str += str;             // 将输入的字符串追加到 JSON 字符串中
            json_str += '"';
            return json_str;
        }

        /*
         * 函数名: generate_array
         * 参数: array - 要生成 JSON 字符串的数组
         * 返回值: 一个字符串，包含了生成的 JSON 数组字符串
         * 描述: 根据输入的数组生成相应的 JSON 数组字符串。数组中的每个元素都会被递归生成其对应的 JSON 字符串。
         */
        static auto generate_array(const Array &array) -> std::string
        {
            std::string json_str = "["; // 在数组前添加左方括号
            for (const auto &node : array)
            {
                json_str += generate(node); // 对数组中的每个元素递归调用 generate 函数生成 JSON 字符串，并追加到结果字符串中
                json_str += ',';            // 在元素之间添加逗号分隔
            }
            if (!array.empty())
                json_str.pop_back(); // 移除最后一个元素后面的逗号
            json_str += ']';         // 在数组后添加右方括号
            return json_str;
        }

        /*
         * 函数名: generate_object
         * 参数: object - 要生成 JSON 字符串的对象
         * 返回值: 一个字符串，包含了生成的 JSON 对象字符串
         * 描述: 根据输入的对象生成相应的 JSON 对象字符串。对象中的每个键值对都会被递归生成其对应的 JSON 字符串。
         */
        static auto generate_object(const Object &object) -> std::string
        {
            std::string json_str = "{"; // 在对象前添加左花括号
            for (const auto &[key, node] : object)
            {
                json_str += generate_string(key); // 生成键的 JSON 字符串，并追加到结果字符串中
                json_str += ':';                  // 键值对之间添加冒号分隔
                json_str += generate(node);       // 递归调用 generate 函数生成值的 JSON 字符串，并追加到结果字符串中
                json_str += ',';                  // 键值对之间添加逗号分隔
            }
            if (!object.empty())
                json_str.pop_back(); // 移除最后一个键值对后面的逗号
            json_str += '}';         // 在对象后添加右花括号
            return json_str;
        }
    };

    /*
     * 函数名: generate
     * 参数: node - 要生成 JSON 字符串的节点
     * 返回值: 一个字符串，包含了生成的 JSON 数据
     * 描述: 调用 JsonGenerator 类的静态方法 generate，生成给定节点的 JSON 字符串，并返回结果。
     */
    inline auto generate(const Node &node) -> std::string { return JsonGenerator::generate(node); }

    /*
     * 函数名: operator<<
     * 参数: out - 输出流对象，用于打印 JSON 字符串
     *       t - 要打印的节点
     * 返回值: 一个输出流对象的引用，用于支持连续的流操作
     * 描述: 重载输出流操作符<<，以便能够通过输出流打印给定节点的 JSON 字符串表示。调用 JsonGenerator 类的静态方法 generate，生成节点的 JSON 字符串，并将其写入到输出流 out 中，然后返回输出流 out 的引用，以支持连续的流操作。
     */
    auto operator<<(std::ostream &out, const Node &t) -> std::ostream &
    {
        out << JsonGenerator::generate(t);
        return out;
    }

}
using namespace json; // 使用 json 命名空间

int main()
{
    // 打开文件并读取 JSON 数据
    std::ifstream fin("json.txt");
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string s{ss.str()}; // 将文件内容转换为字符串

    // 解析 JSON 字符串并获取节点
    auto x = parser(s).value(); // 解析 JSON 字符串，并获取解析结果的有效节点

    // 打印解析结果
    std::cout << x << "\n"; // 打印解析后的 JSON 数据

    // 修改 JSON 节点数据
    x["configurations"].push({true});   // 在 configurations 数组中添加一个布尔类型节点
    x["configurations"].push({Null{}}); // 在 configurations 数组中添加一个空节点
    x["version"] = {114514LL};          // 将 version 键对应的值修改为长整型数值 114514

    // 再次打印修改后的 JSON 数据
    std::cout << x << "\n"; // 打印修改后的 JSON 数据

    return 0;
}
