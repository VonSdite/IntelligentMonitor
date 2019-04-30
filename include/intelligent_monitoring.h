#ifndef _INTELLIGENTMONITORING_H
#define _INTELLIGENTMONITORING_H

#define OPENCV
#define GPU

#include "yolo_v2_class.hpp"
#include "opencv2\videoio\legacy\constants_c.h"
#include <map>
#include <fstream>

#if defined(_MSC_VER) && _MSC_VER < 1900
#define inline __inline
#endif

#ifdef EXPORT_API
#if defined(_MSC_VER)
#define INTELLIGENT_API __declspec(dllexport)
#else
#define INTELLIGENT_API __attribute__((visibility("default")))
#endif
#else
#ifdef NO_API
#define INTELLIGENT_API
#else
#define INTELLIGENT_API __declspec(dllimport)
#endif
#endif

#define DEFAULT_THREASH 0.3
#define DISAPPEAR_THREASH 5

class IntelligentMonitor
{
public:
    INTELLIGENT_API IntelligentMonitor();

    INTELLIGENT_API void init(const char* names_file, const char* cfg_file, const char* weights_file, int gpu_id = 0);
    
    INTELLIGENT_API IntelligentMonitor(const char* names_file, const char* cfg_file, const char* weights_file, int gpu_id = 0);
    INTELLIGENT_API IntelligentMonitor(std::string names_file, std::string cfg_file, std::string weights_file, int gpu_id = 0);
    INTELLIGENT_API ~IntelligentMonitor();

///////////////////////////////////////////////////////////////// extract
    /*
    参数:
    cv::VideoCapture& cap:                      视频捕获对象(请不要传摄像头的)
    std::vector<char *>& obj_to_extract:        需要提取的视频中所包含的目标组
    const char* save_path="./":                 提取出来的视频保存的路径
    const char * prefix=NULL:                   提取出来的视频命名的前缀
    int disappear_thresh=DISAPPEAR_THREASH:     所检测的目标最大消失的阈值, 秒为单位;
    比如设置为5,表示5秒内目标没出现, 则分一个片段
    bool draw_box=false:                        是否需要绘制框住目标的框框
    float thresh=DEFAULT_THREASH:               置信度的下限(即分类时若大于该值, 才认定对象属于该分类)
    double* progress_rate=NULL:                 提取的进度率, 多线程的时候可以拿这个来显示进度
    int interval_frame = 0:                     设置每检测一帧后跳过几帧, 加快检测速度

    返回值:
    bool:                                       表明提取是否成功
    */
    INTELLIGENT_API bool extract(
       cv::VideoCapture& cap,
       std::vector<char *>& obj_to_extract,
       const char* save_path = "./",
       const char * prefix = NULL,
       int disappear_thresh = DISAPPEAR_THREASH,
       bool draw_box = false,
       float thresh = DEFAULT_THREASH,
       double* progress_rate = NULL,
       int interval_frame = 0
    );

    /*
    参数:
    const char* video_file:                     视频文件的路径
    std::vector<char *>& obj_to_extract:        需要提取的视频中所包含的目标组
    const char* save_path="./":                 提取出来的视频保存的路径
    const char * prefix=NULL:                   提取出来的视频命名的前缀
    int disappear_thresh=DISAPPEAR_THREASH:     所检测的目标最大消失的阈值, 秒为单位;
    比如设置为5,表示5秒内目标没出现, 则分一个片段
    bool draw_box=false:                        是否需要绘制框住目标的框框
    float thresh=DEFAULT_THREASH:               置信度的下限(即分类时若大于该值, 才认定对象属于该分类)
    double* progress_rate=NULL:                 提取的进度率, 多线程的时候可以拿这个来显示进度
    int interval_frame = 0:                     设置每检测一帧后跳过几帧, 加快检测速度

    返回值:
    bool:                                       表明提取是否成功
    */
    INTELLIGENT_API bool extract(
       const char* video_file,
       std::vector<char *>& obj_to_extract,
       const char* save_path = "./",
       const char * prefix = NULL,
       int disappear_thresh = DISAPPEAR_THREASH,
       bool draw_box = false,
       float thresh = DEFAULT_THREASH,
       double* progress_rate = NULL,
       int interval_frame = 0
    );
///////////////////////////////////////////////////////////////// end

///////////////////////////////////////////////////////////////// detect_something_appear
    /*
    参数:
    cv::Mat& mat:                       图片或者视频某帧的矩阵
    const char* something:              检测的对象
    bool draw_box=false:                是否绘制框住对象的框
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    bool:                               表明检测对象是否出现
    */
    INTELLIGENT_API bool detect_something_appear(
       cv::Mat& mat,
       const char* something,
       bool draw_box = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    const char* image_filename:         图片名
    const char* something:              检测的对象
    bool draw_box=false:                是否绘制框住对象的框
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    bool:                               表明检测对象是否出现
    */
    INTELLIGENT_API bool detect_something_appear(
       const char* image_filename,
       const char* something,
       bool draw_box = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    cv::Mat& mat:                       图片或者视频某帧的矩阵
    std::vector<char*>& something:      检测的一组对象
    bool draw_box=false:                是否绘制框住对象的框
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    bool:                               只要对象组中一个对象出现, 就返回true, 全没出现, 返回false
    */
    INTELLIGENT_API bool detect_something_appear(
       cv::Mat& mat,
       std::vector<char*>& something,
       bool draw_box = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    const char* image_filename:         图片名
    std::vector<char*>& something:      检测的一组对象
    bool draw_box=false:                是否绘制框住对象的框
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    bool:                               只要对象组中一个对象出现, 就返回true, 全没出现, 返回false
    */
    INTELLIGENT_API bool detect_something_appear(
       const char* image_filename,
       std::vector<char*>& something,
       bool draw_box = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );
///////////////////////////////////////////////////////////////// end

//////////////////////////////////////////////////////////////// detect_something_appear_times
    /*
    参数:
    cv::Mat& mat:                       图片或者视频某帧的矩阵
    const char* something:              检测的对象
    bool draw_box=false:                是否绘制框住对象的框
    bool show_times=false:              是否显示检测对象的出现次数
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    int:                                表明检测对象出现的次数
    */
    INTELLIGENT_API int detect_something_appear_times(
       cv::Mat& mat,
       const char* something,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    const char* image_filename:         图片名
    const char* something:              检测的对象
    bool draw_box=false:                是否绘制框住对象的框
    bool show_times=false:              是否显示检测对象的出现次数
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    int:                                表明检测对象出现的次数
    */
    INTELLIGENT_API int detect_something_appear_times(
       const char* image_filename,
       const char* something,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    cv::Mat& mat:                       图片或者视频某帧的矩阵
    std::vector<char*>& something:      检测的一组对象
    std::map<std::string, int>& in_map: 记录检测对象出现的次数
    bool draw_box=false:                是否绘制框住对象的框
    bool show_times=false:              是否显示检测对象的出现次数
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    无返回值, 次数保存在in_map中
    */
    INTELLIGENT_API void detect_something_appear_times(
       cv::Mat& mat,
       std::vector<char*>& something,
       std::map<std::string, int>& in_map,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    const char* image_filename:             图片名
    std::vector<char*>& something:          检测的一组对象
    std::map<std::string, int>& in_map:     记录检测对象出现的次数
    bool draw_box=false:                    是否绘制框住对象的框
    bool show_times=false:                  是否显示检测对象的出现次数
    const char* save_file_name=NULL:        保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:           置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    无返回值, 次数保存在in_map中
    */
    INTELLIGENT_API void detect_something_appear_times(
       const char* image_filename,
       std::vector<char*>& something,
       std::map<std::string, int>& in_map,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );
//////////////////////////////////////////////////////////////// end

//////////////////////////////////////////////////////////////// detect_all_appear_times
    /*
    参数:
    cv::Mat& mat:                       图片或者视频某帧的矩阵
    std::map<std::string, int>& in_map: 保存检测的对象出现次数的映射表
    bool draw_box=false:                是否绘制框住对象的框
    bool show_times=false:              是否显示检测对象的出现次数
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    无返回值, 次数保存在in_map中
    */
    INTELLIGENT_API void detect_all_appear_times(
       cv::Mat& mat,
       std::map<std::string, int>& in_map,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );

    /*
    参数:
    const char* image_filename:         图片名
    std::map<std::string, int>& in_map: 保存检测的对象出现次数的映射表
    bool draw_box=false:                是否绘制框住对象的框
    bool show_times=false:              是否显示检测对象的出现次数
    const char* save_file_name=NULL:    保存处理后的图片名, NULL表示不保存
    float thresh=DEFAULT_THREASH:       置信度的下限(即分类时若大于该值, 才认定对象属于该分类)

    返回值:
    无返回值, 次数保存在in_map中
    */
    INTELLIGENT_API void detect_all_appear_times(
       const char* image_filename,
       std::map<std::string, int>& in_map,
       bool draw_box = false,
       bool show_times = false,
       const char* save_file_name = NULL,
       float thresh = DEFAULT_THREASH
    );
//////////////////////////////////////////////////////////////// end

private:
    int detect_appear(cv::Mat& mat_img, std::vector<bbox_t>& result_vec, const char* something_to_count, bool draw_box = false, bool show_count = false, const char* save_file_name = NULL);
    void detect_appear(cv::Mat& mat_img, std::vector<bbox_t>& result_vec, std::vector<char*>& obj_to_detect, std::map<std::string, int>& in_map, bool draw_box = false, bool show_count = false, const char* save_file_name = NULL);
    void detect_appear(cv::Mat& mat_img, std::vector<bbox_t>& result_vec, std::map<std::string, int>& in_map, bool draw_box = false, bool show_count = false, const char* save_file_name = NULL);

    std::vector<std::string> objects_names_from_file(std::string const file_name);

    bool somthing_appear(std::vector<bbox_t>& vec, const char* something);
    bool somthing_appear(std::vector<bbox_t>& vec, std::vector<char*>& something);

private:
    std::vector<std::string>    m_obj_names;
    Detector                    m_detector;
    bool                        m_isInit;
};

#endif
