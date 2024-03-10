
#include<opencv.hpp>
#include<vector>
#include<string>
#include <iostream>
#include<fstream>
#include<map>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include<experimental/filesystem>

#include<json.hpp>

//win32内存映射
#include <Windows.h>
#include<tchar.h>

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;

using json = nlohmann::json;


void ListFilesInDirectory(const fs::path& directory_path) {
    if (fs::exists(directory_path) && fs::is_directory(directory_path)) {
        std::cout << "Listing files in " << directory_path << ":\n";
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            std::cout << entry.path() << std::endl;
        }
    }
    else {
        std::cerr << "The provided path does not exist or is not a directory." << std::endl;
    }
}

typedef struct jdata
{
    using point = cv::Point;

    typedef struct shape
    {
        string label;
        vector<point>points;
        string group_id;
        string shape_type;
        //string flags;
    }shape;

    string version;
    //string flags;
    vector<shape>shapes;
    int imageWidth, imageHeight;
}jdata;

void json2jdata(json& json_, jdata& jdata_)
{
    jdata_.version   = !json_["version"].empty()     ? string(json_["version"])   : "";
    jdata_.imageWidth   = !json_["imageWidth"].empty()  ? int(json_["imageWidth"])   : 0;
    jdata_.imageHeight  = !json_["imageHeight"].empty() ? int(json_["imageHeight"])  : 0;
    
    if (!json_["shapes"].empty())
    {
        json shapes = json_["shapes"];
        
        for (int i = 0; i < json_["shapes"].size(); ++i)
        {
            jdata::shape shape_;
            shape_.label = !shapes[i]["label"].empty() ? shapes[i]["label"] : "";
            shape_.group_id = !shapes[i]["group_id"].empty() ? shapes[i]["group_id"] : "";
            shape_.shape_type = !shapes[i]["shape_type"].empty() ? shapes[i]["shape_type"] : "";
            if (!shapes[i]["points"].empty())
            {
                for (int j = 0; j < shapes[i]["points"].size(); ++j)
                {
                    auto pp = shapes[i]["points"][j];
                    jdata::point pt;
                    if (pp.size() != 2)
                    {
                        continue;
                    }
                    pt.x = pp[0];
                    pt.y = pp[1];
                    shape_.points.emplace_back(pt);
                }
            }
            jdata_.shapes.emplace_back(shape_);
        }
    }

}


//内存映射存储图像
bool mmap_example()
{

    /*
   This program demonstrates file mapping, especially how to align a
   view with the system file allocation granularity.
*/

#define BUFFSIZE 1024 // size of the memory to examine at any one time

#define FILE_MAP_START 138240 // starting point within the file of
// the data to examine (135K)

/* The test file. The code below creates the file and populates it,
   so there is no need to supply it in advance. */

    TCHAR* lpcTheFile = TEXT("fmtest.txt"); // the file to be manipulated

        HANDLE hMapFile;      // handle for the file's memory-mapped region
        HANDLE hFile;         // the file handle
        BOOL bFlag;           // a result holder
        DWORD dBytesWritten;  // number of bytes written
        DWORD dwFileSize;     // temporary storage for file sizes
        DWORD dwFileMapSize;  // size of the file mapping
        DWORD dwMapViewSize;  // the size of the view
        DWORD dwFileMapStart; // where to start the file map view
        DWORD dwSysGran;      // system allocation granularity
        SYSTEM_INFO SysInfo;  // system information; used to get granularity
        LPVOID lpMapAddress;  // pointer to the base address of the
                              // memory-mapped region
        char* pData;         // pointer to the data
        int i;                // loop counter
        int iData;            // on success contains the first int of data
        int iViewDelta;       // the offset into the view where the data
                              //shows up

        // Create the test file. Open it "Create Always" to overwrite any
        // existing file. The data is re-created below
        hFile = CreateFile(lpcTheFile,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            return 4;
        }

        // Get the system allocation granularity.
        GetSystemInfo(&SysInfo);
        dwSysGran = SysInfo.dwAllocationGranularity;

        // Now calculate a few variables. Calculate the file offsets as
        // 64-bit values, and then get the low-order 32 bits for the
        // function calls.

        // To calculate where to start the file mapping, round down the
        // offset of the data into the file to the nearest multiple of the
        // system allocation granularity.
        dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;

        // Calculate the size of the file mapping view.
        dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;

        // How large will the file mapping object be?
        dwFileMapSize = FILE_MAP_START + BUFFSIZE;

        // The data of interest isn't at the beginning of the
        // view, so determine how far into the view to set the pointer.
        iViewDelta = FILE_MAP_START - dwFileMapStart;

        // Now write a file with data suitable for experimentation. This
        // provides unique int (4-byte) offsets in the file for easy visual
        // inspection. Note that this code does not check for storage
        // medium overflow or other errors, which production code should
        // do. Because an int is 4 bytes, the value at the pointer to the
        // data should be one quarter of the desired offset into the file

        for (i = 0; i < (int)dwSysGran; i++)
        {
            WriteFile(hFile, &i, sizeof(i), &dBytesWritten, NULL);
        }

        // Verify that the correct file size was written.
        dwFileSize = GetFileSize(hFile, NULL);

        // Create a file mapping object for the file
        // Note that it is a good idea to ensure the file size is not zero
        hMapFile = CreateFileMapping(hFile,          // current file handle
            NULL,           // default security
            PAGE_READWRITE, // read/write permission
            0,              // size of mapping object, high
            dwFileMapSize,  // size of mapping object, low
            NULL);          // name of mapping object

        if (hMapFile == NULL)
        {
            return (2);
        }

        // Map the view and test the results.

        lpMapAddress = MapViewOfFile(hMapFile,            // handle to
                                                          // mapping object
            FILE_MAP_ALL_ACCESS, // read/write
            0,                   // high-order 32
                                 // bits of file
                                 // offset
            dwFileMapStart,      // low-order 32
                                 // bits of file
                                 // offset
            dwMapViewSize);      // number of bytes
                                 // to map
        if (lpMapAddress == NULL)
        {
            return 3;
        }

        // Calculate the pointer to the data.
        pData = (char*)lpMapAddress + iViewDelta;

        // Extract the data, an int. Cast the pointer pData from a "pointer
        // to char" to a "pointer to int" to get the whole thing
        iData = *(int*)pData;


        // Close the file mapping object and the open file

        bFlag = UnmapViewOfFile(lpMapAddress);
        bFlag = CloseHandle(hMapFile); // close the file mapping object

        if (!bFlag)
        {
        }

        bFlag = CloseHandle(hFile);   // close the file itself

        if (!bFlag)
        {
        }

        return 0;
}

//内存映射存数据
bool save_mmap(cv::Mat& src, string path)
{
    //变量命名 winAPI
    HANDLE hFile;
    HANDLE hMapFile;
    TCHAR* lpcfilename = const_cast<char*>(path.c_str());
    LPVOID lpMapAddress;

    DWORD dBytesWritten;  // number of bytes written
    DWORD dwFileSize;     // temporary storage for file sizes
    DWORD dwFileMapSize;  // size of the file mapping
    DWORD dwMapViewSize;  // the size of the view
    DWORD dwFileMapStart; // where to start the file map view

    //变量命名 cpp
    char* pData;
    int i;
    int iData;
    int iViewDelta;

    //计算数据量与各种offset

    //创建filehandle
    hFile = CreateFile(
        lpcfilename,
        GENERIC_READ | GENERIC_WRITE,
        0,//不允许共享对象
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    //计算映射字节数
    DWORD depth;
    if (src.depth() == CV_8U || src.depth() == CV_8S)
    {
        depth = 1;
    }
    else if (src.depth() == CV_16U || src.depth() == CV_16S || src.depth() == CV_16F)
    {
        depth = 2;
    }
    else if (src.depth() == CV_32S || src.depth() == CV_32F)
    {
        depth = 4;
    }
    else if (src.depth() == CV_64F)
    {
        depth = 8;
    }
    dwMapViewSize = depth * src.rows * src.cols;

    //
    hMapFile = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        dwMapViewSize,
        NULL);
    if (hMapFile == NULL)
    {
        return false;
    }

    lpMapAddress = MapViewOfFile(
        hMapFile,            // handle to
                             // mapping object
        FILE_MAP_ALL_ACCESS, // read/write
        0,                   // high-order 32
                             // bits of file
                             // offset
        0,                   // low-order 32
                             // bits of file
                             // offset
        dwMapViewSize        // number of bytes to map
        );
    if (lpMapAddress == NULL)
    {
        return false;
    }

    //待完善，需要加上png数据头，否则格式不正确
    auto re = memcpy(lpMapAddress, src.ptr(), dwMapViewSize);
    
    bool bFlag = UnmapViewOfFile(lpMapAddress);
    bFlag = CloseHandle(hMapFile); // close the file mapping object

    if (!bFlag)
    {

    }

    bFlag = CloseHandle(hFile);   // close the file itself

    if (!bFlag)
    {
        
    }

    return true;
}

//存储图像
bool save_image(cv::Mat& src, string filename, int stype)
{
    switch (stype)
    {
        //
    case -1:
        cv::imwrite(filename, src);
        break;
    case 1:
        save_mmap(src, filename);
        break;
    }
    return true;
}

int main()
{

#pragma region 提取json文件

	string path_head = "F:/model_tuning/data/viod/";
	string path_train = path_head + "train";
	string path_test = path_head + "test";
    map<string, string> datapath = {
        {"train",path_train},
        {"test",path_test}
    };
    
    std::map<string,jdata>json_map;

    for (auto path : datapath)
    {
        if (fs::exists(path.second) && fs::is_directory(path.second)) {
            std::cout << "Listing files in " << path.second << ":\n";
            for (const auto& entry : fs::directory_iterator(path.second)) {
                std::cout << entry.path() << std::endl;
                string& jpath = entry.path().u8string();
                int len = size(".json") - 1;
                string&& ipath = string(jpath.begin(),jpath.end() - len) + ".jpg";
                //== return 0 if equal
                if (jpath.compare(jpath.length() - len, len, ".json"))
                {
                    continue;
                }
                //解析json文件
                std::ifstream f(jpath);
                json data = json::parse(f);

                jdata jdata_;
                json2jdata(data, jdata_);
                json_map.insert({ ipath, jdata_ });
            }
        }
        else {
            std::cerr << "The provided path does not exist or is not a directory." << std::endl;
        }
    }
    
#pragma endregion

#pragma region 生成训练数据
    for (auto item : json_map)
    {
        string image_path = item.first;
        cv::Mat src = cv::imread(image_path, IMREAD_UNCHANGED);
        cv::Mat mask = Mat::zeros(src.size(),src.depth());
        jdata jdata_ = item.second;
        vector<vector<cv::Point>> contours;
        for (auto shape : jdata_.shapes)
        {
            for (auto pt : shape.points)
            {
                drawMarker(mask, cv::Point(pt), cv::Scalar{ 255 }, 0, 5);
            }
            contours.emplace_back(shape.points);
        }
        //drawContours只接受整型cv::Point
        drawContours(mask, contours, -1, cv::Scalar{ 255 }, -1);

        string str_ = item.first;
        int len = size(".jpg") - 1;
        string&& name = string(str_.begin(), str_.end() - len) + ".png";
        save_image(mask, name, -1);
    }


#pragma endregion
}