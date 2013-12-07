#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

cv::RNG rng(12345);
void GetCropFromImage(std::string imagePath,cv::Mat &matCrop);


int main()
{
    std::string imagePath = "G://image.032725.jpg";
    cv::Mat crop;
    GetCropFromImage(imagePath,crop);
    return 0;
}


void GetCropFromImage(std::string imagePath,cv::Mat &matCrop)
{
    cv::Mat matGray= cv::imread(imagePath.c_str(),0);
    cv::GaussianBlur(matGray,matGray,cv::Size(3,3),1.5);
    cv::Canny(matGray,matGray,50,150);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( matGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    std::vector<cv::RotatedRect> minRect( contours.size() );
    std::vector<cv::RotatedRect> minEllipse( contours.size() );

    for( int i = 0; i < contours.size(); i++ )
    {
        minRect[i] = cv::minAreaRect( cv::Mat(contours[i]) );
        if( contours[i].size() > 5 )
        {
            minEllipse[i] = cv::fitEllipse( cv::Mat(contours[i]) );
        }
    }

    matCrop = cv::Mat::zeros( matGray.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        cv::Scalar color( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        // contour
        cv::drawContours( matCrop, contours, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
        // ellipse
        cv::ellipse( matCrop, minEllipse[i], color, 2, 8 );
        // rotated rectangle
        cv::Point2f rect_points[4]; minRect[i].points( rect_points );
        for( int j = 0; j < 4; j++ )
            cv::line( matCrop, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
    }

        cv::imwrite("g://result.jpg",matCrop);
        cv::imshow("hehe",matCrop);
        cv::waitKey();
}
