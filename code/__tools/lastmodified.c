#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[]) {

    FILETIME ft;
    SYSTEMTIME st;
    int i = 1;
    int newLine = 0;
    HANDLE currentFile;   

    if(argc == 1 || (argc > 1 && lstrcmpA(argv[1], "/?") == 0)) {
        //WriteConsoleA(stdOut, "Usage: lastmodified.exe [\"PathToFileInQuotes1\"] [\"PathToFileInQuotes2\"] [\"...\"]\n",
        printf("Usage: lastmodified.exe [\"PathToFileInQuotes1\"] [\"PathToFileInQuotes2\"] [\"...\"]\n");
        printf("Format is: \"file path given\" ; HH-MM-SS ; DD-MM-YY");
        return 0;
    }

    for(; i < argc; ++i) {

        currentFile = CreateFileA(argv[i], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if(currentFile != INVALID_HANDLE_VALUE) {
            if(newLine) {
                printf("\n");
            }
            else {
                newLine = 1;
            }
            GetFileTime(currentFile, NULL, NULL, &ft);
            FileTimeToSystemTime(&ft, &st);
            printf("\"%s\" ; %02d:%02d:%02d ; %02d-%02d-%02d", argv[i], st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear);
            CloseHandle(currentFile);
        }
        
    }

    

    return 0;
}

