#ifndef SQLOP_H
#define SQLOP_H

#include <QtSql>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

#include<ActiveQt/QAxObject>  // 使用时需在pro文件中加入 QT += axcontainer
#include "qvector.h"


/********************************************************************************************
* 类名称  ：SqlOp
* 基本功能：实现相关数据库操作，增删改查
*
* 作    者：陈梦林
* 完成日期：2016年6月17日
*
* 更新历史：
* 1，更新时间：2018.7.24
*    更新内容：增加对MYSQL的支持
*    作    者：卢浩杰
* *********************************************************************************************/

class SqlOp
{
public:
	SqlOp();
	~SqlOp();
	

    static bool CreateConnection(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password);
	static bool CloseConnection();

	//不需要动态参数的SQL语句，若有字符类型，需要用单引号引住
	//需要用到动态参数的SQL语句，不需要理会
	//如下 name是varchar类型，age是整形
	//不需要动态参数：insert into person (name,age) values("zhangsan",13)  
	//需要动态参数： insert into person (name ,age) values(:zhangsan,:age)

	//select column from table where column=?
	//select column from table where column=:column
	static QVector< QVector <QString> > SelectQuery(QString select, int num);//查询语句，第一个参数为SQL语句，第二个参数为需要的列数
	static QVector< QVector <QString> > SelectQuery(QString select, int num, QVector<QString> bindValue);//查询语句，第一个参数为SQL语句，第二个参数为参数，第三个为列数
	//查数据推荐使用下面两个
	static QVector< QVector <QVariant> > SelectQuery2(QString select, int num);//查询语句，第一个参数为SQL语句，第二个参数为需要的列数
	static QVector< QVector <QVariant> > SelectQuery2(QString select, int num, QVector<QString> bindValue);//查询语句，第一个参数为SQL语句，第二个参数为参数，第三个为列数
	//查询外部数据库里面的数据使用第三个
    static QVector< QVector <QVariant> > SelectQuery3(QString select, int num,  QSqlDatabase qSqlDB);//查询语句，第一个参数为SQL语句，第二个参数为需要的列数，第三个为指定的外部数据
	//delete from table where column=?
	//delete from table where column=:column
	//update table set column=? where column=?
	//update table set column=:column where column=:column
	static bool Delete_UpdateQuery(QString del);
	static bool Delete_UpdateQuery(QString del, QVector<QVariant> bindValue);
	//insert into table (column1,cloumn2,column3,……) values(:column1,:column2,:column3,……)
	//insert into table (column1,cloumn2,column3,……) values(?,?,?,……)
	static bool InsertQuery(QString inserst);
	static bool InsertQuery(QString insert, QVector<QVariant> bindValue);

	//导出项目
	static int ExportProjectDb(const QString &dbName, const QString &ID);
	//导入项目
	static QVector< QVector <QVariant>> ImportProjectDbInfo(const QString &dbPath);
	static int ImportProjectDb(const QString &dbPath,int type);//type==1：导入全部数据；type==2：只导入项目信息和钢板尺寸；
	

};

#endif // SQL_H
