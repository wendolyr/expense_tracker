#include "register_window.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QtNetwork/QNetworkReply>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setWindowTitle("Registration");
    resize(400, 300);
    setWindowModality(Qt::ApplicationModal);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    full_name_label = new QLabel("Full Name:", centralWidget);
    email_label = new QLabel("Email:", centralWidget);
    password_label = new QLabel("Password:", centralWidget);
    confirm_password_label = new QLabel("Confirm Password:", centralWidget);
    passwordMatchLabel = new QLabel("", centralWidget);

    full_name = new QLineEdit(centralWidget);
    email = new QLineEdit(centralWidget);
    password = new QLineEdit(centralWidget);
    confirm_password = new QLineEdit(centralWidget);

    password->setPlaceholderText("Enter password");
    confirm_password->setPlaceholderText("Confirm password");
    password->setEchoMode(QLineEdit::Password);
    confirm_password->setEchoMode(QLineEdit::Password);

    create = new QPushButton("Create Account", centralWidget);
    exit = new QPushButton("Exit", centralWidget);

    QGridLayout *fieldsLayout = new QGridLayout();

    // Добавляем метки и поля в сетку
    fieldsLayout->addWidget(full_name_label, 0, 0);
    fieldsLayout->addWidget(full_name, 0, 1);

    fieldsLayout->addWidget(email_label, 1, 0);
    fieldsLayout->addWidget(email, 1, 1);

    fieldsLayout->addWidget(password_label, 2, 0);
    fieldsLayout->addWidget(password, 2, 1);

    fieldsLayout->addWidget(confirm_password_label, 3, 0);
    fieldsLayout->addWidget(confirm_password, 3, 1);

    fieldsLayout->addWidget(passwordMatchLabel, 4, 0, 1, 2); // row=4, column=0, rowSpan=1, columnSpan=2

    // Layout для кнопок
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();                    // Растягивающийся элемент слева
    buttonsLayout->addWidget(create);              // Кнопка 1
    buttonsLayout->addSpacing(10);                 // Промежуток между кнопками
    buttonsLayout->addWidget(exit);                // Кнопка 2
    buttonsLayout->addStretch();

    // Основной вертикальный layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(fieldsLayout);
    mainLayout->addStretch(); // Добавляем растягивающееся пространство
    mainLayout->addLayout(buttonsLayout);

    connect(exit, &QPushButton::clicked, this, &RegisterWindow::onExitClicked);
    connect(create, &QPushButton::clicked, this, &RegisterWindow::onCreateClicked);

    connect(password, &QLineEdit::textChanged, this, &RegisterWindow::checkPasswords);
    connect(confirm_password, &QLineEdit::textChanged, this, &RegisterWindow::checkPasswords);

    networkManager = new QNetworkAccessManager(this);

    // Соединяем сигнал finished со слотом обработки ответа
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &RegisterWindow::onRequestFinished);
}

void RegisterWindow::onExitClicked() {
    close();
}

void RegisterWindow::onCreateClicked() {
    if (password->text() != confirm_password->text()) {
        QMessageBox::warning(this, "Password Mismatch",
                             "Passwords do not match. Please check your passwords.");
        return;  // Не закрываем окно, если пароли не совпадают
    }

    // Дополнительные проверки можно добавить здесь
    if (full_name->text().isEmpty() || email->text().isEmpty() ||
        password->text().isEmpty()) {
        QMessageBox::warning(this, "Empty Fields",
                             "Please fill in all required fields.");
        return;
    }

    QByteArray requestData = prepareRequestData();

    sendRegistrationRequest(requestData);

    // Меняем текст кнопки чтобы показать что идет отправка
    create->setText("Sending...");
    create->setEnabled(false);
    // qDebug() << "Request data:" << requestData;

    // // QMessageBox::information(this, "Request Data",
    //                          // "POST data prepared:\n" + QString(requestData));


    // QMessageBox::information(this, "Success",
    //                          "Account created successfully!");
    // close();
}


void RegisterWindow::checkPasswords()
{
    QString pass1 = password->text();
    QString pass2 = confirm_password->text();

    if (pass1.isEmpty() && pass2.isEmpty()) {
        passwordMatchLabel->setText("");
        passwordMatchLabel->setStyleSheet("");
        return;
    }

    if (pass1 == pass2) {
        passwordMatchLabel->setText("✓ Passwords match");
        passwordMatchLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        passwordMatchLabel->setText("✗ Passwords do not match");
        passwordMatchLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

QByteArray RegisterWindow::prepareRequestData() {
    QJsonObject json;

    // Добавляем данные из полей
    json["full_name"] = full_name->text();
    json["email"] = email->text();
    json["password"] = password->text();
    // Можем добавить дополнительные поля если нужно
    // json["confirm_password"] = confirm_password->text();

    // Преобразуем JSON в QByteArray
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    return data;
}

void RegisterWindow::sendRegistrationRequest(const QByteArray &data)
{
    // Создаем URL (используем твой адрес сервера)
    QUrl url("http://localhost:8080/register");

    // Создаем запрос
    QNetworkRequest request(url);

    // Устанавливаем заголовки (как в curl команде)
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Отправляем POST запрос с данными
    networkManager->post(request, data);

    qDebug() << "Sending POST request to:" << url.toString();
    qDebug() << "Data:" << data;
}

void RegisterWindow::onRequestFinished(QNetworkReply *reply)
{
    // Восстанавливаем кнопку
    create->setText("Create Account");
    create->setEnabled(true);

    // Проверяем на ошибки сети
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Network Error",
                              "Failed to connect to server:\n" + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Получаем статус код
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // Читаем ответ
    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);

    qDebug() << "Response status:" << statusCode;
    qDebug() << "Response data:" << responseString;

    // Парсим JSON ответ
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

    if (jsonResponse.isObject()) {
        QJsonObject jsonObject = jsonResponse.object();

        // ===== ИСПРАВЛЕНИЕ: ПРОВЕРЯЕМ КОДЫ ТВОЕГО СЕРВЕРА =====
        if (statusCode == 200 || statusCode == 201) {  // 201 Created - для регистрации
            // Успешная регистрация - получаем токены
            QString userId = jsonObject["user_id"].toString();
            QString userEmail = jsonObject["email"].toString();
            QString accessToken = jsonObject["access_token"].toString();
            QString refreshToken = jsonObject["refresh_token"].toString();

            // Для безопасности не показываем полные токены
            QString displayAccessToken = accessToken.length() > 20
                                             ? accessToken.left(20) + "..."
                                             : accessToken;

            QMessageBox::information(this, "Registration Successful",
                                     QString("User registered successfully!\n\n"
                                             "User ID: %1\n"
                                             "Email: %2\n"
                                             "Access Token: %3\n"
                                             "Refresh Token: %4")
                                         .arg(userId)
                                         .arg(userEmail)
                                         .arg(displayAccessToken)
                                         .arg(refreshToken.length() > 20
                                                  ? refreshToken.left(20) + "..."
                                                  : refreshToken));

            // Здесь можно сохранить токены (например, в QSettings или переменные)
            qDebug() << "Access token:" << accessToken;
            qDebug() << "Refresh token:" << refreshToken;

            // Закрываем окно регистрации
            close();

        } else if (statusCode == 400) {
            // Bad Request - валидация не прошла
            QString errorMessage = jsonObject.contains("error")
                                       ? jsonObject["error"].toString()
                                       : "Invalid request data";

            QMessageBox::warning(this, "Registration Failed",
                                 "Bad request:\n" + errorMessage);

        } else if (statusCode == 409) {
            // Conflict - пользователь уже существует
            QString errorMessage = jsonObject.contains("error")
                                       ? jsonObject["error"].toString()
                                       : "User already exists";

            QMessageBox::warning(this, "Registration Failed",
                                 "User already exists:\n" + errorMessage);

        } else {
            // Другие ошибки сервера
            QString errorMessage = jsonObject.contains("error")
                                       ? jsonObject["error"].toString()
                                       : "Unknown server error";

            QMessageBox::warning(this, "Registration Failed",
                                 QString("Server error (code %1):\n%2")
                                     .arg(statusCode)
                                     .arg(errorMessage));
        }
    } else {
        // Невалидный JSON
        QMessageBox::warning(this, "Server Error",
                             "Invalid JSON response from server:\n" + responseString);
    }

    // Освобождаем память
    reply->deleteLater();
}
