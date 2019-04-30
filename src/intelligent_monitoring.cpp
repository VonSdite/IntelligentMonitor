#include "intelligent_monitoring.h"

#include <io.h>
#include <direct.h>

IntelligentMonitor::IntelligentMonitor() : m_isInit(false)
{
}

void IntelligentMonitor::init(const char * names_file, const char * cfg_file, const char * weights_file, int gpu_id)
{
    if (m_isInit) return;
    this->m_isInit = true;
    this->m_obj_names = objects_names_from_file(names_file);
    this->m_detector.init_detector(cfg_file, weights_file, gpu_id);
}

IntelligentMonitor::IntelligentMonitor(const char * names_file, const char * cfg_file, const char * weights_file, int gpu_id) : m_isInit(true)
{
    this->m_obj_names = objects_names_from_file(names_file);
    this->m_detector.init_detector(cfg_file, weights_file, gpu_id);
}

IntelligentMonitor::IntelligentMonitor(std::string names_file, std::string cfg_file, std::string weights_file, int gpu_id) : m_isInit(true)
{
    this->m_obj_names = objects_names_from_file(names_file.c_str());
    this->m_detector.init_detector(cfg_file.c_str(), weights_file.c_str(), gpu_id);
}

IntelligentMonitor::~IntelligentMonitor()
{
}

bool IntelligentMonitor::extract(cv::VideoCapture & cap, std::vector<char*>& obj_to_extract, const char * save_path, const char * prefix, int disappear_thresh, bool draw_box, float thresh, double* progress_rate, int interval_frame)
{
    double fps = cap.get(CV_CAP_PROP_FPS);
    long curent_frame = 0;
    long total_frame = cap.get(CV_CAP_PROP_FRAME_COUNT);

    int disappear_frame_thresh = fps * disappear_thresh;

    cv::Size sWH = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

    cv::Mat frame;
    cv::VideoWriter outputVideo;

    int base_num = 1;

    if (_access(save_path, 0) == -1)
    {
        _mkdir(save_path);
    }

    std::string base_name;
    if (prefix != NULL)
        base_name = std::string(save_path) + "/" + std::string(prefix);
    else
        base_name = std::string(save_path) + "/";

    int count_frame = disappear_frame_thresh;
    bool appear_flag = false;
    bool recording_flag = false;

    bool pre_appear_flag = false;
    int current_interval_frame = 0;

    while (cap.isOpened())
    {
        cap >> frame;
        ++curent_frame;
        if (progress_rate != NULL)
            *progress_rate = (double)curent_frame / (double)total_frame;

        if (frame.empty() || curent_frame >= total_frame)
        {
            outputVideo.release();
            return true;
        }

        if (current_interval_frame % (interval_frame + 1) == 0)
            appear_flag = detect_something_appear(frame, obj_to_extract, draw_box, NULL, thresh);
        else
            appear_flag = pre_appear_flag;
        ++current_interval_frame;

        if (!appear_flag) ++count_frame;
        else count_frame = 0;

        if (count_frame > disappear_frame_thresh)
        {
            if (outputVideo.isOpened())
            {
                outputVideo.release();
            }
            recording_flag = false;
        }
        else
        {
            if (!recording_flag)
            {
                outputVideo.open(base_name + std::to_string(base_num) + ".avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, sWH);
                ++base_num;
                recording_flag = true;
            }
            outputVideo << frame;
        }
    }

    return false;
}

bool IntelligentMonitor::extract(const char * video_file, std::vector<char*>& obj_to_extract, const char * save_path, const char * prefix, int disappear_thresh, bool draw_box, float thresh, double * progress_rate, int interval_frame)
{
    cv::VideoCapture capture;
    capture.open(video_file);
    if (!capture.isOpened())
        return false;
    extract(capture, obj_to_extract, save_path, prefix, disappear_thresh, draw_box, thresh, progress_rate, interval_frame);
}

bool IntelligentMonitor::detect_something_appear(cv::Mat& mat, const char* something, bool draw_box, const char* save_file_name, float thresh)
{
    std::vector<bbox_t> result_vec = m_detector.detect(mat, thresh);
    if (draw_box || save_file_name != NULL)
        return detect_appear(mat, result_vec, something, draw_box, false, save_file_name);
    else
        return somthing_appear(result_vec, something);
}

bool IntelligentMonitor::detect_something_appear(const char* image_filename, const char* something, bool draw_box, const char* save_file_name, float thresh)
{
    cv::Mat mat = cv::imread(image_filename, -1);
    return detect_something_appear(mat, something, draw_box, save_file_name, thresh);
}

bool IntelligentMonitor::detect_something_appear(cv::Mat & mat, std::vector<char*>& something, bool draw_box, const char* save_file_name, float thresh)
{
    std::vector<bbox_t> result_vec = m_detector.detect(mat, thresh);
    if (draw_box || save_file_name != NULL)
    {
        std::map<std::string, int> tmp;
        tmp.clear();
        detect_appear(mat, result_vec, something, tmp, draw_box, false, save_file_name);
        return !tmp.empty();
    }
    else
        return somthing_appear(result_vec, something);
}

bool IntelligentMonitor::detect_something_appear(const char* image_filename, std::vector<char*>& something, bool draw_box, const char* save_file_name, float thresh)
{
    cv::Mat mat = cv::imread(image_filename, -1);
    return detect_something_appear(mat, something, draw_box, save_file_name, thresh);
}

int IntelligentMonitor::detect_something_appear_times(cv::Mat& mat, const char * something, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    std::vector<bbox_t> result_vec = m_detector.detect(mat, thresh);
    return detect_appear(mat, result_vec, something, draw_box, show_times, save_file_name);
}

int IntelligentMonitor::detect_something_appear_times(const char* image_filename, const char * something, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    cv::Mat mat = cv::imread(image_filename, -1);
    return detect_something_appear_times(mat, something, draw_box, show_times, save_file_name, thresh);
}

void IntelligentMonitor::detect_something_appear_times(cv::Mat & mat, std::vector<char*>& something, std::map<std::string, int>& in_map, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    in_map.clear();
    std::vector<bbox_t> result_vec = m_detector.detect(mat, thresh);
    detect_appear(mat, result_vec, something, in_map, draw_box, show_times, save_file_name);
}

void IntelligentMonitor::detect_something_appear_times(const char* image_filename, std::vector<char*>& something, std::map<std::string, int>& in_map, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    cv::Mat mat = cv::imread(image_filename, -1);
    detect_something_appear_times(mat, something, in_map, draw_box, show_times, save_file_name, thresh);
}

void IntelligentMonitor::detect_all_appear_times(cv::Mat& mat, std::map<std::string, int>& in_map, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    in_map.clear();
    std::vector<bbox_t> result_vec = m_detector.detect(mat, thresh);
    detect_appear(mat, result_vec, in_map, draw_box, show_times, save_file_name);
}

void IntelligentMonitor::detect_all_appear_times(const char* image_filename, std::map<std::string, int>& in_map, bool draw_box, bool show_times, const char* save_file_name, float thresh)
{
    cv::Mat mat = cv::imread(image_filename, -1);
    detect_all_appear_times(mat, in_map, draw_box, show_times, save_file_name, thresh);
}

int IntelligentMonitor::detect_appear(cv::Mat& mat_img, std::vector<bbox_t>& result_vec, const char* something_to_count, bool draw_box, bool show_count, const char* save_file_name)
{
    int count = 0;

    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
    float const font_size = mat_img.rows / 1000.f + 0.3;
    cv::Scalar color;

    for (auto &i : result_vec)
    {
        if (m_obj_names.size() > i.obj_id)
        {
            std::string obj_name = m_obj_names[i.obj_id];
            if (!strncmp(obj_name.c_str(), "dont_show", 9)) continue;

            if (obj_name == something_to_count)
            {
                ++count;

                if (draw_box)
                {
                    color = obj_id_to_color(i.obj_id);
                    cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);

                    if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
                    cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
                    int max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
                    max_width = std::max(max_width, (int)i.w + 2);
                    std::string coords_3d;
                    if (!std::isnan(i.z_3d)) {
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << "x:" << i.x_3d << "m y:" << i.y_3d << "m z:" << i.z_3d << "m ";
                        coords_3d = ss.str();
                        cv::Size const text_size_3d = getTextSize(ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0);
                        int const max_width_3d = (text_size_3d.width > i.w + 2) ? text_size_3d.width : (i.w + 2);
                        if (max_width_3d > max_width) max_width = max_width_3d;
                    }

                    cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 35, 0)),
                        cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
                        color, CV_FILLED, 8, 0);
                    putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 16), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
                    if (!coords_3d.empty()) putText(mat_img, coords_3d, cv::Point2f(i.x, i.y - 1), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 0, 0), 1);
                }
            }
        }
    }

    int show_y = 20;
    if (show_count && count > 0)
    {
        std::string str_count_something = std::string(something_to_count) + " appear: " + std::to_string(count) + " times";
        int baseline;
        cv::Size text_size = cv::getTextSize(str_count_something, cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, 2, &baseline);
        cv::rectangle(mat_img, cv::Rect(8, show_y - 2 - text_size.height, text_size.width + 5, text_size.height + 7), cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
        putText(mat_img, str_count_something, cv::Point2f(10, show_y), cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, cv::Scalar(0, 0, 0), 1);
    }

    if (save_file_name != NULL)
    {
        char dir[4096];
        _splitpath(save_file_name, NULL, dir, NULL, NULL);
        // �ж�Ŀ¼�Ƿ���ڣ��������򴴽�
        if (_access(dir, 0) == -1)
        {
            _mkdir(dir);
        }
        cv::imwrite(save_file_name, mat_img);
    }
    return count;
}

void IntelligentMonitor::detect_appear(cv::Mat & mat_img, std::vector<bbox_t>& result_vec, std::vector<char*>& obj_to_detect, std::map<std::string, int>& in_map, bool draw_box, bool show_count, const char* save_file_name)
{
    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
    float const font_size = mat_img.rows / 1000.f + 0.3;
    cv::Scalar color;

    for (auto &i : result_vec)
    {
        if (m_obj_names.size() > i.obj_id)
        {
            std::string obj_name = m_obj_names[i.obj_id];
            if (!strncmp(obj_name.c_str(), "dont_show", 9)) continue;

            for (auto &j : obj_to_detect)
            {
                if (obj_name == j)
                {
                    ++in_map[obj_name];

                    if (draw_box)
                    {
                        color = obj_id_to_color(i.obj_id);
                        cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);

                        if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
                        cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
                        int max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
                        max_width = std::max(max_width, (int)i.w + 2);
                        std::string coords_3d;
                        if (!std::isnan(i.z_3d)) {
                            std::stringstream ss;
                            ss << std::fixed << std::setprecision(2) << "x:" << i.x_3d << "m y:" << i.y_3d << "m z:" << i.z_3d << "m ";
                            coords_3d = ss.str();
                            cv::Size const text_size_3d = getTextSize(ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0);
                            int const max_width_3d = (text_size_3d.width > i.w + 2) ? text_size_3d.width : (i.w + 2);
                            if (max_width_3d > max_width) max_width = max_width_3d;
                        }

                        cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 35, 0)),
                            cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
                            color, CV_FILLED, 8, 0);
                        putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 16), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
                        if (!coords_3d.empty()) putText(mat_img, coords_3d, cv::Point2f(i.x, i.y - 1), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 0, 0), 1);
                    }
                    break;
                }
            }
        }
    }

    int show_y = 20;
    if (show_count)
    {
        for (auto obj : in_map)
        {
            if (obj.second > 0)
            {
                std::string str_count_something = obj.first + " appear: " + std::to_string(obj.second) + " times";
                int baseline;
                cv::Size text_size = cv::getTextSize(str_count_something, cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, 2, &baseline);
                cv::rectangle(mat_img, cv::Rect(8, show_y - 2 - text_size.height, text_size.width + 5, text_size.height + 7), cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
                putText(mat_img, str_count_something, cv::Point2f(10, show_y), cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, cv::Scalar(0, 0, 0), 1);
                show_y += 20;
            }
        }
    }

    if (save_file_name != NULL)
    {
        char dir[4096];
        _splitpath(save_file_name, NULL, dir, NULL, NULL);
        if (_access(dir, 0) == -1)
        {
            _mkdir(dir);
        }
        cv::imwrite(save_file_name, mat_img);
    }

}

void IntelligentMonitor::detect_appear(cv::Mat& mat_img, std::vector<bbox_t>& result_vec, std::map<std::string, int>& in_map, bool draw_box, bool show_count, const char* save_file_name)
{
    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
    float const font_size = mat_img.rows / 1000.f + 0.3;
    cv::Scalar color;

    for (auto &i : result_vec)
    {
        if (m_obj_names.size() > i.obj_id)
        {
            std::string obj_name = m_obj_names[i.obj_id];
            if (!strncmp(obj_name.c_str(), "dont_show", 9)) continue;

            ++in_map[obj_name];

            if (draw_box)
            {
                color = obj_id_to_color(i.obj_id);
                cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);

                if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
                cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
                int max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
                max_width = std::max(max_width, (int)i.w + 2);
                std::string coords_3d;
                if (!std::isnan(i.z_3d)) {
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << "x:" << i.x_3d << "m y:" << i.y_3d << "m z:" << i.z_3d << "m ";
                    coords_3d = ss.str();
                    cv::Size const text_size_3d = getTextSize(ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0);
                    int const max_width_3d = (text_size_3d.width > i.w + 2) ? text_size_3d.width : (i.w + 2);
                    if (max_width_3d > max_width) max_width = max_width_3d;
                }

                cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 35, 0)),
                    cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
                    color, CV_FILLED, 8, 0);
                putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 16), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
                if (!coords_3d.empty()) putText(mat_img, coords_3d, cv::Point2f(i.x, i.y - 1), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 0, 0), 1);
            }
        }
    }

    int show_y = 20;
    if (show_count)
    {
        for (auto obj : in_map)
        {
            if (obj.second > 0)
            {
                std::string str_count_something = obj.first + " appear: " + std::to_string(obj.second) + " times";
                int baseline;
                cv::Size text_size = cv::getTextSize(str_count_something, cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, 2, &baseline);
                cv::rectangle(mat_img, cv::Rect(8, show_y - 2 - text_size.height, text_size.width + 5, text_size.height + 7), cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
                putText(mat_img, str_count_something, cv::Point2f(10, show_y), cv::FONT_HERSHEY_COMPLEX_SMALL, font_size, cv::Scalar(0, 0, 0), 1);
                show_y += 20;
            }
        }
    }

    if (save_file_name != NULL)
    {
        char dir[4096];
        _splitpath(save_file_name, NULL, dir, NULL, NULL);
        if (_access(dir, 0) == -1)
        {
            _mkdir(dir);
        }
        cv::imwrite(save_file_name, mat_img);
    }
}

std::vector<std::string> IntelligentMonitor::objects_names_from_file(std::string const file_name) {
    std::ifstream file(file_name);
    std::vector<std::string> file_lines;
    if (!file.is_open()) return file_lines;
    for (std::string line; getline(file, line);) file_lines.push_back(line);
    std::cout << "object names loaded \n";
    return file_lines;
}

bool IntelligentMonitor::somthing_appear(std::vector<bbox_t>& vec, const char * something)
{
    for (auto &i : vec)
    {
        if (this->m_obj_names.size() > i.obj_id && this->m_obj_names[i.obj_id] == something)
            return true;
    }
    return false;
}

bool IntelligentMonitor::somthing_appear(std::vector<bbox_t>& vec, std::vector<char*>& something)
{
    for (auto &i : vec)
    {
        for (auto &j : something)
        {
            if (this->m_obj_names.size() > i.obj_id && this->m_obj_names[i.obj_id] == j)
                return true;
        }
    }
    return false;
}
