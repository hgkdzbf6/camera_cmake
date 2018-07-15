#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
int main(){
    //declare a capture object
    cv::VideoCapture cap(0 + cv::CAP_DSHOW);
    cap.open(0 + cv::CAP_DSHOW);
    if (!cap.isOpened())
    {
        std::cout << "No input image";
        return 1;
    }    
    //获取图像帧率
    bool stop = false;
    cv::Mat frame;
    cv::namedWindow("Example");

    while (!stop)
    {
        if (!cap.read(frame))
            break;
        imshow("Example",frame);
        if (cv::waitKey(1000) >= 0)
            stop = true;
    }
}