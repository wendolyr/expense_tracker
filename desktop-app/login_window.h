#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QtNetwork/QNetworkAccessManager>

class LoginWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit LoginWindow(QWidget* parent = nullptr);

 private:
  QByteArray prepareRequestData();
  void sendLoginRequest(const QByteArray& data);

 private slots:
  void onExitClicked();
  void onLoginClicked();
  void onRequestFinished(QNetworkReply* reply);

 private:
  QWidget* centralWidget;

  QLabel* email_label;
  QLabel* password_label;

  QLineEdit* email;
  QLineEdit* password;

  QPushButton* login;
  QPushButton* exit;

  QNetworkAccessManager* networkManager;
};

#endif  // LOGIN_WINDOW_H
