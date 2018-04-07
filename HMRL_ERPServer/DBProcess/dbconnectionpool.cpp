#include "dbconnectionpool.h"

#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QSettings>

QMutex DBConnectionPool::mutex;
QWaitCondition DBConnectionPool::waitConnection;
DBConnectionPool* DBConnectionPool::instance = NULL;

QString DBConnectionPool::DB_Name="onlinedatabase";
QString DBConnectionPool::DB_HostAddress="localhost";
QString DBConnectionPool::DB_UserName="swt518";
int DBConnectionPool::DB_Port=3306;
QString DBConnectionPool::DB_Password="swt123456789";

DBConnectionPool::DBConnectionPool() {


    databaseType = "QMYSQL";
    testOnBorrow = true;
    testOnBorrowSql = "SELECT 1";

    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConnectionCount  = 5;
}

DBConnectionPool::~DBConnectionPool() {
    // 销毁连接池的时候删除所有的连接
    foreach(QString connectionName, usedConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    foreach(QString connectionName, unusedConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

DBConnectionPool& DBConnectionPool::getInstance() {
    if (NULL == instance) {
        QMutexLocker locker(&mutex);

        if (NULL == instance) {
            instance = new DBConnectionPool();
        }
    }

    return *instance;
}

void DBConnectionPool::release() {
    QMutexLocker locker(&mutex);
    delete instance;
    instance = NULL;
}

QSqlDatabase DBConnectionPool::openConnection() {
    DBConnectionPool& pool = DBConnectionPool::getInstance();
    QString connectionName;

    QMutexLocker locker(&mutex);

    // 已创建连接数
    int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

    // 如果连接已经用完，等待 waitInterval 毫秒看看是否有可用连接，最长等待 maxWaitTime 毫秒
    for (int i = 0;
         i < pool.maxWaitTime
         && pool.unusedConnectionNames.size() == 0 && connectionCount == pool.maxConnectionCount;
         i += pool.waitInterval) {
        waitConnection.wait(&mutex, pool.waitInterval);

        // 重新计算已创建连接数
        connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
    }

    if (pool.unusedConnectionNames.size() > 0)
    {
        connectionName = pool.unusedConnectionNames.dequeue();// 有已经回收的连接，复用它们
    }
    else if (connectionCount < pool.maxConnectionCount)
    {
        connectionName = QString("Connection-%1").arg(connectionCount + 1);// 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
    } else {
        // 已经达到最大连接数
        qDebug() << "Cannot create more connections.";
        return QSqlDatabase();
    }

    // 创建连接
    QSqlDatabase db = pool.createConnection(connectionName);

    // 有效的连接才放入 usedConnectionNames
    if (db.isOpen()) {
        pool.usedConnectionNames.enqueue(connectionName);
    }

    return db;
}

void DBConnectionPool::closeConnection(QSqlDatabase connection) {
    DBConnectionPool& pool = DBConnectionPool::getInstance();
    QString connectionName = connection.connectionName();

    // 如果是我们创建的连接，从 used 里删除，放入 unused 里
    if (pool.usedConnectionNames.contains(connectionName)) {
        QMutexLocker locker(&mutex);
        pool.usedConnectionNames.removeOne(connectionName);
        pool.unusedConnectionNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }
}

QSqlDatabase DBConnectionPool::createConnection(const QString &connectionName) {
    // 连接已经创建过了，复用它，而不是重新创建
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db1 = QSqlDatabase::database(connectionName);

        if (testOnBorrow) {
            // 返回连接前访问数据库，如果连接断开，重新建立连接
            qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
            QSqlQuery query(testOnBorrowSql, db1);

            if (query.lastError().type() != QSqlError::NoError && !db1.open()) {
                qDebug() << "Open datatabase error:" << db1.lastError().text();
                return QSqlDatabase();
            }
        }

        return db1;
    }

    // 创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(databaseType, connectionName);

    db.setHostName(DB_HostAddress);      //远程IP地址：139.159.225.248<-->本地：localhost
    //db.setHostName("139.159.225.248");
    db.setPort(DB_Port);
    db.setDatabaseName(DB_Name);
    db.setUserName(DB_UserName);              //远程不可使用root账户登录,
    db.setPassword(DB_Password);

    if (!db.open()) {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }
    return db;
}
