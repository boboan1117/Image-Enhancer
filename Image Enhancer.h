#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int _brightness = 100;
int _contrast = 100;
int _colorReduce = 100;
int _sharpen =100;
Mat image;

static void sharpen2D(int, void*)
{
	Mat dst;
	dst = image.clone();
	cv::Mat kernel (3,3,CV_32F, cv::Scalar(0) );
	kernel.at<float>(1,1) = 5.0 * _sharpen;
	kernel.at<float>(0,1) = -1.0 * _sharpen;
	kernel.at<float>(2,1) = -1.0 * _sharpen;
	kernel.at<float>(1,0) = -1.0 * _sharpen;
	kernel.at<float>(1,2) = -1.0 * _sharpen;
	cv::filter2D(image, dst, image.depth(), kernel);
	imshow("image", dst);
}

static void updateBrightnessContrast( int /*arg*/, void* )
{
    int histSize = 64;
    int brightness = _brightness - 100;
    int contrast = _contrast - 100;
    double a, b;
    if( contrast > 0 )
    {
        double delta = 127.*contrast/100;
        a = 255./(255. - delta*2);
        b = a*(brightness - delta);
    }
    else
    {
        double delta = -128.*contrast/100;
        a = (256.-delta*2)/255.;
        b = a*brightness + delta;
    }

    Mat dst, hist;
    image.convertTo(dst, CV_8U, a, b);
    imshow("image", dst);

    calcHist(&dst, 1, 0, Mat(), hist, 1, &histSize, 0);
    Mat histImage = Mat::ones(200, 320, CV_8U)*255;

    normalize(hist, hist, 0, histImage.rows, CV_MINMAX, CV_32F);

    histImage = Scalar::all(255);
    int binW = cvRound((double)histImage.cols/histSize);

    for( int i = 0; i < histSize; i++ )
        rectangle( histImage, Point(i*binW, histImage.rows),
                   Point((i+1)*binW, histImage.rows - cvRound(hist.at<float>(i))),
                   Scalar::all(0), -1, 8, 0 );
    imshow("histogram", histImage);
}


void colorReduce(int, void*) 
{
  int numRows = image.rows;
  int numCols = image.cols * image.channels();
  for (int row=0; row < numRows; row++) 
 {
    uchar* data= image.ptr<uchar>(row);

    for (int col=0; col < numCols; col++) 
    {
   
      data[col]=    data[col]/_colorReduce*_colorReduce + _colorReduce/2;

    }
  }
    imshow("image", image);
}
const char* keys = 
{
 "{1| |baboon.jpg|input image file}"
};
int main( int argc, const char** argv )
{

    CommandLineParser parser(argc, argv, keys);
    string inputImage = parser.get<string>("1");
    image = imread( inputImage, 0 );
    if(image.empty())
    {
        std::cerr << "Cannot read image file: " << inputImage << std::endl;
        return -1;
    }

    namedWindow("image", 0);
    namedWindow("histogram", 0);

    createTrackbar("brightness", "image", &_brightness, 200, updateBrightnessContrast);
    createTrackbar("contrast", "image", &_contrast, 200, updateBrightnessContrast);
    createTrackbar("colorReduce","image", &_colorReduce, 200, colorReduce);
    createTrackbar("sharpen","image", &_sharpen, 200, sharpen2D);
    updateBrightnessContrast(0, 0);
    waitKey();

    return 0;
}