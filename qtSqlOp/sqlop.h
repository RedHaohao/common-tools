#ifndef SQLOP_H
#define SQLOP_H

#include <QtSql>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include "ThreadParameter.h"
#include "qvector.h"


/********************************************************************************************
* 类名称  ：SqlOp
* 基本功能：实现相关数据库操作，增删改查
* //导入时 与本地文件的数据库连接  map对应的key 为1 
* //导出时 与本地文件的数据库连接  map对应的key 为2
* //其他与服务器上数据库的连接 map 对应的key 都为年份 （int）

* *********************************************************************************************/
enum
{
	importYear = 1, //导入时 与本地文件的数据库连接  map对应的key 为1 
	exportYear = 2, //导出时 与本地文件的数据库连接  map对应的key 为2
};
class SqlOp 
{

public:
	SqlOp();
	~SqlOp();
	//创建MySQL的数据库  CreateDatabase("118.178.136.10", "cgpc2015", "tomato", "tomato");
	static bool CreateDatabase(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password);

	
	//创建MySql数据库的默认连接：SqlOp::CreateConnection("118.178.136.10", "cgpc2018", "tomato", "tomato");
	//创建Sqlite数据库的默认连接：SqlOp::CreateConnection("118.178.136.10", "cgpc2017", "tomato", "tomato","","QSQLITE");
	//创建MySql数据库的非默认连接：SqlOp::CreateConnection("118.178.136.10", "cgpc2018", "tomato", "tomato","connectionName");
	//创建Sqlite数据库的默认连接：SqlOp::CreateConnection("118.178.136.10", "cgpc2017", "tomato", "tomato","connectionName","QSQLITE");
	static bool CreateConnection(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password, QString ConnectionName = QString(), const QString DbType = "QMYSQL");

	//获取服务器MySql中所有 关于项目数据库的名字
	static QVector<QString> GetDatabaseName(const QString& HostName,  const QString& UserName, const QString& Password);
	
	//根据服务器中数据库的名字  建立连接，并将信息添加到参数类的 mapDbConnectionName
	static bool InitConnection(QVector<QString > DbName, const QString& HostName, const QString& UserName, const QString& Password);
	
	//检测当前年份 是否有数据库，无则建库 建连接，并将信息添加到参数类的 mapDbConnectionName
	static bool CreateCurrentYearDB(QMap<int, QString>& map, const QString& HostName, const QString& UserName, const QString& Password); 
	
	//关闭默认连接：CloseConnection();
	//关闭非默认连接：CloseConnection("ConnectionName");
	static bool CloseConnection(const QString ConnectionName = QSqlDatabase::defaultConnection);

	//关闭参数中所有数据库连接
	static bool CloseAllConnection(QMap<int, QString>& map);


	//不需要动态参数的SQL语句，若有字符类型，需要用单引号引住
	//需要用到动态参数的SQL语句，不需要理会
	//如下 name是varchar类型，age是整形
	//不需要动态参数：insert into person (name,age) values("zhangsan",13)  
	//需要动态参数： insert into person (name ,age) values(:zhangsan,:age)

	//select column from table where column=?
	//select column from table where column=:column

	//查询语句，第一个参数为SQL语句，第二个参数为需要的列数，第三个为具体年份数据库
	static QVector< QVector <QString> > SelectQuery(QString select, int num, const int year);
	//查询语句，第一个参数为SQL语句，第二个参数为列数,第三个为动态参数，第四个为具体年份数据库
	static QVector< QVector <QString> > SelectQuery(QString select, int num, QVector<QString> bindValue, const int year);
	
	//查数据推荐使用下面两个
	//第一个参数为SQL语句，第二个参数为需要的列数，第三个为具体年份数据库
	static QVector< QVector <QVariant> > SelectQuery2(QString select, int num, const int year);
	//查询语句，第一个参数为SQL语句，第二个参数为列数,第三个为动态参数，第四个为具体年份数据库
	static QVector< QVector <QVariant> > SelectQuery2(QString select, int num, QVector<QString> bindValue, const int year);
	
	//查询外部数据库里面的数据使用第三个
	//查询语句，第一个参数为SQL语句，第二个参数为需要的列数，第三个为指定的外部数据
	static QVector< QVector <QVariant> > SelectQuery3(QString select, int num, QSqlDatabase qSqlDB);
	
	//delete from table where column=?
	//delete from table where column=:column
	//update table set column=? where column=?
	//update table set column=:column where column=:column
	static bool Delete_UpdateQuery(QString del, const int year);
	static bool Delete_UpdateQuery(QString del, QVector<QVariant> bindValue, const int year);
	//insert into table (column1,cloumn2,column3,……) values(:column1,:column2,:column3,……)
	//insert into table (column1,cloumn2,column3,……) values(?,?,?,……)
	static bool InsertQuery(QString insert, const int year);
	static bool InsertQuery(QString insert, QVector< QVariant> bindValue, const int year);
	//批量插入

	//区别是上面的bindvalue 是一行一行字段
	//下面的是一列一列的字段
	static bool InsertQuery(QString insert, QVector<QVector<QVariant> > bindValue, const int year);
	static bool	InsertQuery1(QString insert, QVector<QVector<QVariant> > bindValue, const int year);
	//插入 trackCheck等表
	static bool InsertTable(QString insert, QString append,QVector<QVector<QVariant> > result, int num, QSqlDatabase qSqlDB);

	//导出项目
	static int ExportProjectDb(const QString &dbName, const QString &ID,const int year);
	//导入项目
	static QVector< QVector <QVariant>> ImportProjectDbInfo(const QString &dbPath);
	static int ImportProjectDb(int year, int type);//type==1：导入全部数据；type==2：只导入项目信息和钢板尺寸；
	
private:

	static QVector<QString> m_vSqlCreatable;//存储SQL语句
	
};



#endif // SQL_H
