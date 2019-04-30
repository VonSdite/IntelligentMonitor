
#include "intelligent_monitoring.h"
#include "Ini.h"

#include <opencv2/opencv.hpp>               // C++
#include "opencv2/highgui/highgui.hpp"  

#include <string>

#pragma comment(lib, "opencv_world410.lib")
#pragma comment(lib, "IntelligentMonitor.lib")


void videoSave(const char *openFileName, const char * saveFileName, const int cv_fourcc);

void test_rtsp(IntelligentMonitor& monitor, const char * rtsp);


int main(int argc, char const *argv[])
{
    //videoSave("test_data/7.mp4", "test_save_file/test/XVID.avi", CV_FOURCC('X', 'V', 'I', 'D'));
    //videoSave("test_data/7.mp4", "test_save_file/test/X264.avi", CV_FOURCC('X', '2', '6', '4'));
    //videoSave("test_data/7.mp4", "test_save_file/test/mp4v.mp4", CV_FOURCC('m', 'p', '4', 'v'));
    //videoSave("test_data/7.mp4", "test_save_file/test/MP42.avi", CV_FOURCC('M', 'P', '4', '2'));
    //videoSave("test_data/7.mp4", "test_save_file/test/flv.avi", CV_FOURCC('F', 'L', 'V', '1'));

    ReadIni ini("config.ini");
    std::string names_file = ini.GetIniKeyString("yolo", "names_file");
    std::string cfg_file = ini.GetIniKeyString("yolo", "cfg_file");
    std::string weights_file = ini.GetIniKeyString("yolo", "weights_file");
    std::string test_file = ini.GetIniKeyString("test", "test_file");
    std::string pic_file = ini.GetIniKeyString("test", "pic_file");


    IntelligentMonitor monitor = IntelligentMonitor(names_file, cfg_file, weights_file);

    cv::Mat imge_mat;
    cv::VideoCapture capture;
    std::vector<char*> obj_to_detect;
    obj_to_detect.push_back("person");
    std::map<std::string, int> in_map;

    //////////////////////////////////////////////////////////////////////////
    // test monitor detect_something_appear function
    std::cout << "person: " <<
        monitor.detect_something_appear(pic_file.c_str(), "person", true, "test_save_file/1_image_file_name.jpg")
        << std::endl;

    imge_mat = cv::imread(pic_file, -1);
    std::cout << "person: " <<
        monitor.detect_something_appear(imge_mat, "person", true, "test_save_file/2_image_cvMat.jpg")
        << std::endl;

    std::cout << "person: " <<
        monitor.detect_something_appear(pic_file.c_str(), obj_to_detect, true, "test_save_file/3_vec_image_file_name.jpg")
        << std::endl;

    imge_mat = cv::imread(pic_file);
    std::cout << "person: " <<
        monitor.detect_something_appear(imge_mat, obj_to_detect, true, "test_save_file/4_vec_image_cvMat.jpg")
        << std::endl;
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // test monitor detect_something_appear_times function
    std::cout << "person: " <<
        monitor.detect_something_appear_times(pic_file.c_str(), "person", true, true, "test_save_file/5_image_file_name_with_times.jpg")
        << std::endl;

    imge_mat = cv::imread(pic_file, -1);
    std::cout << "person: " << 
        monitor.detect_something_appear_times(imge_mat, "person", true, true, "test_save_file/6_image_cvMat_with_times.jpg")
        << std::endl;

    in_map.clear();
    monitor.detect_something_appear_times(pic_file.c_str(), obj_to_detect, in_map, true, true, "test_save_file/7_vec_image_file_name_with_times.jpg");
    for (auto m : in_map)
    {
        std::cout << m.first << ": " << m.second << std::endl;
    }

    in_map.clear();
    imge_mat = cv::imread(pic_file);
    monitor.detect_something_appear_times(imge_mat, obj_to_detect, in_map, true, true, "test_save_file/8_vec_image_cvMat_with_times.jpg");
    for (auto m : in_map)
    {
        std::cout << m.first << ": " << m.second << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // test monitor detect_all_appear_times function
    in_map.clear();
    imge_mat = cv::imread(pic_file);
    monitor.detect_all_appear_times(imge_mat, in_map, true, true, "test_save_file/9_all_image_cvMat_with_times.jpg");
    for (auto m : in_map)
    {
        std::cout << m.first << ": " << m.second << std::endl;
    }


    in_map.clear();
    monitor.detect_all_appear_times(pic_file.c_str(), in_map, true, true, "test_save_file/10_all_image_file_name_with_times.jpg");
    for (auto m : in_map)
    {
        std::cout << m.first << ": " << m.second << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // test monitor extract funtcion
    std::cout << "extract: " 
        << monitor.extract(test_file.c_str(), obj_to_detect, "test_save_file/", "file_", 5, true) << std::endl;

    capture.open(test_file);
    if (!capture.isOpened())
    {
        printf("文件打开失败");
    }
    std::cout << "extract: "
        << monitor.extract(capture, obj_to_detect, "test_save_file/", "caputure_", 5, true) << std::endl;

    //////////////////////////////////////////////////////////////////////////

    return 0;
}

void test_rtsp(IntelligentMonitor& monitor, const char * rtsp)
{
    cv::Mat imge_mat;
    cv::VideoCapture capture;
    std::map<std::string, int> in_map;

    if (!capture.open(rtsp))
    {
        return;
    }

    cv::VideoWriter writer;
    double fps = capture.get(CV_CAP_PROP_FPS);
    cv::Size sWH = cv::Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    writer.open("1.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, sWH);


    cv::namedWindow("Show", cv::WINDOW_NORMAL);
    cv::moveWindow("Show", 0, 0);
    cv::resizeWindow("Show", 1300, 700);
    while (true)
    {
        capture >> imge_mat;
        monitor.detect_all_appear_times(imge_mat, in_map, true, true);
        writer << imge_mat;
        cv::imshow("Show", imge_mat);
        int c = cv::waitKey(1);

        if (c == 27 || c == 1048603) // ESC - exit (OpenCV 2.x / 3.x)
        {
            writer.release();
            return;
        }
        else if (c == 32) {

            do {
                c = cvWaitKey(1);
                if (c == 27 || c == 1048603) // ESC - exit (OpenCV 2.x / 3.x)
                {
                    writer.release();
                    return;
                }
                else if (c == 32) break;
            } while (c != 32);
        }
    }
}

void videoSave(const char *openFileName, const char * saveFileName, const int cv_fourcc)
{
    cv::VideoCapture capture;
    capture.open(openFileName);
    if (!capture.isOpened())
    {
        std::cout << "打开失败" << std::endl;
        return;
    }

    double fps = capture.get(CV_CAP_PROP_FPS);
    cv::Size swH = cv::Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    cv::VideoWriter outputVideo;
    outputVideo.open(saveFileName, cv_fourcc, fps, swH);

    cv::Mat frame;
    while (capture.isOpened())
    {
        capture >> frame;
        if (frame.empty())
        {
            outputVideo.release();
            return;
        }
        outputVideo << frame;
    }
}
