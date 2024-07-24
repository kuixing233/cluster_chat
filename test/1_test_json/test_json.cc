#include "json.hpp"
using Json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;


int main()
{
    Json js; 
    js["1"] = 123;
    js["2"] = "hello";

    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    map<int, string> city;
    city.insert(make_pair(1, "黄山"));
    city.insert(make_pair(2, "华山"));
    city.insert(make_pair(3, "泰山"));

    js["num"] = vec;
    js["city"] = city;

    string jsonStr = js.dump();
    cout << js << endl;
    cout << jsonStr << endl;

    ofstream ofs("tmp.json");
    ofs << jsonStr;

    ifstream ifs("tmp.json");
    string readStr;
    ifs >> readStr;
    cout << readStr << endl;

    // Json jsonBuf = Json::parse(jsonStr);
    Json jsonBuf = Json::parse(readStr);
    // vector<int> num2 = jsonBuf["num"];
    // for (auto i : num2)
    //     cout << i << " ";
    // cout << endl;

    // map<int, string> city2 = jsonBuf["city"];
    // for (auto m : city2)
    //     cout << m.first << ": " << m.second << endl;

    return 0;
}