#ifndef REGISTER_WINDOW_H
#define REGISTER_WINDOW_H

#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QtNetwork/QNetworkAccessManager>

class RegisterWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit RegisterWindow(QWidget* parent = nullptr);
  void AddTextField(QString name);

 private:
  QByteArray prepareRequestData();
  void sendRegistrationRequest(const QByteArray& data);

 private slots:
  void onExitClicked();
  void onCreateClicked();
  void checkPasswords();
  void onRequestFinished(QNetworkReply* reply);

 private:
  QWidget* centralWidget;

  QLabel* full_name_label;
  QLabel* email_label;
  QLabel* password_label;
  QLabel* confirm_password_label;
  QLabel* passwordMatchLabel;

  QLineEdit* full_name;
  QLineEdit* email;
  QLineEdit* password;
  QLineEdit* confirm_password;

  QPushButton* create;
  QPushButton* exit;

  QNetworkAccessManager* networkManager;
};

#endif  // REGISTER_WINDOW_H
