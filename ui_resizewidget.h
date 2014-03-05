/********************************************************************************
** Form generated from reading UI file 'resizewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESIZEWIDGET_H
#define UI_RESIZEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResizeWidget
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout1;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout;
    QSlider *brushSizeSlider;
    QRadioButton *retainRadio;
    QLabel *label_2;
    QLabel *label;
    QRadioButton *removeRadio;
    QSlider *brushWeightSlider;
    QRadioButton *clearRadio;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *clearButton;
    QGroupBox *removeGroup;
    QGridLayout *gridLayout1;
    QComboBox *removeMode;
    QLabel *label_5;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    QPushButton *removeButton;
    QCheckBox *iterateCheckBox;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout2;
    QLabel *convLabel;
    QComboBox *edgeDetector;
    QCheckBox *hdCheckBox;
    QCheckBox *energyCheckBox;
    QLabel *weightScaleLabel;
    QLineEdit *weightScaleLineEdit;
    QGroupBox *groupBox;
    QGridLayout *gridLayout3;
    QLabel *widthLabel;
    QLineEdit *widthLineEdit;
    QLabel *pixel1Label;
    QLabel *heightLabel;
    QLineEdit *heightLineEdit;
    QLabel *pixel2Label;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem2;
    QPushButton *resizeButton;
    QSpacerItem *spacerItem3;

    void setupUi(QWidget *ResizeWidget)
    {
        if (ResizeWidget->objectName().isEmpty())
            ResizeWidget->setObjectName(QStringLiteral("ResizeWidget"));
        ResizeWidget->resize(315, 583);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ResizeWidget->sizePolicy().hasHeightForWidth());
        ResizeWidget->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(ResizeWidget);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        groupBox_2 = new QGroupBox(ResizeWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        vboxLayout1 = new QVBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
        vboxLayout1->setContentsMargins(6, 6, 6, 6);
        vboxLayout1->setObjectName(QStringLiteral("vboxLayout1"));
        groupBox_4 = new QGroupBox(groupBox_2);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout = new QGridLayout(groupBox_4);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        brushSizeSlider = new QSlider(groupBox_4);
        brushSizeSlider->setObjectName(QStringLiteral("brushSizeSlider"));
        brushSizeSlider->setMinimum(3);
        brushSizeSlider->setMaximum(63);
        brushSizeSlider->setSingleStep(4);
        brushSizeSlider->setPageStep(8);
        brushSizeSlider->setValue(15);
        brushSizeSlider->setSliderPosition(15);
        brushSizeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(brushSizeSlider, 3, 2, 1, 1);

        retainRadio = new QRadioButton(groupBox_4);
        retainRadio->setObjectName(QStringLiteral("retainRadio"));
        retainRadio->setChecked(true);

        gridLayout->addWidget(retainRadio, 0, 0, 1, 3);

        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        label = new QLabel(groupBox_4);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 3, 0, 1, 2);

        removeRadio = new QRadioButton(groupBox_4);
        removeRadio->setObjectName(QStringLiteral("removeRadio"));

        gridLayout->addWidget(removeRadio, 1, 0, 1, 3);

        brushWeightSlider = new QSlider(groupBox_4);
        brushWeightSlider->setObjectName(QStringLiteral("brushWeightSlider"));
        brushWeightSlider->setMinimum(1);
        brushWeightSlider->setMaximum(100);
        brushWeightSlider->setSingleStep(5);
        brushWeightSlider->setPageStep(20);
        brushWeightSlider->setValue(100);
        brushWeightSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(brushWeightSlider, 4, 1, 1, 2);

        clearRadio = new QRadioButton(groupBox_4);
        clearRadio->setObjectName(QStringLiteral("clearRadio"));

        gridLayout->addWidget(clearRadio, 2, 0, 1, 3);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        clearButton = new QPushButton(groupBox_4);
        clearButton->setObjectName(QStringLiteral("clearButton"));

        hboxLayout->addWidget(clearButton);


        gridLayout->addLayout(hboxLayout, 5, 0, 1, 3);


        vboxLayout1->addWidget(groupBox_4);

        removeGroup = new QGroupBox(groupBox_2);
        removeGroup->setObjectName(QStringLiteral("removeGroup"));
        removeGroup->setEnabled(true);
        gridLayout1 = new QGridLayout(removeGroup);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        removeMode = new QComboBox(removeGroup);
        removeMode->setObjectName(QStringLiteral("removeMode"));
        removeMode->setMaxVisibleItems(3);

        gridLayout1->addWidget(removeMode, 0, 1, 1, 1);

        label_5 = new QLabel(removeGroup);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout1->addWidget(label_5, 0, 0, 1, 1);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        removeButton = new QPushButton(removeGroup);
        removeButton->setObjectName(QStringLiteral("removeButton"));

        hboxLayout1->addWidget(removeButton);


        gridLayout1->addLayout(hboxLayout1, 2, 0, 1, 2);

        iterateCheckBox = new QCheckBox(removeGroup);
        iterateCheckBox->setObjectName(QStringLiteral("iterateCheckBox"));

        gridLayout1->addWidget(iterateCheckBox, 1, 0, 1, 2);


        vboxLayout1->addWidget(removeGroup);


        vboxLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(ResizeWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout2 = new QGridLayout(groupBox_3);
        gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
        convLabel = new QLabel(groupBox_3);
        convLabel->setObjectName(QStringLiteral("convLabel"));

        gridLayout2->addWidget(convLabel, 0, 0, 1, 1);

        edgeDetector = new QComboBox(groupBox_3);
        edgeDetector->setObjectName(QStringLiteral("edgeDetector"));

        gridLayout2->addWidget(edgeDetector, 0, 1, 1, 2);

        hdCheckBox = new QCheckBox(groupBox_3);
        hdCheckBox->setObjectName(QStringLiteral("hdCheckBox"));

        gridLayout2->addWidget(hdCheckBox, 1, 0, 1, 2);

        energyCheckBox = new QCheckBox(groupBox_3);
        energyCheckBox->setObjectName(QStringLiteral("energyCheckBox"));

        gridLayout2->addWidget(energyCheckBox, 1, 2, 1, 1);

        weightScaleLabel = new QLabel(groupBox_3);
        weightScaleLabel->setObjectName(QStringLiteral("weightScaleLabel"));

        gridLayout2->addWidget(weightScaleLabel, 2, 0, 1, 1);

        weightScaleLineEdit = new QLineEdit(groupBox_3);
        weightScaleLineEdit->setObjectName(QStringLiteral("weightScaleLineEdit"));

        gridLayout2->addWidget(weightScaleLineEdit, 2, 1, 1, 2);


        vboxLayout->addWidget(groupBox_3);

        groupBox = new QGroupBox(ResizeWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout3 = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
        gridLayout3->setSpacing(6);
#endif
        gridLayout3->setContentsMargins(6, 6, 6, 6);
        gridLayout3->setObjectName(QStringLiteral("gridLayout3"));
        widthLabel = new QLabel(groupBox);
        widthLabel->setObjectName(QStringLiteral("widthLabel"));

        gridLayout3->addWidget(widthLabel, 0, 0, 1, 1);

        widthLineEdit = new QLineEdit(groupBox);
        widthLineEdit->setObjectName(QStringLiteral("widthLineEdit"));

        gridLayout3->addWidget(widthLineEdit, 0, 1, 1, 1);

        pixel1Label = new QLabel(groupBox);
        pixel1Label->setObjectName(QStringLiteral("pixel1Label"));

        gridLayout3->addWidget(pixel1Label, 0, 2, 1, 1);

        heightLabel = new QLabel(groupBox);
        heightLabel->setObjectName(QStringLiteral("heightLabel"));

        gridLayout3->addWidget(heightLabel, 1, 0, 1, 1);

        heightLineEdit = new QLineEdit(groupBox);
        heightLineEdit->setObjectName(QStringLiteral("heightLineEdit"));

        gridLayout3->addWidget(heightLineEdit, 1, 1, 1, 1);

        pixel2Label = new QLabel(groupBox);
        pixel2Label->setObjectName(QStringLiteral("pixel2Label"));

        gridLayout3->addWidget(pixel2Label, 1, 2, 1, 1);

        hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout2->setSpacing(6);
#endif
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        hboxLayout2->setObjectName(QStringLiteral("hboxLayout2"));
        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem2);

        resizeButton = new QPushButton(groupBox);
        resizeButton->setObjectName(QStringLiteral("resizeButton"));
        resizeButton->setDefault(true);

        hboxLayout2->addWidget(resizeButton);


        gridLayout3->addLayout(hboxLayout2, 2, 0, 1, 3);


        vboxLayout->addWidget(groupBox);

        spacerItem3 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem3);

        QWidget::setTabOrder(retainRadio, removeRadio);
        QWidget::setTabOrder(removeRadio, brushSizeSlider);
        QWidget::setTabOrder(brushSizeSlider, widthLineEdit);
        QWidget::setTabOrder(widthLineEdit, heightLineEdit);
        QWidget::setTabOrder(heightLineEdit, resizeButton);
        QWidget::setTabOrder(resizeButton, clearButton);

        retranslateUi(ResizeWidget);
        QObject::connect(widthLineEdit, SIGNAL(returnPressed()), resizeButton, SLOT(click()));
        QObject::connect(heightLineEdit, SIGNAL(returnPressed()), resizeButton, SLOT(click()));
        QObject::connect(hdCheckBox, SIGNAL(toggled(bool)), removeGroup, SLOT(setDisabled(bool)));

        removeMode->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ResizeWidget);
    } // setupUi

    void retranslateUi(QWidget *ResizeWidget)
    {
        ResizeWidget->setWindowTitle(QApplication::translate("ResizeWidget", "Form", 0));
        groupBox_2->setTitle(QApplication::translate("ResizeWidget", "Retain/Remove", 0));
        groupBox_4->setTitle(QApplication::translate("ResizeWidget", "Mark Areas with Mouse", 0));
        retainRadio->setText(QApplication::translate("ResizeWidget", "Mark area for retaining", 0));
        label_2->setText(QApplication::translate("ResizeWidget", "Brush Weight:", 0));
        label->setText(QApplication::translate("ResizeWidget", "Brush Size:", 0));
        removeRadio->setText(QApplication::translate("ResizeWidget", "Mark area for removal", 0));
        clearRadio->setText(QApplication::translate("ResizeWidget", "Clear marked area", 0));
        clearButton->setText(QApplication::translate("ResizeWidget", "&Clear Marked Area", 0));
        removeGroup->setTitle(QApplication::translate("ResizeWidget", "Remove Marked Areas", 0));
        removeMode->clear();
        removeMode->insertItems(0, QStringList()
         << QApplication::translate("ResizeWidget", "Automatic", 0)
         << QApplication::translate("ResizeWidget", "Vertical", 0)
         << QApplication::translate("ResizeWidget", "Horizontal", 0)
        );
        label_5->setText(QApplication::translate("ResizeWidget", "Mode:", 0));
        removeButton->setText(QApplication::translate("ResizeWidget", "Re&move", 0));
        iterateCheckBox->setText(QApplication::translate("ResizeWidget", "Iterate Until All Marked Removed", 0));
        groupBox_3->setTitle(QApplication::translate("ResizeWidget", "Other Parameters", 0));
        convLabel->setText(QApplication::translate("ResizeWidget", "Edge Detector:", 0));
        edgeDetector->clear();
        edgeDetector->insertItems(0, QStringList()
         << QApplication::translate("ResizeWidget", "V1", 0)
         << QApplication::translate("ResizeWidget", "V_SQUARE", 0)
         << QApplication::translate("ResizeWidget", "Prewitt", 0)
         << QApplication::translate("ResizeWidget", "Sobel", 0)
         << QApplication::translate("ResizeWidget", "Laplacian", 0)
        );
#ifndef QT_NO_TOOLTIP
        hdCheckBox->setToolTip(QApplication::translate("ResizeWidget", "Use the Forward Energy algorithm, which can greatly increase quality in some images. Comes at a minor perfromance cost.", 0));
#endif // QT_NO_TOOLTIP
        hdCheckBox->setText(QApplication::translate("ResizeWidget", "HD Quality", 0));
        energyCheckBox->setText(QApplication::translate("ResizeWidget", "Forward Energy", 0));
        weightScaleLabel->setText(QApplication::translate("ResizeWidget", "Weight Scale:", 0));
        weightScaleLineEdit->setText(QApplication::translate("ResizeWidget", "5000", 0));
        groupBox->setTitle(QApplication::translate("ResizeWidget", "Resize Dimensions", 0));
        widthLabel->setText(QApplication::translate("ResizeWidget", "Width:", 0));
        pixel1Label->setText(QApplication::translate("ResizeWidget", "pixels", 0));
        heightLabel->setText(QApplication::translate("ResizeWidget", "Height:", 0));
        pixel2Label->setText(QApplication::translate("ResizeWidget", "pixels", 0));
        resizeButton->setText(QApplication::translate("ResizeWidget", "&Resize", 0));
    } // retranslateUi

};

namespace Ui {
    class ResizeWidget: public Ui_ResizeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESIZEWIDGET_H
