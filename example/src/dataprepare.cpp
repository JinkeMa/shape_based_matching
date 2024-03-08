
#include<opencv.hpp>
#include<vector>
#include<string>
#include <iostream>
#include<fstream>
#include<map>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include<experimental/filesystem>

#include<json.hpp>

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
/*
{
    {
        "version": "5.0.2",
            "flags" : {},
            "shapes" : [
        {
            "label": "a",
                "points" : [
                    [
                        1354.4354838709678,
                        660.0806451612904
                    ],
                        [
                            1360.8870967741937,
                            669.3548387096774
                        ],
                        [
                            1360.483870967742,
                            675.8064516129032
                        ],
                        [
                            1352.016129032258,
                            670.5645161290323
                        ],
                        [
                            1344.758064516129,
                            667.741935483871
                        ],
                        [
                            1345.5645161290322,
                            662.9032258064516
                        ],
                        [
                            1350.0,
                            660.8870967741935
                        ]
                ],
                "group_id": null,
                    "shape_type" : "polygon",
                    "flags" : {}
        },
}
*/


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
bool save_image_mmap()
{
#include <Windows.h>

    // 图像文件路径
    std::string imagePath = "path_to_your_image.jpg";
    // 映射文件名
    std::string mapFilePath = "mapped_image.dat";

    // 加载图像到 cv::Mat 对象
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        std::cerr << "Error: Image cannot be loaded." << std::endl;
        return -1;
    }

    // 获取图像数据的指针
    const unsigned char* imageData = image.data;
    // 计算图像数据的大小
    size_t imageSize = image.total() * image.elemSize();

    // 打开文件进行内存映射
    std::ofstream file(mapFilePath, std::ios::binary | std::ios::inout | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for memory mapping." << std::endl;
        return -1;
    }

    // 设置文件大小为图像数据的大小
    file.seekp(0, std::ios::end);
    file.write(reinterpret_cast<char*>(&imageSize), sizeof(imageSize));
    file.seekp(0, std::ios::beg);

    // 创建内存映射
    HANDLE fileHandle = (HANDLE)_get_osfhandle(_fileno(file.fileno()));
    HANDLE mappingHandle = CreateFileMapping(fileHandle, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(imageSize), nullptr);
    if (mappingHandle == nullptr) {
        std::cerr << "Error: Unable to create memory mapping." << std::endl;
        return -1;
    }

    // 将图像数据映射到内存
    LPBYTE mappingAddress = reinterpret_cast<LPBYTE>(MapViewOfFile(mappingHandle, FILE_MAP_WRITE, 0, 0, 0));
    if (mappingAddress == nullptr) {
        std::cerr << "Error: Unable to map view of file." << std::endl;
        CloseHandle(mappingHandle);
        return -1;
    }

    // 将图像数据复制到映射的内存
    memcpy(mappingAddress, imageData, static_cast<size_t>(imageSize));

    // 取消映射内存
    UnmapViewOfFile(mappingAddress);
    // 关闭内存映射
    CloseHandle(mappingHandle);
    // 关闭文件
    file.close();

    std::cout << "Image data has been stored in memory-mapped file." << std::endl;

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
                drawMarker(mask, cv::Point(pt), cv::Scalar{ 255 },0,5);
            }
            contours.emplace_back(shape.points);
        }
        //drawContours只接受整型cv::Point
        drawContours(mask, contours, -1, cv::Scalar{ 255 },-1);
    }

#pragma endregion
}