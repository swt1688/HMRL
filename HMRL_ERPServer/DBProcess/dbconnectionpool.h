#ifndef DBCONNECTIONPOOL_H
#define DBCONNECTIONPOOL_H

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class DBConnectionPool
{

public:
    DBConnectionPool();//用于从连接池里获取连接。
    DBConnectionPool(const DBConnectionPool &other);
    DBConnectionPool& operator=(const DBConnectionPool &other);

    // 数据库信息
    static QString DB_HostAddress;
    static QString DB_Name;

    static QString DB_UserName;
    static QString DB_Password;
    static int DB_Port;


public:
    static void release(); // 关闭所有的数据库连接,真正的关闭所有的连接，一般在程序结束的时候才调用，在 main() 函数的 return 语句前
    static QSqlDatabase openConnection();                 // 获取数据库连接

     /**************************closeConnection************************************
      * 并不会真正的关闭连接，而是把连接放回连接池复用。连接的底层是通过 Socket 来通讯的，建立 Socket 连接是非常耗时的，
      * 如果每个连接都在使用完后就给断开 Socket 连接，需要的时候再重新建立 Socket连接是非常浪费的，所以要尽量的复用以提高效率。****/
    static void closeConnection(QSqlDatabase connection); // 释放数据库连接回连接池

    ~DBConnectionPool();

private:
    static DBConnectionPool& getInstance();
    QSqlDatabase createConnection(const QString &connectionName); // 创建数据库连接

    QQueue<QString> usedConnectionNames;   // 已使用的数据库连接名,保存正在被使用的连接的名字，用于保证同一个连接不会同时被多个线程使用。
    QQueue<QString> unusedConnectionNames; // 未使用的数据库连接名




    QString databaseType;//数据库类型1


    bool    testOnBorrow;    // 取得连接的时候验证连接是否有效
    QString testOnBorrowSql; // 测试访问数据库的 SQL

    int maxWaitTime;  // 获取连接最大等待时间
    int waitInterval; // 尝试获取连接时等待间隔时间
    int maxConnectionCount; // 最大连接数

    static QMutex mutex;
    static QWaitCondition waitConnection;
    static DBConnectionPool *instance;

};

#endif // DBCONNECTIONPOOL_H
