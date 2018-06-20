/********************************************************************************
** Form generated from reading UI file 'testgui.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTGUI_H
#define UI_TESTGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_testguiClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_3;
    QPushButton *fenxi;
    QPushButton *Push_start;
    QPushButton *Push_process;
    QPushButton *biaoxin;
    QPushButton *xunlian;
    QPushButton *end;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QComboBox *beijing;
    QComboBox *zhizhenshu;
    QLabel *label_3;
    QComboBox *color1;
    QLabel *label_6;
    QComboBox *xingzhuang;
    QLabel *label_4;
    QComboBox *color2;
    QLabel *label_8;
    QComboBox *qufendu;
    QGridLayout *gridLayout;
    QLabel *label_10;
    QComboBox *leixing;
    QLineEdit *zuixiaozhi;
    QLabel *label_2;
    QLineEdit *result;
    QLineEdit *buchang;
    QLabel *label_9;
    QLabel *label;
    QLabel *label_7;
    QLineEdit *result2;
    QLabel *pic_1;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *testguiClass)
    {
        if (testguiClass->objectName().isEmpty())
            testguiClass->setObjectName(QStringLiteral("testguiClass"));
        testguiClass->resize(652, 498);
        centralWidget = new QWidget(testguiClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        fenxi = new QPushButton(centralWidget);
        fenxi->setObjectName(QStringLiteral("fenxi"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fenxi->sizePolicy().hasHeightForWidth());
        fenxi->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(fenxi, 0, 4, 1, 1);

        Push_start = new QPushButton(centralWidget);
        Push_start->setObjectName(QStringLiteral("Push_start"));

        gridLayout_3->addWidget(Push_start, 0, 1, 1, 1);

        Push_process = new QPushButton(centralWidget);
        Push_process->setObjectName(QStringLiteral("Push_process"));

        gridLayout_3->addWidget(Push_process, 0, 2, 1, 1);

        biaoxin = new QPushButton(centralWidget);
        biaoxin->setObjectName(QStringLiteral("biaoxin"));

        gridLayout_3->addWidget(biaoxin, 0, 3, 1, 1);

        xunlian = new QPushButton(centralWidget);
        xunlian->setObjectName(QStringLiteral("xunlian"));

        gridLayout_3->addWidget(xunlian, 0, 0, 1, 1);

        end = new QPushButton(centralWidget);
        end->setObjectName(QStringLiteral("end"));

        gridLayout_3->addWidget(end, 0, 5, 1, 1);


        verticalLayout->addLayout(gridLayout_3);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        beijing = new QComboBox(centralWidget);
        beijing->setObjectName(QStringLiteral("beijing"));

        gridLayout_2->addWidget(beijing, 0, 1, 1, 1);

        zhizhenshu = new QComboBox(centralWidget);
        zhizhenshu->setObjectName(QStringLiteral("zhizhenshu"));

        gridLayout_2->addWidget(zhizhenshu, 0, 2, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 3, 1, 1);

        color1 = new QComboBox(centralWidget);
        color1->setObjectName(QStringLiteral("color1"));

        gridLayout_2->addWidget(color1, 0, 4, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 0, 5, 1, 1);

        xingzhuang = new QComboBox(centralWidget);
        xingzhuang->setObjectName(QStringLiteral("xingzhuang"));

        gridLayout_2->addWidget(xingzhuang, 0, 6, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 0, 7, 1, 1);

        color2 = new QComboBox(centralWidget);
        color2->setObjectName(QStringLiteral("color2"));

        gridLayout_2->addWidget(color2, 0, 8, 1, 1);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_2->addWidget(label_8, 0, 9, 1, 1);

        qufendu = new QComboBox(centralWidget);
        qufendu->setObjectName(QStringLiteral("qufendu"));

        gridLayout_2->addWidget(qufendu, 0, 10, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout->addWidget(label_10, 0, 8, 1, 1);

        leixing = new QComboBox(centralWidget);
        leixing->setObjectName(QStringLiteral("leixing"));

        gridLayout->addWidget(leixing, 0, 1, 1, 1);

        zuixiaozhi = new QLineEdit(centralWidget);
        zuixiaozhi->setObjectName(QStringLiteral("zuixiaozhi"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(zuixiaozhi->sizePolicy().hasHeightForWidth());
        zuixiaozhi->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(zuixiaozhi, 0, 5, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 4, 1, 1);

        result = new QLineEdit(centralWidget);
        result->setObjectName(QStringLiteral("result"));

        gridLayout->addWidget(result, 0, 7, 1, 1);

        buchang = new QLineEdit(centralWidget);
        buchang->setObjectName(QStringLiteral("buchang"));

        gridLayout->addWidget(buchang, 0, 3, 1, 1);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 0, 6, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 2, 1, 1);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        result2 = new QLineEdit(centralWidget);
        result2->setObjectName(QStringLiteral("result2"));

        gridLayout->addWidget(result2, 0, 9, 1, 1);


        verticalLayout->addLayout(gridLayout);

        pic_1 = new QLabel(centralWidget);
        pic_1->setObjectName(QStringLiteral("pic_1"));
        sizePolicy.setHeightForWidth(pic_1->sizePolicy().hasHeightForWidth());
        pic_1->setSizePolicy(sizePolicy);
        pic_1->setMinimumSize(QSize(291, 241));
        pic_1->setScaledContents(true);

        verticalLayout->addWidget(pic_1);

        testguiClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(testguiClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 652, 23));
        testguiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(testguiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        testguiClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(zuixiaozhi);
        label->setBuddy(buchang);
#endif // QT_NO_SHORTCUT

        retranslateUi(testguiClass);
        QObject::connect(biaoxin, SIGNAL(clicked()), testguiClass, SLOT(flag3()));
        QObject::connect(Push_start, SIGNAL(clicked()), testguiClass, SLOT(flag1()));
        QObject::connect(Push_process, SIGNAL(clicked()), testguiClass, SLOT(flag2()));
        QObject::connect(zhizhenshu, SIGNAL(activated(int)), testguiClass, SLOT(zhizhennum()));
        QObject::connect(zuixiaozhi, SIGNAL(editingFinished()), testguiClass, SLOT(minnum()));
        QObject::connect(buchang, SIGNAL(editingFinished()), testguiClass, SLOT(buchang()));
        QObject::connect(color1, SIGNAL(activated(int)), testguiClass, SLOT(color1()));
        QObject::connect(color2, SIGNAL(activated(int)), testguiClass, SLOT(color2()));
        QObject::connect(beijing, SIGNAL(activated(int)), testguiClass, SLOT(withe()));
        QObject::connect(xingzhuang, SIGNAL(activated(int)), testguiClass, SLOT(xingzhuang()));
        QObject::connect(leixing, SIGNAL(activated(int)), testguiClass, SLOT(leixing()));
        QObject::connect(qufendu, SIGNAL(activated(int)), testguiClass, SLOT(qufendu()));
        QObject::connect(fenxi, SIGNAL(clicked()), testguiClass, SLOT(fenxi()));
        QObject::connect(xunlian, SIGNAL(clicked()), testguiClass, SLOT(start()));
        QObject::connect(result, SIGNAL(returnPressed()), testguiClass, SLOT(result()));
        QObject::connect(end, SIGNAL(clicked()), testguiClass, SLOT(close()));

        QMetaObject::connectSlotsByName(testguiClass);
    } // setupUi

    void retranslateUi(QMainWindow *testguiClass)
    {
        testguiClass->setWindowTitle(QApplication::translate("testguiClass", "testgui", 0));
        fenxi->setText(QApplication::translate("testguiClass", "\345\210\206\346\236\220", 0));
        Push_start->setText(QApplication::translate("testguiClass", "\351\205\215\345\207\206\345\214\272\345\237\237", 0));
        Push_process->setText(QApplication::translate("testguiClass", "\346\240\207\345\210\273\345\272\246", 0));
        biaoxin->setText(QApplication::translate("testguiClass", "\346\240\207\350\241\250\345\277\203", 0));
        xunlian->setText(QApplication::translate("testguiClass", "\350\256\255\347\273\203", 0));
        end->setText(QApplication::translate("testguiClass", "\347\273\223\346\235\237", 0));
        label_5->setText(QApplication::translate("testguiClass", "\350\241\250\350\203\214\346\231\257", 0));
        beijing->clear();
        beijing->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\347\231\275", 0)
         << QApplication::translate("testguiClass", "\351\235\236\347\231\275", 0)
        );
        zhizhenshu->clear();
        zhizhenshu->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\345\215\225\346\214\207\351\222\210", 0)
         << QApplication::translate("testguiClass", "\345\217\214\346\214\207\351\222\210", 0)
        );
        label_3->setText(QApplication::translate("testguiClass", "\346\214\207\351\222\2101\351\242\234\350\211\262", 0));
        color1->clear();
        color1->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\351\273\221", 0)
         << QApplication::translate("testguiClass", "\347\231\275", 0)
         << QApplication::translate("testguiClass", "\347\272\242", 0)
         << QApplication::translate("testguiClass", "\351\273\204", 0)
        );
        label_6->setText(QApplication::translate("testguiClass", "\346\214\207\351\222\210\345\275\242\347\212\266", 0));
        xingzhuang->clear();
        xingzhuang->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\347\273\206", 0)
         << QApplication::translate("testguiClass", "\346\255\243\345\270\270", 0)
        );
        label_4->setText(QApplication::translate("testguiClass", "\346\214\207\351\222\2102\351\242\234\350\211\262", 0));
        color2->clear();
        color2->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\351\273\221", 0)
         << QApplication::translate("testguiClass", "\347\231\275", 0)
         << QApplication::translate("testguiClass", "\347\272\242", 0)
         << QApplication::translate("testguiClass", "\351\273\204", 0)
        );
        label_8->setText(QApplication::translate("testguiClass", "\346\250\241\347\263\212\345\272\246", 0));
        qufendu->clear();
        qufendu->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\346\270\205\346\231\260", 0)
         << QApplication::translate("testguiClass", "\346\250\241\347\263\212", 0)
         << QApplication::translate("testguiClass", "\345\205\266\345\256\203", 0)
        );
        label_10->setText(QApplication::translate("testguiClass", "\346\214\207\351\222\2102\345\200\274", 0));
        leixing->clear();
        leixing->insertItems(0, QStringList()
         << QApplication::translate("testguiClass", "\347\237\251\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\234\206\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\234\206\347\216\257", 0)
         << QApplication::translate("testguiClass", "\345\234\206\347\216\257+\347\237\251\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\234\206\345\275\242+\347\237\251\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\234\206\345\275\242+2\344\270\252\347\237\251\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\234\206\345\275\242+3\344\270\252\347\237\251\345\275\242", 0)
         << QApplication::translate("testguiClass", "\345\205\266\345\256\203", 0)
        );
        zuixiaozhi->setText(QApplication::translate("testguiClass", "0", 0));
        label_2->setText(QApplication::translate("testguiClass", "\346\234\200\345\260\217\345\200\274", 0));
#ifndef QT_NO_TOOLTIP
        buchang->setToolTip(QApplication::translate("testguiClass", "<html><head/><body><p><br/></p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        buchang->setText(QApplication::translate("testguiClass", "1", 0));
        label_9->setText(QApplication::translate("testguiClass", "\346\214\207\351\222\2101\345\200\274", 0));
        label->setText(QApplication::translate("testguiClass", "\346\255\245\351\225\277", 0));
        label_7->setText(QApplication::translate("testguiClass", "\350\241\250\347\233\230\347\261\273\345\236\213", 0));
        pic_1->setText(QApplication::translate("testguiClass", "                                   \347\202\271\345\207\273\350\256\255\347\273\203\345\274\200\345\247\213", 0));
    } // retranslateUi

};

namespace Ui {
    class testguiClass: public Ui_testguiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTGUI_H
