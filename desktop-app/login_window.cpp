#include "login_window.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QtNetwork/QNetworkReply>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setWindowTitle("Registration");
    resize(400, 300);
    setWindowModality(Qt::ApplicationModal);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    email_label = new QLabel("Email:", centralWidget);
    password_label = new QLabel("Password:", centralWidget);

    email = new QLineEdit(centralWidget);
    password = new QLineEdit(centralWidget);

    password->setPlaceholderText("Enter password");
    password->setEchoMode(QLineEdit::Password);

    login = new QPushButton("Login", centralWidget);
    exit = new QPushButton("Exit", centralWidget);

    QGridLayout *fieldsLayout = new QGridLayout();


    fieldsLayout->addWidget(email_label, 1, 0);
    fieldsLayout->addWidget(email, 1, 1);

    fieldsLayout->addWidget(password_label, 2, 0);
    fieldsLayout->addWidget(password, 2, 1);

    // Layout для кнопок
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();                    // Растягивающийся элемент слева
    buttonsLayout->addWidget(login);              // Кнопка 1
    buttonsLayout->addSpacing(10);                 // Промежуток между кнопками
    buttonsLayout->addWidget(exit);                // Кнопка 2
    buttonsLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(fieldsLayout);
    mainLayout->addStretch(); // Добавляем растягивающееся пространство
    mainLayout->addLayout(buttonsLayout);

    connect(exit, &QPushButton::clicked, this, &LoginWindow::onExitClicked);
    connect(login, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished,
            this, &LoginWindow::onRequestFinished);
}

void LoginWindow::onExitClicked() {
    close();
}

void LoginWindow::onLoginClicked() {
    if (email->text().isEmpty() ||
        password->text().isEmpty()) {
        QMessageBox::warning(this, "Empty Fields",
                             "Please fill in all required fields.");
        return;
    }

    QByteArray requestData = prepareRequestData();

    sendLoginRequest(requestData);

    // Меняем текст кнопки чтобы показать что идет отправка
    login->setText("Sending...");
    login->setEnabled(false);
}

QByteArray LoginWindow::prepareRequestData(){
    QJsonObject json;

    json["email"] = email->text();
    json["password"] = password->text();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    return data;
}


void LoginWindow::sendLoginRequest(const QByteArray &data) {
    // Создаем URL (используем твой адрес сервера)
    QUrl url("http://localhost:8080/login");

    // Создаем запрос
    QNetworkRequest request(url);

    // Устанавливаем заголовки (как в curl команде)
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Отправляем POST запрос с данными
    networkManager->post(request, data);

    qDebug() << "Sending POST request to:" << url.toString();
    qDebug() << "Data:" << data;
}

void LoginWindow::onRequestFinished(QNetworkReply *reply)
{
    login->setText("Login");
    login->setEnabled(true);

    QNetworkReply::NetworkError error = reply->error();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (error != QNetworkReply::NoError &&
        error != QNetworkReply::AuthenticationRequiredError &&
        error != QNetworkReply::ContentAccessDenied &&
        error != QNetworkReply::ContentNotFoundError &&
        error != QNetworkReply::ProtocolInvalidOperationError)
    {
        QMessageBox::critical(this, "Network Error",
                              "Failed to connect to server:\n" + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);

    qDebug() << "Response status:" << statusCode;
    qDebug() << "Response data:" << responseString;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

    if (jsonResponse.isObject()) {
        QJsonObject jsonObject = jsonResponse.object();

        if (statusCode == 200) {
            QString userId = jsonObject["user_id"].toString();
            QString userEmail = jsonObject["email"].toString();
            QString accessToken = jsonObject["access_token"].toString();
            QString refreshToken = jsonObject["refresh_token"].toString();

            QString displayAccessToken = accessToken.length() > 20
                                             ? accessToken.left(20) + "..."
                                             : accessToken;

            QMessageBox::information(this, "Login Successful", QString("Login successful!"));

            qDebug() << "Access token:" << accessToken;
            qDebug() << "Refresh token:" << refreshToken;

            close();

        } else if (statusCode == 401 || statusCode == 400) {
            // Unauthorized - неверные учетные данные
            QString errorMessage = jsonObject.contains("error")
                                       ? jsonObject["error"].toString()
                                       : "Invalid email or password";

            QMessageBox::warning(this, "Login Failed", "Invalid email or password");

        } else {
            // Другие ошибки сервера
            QString errorMessage = jsonObject.contains("error")
                                       ? jsonObject["error"].toString()
                                       : "Unknown server error";

            QMessageBox::warning(this, "Login Failed",
                                 QString("Server error (code %1):\n%2")
                                     .arg(statusCode)
                                     .arg(errorMessage));
        }
    } else {
        QMessageBox::warning(this, "Server Error",
                             "Invalid JSON response from server:\n" + responseString);
    }

    reply->deleteLater();
}
