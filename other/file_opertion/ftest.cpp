#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <fstream>
#include <array>
#include <type_traits>


using namespace std;

enum class OpenMode : uint8_t {
    kBeg = 1 << 0,
    kEnd = 1 << 1,
    kTte = 1 << 2,
    kApd = 1 << 3,
    kTxt = 1 << 4,
    kBin = 1 << 5
};

std::string openModeToString(OpenMode mode) {
    switch (mode) {
        case OpenMode::kBeg: return "kBeg";
        case OpenMode::kEnd: return "kEnd";
        case OpenMode::kTte: return "kTte";
        case OpenMode::kApd: return "kApd";
        case OpenMode::kTxt: return "kTxt";
        case OpenMode::kBin: return "kBin";
        default: return "Unknown mode";
    }
}



class CalcTimeBase{
public:
    CalcTimeBase()
    {
        gettimeofday(&m_start, nullptr);
    }

    ~CalcTimeBase(){}

    uint64_t GetCalcTimeBase()
    {
        struct timeval end;
        gettimeofday(&end, nullptr);
        uint64_t time = (end.tv_sec - m_start.tv_sec) * 1000000 + (end.tv_usec - m_start.tv_usec);  // microseconds
        return time;
    }
private:
    struct timeval m_start;
};


// read file content
string ReadFromFile(string path)
{
    ifstream ifs(path.c_str());
    if(!ifs.is_open()) {
        cout << "open file failed" << endl;
        return "";
    }

    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    ifs.close();
    return content;
}

// write file content
void WriteTofile(string path, string content)
{
    ofstream ofs(path.c_str(), ios::out | ios::trunc);
    if(!ofs.is_open()) {
        cout << "write open file failed" << endl;
        return;
    }

    ofs << content;
    ofs.close();
}

void TestTinyCtypRW()
{
    cout << "TestTinyCtypRW" << endl;

    // clear file
    std::ofstream ofs;
    ofs.open("test.txt", std::ios::out);

    auto tmpStr = ReadFromFile("test.txt");
    cout << "read file len=" << tmpStr.size() << ",content=" << tmpStr << endl;
    
}

int main()
{
    std::cout << "sizeof(OpenMode):" << sizeof(OpenMode) << std::endl;
    std::cout << "sizeof(uint8_t):" << sizeof(uint8_t) << std::endl;

    char xcontent[] = "xsdfsf_+@333&&><sdf";
    string write_text = "";
    string text = string(xcontent);
    for (int i = 0; i < 100; i++) {
        write_text.append(text);
    }
    auto text_len = write_text.length();
    std::cout << "text_len:" << text_len << std::endl;
    char buf[text_len+1] = {0};
    memcpy(buf, write_text.data(), text_len);



    const std::array<OpenMode, 6> openModes = {
        OpenMode::kBeg,
        OpenMode::kEnd,
        OpenMode::kTte,
        OpenMode::kApd,
        OpenMode::kTxt,
        OpenMode::kBin
    };

    for(const auto& mode : openModes) {
        std::cout << "OpenMode: " << openModeToString(mode) << ":"
                //   << static_cast<std::underlying_type_t<OpenMode>>(mode) << std::endl;
                  << static_cast<std::uint16_t>(mode) << std::endl;
    }

    ReadFromFile("test.txt");
    WriteTofile("test.txt", "hello world");
    CalcTimeBase calcTime;
    uint64_t time = calcTime.GetCalcTimeBase();
    std::cout << "calc time:" << time << std::endl;


    return 0;
}


