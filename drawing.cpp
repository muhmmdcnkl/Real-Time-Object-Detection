#include "drawing.h"

QImage Drawing::MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));

        const uchar *qImageBuffer = (const uchar*)mat.data;

        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {

        const uchar *qImageBuffer = (const uchar*)mat.data;

        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    return QImage();
}

QImage Drawing::MatToQImage(const cv::Mat& mat, int imageSourceIndex)
{

    QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    if(imageSourceIndex==2)
    {

        return img;
    }
    else
    {

        return img.rgbSwapped();
    }
    return QImage();
}

QRect Drawing::getImageViewerGeometry(int imgWith, int imgHeight, int frameWidth, int frameHeight)
{
    QRect QRectResult;
    int imageViewerHeight = frameHeight;
    int imageViewerWidth = frameWidth;
    if(frameWidth < imgWith)
    {
        imageViewerWidth = int(imageViewerHeight * imgWith / imgHeight);
    }
    else
    {
        imageViewerHeight = int(imageViewerWidth * imgHeight / imgWith);
    }

    QRectResult.setX(int((frameWidth - imageViewerWidth) / 2));
    QRectResult.setY(0);
    QRectResult.setWidth(imageViewerWidth);
    QRectResult.setHeight(imageViewerHeight);

    return QRectResult;
}

Mat Drawing::DrawReticle(Mat image, Rect2d box, bool isKalman) {
    Point p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12;
    Point selectionCenterPoint2;
    selectionCenterPoint2.x = int(box.x + box.width/2);
    selectionCenterPoint2.y = int(box.y + box.height/2);
    p1.x = int(box.x);
    p1.y = int(box.y);
    p2.x = int(box.x + box.width / 3);
    p2.y = int(box.y);
    p3.x = int(box.x);
    p3.y = int(box.y + box.height / 3);
    p4.x = int(box.x + box.width * 2 / 3);
    p4.y = int(box.y);
    p5.x = int(box.x + box.width);
    p5.y = int(box.y);
    p6.x = int(box.x + box.width);
    p6.y = int(box.y + box.height / 3);
    p7.x = int(box.x);
    p7.y = int(box.y + box.height);
    p8.x = int(box.x + box.width / 3);
    p8.y = int(box.y + box.height);
    p9.x = int(box.x);
    p9.y = int(box.y + box.height * 2 / 3);
    p10.x = int(box.x + box.width * 2 / 3);
    p10.y = int(box.y + box.height);
    p11.x = int(box.x + box.width);
    p11.y = int(box.y + box.height);
    p12.x = int(box.x + box.width);
    p12.y = int(box.y + box.height * 2 / 3);

    if (!isKalman)
    {
        line(image, p1, p2, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p1, p3, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p5, p4, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p5, p6, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p7, p8, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p7, p9, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p11, p10, Scalar(0, 0, 255), 2, LINE_4);
        line(image, p11, p12, Scalar(0, 0, 255), 2, LINE_4);

        selectionCenterPoint2.x = int(box.x + box.width / 2);
        selectionCenterPoint2.y = int(box.y + box.height / 2);
        circle(image, selectionCenterPoint2, 1, Scalar(0, 255, 0), 1);
    }

    else
    {
        selectionCenterPoint2.x = int(box.x + box.width / 2);
        selectionCenterPoint2.y = int(box.y + box.height / 2);
        circle(image, selectionCenterPoint2, 2, Scalar(0, 0, 255), 2);
    }
    return image;
}
Mat Drawing::DrawArticle(Mat readFrame)
{
    int frCenterX, frCenterY;
    frCenterX = readFrame.cols/2;
    frCenterY = readFrame.rows/2;

    Point c, a1, a2, a3, a4;
    a1.x = frCenterX - 30; // sol
    a1.y = frCenterY;

    a2.x = frCenterX + 30; // sağ
    a2.y = frCenterY;

    a3.x = frCenterX; // asagı
    a3.y = frCenterY - 30;

    a4.x = frCenterX; // yukarı
    a4.y = frCenterY + 30;

    c.x = frCenterX;
    c.y = frCenterY;
    Point c1, c2, c3, c4;
    c1.x = c.x - 10;
    c1.y = c.y;
    c2.x = c.x + 10;
    c2.y = c.y;
    c3.x = c.x;
    c3.y = c.y - 10;
    c4.x = c.x;
    c4.y = c.y + 10;
    line(readFrame, c1, a1, Scalar(255, 255, 255), 2, LINE_4);
    line(readFrame, c2, a2, Scalar(255, 255, 255), 2, LINE_4);
    line(readFrame, c3, a3, Scalar(255, 255, 255), 2, LINE_4);
    line(readFrame, c4, a4, Scalar(255, 255, 255), 2, LINE_4);
    return readFrame;
}

