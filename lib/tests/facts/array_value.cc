#include <gmock/gmock.h>
#include <facter/facts/array_value.hpp>
#include <facter/facts/scalar_value.hpp>
#include <rapidjson/document.h>
#include <yaml-cpp/yaml.h>
#include <sstream>

using namespace std;
using namespace facter::facts;
using namespace rapidjson;
using namespace YAML;

TEST(facter_facts_array_value, default_constructor) {
    array_value value;
    ASSERT_EQ(0u, value.elements().size());
}

TEST(facter_facts_array_value, vector_constructor) {
    vector<unique_ptr<value>> subelements;
    subelements.emplace_back(make_value<string_value>("child"));

    vector<unique_ptr<value>> elements;
    elements.emplace_back(make_value<string_value>("1"));
    elements.emplace_back(make_value<integer_value>(2));
    elements.emplace_back(make_value<array_value>(move(subelements)));

    array_value value(move(elements));
    ASSERT_EQ(3u, value.elements().size());

    auto str = value.get<string_value>(0);
    ASSERT_NE(nullptr, str);
    ASSERT_EQ("1", str->value());

    auto integer = value.get<integer_value>(1);
    ASSERT_NE(nullptr, integer);
    ASSERT_EQ(2, integer->value());

    auto array = value.get<array_value>(2);
    ASSERT_NE(nullptr, array);
    ASSERT_EQ(1u, array->elements().size());

    str = array->get<string_value>(0);
    ASSERT_NE(nullptr, str);
    ASSERT_EQ("child", str->value());
}

TEST(facter_facts_array_value, to_json) {
    vector<unique_ptr<value>> subelements;
    subelements.emplace_back(make_value<string_value>("child"));

    vector<unique_ptr<value>> elements;
    elements.emplace_back(make_value<string_value>("1"));
    elements.emplace_back(make_value<integer_value>(2));
    elements.emplace_back(make_value<array_value>(move(subelements)));

    array_value value(move(elements));

    rapidjson::Value json_value;
    MemoryPoolAllocator<> allocator;
    value.to_json(allocator, json_value);
    ASSERT_TRUE(json_value.IsArray());
    ASSERT_EQ(3u, json_value.Size());

    ASSERT_TRUE(json_value[0u].IsString());
    ASSERT_EQ("1", string(json_value[0u].GetString()));

    ASSERT_TRUE(json_value[1u].IsNumber());
    ASSERT_EQ(2ll, json_value[1u].GetInt64());

    ASSERT_TRUE(json_value[2u].IsArray());
    ASSERT_EQ(1u, json_value[2u].Size());
    ASSERT_TRUE(json_value[2u][0u].IsString());
    ASSERT_EQ("child", string(json_value[2u][0u].GetString()));
}

TEST(facter_facts_array_value, insertion_operator) {
    vector<unique_ptr<value>> subelements;
    subelements.emplace_back(make_value<string_value>("child"));

    vector<unique_ptr<value>> elements;
    elements.emplace_back(make_value<string_value>("1"));
    elements.emplace_back(make_value<integer_value>(2));
    elements.emplace_back(make_value<array_value>(move(subelements)));

    array_value value(move(elements));

    ostringstream stream;
    stream << value;
    ASSERT_EQ("[ 1, 2, [ child ] ]", stream.str());
}

TEST(facter_facts_array_value, yaml_insertion_operator) {
    vector<unique_ptr<value>> subelements;
    subelements.emplace_back(make_value<string_value>("child"));

    vector<unique_ptr<value>> elements;
    elements.emplace_back(make_value<string_value>("1"));
    elements.emplace_back(make_value<integer_value>(2));
    elements.emplace_back(make_value<array_value>(move(subelements)));

    array_value value(move(elements));

    Emitter emitter;
    emitter << value;
    ASSERT_EQ("- \"1\"\n- 2\n-\n  - \"child\"", string(emitter.c_str()));
}
