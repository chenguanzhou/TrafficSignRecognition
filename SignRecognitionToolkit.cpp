#include "SignRecognitionToolkit.h"
#include <iostream>
#include <ctime>
#include <QMessageBox>

SignRecognitionToolkit::SignRecognitionToolkit()
{
}

void SignRecognitionToolkit::GetTestImageCrop(const cv::Mat &inputImage, std::vector<cv::Mat> &vecCropImage)
{
    cv::Mat hsv;
    cv::cvtColor(inputImage,hsv,cv::COLOR_BGR2Lab);
    std::vector<cv::Mat> splitMat;

    cv::split(hsv,splitMat);
    cv::Mat red = splitMat[1].clone();

    uchar *p,*q;
    for(int i = 0; i < red.rows; ++i)
    {
        p = red.ptr<uchar>(i);
        q = splitMat[2].ptr<uchar>(i);
        for (int j=0;j<red.cols;++j)
        {
            if (p[j]>160)
                p[j] = 255;
            else
                p[j] = 0;
        }
    }
    cv::Canny(red,red,50,200);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(red,contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


    std::vector<cv::RotatedRect> minEllipse;

    for( int i = 0; i < contours.size(); i++ )
    {
        if (contours[i].size()<25)
            continue;
        cv::RotatedRect ellipse = cv::fitEllipse( cv::Mat(contours[i]));
        float elongation = ellipse.size.width/(float)ellipse.size.height;
        elongation = elongation<1?1./elongation:elongation;
        if( ellipse.boundingRect().area() > 200 && elongation<1.3)
        {
            minEllipse.push_back(ellipse);
        }
    }

    vecCropImage.clear();
    for( int i = 0; i < minEllipse.size(); i++ )
    {
        float angle = minEllipse[i].angle;
        cv::Size rect_size = minEllipse[i].size;
        if (angle < -45.)
        {
            angle += 90.0;
            int temp = rect_size.width;
            rect_size.width = rect_size.height;
            rect_size.height= temp;
        }

        cv::Mat matTrans = cv::getRotationMatrix2D(minEllipse[i].center, angle, 1.0);
        cv::Mat rotated;
        cv::warpAffine(inputImage, rotated, matTrans, inputImage.size(), cv::INTER_CUBIC);
        cv::getRectSubPix(rotated, rect_size, minEllipse[i].center, rotated);
        cv::resize(rotated,rotated,cv::Size(30,30));
        vecCropImage.push_back(rotated);
    }
}

std::vector<cv::Mat> SignRecognitionToolkit::GetTrainImageCrops(const QStringList &inputFileList, const QString &configPath)
{
    std::vector<cv::Mat> listMat;
    QFile configFile(configPath);
    if (configFile.exists()==false)
    {
        QMessageBox::information(NULL,QObject::tr("Error"),QObject::tr("Config File Not Exist!"));
        return listMat;
    }
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(NULL,QObject::tr("Error"),QObject::tr("Config File Cannot Open!"));
        return listMat;
    }
    QTextStream textStream(&configFile);
    QString line = textStream.readLine();
    while (!line.isEmpty())
    {
        QStringList paraList = line.split(';');
        line = textStream.readLine();

        QString imageName = QString(".*")+paraList[0]+QString(".*");
        bool isValid = false;
        int fileNameID = inputFileList.indexOf(QRegExp(imageName) );
        if (fileNameID==-1) continue;
        imageName = inputFileList[fileNameID];
        int nYOff = paraList[3].toInt(&isValid);if (isValid==false) continue;
        int nXOff = paraList[4].toInt(&isValid);if (isValid==false) continue;
        int nYEnd = paraList[5].toInt(&isValid);if (isValid==false) continue;
        int nXEnd = paraList[6].toInt(&isValid);if (isValid==false) continue;

        cv::Mat src = cv::imread(imageName.toStdString());
        cv::Mat dst = src(cv::Rect(nXOff, nYOff, nXEnd-nXOff, nYEnd-nYOff));
        cv::resize(dst,dst,cv::Size(30,30),0,0,cv::INTER_AREA);
//        cv::imshow("hehe",dst);
//        cv::waitKey(1000);
        listMat.push_back(dst);
    }    
}

cv::Mat SignRecognitionToolkit::GetCropFeature(const cv::Mat &crop, FeatureMethod method)
{
    cv::Mat feature;
    if (method == PAPER_63)
    {
        feature = cv::Mat(1,63,CV_32FC1);
        float* p = feature.ptr<float>(0);
        std::vector<cv::Mat> vecCrop;
        cv::split(crop,vecCrop);
        *p++ = cv::mean(vecCrop[2])[0]/256.;//MR
        *p++ = cv::mean(vecCrop[1])[0]/256.;//MG
        *p++ = cv::mean(vecCrop[0])[0]/256.;//MB

        cv::Mat gray = vecCrop[2]*0.49 + vecCrop[1]*0.29 + vecCrop[2]*0.22;
//        cv::imshow("hehe",gray);
//        cv::waitKey();
        float treshold = cv::mean(gray)[0];
        //vh
        for (int i=0;i<30;++i)
        {
            float vh = 0;
            for (int j=0;j<30;++j)
            {
                float dn = gray.at<uchar>(i,j);
                if (dn>treshold)
                    vh += treshold;
            }
            *p++ = vh/30;
        }

        for (int i=0;i<30;++i)
        {
            float vh = 0;
            for (int j=0;j<30;++j)
            {
                float dn = gray.at<uchar>(j,i);
                if (dn>treshold)
                    vh += treshold;
            }
            *p++ = vh/30;
        }
    }

    return feature;
}

cv::Mat SignRecognitionToolkit::CreateFeatureMat(const QVector<cv::Mat> &crops)
{
    cv::Mat featureMat(crops.size(),crops[0].cols,CV_64FC1);
    for (int i=0;i<crops.size();++i)
    {
        float* pFeature = featureMat.ptr<float>(i);
        const float* pCrop = crops[i].ptr<float>(0);
        std::copy(pCrop,pCrop+featureMat.cols,pFeature);
    }
    return featureMat;
}
