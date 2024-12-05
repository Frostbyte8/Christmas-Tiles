#include <fstream>
#include <streambuf>
#include <string>

int main(int argc, char* argv[]) {

    if(argc < 3) {
        printf("Not enough args.");
        return 0;
    }

    std::ifstream file1(argv[1], std::ios::in | std::ios::binary);
    std::ifstream file2(argv[2], std::ios::in | std::ios::binary);

    if(!file1 || !file2) {
        printf("File not found.");
        return 0;
    }
    
    file1.seekg(0, std::ios::end);
    file2.seekg(0, std::ios::end);

    std::string buf1;
    std::string buf2;

    buf1.resize(file1.tellg());
    buf2.resize(file2.tellg());

    file1.seekg(0, std::ios::beg);
    file2.seekg(0, std::ios::beg);

    file1.read(&buf1[0], buf1.size());
    file2.read(&buf2[0], buf2.size());

    file1.close();
    file2.close();

    size_t prevOffset1 = 0;
    size_t prevOffset2 = 0;
    for(;;) {

        size_t offset1 = buf1.find_first_of("\r\n", prevOffset1);
        size_t offset2 = buf2.find_first_of("\r\n", prevOffset2);

        if(offset1 == std::string::npos) {
            if(prevOffset1 < buf1.size()) {
                offset1 = buf1.size();
            }
            else {
                break;
            }
        }

        if(offset2 == std::string::npos) {
            if(prevOffset2 < buf2.size()) {
                offset2 = buf2.size();
            }
            else {
                break;
            }
        }

        std::string temp1 = buf1.substr(prevOffset1, offset1 - prevOffset1);
        std::string temp2 = buf2.substr(prevOffset2, offset2 - prevOffset2);

        if(temp1.compare(temp2) != 0) {
            printf("%s\n", temp1.c_str());
        }

        prevOffset1 = offset1 + 2;
        prevOffset2 = offset2 + 2;

    }   

    return 0;
}