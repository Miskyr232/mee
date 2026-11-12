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
    static std::string remove_trailing_zeros(float num, int precision)  {
        std::string str = std::to_string(num);
        str.erase(str.find('.') + precision + 1);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos );
        if (str.back() == '.')
            str.pop_back();

        return str;
    }
};
