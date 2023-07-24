#include "LogoWidget.h"
#include "BgiLogger.h"
#include "BgiFileTools.h"



LogoWidget::LogoWidget(QWidget* parent)
    :m_pParentWidget(parent)
{
    m_pLogoLabel = new QLabel(m_pParentWidget);
    m_pLogoLabel->move(0, 0);
    m_pLogoLabel->resize(m_pParentWidget->width(), m_pParentWidget->height());
       
    QString data_path = "";
    BgiFileTools::GetDataDirPath(data_path);
    QString fileName = data_path + "//Data//Pic//BgiLogo.jpeg";

    //m_pLogoLabel->setStyleSheet(QString::fromUtf8("border-image:url(E://STOmics//Data//Pic//BgiLogo.jpeg)"));

    QImage Image;
    Image.load(fileName);
    QPixmap pixmap = QPixmap::fromImage(Image);
    int with = m_pLogoLabel->width();
    int height = m_pLogoLabel->height();
    //QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    //m_pLogoLabel->setPixmap(fitpixmap);
    QPalette palette;
    palette.setBrush(m_pLogoLabel->backgroundRole(), QBrush(fitpixmap));
    m_pLogoLabel->setPalette(palette);
    m_pLogoLabel->setMask(fitpixmap.mask());
    m_pLogoLabel->setAutoFillBackground(true);
    m_pLogoLabel->setWindowOpacity(0.9);//透明度
    

}

LogoWidget::~LogoWidget()
{

}
