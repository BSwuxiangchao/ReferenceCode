#include "PromptInfoWidget.h"
#include "BgiLogger.h"


PromptInfoWidget::PromptInfoWidget(QWidget* parent)
    :m_pParentWidget(parent)
{
    QLabel* m_pPromptInfoLabel = new QLabel(m_pParentWidget);
    // 大小
    m_pPromptInfoLabel->resize(m_pParentWidget->size());
    // 对齐居中
    m_pPromptInfoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 
	// 颜色
	QPalette label_pale;
	label_pale.setColor(QPalette::WindowText, QColor(0, 172, 210));  //与LOGO一样的颜色
	m_pPromptInfoLabel->setPalette(label_pale);

    // 字体
    QFont font;
    font.setFamily("华文楷体");
    font.setPointSize(48);
    m_pPromptInfoLabel->setFont(font);
    m_pPromptInfoLabel->setText("涂布机主控软件");
}

PromptInfoWidget::~PromptInfoWidget()
{

}
