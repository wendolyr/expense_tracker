#include "mainwindow.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

#include "./ui_mainwindow.h"
#include "login_window.h"
#include "register_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Получаем центральный виджет
  QWidget* centralWidget = this->centralWidget();

  // Если центрального виджета нет, создаем его
  if (!centralWidget) {
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
  }

  QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

  QPushButton* log = new QPushButton("Login", this);
  QPushButton* reg = new QPushButton("Registration", this);

  log->setFixedSize(100, 40);
  reg->setFixedSize(100, 40);

  QHBoxLayout* hLayout = new QHBoxLayout();
  QVBoxLayout* vLayout = new QVBoxLayout();

  vLayout->addWidget(log);
  vLayout->addWidget(reg);
  vLayout->setSpacing(15);

  hLayout->addStretch();
  hLayout->addLayout(vLayout);
  hLayout->addStretch();

  mainLayout->addStretch();
  mainLayout->addLayout(hLayout);
  mainLayout->addStretch();

  connect(reg, &QPushButton::clicked, this, &MainWindow::openRegWindow);
  connect(log, &QPushButton::clicked, this, &MainWindow::openLoginWindow);
}

void MainWindow::openRegWindow() {
  RegisterWindow* reg_window = new RegisterWindow(this);
  reg_window->show();
}

void MainWindow::openLoginWindow() {
  LoginWindow* login_window = new LoginWindow(this);
  login_window->show();
}

MainWindow::~MainWindow() { delete ui; }
