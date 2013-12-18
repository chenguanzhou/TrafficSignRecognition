#include "SignRecognitionToolkit.h"
#include <iostream>
#include <ctime>
#include <QMessageBox>
#include <QFileInfo>

SignRecognitionToolkit::SignRecognitionToolkit()
{
}

void SignRecognitionToolkit::GetTestImageCrop(const cv::Mat &inputImage, std::vector<cv::Mat> &vecCropImage)
{
    cv::Mat hsv;
    cv::cvtColor(inputImage,hsv,cv::COLOR_BGR2Lab);
    //    std::vector<cv::Mat> splitMat;
    //    cv::split(hsv,splitMat);
    //    uchar *p,*q;
    //    cv::Mat red = splitMat[1].clone();
    //    for(int i = 0; i < red.rows; ++i)
    //    {
    //        p = red.ptr<uchar>(i);
    //        q = splitMat[2].ptr<uchar>(i);
    //        for (int j=0;j<red.cols;++j)
    //        {
    //            if (p[j]>160)
    //                p[j] = 255;
    //            else
    //                p[j] = 0;
    //        }
    //    }

    std::vector<cv::Mat> splitMat;
    cv::split(inputImage,splitMat);
    cv::Mat red = splitMat[2].clone();
    uchar *p,*r,*g,*b;
    for(int i = 0; i < red.rows; ++i)
    {
        p = red.ptr<uchar>(i);
        r = splitMat[2].ptr<uchar>(i);
        g = splitMat[1].ptr<uchar>(i);
        b = splitMat[0].ptr<uchar>(i);
        for (int j=0;j<red.cols;++j)
        {
            if (r[j]>2*g[j]&&r[j]>2*b[j])
                p[j] = 255;
            else
                p[j] = 0;
        }
    }



    //    cv::Canny(red,red,50,200);


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
            CvScalar color;
            color.val[0]=255;color.val[1]=255;color.val[2]=0;
            cv::ellipse(red,ellipse,color,2);
        }
    }

//    cv::imshow("red",red);

    qDebug()<<"minEllipse.size():"<<minEllipse.size();
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
        imageName = QDir::fromNativeSeparators(inputFileList[fileNameID]);
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
    return listMat;
}

cv::Mat SignRecognitionToolkit::GetCropFeature(const cv::Mat &crop, FeatureMethod method)
{    
    if (method == PAPER_63)
    {
        cv::Mat feature(1,63,CV_32FC1);
//        std::vector<cv::Mat> vecCrop;
//        cv::split(crop,vecCrop);\

        cv::Mat gray(30,30,CV_8UC1);
        float r(0),g(0),b(0);
        for (int i=0;i<30;++i)
        {
            for (int j=0;j<30;++j)
            {
                cv::Vec3b pixel = crop.at<cv::Vec3b>(i, j);
                r += pixel[2];
                g += pixel[1];
                b += pixel[0];
                gray = pixel[2]*0.49+pixel[1]*0.29+pixel[2]*0.22;
            }
        }

        feature.at<float>(0,0) = r/900./256.;
        feature.at<float>(0,1) = g/900./256.;
        feature.at<float>(0,2) = b/900./256.;

//        feature.at<float>(0,0) = cv::mean(vecCrop[2])[0]/256.;//MR
//        feature.at<float>(0,1) = cv::mean(vecCrop[1])[0]/256.;//MG
//        feature.at<float>(0,2) = cv::mean(vecCrop[0])[0]/256.;//MB

//        cv::Mat gray = vecCrop[2]*0.49 + vecCrop[1]*0.29 + vecCrop[2]*0.22;

        float treshold = cv::mean(gray)[0];

        for (int i=0;i<30;++i)
        {
            float vh = 0;
            for (int j=0;j<30;++j)
            {
                uchar dn = gray.at<uchar>(i,j);
                if (dn>treshold)
                    vh += dn;
            }
            feature.at<float>(0,i+3) = vh/30./256.;
        }

        for (int i=0;i<30;++i)
        {
            if (i==29)
            {
                int cgz=0;
            }
            float vh = 0;
            for (int j=0;j<30;++j)
            {
                uchar dn = gray.at<uchar>(j,i);
                if (dn>treshold)
                    vh += dn;
            }
            feature.at<float>(0,i+33) = vh/30./256.;
        }
        return feature;
    }

    return cv::Mat();
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
