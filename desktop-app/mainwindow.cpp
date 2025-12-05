#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "register_window.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    QPushButton *b = new QPushButton("but1", this);
    l->addWidget(b);
    setLayout(l);

    connect(b, &QPushButton::clicked, this, &MainWindow::Login);
}

void MainWindow::Login() {
    RegisterWindow *login_window = new RegisterWindow(this);
    // login_window->AddTextField("Full name");
    // login_window->AddTextField("Login");
    // login_window->AddTextField("Password");
    // login_window->AddTextField("Confirm password");
    login_window->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
