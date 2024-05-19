/**
 * @file ini.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief ini ファイルのパースを行う @ref Utility::Ini クラスの定義ヘッダー
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_INI_HPP_
#define _UTILITY_INI_HPP_

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>

class Ini final
{

public:

    /**
     * @fn Ini
     * @brief コンストラクタ
     * 
     * @param std::string file_path iniファイルへのパス 
     */
    Ini(const std::string &file_path)
     :  file_path_(std::string(file_path.c_str())), field_separator_('='), comment_prefix_list_({"#", ";"})
    {}

    /**
     * @fn set_field_separator
     * @brief フィールド取得時の区切り文字の変更メソッド
     * @param char field_separator 区切り文字
     * @return Ini& 区切り文字変更後のパーサインスタンス
     * @note インスタンス生成時のデフォルト区切り文字は'='としている。
     */
    Ini& set_field_separator(const char field_separator)
    {
        field_separator_ = field_separator;
        return *this;
    }

    /**
     * @fn set_comment_prefix_list
     * @brief コメント行の先頭文字列指定メソッド
     * @param std::vector<std::string> コメント行の先頭文字列(複数指定可能)
     * @return Ini& コメント行の先頭文字列変更後のパーサインスタンス
     * @note インスタンス生成時のデフォルトコメント行の先頭文字列は{ "#", ";" }としている。
     */
    Ini& set_comment_prefix_list(const std::vector<std::string>& comment_prefix_list)
    {
        comment_prefix_list_ = comment_prefix_list;
        return *this;
    }

    /**
     * @fn read_bool
     * @brief 真理値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param bool default_value 読取失敗時のデフォルト値
     * @return bool 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     * @note 真理値を表現文字列としては "true" もしくは "false" とする。(大文字/小文字は問わない。)
     */
    bool read_bool(const std::string& section_name, const std::string& field_name, const bool& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;

        std::transform(str.begin(), str.end(), str.begin(), [](const char c){ 
                return static_cast<char>(::toupper(c)); 
            }
        );

        if (str == "TRUE" || str == "1")
            return true;
        else if (str == "FALSE" || str == "0")
            return false;
        else
            return default_value;
    }

    /**
     * @fn read_int
     * @brief 整数値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param int default_value 読取失敗時のデフォルト値
     * @return int 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */
    int read_int(const std::string& section_name, const std::string& field_name, const int& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;

        if (!is_convertable_to_long(str))
            return default_value;

        try
        {
            auto value = std::stol(str);
            return value;
        }
        catch (...)
        {
            return default_value;
        }
    }

    /**
     * @fn read_double
     * @brief 浮動小数点値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param double default_value 読取失敗時のデフォルト値
     * @return double 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */ 
    double read_double(const std::string& section_name, const std::string& field_name, const double& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;
            
        try
        {
            auto value = std::stod(str);
            return value;
        }
        catch (...)
        {
            return default_value;
        }
    }

    /**
     * @fn read_str
     * @brief 文字列取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param std::string default_value 読取失敗時のデフォルト値
     * @return std::string 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */ 
    std::string read_str(const std::string& section_name, const std::string& field_name, const std::string& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;
        return std::string(str);
    }

    /**
     * @fn write_bool
     * @brief 真理値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param bool value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    bool write_bool(const std::string& section_name, const std::string& field_name, const bool &value) const
    {
        std::string result = value ? "true" : "false";
        return set_field(section_name, field_name, result);
    }

    /**
     * @fn write_int
     * @brief 整数値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param int value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    bool write_int(const std::string& section_name, const std::string& field_name, const int& value) const
    {
        std::stringstream ss;
        ss << value;
        return set_field(section_name, field_name, ss.str());
    }

    /**
     * @fn write_double
     * @brief 浮動小数点値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param double value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    bool write_double(const std::string& section_name, const std::string& field_name, const double& value) const
    {
        std::stringstream ss;
        ss << value;
        return set_field(section_name, field_name, ss.str());
    }

    /**
     * @fn write_double
     * @brief 浮動小数点値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param std::string value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    bool write_str(const std::string& section_name, const std::string& field_name, const std::string& value) const
    {
        return set_field(section_name, field_name, value);
    }

private:
    std::string file_path_;
    char field_separator_;
    std::vector<std::string> comment_prefix_list_;

    void trim(std::string& str) const
    {
        const char *whitespaces = " \t\n\r\f\v";
        auto lascurrent_tpos = str.find_last_not_of(whitespaces);
        if (lascurrent_tpos == std::string::npos)
        {
            str.clear();
            return;
        }
        str.erase(lascurrent_tpos + 1);
        str.erase(0, str.find_first_not_of(whitespaces));
    }

    bool is_convertable_to_long(const std::string& value) const
    {
        char *endptr;
        long result;
        result = std::strtol(value.c_str(), &endptr, 10);
        if (*endptr == '\0')
            return true;
        result = std::strtol(value.c_str(), &endptr, 8);
        if (*endptr == '\0')
            return true;
        result = std::strtol(value.c_str(), &endptr, 16);
        if (*endptr == '\0')
            return true;
        return false;
    }

    bool is_ignore_line(const std::string& line) const
    {
        if (line.size() == 0)
            return true;

        for (const auto &prefix : comment_prefix_list_)
        {
            std::size_t pos = line.find(prefix);
            if (pos == 0)
                return true;
        }

        return false;
    }

    bool try_get_field(const std::string& target_section, const std::string& target_field, std::string& value) const
    {
        std::stringstream iss;
        std::ifstream ifs(file_path_);
        iss << ifs.rdbuf();

        int line_num = 0;
        std::string current_section = "";
        std::string line;

        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++line_num;

            if (is_ignore_line(line))
                continue;

            if (line[0] == '[')
            {
                std::size_t pos = line.find("]");
                if (pos == std::string::npos)
                    return false;
                if (pos == 1)
                    return false;
                current_section = line.substr(1, pos - 1);
            }
            else
            {
                if (current_section == "")
                    return false;

                std::size_t pos = line.find(field_separator_);

                if (pos == std::string::npos)
                {
                    return false;
                }
                else
                {
                    std::string current_field = line.substr(0, pos);
                    trim(current_field);
                    std::string current_value = line.substr(pos + 1, std::string::npos);
                    trim(current_value);

                    if (current_section == target_section && current_field == target_field)
                    {
                        value = current_value;
                        return (value.size() != 0) ? true : false;
                    }
                }
            }
        }
        return false;
    }

    bool set_field(const std::string& target_section, const std::string& target_field, const std::string& value) const
    {
        std::stringstream iss, oss;
        std::ifstream ifs(file_path_);
        iss << ifs.rdbuf();

        enum struct FoundMode { NO_MATCH, SECTION_MATCH, BOTH_MATCH };
        FoundMode found_mode = FoundMode::NO_MATCH;

        int line_num = 0;
        std::string current_section = "";
        std::string line;

        // iterate file line by line
        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++line_num;

            if (is_ignore_line(line))
            {
                oss << line << '\n';
                continue;
            }

            if (line[0] == '[')
            {
                std::size_t pos = line.find("]");
                if (pos == std::string::npos)
                    return false;
                if (pos == 1)
                    return false;
                current_section = line.substr(1, pos - 1);

                if (current_section == target_section)
                    found_mode = FoundMode::SECTION_MATCH;
                else if (found_mode == FoundMode::SECTION_MATCH && current_section != target_section)
                {
                    line = target_field + field_separator_ + value + '\n' + line;
                    found_mode = FoundMode::BOTH_MATCH;
                }
            }
            else
            {
                if (current_section=="")
                    return false;

                std::size_t pos = line.find(field_separator_);
                if (pos==std::string::npos)
                    return false;

                else
                {
                    std::string current_field = line.substr(0, pos);
                    trim(current_field);
                    if (current_section == target_section && current_field == target_field)
                    {
                        line = std::string(target_field.c_str()) + field_separator_ + value;
                        found_mode = FoundMode::BOTH_MATCH;
                    }
                }
            }
            oss << line << '\n';
        }

        if (found_mode == FoundMode::NO_MATCH)
        {
            oss << '[' << target_section << ']' << '\n';
            oss << target_field << field_separator_ << value << '\n';
        }
        else if (found_mode == FoundMode::SECTION_MATCH)
        {
            oss << target_field << field_separator_ << value << '\n';
        }

        std::string result = oss.str();
        result.pop_back();

        std::ofstream ofs(file_path_);
        if(ofs.is_open())
            ofs << result;
        else
            return false;
        
        return true;
    }
};

#endif