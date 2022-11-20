#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFile>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(int nPort, QWidget *parent = 0);
    ~Server();

private slots:
    void slotRead();
    void slotNewConnection();

private:
    Ui::Server *ui;

    QTcpServer  *m_TcpServer;
    QTcpSocket  *m_TcpSocket;
    QGraphicsScene *m_scene;
    qint64       m_blockSize = 0;

    void GoodConnectionGUI();
};

#endif // SERVER_H
