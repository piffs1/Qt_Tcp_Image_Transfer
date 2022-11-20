#include "server.h"
#include "ui_server.h"

Server::Server(int nPort, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server), m_scene(nullptr)
{
    ui->setupUi(this);

    m_TcpServer = new QTcpServer(this);

    //Start server
    if (!m_TcpServer->listen(QHostAddress::Any, nPort)) {

        QMessageBox::critical(0,"Ошибка",
                              QString("Невозможно запустить сервер: %1").arg(m_TcpServer->errorString()));

        m_TcpServer->close();
        return;
    }

    connect(m_TcpServer, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));

}

Server::~Server()
{
    delete m_scene;
    delete ui;
}

void Server::slotRead()
{
    QDataStream in(m_TcpSocket);
    in.setVersion(QDataStream::Qt_5_12);
    if (m_blockSize == 0) {

        if (m_TcpSocket->bytesAvailable() < sizeof(qint64))
            return;

        in >> m_blockSize;
    }

    if (m_TcpSocket->bytesAvailable() < m_blockSize)
        return;

    QString fileName;
    in >> fileName;

    QByteArray packet = m_TcpSocket->readAll();
    QFile target(fileName);

    if (!target.open(QIODevice::WriteOnly)) {
        return;
    }

    target.write(packet);
    target.close();

    QImage Image(fileName);

    delete m_scene;
    m_scene = new QGraphicsScene(this);
    m_scene->addPixmap(QPixmap::fromImage(Image));
    ui->graphicsView->setScene(m_scene);
    m_blockSize = 0;
}

void Server::slotNewConnection()
{
    GoodConnectionGUI();
    m_TcpSocket = m_TcpServer->nextPendingConnection();

    connect(m_TcpSocket, SIGNAL(disconnected()),
            m_TcpSocket, SLOT(deleteLater()));

    connect(m_TcpSocket, SIGNAL(readyRead()),
            this, SLOT(slotRead()));

}

void Server::GoodConnectionGUI()
{
    ui->lineEdit->setText("Соединение установлено");
    ui->lineEdit->setStyleSheet("color: rgb(50,205,50)");
}

