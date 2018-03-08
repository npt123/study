#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textHostname->setText("127.0.0.1");
    ui->textPort->setText("5555");
    ui->textStatus->setReadOnly(true);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    ::close(sockfd);
    delete ui;
}

void MainWindow::on_btnConnect_clicked()
{
    MainWindow::connect();
}

void MainWindow::on_btnTransfer_clicked()
{
    transfer();
}

void MainWindow::connect() {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ui->textStatus->append("Socket created failed!\n");
        return;
    }
    port = (ui->textPort->text()).toInt();
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr((ui->textHostname->text()).toLatin1().data());
    server_addr.sin_port = htons(port);
    bzero(&(server_addr.sin_zero), 8);
    if (::connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        ui->textStatus->append("Connect error!\n");
        return;
    }
    if (recv(sockfd, dir, BUFFER_SIZE, 0) < 0) {
        ui->textStatus->append("Recv Directery error!\n");
        return;
    }
    ui->textStatus->append(dir);
}

void MainWindow::transfer() {
    strcpy(filename, (ui->textFilename->text()).toLatin1().data());
    ui->textStatus->append(filename);
    if (send(sockfd, filename, MAX_FILENAME_LEN, 0) < 0) {
        ui->textStatus->append("Send filename failed!\n");
        return;
    }

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        ui->textStatus->append("File doesn't exist or permission denied!\n");
        return;
    }

    if (recv(sockfd, &fileSize, sizeof(int), 0) < 0) {
        ui->textStatus->append("File doesn't exits or permission denied!\n");
    }

    getSize = 0;
    char data[BUFFER_SIZE];
    int nCount;
    ui->progressBar->setOrientation(Qt::Horizontal);  // 水平方向
    ui->progressBar->setMinimum(0);  // 最小值
    ui->progressBar->setMaximum(fileSize);  // 最大值
    while((nCount = recv(sockfd, data, BUFFER_SIZE, 0)) > 0) {
        fwrite(data, nCount, 1, fp);
        getSize += nCount;
        ui->progressBar->setValue(getSize);
    }
    fclose(fp);
    ui->progressBar->setValue(fileSize);
    ui->textStatus->append("File recieve success!\n");
}
