#include "client.h"
#include "ui_client.h"


Client::Client(const QString& strHost, int nPort, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client), m_File(nullptr)
{

    ui->setupUi(this);

    m_port = nPort;
    m_Host = strHost;

}

Client::~Client()
{
    m_pTcpSocket->close();
    delete m_File;
    delete m_pTcpSocket;
    delete ui;
}

void Client::on_ChangeFile_clicked()
{
    delete m_File;
    m_PathFile = QFileDialog::getOpenFileName(0,"Выберите файл","","*");
    ui->File->setText(m_PathFile);

    m_sizeFile = QFileInfo(m_PathFile).size();
    m_nameFile = QFileInfo(m_PathFile).fileName();
    m_File     = new QFile(m_PathFile);

}

void Client::slotConnected()
{
    EnableLoadGUI();
}


void Client::sendToServer()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);


    if(!m_File->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0,"Ошибка",QString("%1 Невозможно открыть!")
                              .arg(m_Host));
        return;
    } else {
        out << (qint64)0 << m_nameFile;

        data.append(m_File->readAll());
        m_File->close();

        out.device()->seek(0);
        out << (qint64)(data.size() - sizeof(qint64));

        qint64 x = 0;
        while (x < data.size()) {
            quint64 y = m_pTcpSocket->write(data);
            x += y;
        }
    }
}

void Client::EnableLoadGUI()
{
    ui->File->setEnabled(true);
    ui->ChangeFile->setEnabled(true);
    ui->SendFile->setEnabled(true);
    ui->connectButton->setEnabled(false);
    ui->lineEdit->setText("Соединение с сервером установлено");
    ui->lineEdit->setStyleSheet("color: rgb(50,205,50)");
}


void Client::on_SendFile_clicked()
{
    sendToServer();
}


void Client::on_connectButton_clicked()
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(m_Host,m_port);

    bool bConnected = m_pTcpSocket->waitForConnected(1000);
    if (bConnected) {
        EnableLoadGUI();
    }

    //connect(m_pTcpSocket, SIGNAL(connected()), this,SLOT(slotConnected()));
}
