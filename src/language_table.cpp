#include "language_table.h"
#include <stdio.h>
#include <windows.h>

LanguageTable languageTable;

static const long MAX_FILE_SIZE = 65535;

void LanguageTable::loadStrings() {
    
    FILE* fp = NULL; 
    fopen_s(&fp, "lang_en.txt", "rb");

    if(!fp) {
        return;
    }

    // Get Buffer Size

    fseek(fp, 0, SEEK_END);
    const long fileSize = ftell(fp);

    if(fileSize > MAX_FILE_SIZE || fileSize == 0) {
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_SET);

    // Read Buffer into memory

    char* charBuffer = (char *)malloc(sizeof(char) * (fileSize+1));
    fread(charBuffer, sizeof(char), fileSize, fp);
    fclose(fp);
    fp = NULL;
    charBuffer[fileSize] = 0;

    // Now to process strings
    char* bufPos = charBuffer;

    // Now convert char buffer into wchar_t
    // long wideLen = MultiByteToWideChar(CP_UTF8, 0, charBuffer, fileSize, 0, 0);
    wchar_t BAD_ID[] = L"!BAD ID!";
    const size_t BAD_LEN = wcslen(BAD_ID) + 1;
    wchar_t* NotFoundString = (wchar_t*)malloc(sizeof(wchar_t) * BAD_LEN);
    wcscpy_s(NotFoundString, BAD_LEN, BAD_ID);
    languageStrings.push_back(NotFoundString);

    languageStrings.reserve(LangID::LANG_ID_COUNT + 1);

    for(int i = LangID::APP_TITLE; i < LangID::LANG_ID_COUNT; ) {


        if(*bufPos != '#') {
            
            // Get Line length

            char* lineEnd = strstr(bufPos, "\r\n");

            if(!lineEnd) {
                break; // EOF
            }

            const long lineLen = lineEnd - bufPos;

            long wideLen = MultiByteToWideChar(CP_UTF8, 0, bufPos, lineLen, 0, 0);
            wchar_t* inStr = (wchar_t*)malloc(sizeof(wchar_t) * (wideLen+1));
            inStr[wideLen] = 0;
            MultiByteToWideChar(CP_UTF8, 0, bufPos, lineLen, inStr, wideLen);
            languageStrings.push_back(inStr);

            bufPos = lineEnd+2;
            i++;

        }
        else {
            // Skip comment line
            bufPos = strstr(bufPos, "\r\n");
            if(!bufPos) {
                break; // EOF
            }

            bufPos+=2;

        }

    }

    free(charBuffer);
    charBuffer = NULL;

}

wchar_t* GET_LANG_STR(const size_t& ID) {
    const std::vector<wchar_t*> languageStrings = languageTable.getStrings();

    if(ID > languageStrings.size()) {
        return languageStrings[LangID::ERROR_STR_NOT_FOUND];
    }

    return languageStrings[ID];
}