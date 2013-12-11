#ifndef SignRecognitionToolkit_H
#define SignRecognitionToolkit_H

#include <opencv2/opencv.hpp>
#include <QtCore>
#include <vector>



class SignRecognitionToolkit
{
public:
    enum FeatureMethod
    {
        PAPER_63,
        EACH_PIXEL
    };


    SignRecognitionToolkit();

    static void             GetTestImageCrop        (const cv::Mat &inputImage, std::vector<cv::Mat> &vecCropImage);
    static std::vector<cv::Mat> GetTrainImageCrops      (const QStringList &inputFileList,const QString &xmlPath);
    static cv::Mat          GetCropFeature          (const cv::Mat &crop, FeatureMethod method);
    static cv::Mat          CreateFeatureMat        (const QVector<cv::Mat>& crops);

};

#endif // SignRecognitionToolkit_H
