#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

void get_digits();
bool get_comments();
int table_insert();

int state = 0, line = 0, column = 0, cnt_word = 0, cnt_char = 0;
bool notEnd = true;
string in_file_str, out_file_str;
string buffer, token;
string::iterator forward_pointer = buffer.end();
ifstream infile;
ofstream outfile, tablefile;

string words[] = {"auto", "double", "int", "struct", "break", "else", "long", "switch", "case", "enum", "register", "typedef", "char", "extern", "return", "union", "const", "float", "short", "unsigned", "continue", "for", "signed", "void", "default", "goto", "sizeof", "volatile", "do", "if", "static", "while"};
vector<string> table;

int main()
{
    set<string> keywords(words, words + 32);
    char C;
    cout << "源文件（回车默认使用in.c）：" << endl;
    getline(cin, in_file_str);

    if (in_file_str == "")
    {
        in_file_str = "in.c"; // 回车默认
    }

    infile.open(in_file_str.c_str());

    if (!infile)
    {
        cout << "无法打开源文件！" << endl;
        return -1;
    }

    cout << "目标文件（回车默认使用out.txt）：" << endl;
    getline(cin, out_file_str);

    if (out_file_str == "")
    {
        out_file_str = "out.txt"; // 回车默认
    }

    outfile.open(out_file_str.c_str());

    if (!outfile)
    {
        cout << "无法创建目标文件！" << endl;
        return -1;
    }

    tablefile.open("符号表.txt");

    if (!tablefile)
    {
        cout << "无法创建符号表！" << endl;
        return -1;
    }

    while (true)
    {
        if (forward_pointer == buffer.end())
        { // 读到行末
            if (getline(infile, buffer, '\n'))
            {                       // 读到错误或EOF则返回false
                ++line;             // 读取新行
                cnt_char += column; // 每次换行前加上当前行的字符数
                forward_pointer = buffer.begin();
                column = 1;
            }
            else
            {
                infile.close();
                outfile.close();
                int i = 1;

                for (vector<string>::iterator it = table.begin(); it != table.end(); ++it)
                {
                    tablefile << left << i++ << "\t" << *it << endl;
                }

                tablefile.close();
                cout << "总行数：" << line << endl;
                cout << "单词个数：" << cnt_word << endl;
                cout << "字符个数：" << cnt_char << endl;
                return 0;
            }
        }

        while ((forward_pointer != buffer.end()) && isspace(*forward_pointer))
        { // 未读到缓冲区结束，则一直读到非空字符
            ++forward_pointer;
            ++column;
        }

        if (forward_pointer != buffer.end())
        { // 未读到缓冲区末尾
            token = "";
            C = *forward_pointer;

            switch (C)
            {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                while ((forward_pointer != buffer.end()) && (isalnum(*forward_pointer) || *forward_pointer == '_'))
                { // 合法标识符可包含下划线
                    token.append(1, *forward_pointer++);
                    ++column;
                }

                if (keywords.count(token) == 0)
                {
                    outfile << "< ID," << table_insert() << " >" << endl;
                }
                else
                {
                    outfile << "< key," << token << " >" << endl;
                }

                ++cnt_word; // 单词数加一
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                token.append(1, C);
                state = 1; // num1
                ++forward_pointer;
                ++column;
                get_digits();                                 // 读取无符号实数剩余部分
                outfile << "< num," << token << " >" << endl; // DTB(token)
                notEnd = true;
                state = 0;
                ++cnt_word;
                break;

            case '+':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< arith-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '+')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< arith-op," << token << " >" << endl;
                    }
                    else if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< arith-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '-':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< arith-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '-')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< arith-op," << token << " >" << endl;
                    }
                    else if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else if (*forward_pointer == '>')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< ptr-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< arith-op," << token << " > " << endl;
                    }
                }

                ++cnt_word;
                break;

            case '*':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< arith-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< arith-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '/':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< arith-op," << token << " >" << endl; // 行末除号
                    ++cnt_word;
                }
                else
                {
                    if (*forward_pointer == '=')
                    { // 除法复合赋值
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                        ++cnt_word;
                    }
                    else if (*forward_pointer == '/')
                    { // 单行注释，读到行末
                        token.append(1, *forward_pointer++);
                        ++column;

                        while (forward_pointer != buffer.end())
                        {
                            token.append(1, *forward_pointer++);
                            ++column;
                        }

                        outfile << "< comments,"
                                << "- >" << endl;
                    }
                    else if (*forward_pointer == '*')
                    { // 多行注释可以换行
                        token.append(1, *forward_pointer++);
                        ++column;
                        int ret = get_comments();

                        if (ret)
                        {
                            outfile << "< comments,"
                                    << "- >" << endl;
                        }
                        else
                        {
                            infile.close();
                            outfile.close();
                            int i = 1;

                            for (vector<string>::iterator it = table.begin(); it != table.end(); ++it)
                            {
                                tablefile << left << i++ << "\t" << *it << endl;
                            }

                            tablefile.close();
                            cout << "语句行数：" << line << endl;
                            cout << "单词个数：" << cnt_word << endl;
                            cout << "字符个数：" << cnt_char << endl;
                            return 0;
                        }
                    }
                    else
                    { // 除号后是其他字符，为单个除号
                        outfile << "< arith-op," << token << " >" << endl;
                        ++cnt_word;
                    }
                }

                break;

            case '%':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< arith-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< arith-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '&':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< bit-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else if (*forward_pointer == '&')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< log-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< bit-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '|':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< bit-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else if (*forward_pointer == '|')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< log-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< bit-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '^':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< bit-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< bit-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '~':
                token.append(1, C);
                ++forward_pointer;
                ++column;
                outfile << "< bit-op," << token << " >" << endl;
                ++cnt_word;
                break;

            case '<':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< rel-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '=':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< asgn-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< asgn-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '>':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< rel-op," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '!':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if (forward_pointer == buffer.end())
                {
                    outfile << "< punct," << token << " >" << endl;
                }
                else
                {
                    if (*forward_pointer == '=')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< rel-op," << token << " >" << endl;
                    }
                    else
                    {
                        outfile << "< punct," << token << " >" << endl;
                    }
                }

                ++cnt_word;
                break;

            case '\"':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                while (true)
                {
                    while ((forward_pointer != buffer.end()) && (*forward_pointer != '\"'))
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                    }

                    if (forward_pointer == buffer.end())
                    {
                        outfile << "< Error(" << line << "," << column << "): missing terminating \" character >" << endl; //error();
                        break;
                    }
                    else if (*(forward_pointer - 1) == '\\')
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        continue;
                    }
                    else
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                        outfile << "< string,"
                                << "- >" << endl;
                        break;
                    }
                }

                ++cnt_word;
                break;

            case '\'':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                while ((forward_pointer != buffer.end()) && (*forward_pointer != '\''))
                {
                    token.append(1, *forward_pointer++);
                    ++column;
                }

                if (forward_pointer == buffer.end())
                {
                    outfile << "< Error(" << line << "," << column << "): missing terminating \' character >" << endl; //error();
                    break;
                }
                else if (*(forward_pointer - 1) == '\\')
                {
                    token.append(1, *forward_pointer++);
                    ++column;

                    if (forward_pointer != buffer.end())
                    {
                        token.append(1, *forward_pointer++);
                        ++column;
                    }
                    else
                    {
                        outfile << "< Error(" << line << "," << column << "): missing terminating \' character >" << endl; //error();
                        break;
                    }
                }
                else
                {
                    token.append(1, *forward_pointer++);
                    ++column;
                }

                outfile << "< char,"
                        << "- >" << endl;
                ++cnt_word;
                break;

            case '.':
                token.append(1, C);
                ++forward_pointer;
                ++column;

                if ((forward_pointer != buffer.end()) && isdigit(*forward_pointer))
                {
                    token.append(1, *forward_pointer++);
                    ++column;
                    state = 23;
                    get_digits();
                    notEnd = true;
                    state = 0;
                    outfile << "< num," << token << " >" << endl; // DTB(token)
                    ++cnt_word;
                    break;
                }

                outfile << "< punct," << token << " >" << endl;
                ++cnt_word;
                break;

            case '#':
            case '{':
            case '}':
            case '[':
            case ']':
            case '(':
            case ')':
            case '?':
            case ':':
            case ',':
            case ';':
            case '\\':
                token.append(1, C);
                ++forward_pointer;
                ++column;
                outfile << "< punct," << token << " >" << endl;
                ++cnt_word;
                break;

            default:
                ++forward_pointer;
                ++column;
                outfile << "< Error(" << line << "," << column << "): invalid character >" << endl; //error();
                break;
            } // end of switch
        }     // end of if
    }         // end of while

    return 0;
}

int table_insert()
{
    vector<string>::iterator it = find(table.begin(), table.end(), token);

    if (it == table.end())
    {
        table.push_back(token);
        it = find(table.begin(), table.end(), token);
    }

    return distance(table.begin(), it) + 1;
}

void get_digits() // 读取实数，包含E/e指数
{
    while ((forward_pointer != buffer.end()) && notEnd)
    {
        switch (state)
        {
        case 1:
            if (*forward_pointer == '.')
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 23;
            }
            else if (*forward_pointer == 'E' || *forward_pointer == 'e')
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 4;
            }
            else if (isdigit(*forward_pointer))
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 1;
            }
            else
            {
                notEnd = false; // 识别结束
            }

            break;

        case 23:
            if (*forward_pointer == 'E' || *forward_pointer == 'e')
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 4;
            }
            else if (isdigit(*forward_pointer))
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 23;
            }
            else
            {
                notEnd = false;
            }

            break;

        case 4:
            if (*forward_pointer == '+' || *forward_pointer == '-')
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 6;
            }
            else if (isdigit(*forward_pointer))
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 5;
            }
            else
            {
                outfile << "< Error(" << line << "," << column << "): exponent has no digits >" << endl; //error();
                notEnd = false;
            }

            break;

        case 5:
            if (isdigit(*forward_pointer))
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 5;
            }
            else
            {
                notEnd = false;
            }

            break;

        case 6:
            if (isdigit(*forward_pointer))
            {
                token.append(1, *forward_pointer++);
                ++column;
                state = 5;
            }

            break;

        default:
            break;
        }
    }
}

bool get_comments()
{
    while (true)
    {
        while ((forward_pointer != buffer.end()) && (*forward_pointer != '*'))
        {
            token.append(1, *forward_pointer++);
            ++column;
        }

        if (forward_pointer == buffer.end())
        { // 一行注释结束，继续下一行
            if (getline(infile, buffer, '\n'))
            { // 读到错误或EOF则返回false
                ++line;
                forward_pointer = buffer.begin();
                column = 1;
                continue;
            }
            else
            {
                return false;
            }
        }
        else
        { // 读到注释结束符的星号
            token.append(1, *forward_pointer++);
            ++column;

            if (forward_pointer == buffer.end())
            { // 若 '*/' 被换行分开，则继续读取下一行
                continue;
            }
            else if (*forward_pointer == '/')
            { // 注释结束符ok
                token.append(1, *forward_pointer++);
                ++column;
                return true;
            }
            else
            { // 星号后是其他字符，仍继续读取注释
                continue;
            }
        }
    }
}