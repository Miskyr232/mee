#include <string>
class Text{
public:
    static std::string padRight(const std::string& str, int target){
        if(str.size() >= target){
            return str;
        }
        return str + std::string(target - str.size(), ' ');
    }
    static std::string padLeft(const std::string& str, int target){
        if(str.size() >= target){
            return str;
        }
        return std::string(target - str.size(), ' ') + str;
    }
    static std::string padCenter(const std::string& str, int target){
        if(str.size() >= target){
            return str;
        }
        return std::string((target - str.size()) / 2, ' ') + str + std::string((target - str.size()) / 2, ' ');
    }
    static std::string repeat(const std::string& str, int target){
        std::string result = "";
        while(target > 0){
            result += str;
            target -= str.size();
        }
        return result;
    }
    static std::string trimFloat(float num, int precision){
        char buf[32];
        std::snprintf(buf, sizeof buf, "%.*f", precision, num);
        std::string str(buf);
        str.erase(str.find_last_not_of('0') + 1);
        if(str.back() == '.')
            str.pop_back();
        if(str == "-0")
            str = "0";
        return str;
    }
};
