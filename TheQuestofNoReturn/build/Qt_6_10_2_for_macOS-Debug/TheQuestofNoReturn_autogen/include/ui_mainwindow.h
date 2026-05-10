/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *mainMenuPage;
    QFrame *backgroundFrame;
    QFrame *menuOverlayFrame;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *startButton;
    QPushButton *continueButton;
    QPushButton *difficultyButton;
    QPushButton *howToPlayButton;
    QPushButton *exitButton;
    QWidget *storyPage;
    QLabel *storyBackgroundLabel;
    QWidget *characterSetupScreen;
    QLabel *characterBackground;
    QFrame *setupOverlayFrame;
    QLabel *chooseYourIdentityTitle;
    QLabel *portraitFrame;
    QLineEdit *lineEdit;
    QLabel *portraitLabel;
    QLabel *portraitFrame_2;
    QLabel *chooseYourIdentityTitle_2;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *zaraButton;
    QPushButton *karimButton;
    QPushButton *nourButton;
    QPushButton *ramiButton;
    QLabel *label;
    QLabel *descriptionLabel;
    QLabel *traitDescriptionLabel;
    QPushButton *survivorButton;
    QPushButton *scholarButton;
    QPushButton *speedsterButton;
    QPushButton *confirmButton;
    QLabel *traitIconLabel;
    QLabel *pickTraitTitle;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1276, 900);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(-80, 0, 1201, 641));
        stackedWidget->setAutoFillBackground(false);
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        mainMenuPage = new QWidget();
        mainMenuPage->setObjectName("mainMenuPage");
        mainMenuPage->setAutoFillBackground(false);
        mainMenuPage->setStyleSheet(QString::fromUtf8(""));
        backgroundFrame = new QFrame(mainMenuPage);
        backgroundFrame->setObjectName("backgroundFrame");
        backgroundFrame->setGeometry(QRect(0, 10, 1191, 821));
        backgroundFrame->setAutoFillBackground(false);
        backgroundFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    border-image: url(:/images/background.png);\n"
"}"));
        backgroundFrame->setFrameShape(QFrame::Shape::StyledPanel);
        backgroundFrame->setFrameShadow(QFrame::Shadow::Raised);
        menuOverlayFrame = new QFrame(mainMenuPage);
        menuOverlayFrame->setObjectName("menuOverlayFrame");
        menuOverlayFrame->setGeometry(QRect(-1, 10, 1191, 231));
        menuOverlayFrame->setSizeIncrement(QSize(500, 500));
        menuOverlayFrame->setAutoFillBackground(false);
        menuOverlayFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background-color: rgba(0, 0, 0, 120);\n"
"    border-radius: 20px;\n"
"}"));
        menuOverlayFrame->setFrameShape(QFrame::Shape::StyledPanel);
        menuOverlayFrame->setFrameShadow(QFrame::Shadow::Raised);
        titleLabel = new QLabel(menuOverlayFrame);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(170, 30, 691, 71));
        titleLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: rgb(255, 215, 120);\n"
"    font-size: 46px;\n"
"    font-weight: bold;\n"
"    font-family: \"Copperplate\";\n"
"    background: transparent;\n"
"}"));
        titleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        subtitleLabel = new QLabel(menuOverlayFrame);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setGeometry(QRect(370, 100, 311, 31));
        subtitleLabel->setAutoFillBackground(false);
        subtitleLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: rgb(220, 220, 220);\n"
"    font-size: 18px;\n"
"    font-family: \"Helvetica\";\n"
"    background: transparent;\n"
"}"));
        layoutWidget = new QWidget(menuOverlayFrame);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 160, 1131, 61));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        startButton = new QPushButton(layoutWidget);
        startButton->setObjectName("startButton");
        startButton->setAutoFillBackground(false);
        startButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 140, 0, 180);\n"
"    color: white;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 180, 50, 220);\n"
"}"));

        horizontalLayout->addWidget(startButton);

        continueButton = new QPushButton(layoutWidget);
        continueButton->setObjectName("continueButton");
        continueButton->setAutoFillBackground(false);
        continueButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 140, 0, 180);\n"
"    color: white;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 180, 50, 220);\n"
"}"));

        horizontalLayout->addWidget(continueButton);

        difficultyButton = new QPushButton(layoutWidget);
        difficultyButton->setObjectName("difficultyButton");
        difficultyButton->setAutoFillBackground(false);
        difficultyButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 140, 0, 180);\n"
"    color: white;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 180, 50, 220);\n"
"}"));

        horizontalLayout->addWidget(difficultyButton);

        howToPlayButton = new QPushButton(layoutWidget);
        howToPlayButton->setObjectName("howToPlayButton");
        howToPlayButton->setAutoFillBackground(false);
        howToPlayButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 140, 0, 180);\n"
"    color: white;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 180, 50, 220);\n"
"}"));

        horizontalLayout->addWidget(howToPlayButton);

        exitButton = new QPushButton(layoutWidget);
        exitButton->setObjectName("exitButton");
        exitButton->setAutoFillBackground(false);
        exitButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 140, 0, 180);\n"
"    color: white;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 180, 50, 220);\n"
"}"));

        horizontalLayout->addWidget(exitButton);

        stackedWidget->addWidget(mainMenuPage);
        storyPage = new QWidget();
        storyPage->setObjectName("storyPage");
        storyBackgroundLabel = new QLabel(storyPage);
        storyBackgroundLabel->setObjectName("storyBackgroundLabel");
        storyBackgroundLabel->setGeometry(QRect(70, 10, 1131, 611));
        storyBackgroundLabel->setAutoFillBackground(true);
        storyBackgroundLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    border-image: url(:/images/storyBackground.png);\n"
"}"));
        storyBackgroundLabel->setPixmap(QPixmap(QString::fromUtf8("storyBackground.png")));
        stackedWidget->addWidget(storyPage);
        characterSetupScreen = new QWidget();
        characterSetupScreen->setObjectName("characterSetupScreen");
        characterSetupScreen->setStyleSheet(QString::fromUtf8(""));
        characterBackground = new QLabel(characterSetupScreen);
        characterBackground->setObjectName("characterBackground");
        characterBackground->setGeometry(QRect(-130, -30, 1331, 671));
        characterBackground->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    border-image: url(:/images/characterPage.png) 0 0 0 0 stretch stretch;\n"
"}"));
        characterBackground->setPixmap(QPixmap(QString::fromUtf8("characterPage.png")));
        characterBackground->setScaledContents(true);
        setupOverlayFrame = new QFrame(characterSetupScreen);
        setupOverlayFrame->setObjectName("setupOverlayFrame");
        setupOverlayFrame->setGeometry(QRect(120, 20, 951, 61));
        setupOverlayFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background-color: rgba(0, 0, 0, 140);\n"
"    border-radius: 20px;\n"
"}"));
        setupOverlayFrame->setFrameShape(QFrame::Shape::StyledPanel);
        setupOverlayFrame->setFrameShadow(QFrame::Shadow::Raised);
        chooseYourIdentityTitle = new QLabel(setupOverlayFrame);
        chooseYourIdentityTitle->setObjectName("chooseYourIdentityTitle");
        chooseYourIdentityTitle->setGeometry(QRect(300, 10, 351, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("Brush scipt MT")});
        font.setBold(true);
        font.setItalic(true);
        chooseYourIdentityTitle->setFont(font);
        chooseYourIdentityTitle->setStyleSheet(QString::fromUtf8("color: #e8d080;\n"
"font-size: 34px;\n"
"font-weight: bold;\n"
"font-family: \"Brush scipt MT\";\n"
"background: transparent;"));
        portraitFrame = new QLabel(characterSetupScreen);
        portraitFrame->setObjectName("portraitFrame");
        portraitFrame->setGeometry(QRect(70, 90, 341, 521));
        portraitFrame->setStyleSheet(QString::fromUtf8("border: 3px solid #c8a03c;\n"
"border-radius: 14px;\n"
"background-color: rgba(20, 12, 5, 180);"));
        lineEdit = new QLineEdit(characterSetupScreen);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(100, 450, 281, 61));
        lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: rgba(20,12,5,220);\n"
"    color: #e2cfa0;\n"
"    border: 2px solid #c8a03c;\n"
"    border-radius: 10px;\n"
"    padding: 8px;\n"
"    font-size: 16px;\n"
"}"));
        portraitLabel = new QLabel(characterSetupScreen);
        portraitLabel->setObjectName("portraitLabel");
        portraitLabel->setGeometry(QRect(110, 120, 241, 201));
        portraitLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    border: 3px solid #c8a03c;\n"
"    border-radius: 16px;\n"
"    background-color: rgba(10, 10, 10, 170);\n"
"}"));
        portraitFrame_2 = new QLabel(characterSetupScreen);
        portraitFrame_2->setObjectName("portraitFrame_2");
        portraitFrame_2->setGeometry(QRect(789, 91, 341, 521));
        portraitFrame_2->setStyleSheet(QString::fromUtf8("border: 3px solid #c8a03c;\n"
"border-radius: 14px;\n"
"background-color: rgba(20, 12, 5, 180);"));
        chooseYourIdentityTitle_2 = new QLabel(characterSetupScreen);
        chooseYourIdentityTitle_2->setObjectName("chooseYourIdentityTitle_2");
        chooseYourIdentityTitle_2->setGeometry(QRect(190, 410, 121, 51));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Cinzel")});
        font1.setBold(true);
        chooseYourIdentityTitle_2->setFont(font1);
        chooseYourIdentityTitle_2->setStyleSheet(QString::fromUtf8("color: #e8d080;\n"
"font-size: 14px;\n"
"font-weight: bold;\n"
"font-family: \"Cinzel\";\n"
"background: transparent;"));
        layoutWidget1 = new QWidget(characterSetupScreen);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(80, 520, 321, 81));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        zaraButton = new QPushButton(layoutWidget1);
        zaraButton->setObjectName("zaraButton");
        zaraButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}"));

        horizontalLayout_2->addWidget(zaraButton);

        karimButton = new QPushButton(layoutWidget1);
        karimButton->setObjectName("karimButton");
        karimButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}\n"
""));

        horizontalLayout_2->addWidget(karimButton);

        nourButton = new QPushButton(layoutWidget1);
        nourButton->setObjectName("nourButton");
        nourButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}\n"
""));

        horizontalLayout_2->addWidget(nourButton);

        ramiButton = new QPushButton(layoutWidget1);
        ramiButton->setObjectName("ramiButton");
        ramiButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}"));

        horizontalLayout_2->addWidget(ramiButton);

        label = new QLabel(characterSetupScreen);
        label->setObjectName("label");
        label->setGeometry(QRect(720, 220, 58, 16));
        descriptionLabel = new QLabel(characterSetupScreen);
        descriptionLabel->setObjectName("descriptionLabel");
        descriptionLabel->setGeometry(QRect(100, 340, 281, 71));
        descriptionLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"\n"
"    background-color: rgba(35, 5, 5, 220);\n"
"\n"
"    color: #ffcf99;\n"
"\n"
"    border: 3px solid #ff5e3a;\n"
"\n"
"    border-radius: 22px;\n"
"\n"
"    padding: 14px;\n"
"\n"
"    font-size: 10px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"    font-family: \"Copperplate\";\n"
"\n"
"    qproperty-alignment: AlignCenter;\n"
"\n"
"    letter-spacing: 1px;\n"
"\n"
"}\n"
""));
        descriptionLabel->setWordWrap(true);
        traitDescriptionLabel = new QLabel(characterSetupScreen);
        traitDescriptionLabel->setObjectName("traitDescriptionLabel");
        traitDescriptionLabel->setGeometry(QRect(810, 440, 311, 161));
        traitDescriptionLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"\n"
"    background-color: rgba(35, 5, 5, 220);\n"
"\n"
"    color: #ffcf99;\n"
"\n"
"    border: 3px solid #ff5e3a;\n"
"\n"
"    border-radius: 22px;\n"
"\n"
"    padding: 14px;\n"
"\n"
"    font-size: 10px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"    font-family: \"Copperplate\";\n"
"\n"
"    qproperty-alignment: AlignCenter;\n"
"\n"
"    letter-spacing: 1px;\n"
"\n"
"}\n"
""));
        traitDescriptionLabel->setWordWrap(true);
        survivorButton = new QPushButton(characterSetupScreen);
        survivorButton->setObjectName("survivorButton");
        survivorButton->setGeometry(QRect(810, 320, 299, 42));
        survivorButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}"));
        scholarButton = new QPushButton(characterSetupScreen);
        scholarButton->setObjectName("scholarButton");
        scholarButton->setGeometry(QRect(810, 260, 299, 42));
        scholarButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}"));
        speedsterButton = new QPushButton(characterSetupScreen);
        speedsterButton->setObjectName("speedsterButton");
        speedsterButton->setGeometry(QRect(810, 380, 299, 42));
        speedsterButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: rgba(35, 20, 5, 220);\n"
"\n"
"    color: #f0d080;\n"
"\n"
"    border: 2px solid #c8a03c;\n"
"\n"
"    border-radius: 12px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"    font-size: 15px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: rgba(70, 40, 10, 240);\n"
"\n"
"    border: 2px solid #e8d080;\n"
"\n"
"}"));
        confirmButton = new QPushButton(characterSetupScreen);
        confirmButton->setObjectName("confirmButton");
        confirmButton->setGeometry(QRect(480, 270, 256, 121));
        confirmButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"\n"
"    background-color: qradialgradient(\n"
"        cx:0.5, cy:0.5,\n"
"        radius:1.2,\n"
"        fx:0.5, fy:0.5,\n"
"        stop:0 #ffcc66,\n"
"        stop:0.4 #d97706,\n"
"        stop:1 #5a2d0c\n"
"    );\n"
"\n"
"    color: #2b1200;\n"
"\n"
"    border: none;\n"
"\n"
"    border-radius: 45px;\n"
"\n"
"    font-size: 20px;\n"
"\n"
"    font-weight: bold;\n"
"\n"
"    font-family: \"Copperplate\";\n"
"\n"
"    padding: 18px;\n"
"\n"
"    \n"
"\n"
"    /* subtle inner glow feeling */\n"
"\n"
"    min-width: 220px;\n"
"\n"
"    min-height: 70px;\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"QPushButton:hover {\n"
"\n"
"    background-color: qradialgradient(\n"
"        cx:0.5, cy:0.5,\n"
"        radius:1.3,\n"
"        fx:0.5, fy:0.5,\n"
"        stop:0 #ffe29a,\n"
"        stop:0.4 #f59e0b,\n"
"        stop:1 #7c2d12\n"
"    );\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"QPushButton:pressed {\n"
"\n"
"    background-color: #4a1f00;\n"
"\n"
"    color: #ffdd99;\n"
"\n"
"}"));
        traitIconLabel = new QLabel(characterSetupScreen);
        traitIconLabel->setObjectName("traitIconLabel");
        traitIconLabel->setGeometry(QRect(800, 130, 321, 121));
        traitIconLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"\n"
"    background-color: rgba(25, 10, 5, 210);\n"
"\n"
"    border: 3px solid #d4a017;\n"
"\n"
"    border-radius: 20px;\n"
"\n"
"    padding: 10px;\n"
"\n"
"}"));
        pickTraitTitle = new QLabel(characterSetupScreen);
        pickTraitTitle->setObjectName("pickTraitTitle");
        pickTraitTitle->setGeometry(QRect(910, 90, 91, 41));
        pickTraitTitle->setFont(font1);
        pickTraitTitle->setStyleSheet(QString::fromUtf8("color: #e8d080;\n"
"font-size: 20px;\n"
"font-weight: bold;\n"
"font-family: \"Cinzel\";\n"
"background: transparent;"));
        stackedWidget->addWidget(characterSetupScreen);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "The Quest of No Return", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("MainWindow", "An Egyptian Temple Adventure", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "Start Game ", nullptr));
        continueButton->setText(QCoreApplication::translate("MainWindow", "Continue", nullptr));
        difficultyButton->setText(QCoreApplication::translate("MainWindow", "Difficulty", nullptr));
        howToPlayButton->setText(QCoreApplication::translate("MainWindow", "How to Play", nullptr));
        exitButton->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        storyBackgroundLabel->setText(QString());
        characterBackground->setText(QString());
        chooseYourIdentityTitle->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p align=\"center\"><span style=\" font-weight:700; font-style:italic;\">Choose Your Identity</span></p></body></html>", nullptr));
        portraitFrame->setText(QString());
        lineEdit->setText(QString());
        portraitLabel->setText(QString());
        portraitFrame_2->setText(QString());
        chooseYourIdentityTitle_2->setText(QCoreApplication::translate("MainWindow", "EXPLORER NAME:", nullptr));
        zaraButton->setText(QCoreApplication::translate("MainWindow", "Zara", nullptr));
        karimButton->setText(QCoreApplication::translate("MainWindow", "Karim", nullptr));
        nourButton->setText(QCoreApplication::translate("MainWindow", "Nour", nullptr));
        ramiButton->setText(QCoreApplication::translate("MainWindow", "Rami", nullptr));
        label->setText(QString());
        descriptionLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        traitDescriptionLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        survivorButton->setText(QCoreApplication::translate("MainWindow", "Survivor", nullptr));
        scholarButton->setText(QCoreApplication::translate("MainWindow", " Scholar", nullptr));
        speedsterButton->setText(QCoreApplication::translate("MainWindow", "Speedster", nullptr));
        confirmButton->setText(QCoreApplication::translate("MainWindow", "Enter The Tomb", nullptr));
        traitIconLabel->setText(QString());
        pickTraitTitle->setText(QCoreApplication::translate("MainWindow", "Pick Trait", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
