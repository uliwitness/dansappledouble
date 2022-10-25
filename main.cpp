#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "ReClassicfication/InterfaceLib/FakeResources.h"

#define ENDIAN_SWAP_32(n)      (((n) & 0xFF000000UL) >> 24 | \
                                ((n) & 0x00FF0000UL) >> 8 | \
                                ((n) & 0x0000FF00UL) << 8 | \
                                ((n) & 0x000000FFUL) << 24)

#define ENDIAN_SWAP_16(n)      (((n) & 0xFF00U) >> 8 | \
                                ((n) & 0x00FFU) << 8)

enum {
    APPLE_SINGLE_MAGIC_NUMBER = 0x00051600UL,
    APPLE_DOUBLE_MAGIC_NUMBER = 0x00051607UL,
    APPLE_DOUBLE_VERSION = 0x00020000UL
};

enum EntryID {
    DATA_FORK = 1,
    RESOURCE_FORK = 2,
    REAL_NAME = 3,
    COMMENT = 4,
    ICON_BW = 5,
    ICON_COLOR = 6,
    FILE_INFO_V1 = 7,           // Old v1 file info combining FILE_DATES_INFO and MACINTOSH_FILE_INFO.
    FILE_DATES_INFO = 8,
    FINDER_INFO = 9,            // FinderInfo (16) + FinderXInfo (16)
    MACINTOSH_FILE_INFO = 10,   // 32 bits, bits 31 = protected and 32 = locked
    PRODOS_FILE_INFO = 11,
    MSDOS_FILE_INFO = 12,
    SHORT_NAME = 13,    // AFP short name.
    AFP_FILE_INFO = 14,
    DIRECTORY_ID = 15,  // AFP directory ID.
};

// Seconds since January 1, 2000 Greenwich Mean Time (GMT)
typedef int32_t AppleDoubleDateTime;

#define UNKNOWN_DATETIME 0x80000000

struct AppleDoubleEntry {
    uint32_t offset;    // from start of file.
    uint32_t length;
};

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error: Expected at least one argument, path of file to read." << endl;
        return 1;
    }

    ifstream doubleFile;
    doubleFile.open(argv[1], ios_base::in);

    if (!doubleFile.is_open()) {
        cerr << "Error: Couldn't open file " << argv[1] << endl;
        return 2;
    }

    uint32_t    magic = 0;
    uint32_t    version = 0;
    uint8_t     filler[16] = {0};
    char        fillerStr[17] = {0};
    uint16_t    entryCount = 0;

    doubleFile.read( (char*)&magic, sizeof(magic));
    magic = ENDIAN_SWAP_32(magic);
    cout << "  magic: " << setw(8) << setfill('0') << hex << magic << dec << setw(0) << endl;
    doubleFile.read((char*)&version, sizeof(version));
    version = ENDIAN_SWAP_32(version);
    cout << "version: " << setw(8) << setfill('0') << hex << version << dec << setw(0) << endl;
    doubleFile.read((char*)filler, sizeof(filler));
    memcpy(fillerStr, filler, sizeof(filler));
    cout << " filler: \"" << fillerStr << "\"" << endl;
    doubleFile.read((char*)&entryCount, sizeof(entryCount));
    entryCount = ENDIAN_SWAP_16(entryCount);
    cout << "  count: " << entryCount << endl;

    map<uint32_t, AppleDoubleEntry>    entries;

    for (int x = 0; x < entryCount; ++x) {
        uint32_t entryID = 0;
        AppleDoubleEntry currEntry;
        doubleFile.read( (char*)&entryID, sizeof(entryID));
        entryID = ENDIAN_SWAP_32(entryID);
        cout << "\n" << "\t     id: " << entryID << endl;
        doubleFile.read( (char*)&currEntry.offset, sizeof(typeof(AppleDoubleEntry::offset)));
        currEntry.offset = ENDIAN_SWAP_32(currEntry.offset);
        cout << "\t offset: " << currEntry.offset << endl;
        doubleFile.read( (char*)&currEntry.length, sizeof(typeof(AppleDoubleEntry::length)));
        currEntry.length = ENDIAN_SWAP_32(currEntry.length);
        cout << "\t length: " << currEntry.length << endl;
        entries[entryID] = currEntry;
    }

    for (auto &currEntryPair : entries) {
        auto &currEntry = currEntryPair.second;

        switch (currEntryPair.first) {
            case FINDER_INFO: {
                char macType[5] = {0};
                char macCreator[5] = {0};
                doubleFile.seekg(currEntry.offset);
                doubleFile.read(macType, 4);
                doubleFile.read(macCreator, 4);
                cout << "\n";
                cout << "   Type: " << macType << "\n";
                cout << "Creator: " << macCreator << "\n";
                ofstream jsonFInfoFile("FInfo.json");
                jsonFInfoFile << "{\n    \"type\": \"" << macType << "\",\n"
                              << "    \"creator\": \"" << macCreator << "\"\n"
                              << "}";
                vector<char> fInfo(currEntry.length, 0);
                doubleFile.seekg(currEntry.offset);
                doubleFile.read(fInfo.data(), fInfo.size());
                ofstream resDataFile("FInfo.bin");
                resDataFile.write(fInfo.data(), fInfo.size());
                break;
            }

            case DATA_FORK: {
                vector<char> fData(currEntry.length, 0);
                doubleFile.seekg(currEntry.offset);
                doubleFile.read(fData.data(), fData.size());
                string fName(argv[1]);
                auto pos = fName.rfind('/');
                if (pos != string::npos) {
                    fName = fName.substr(pos + 1);
                }
                ofstream fDataFile(fName);
                fDataFile.write(fData.data(), fData.size());
                break;
            }

            case RESOURCE_FORK: {
                short refNum = fakeresfileopen(argv[1], "r", currEntry.offset);
                short numTypes = FakeCount1Types();
                for (short x = 1; x <= numTypes; ++x) {
                    uint32_t currType = 0;
                    FakeGet1IndType(&currType, x);
                    short numResources = FakeCount1Resources(currType);
                    for(short y = 1; y <= numResources; ++y) {
                        Handle res = FakeGet1IndResource(currType, y);
                        int16_t theID = 0;
                        uint32_t theType = 0;
                        unsigned char name[257] = {0};
                        FakeGetResInfo(res, &theID, &theType,
                                       name);
                        theType = ENDIAN_SWAP_32(theType);
                        name[name[0] + 1] = 0;
                        string fname;
                        fname.append((char*)&theType, 4);
                        fname.append("_");
                        fname.append(to_string(theID));
                        fname.append("_");
                        fname.append(((char*)name) + 1);
                        fname.append(".bin");

                        cout << "Writing " << fname << "..." << endl;

                        ofstream resDataFile(fname);
                        resDataFile.write(*res, FakeGetHandleSize(res));
                    }
                }
                break;
            }
        }
    }

    return 0;
}
