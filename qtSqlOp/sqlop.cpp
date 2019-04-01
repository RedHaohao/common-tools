#pragma execution_character_set("utf-8")
#include "sqlOp.h"
#include"getUUID.cpp"
#include "qdebug.h"
#include<QVectorIterator>
//#include <sqlite3.h>

QVector<QString> SqlOp::m_vSqlCreatable{ 1 };
SqlOp::SqlOp()
{
	//初始化sql语句
	//MFL_DevInfo 
	m_vSqlCreatable.pop_back();
	m_vSqlCreatable.append("建表语句");



	
	
}

SqlOp::~SqlOp()
{

}




/*******************************************************************************************
* 函数名称：CreateDatabase
* 函数介绍：创建新的mysql数据库
* 输入参数：（1）HostName：数据库网络地址； （2）DatabaseName：数据库名称； （3）UserName：用户名 ；（4）Password：密码
* 输出参数：无
* 返回值  ：创建成功返回true  失败时返回false
* 注：新建的数据库名不可以为纯数字
********************************************************************************************/

bool SqlOp::CreateDatabase(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(HostName);
	db.setPort(3306);
	db.setUserName(UserName);
	db.setPassword(Password);
	if (!db.isValid())
	{
		qDebug() << db.lastError().text();
		return false;
	}
	db.open();
	QString  str = QString("Create Database If  Not Exists %1 Character Set UTF8;").arg(DatabaseName);//新建数据库的SQL语句
	QSqlQuery query = db.exec(str);  //执行建库的SQL语句
	db.setDatabaseName(DatabaseName);

	if (db.open())
	{

		QVectorIterator<QString> i(m_vSqlCreatable);   //java形式的迭代器
		while (i.hasNext())      //遍历执行建表语句
		{
			
			query.exec(i.next());
			if (query.lastError().isValid())
			{
				qDebug() <<query.lastError();
				return false;
			}
		
		}
		qDebug() << "CreatDatabase " + DatabaseName + "success";
		return true;
	}
	else
	{
		return false;
	}

}


/*******************************************************************************************
* 函数名称：CreateConnection
* 函数介绍：新建数据库连接，若不指定连接名则建立默认连接
* 输入参数：（1）HostName：数据库网络地址； （2）DatabaseName：数据库名称； （3）UserName：用户名 ；（4）Password：密码
（5）ConnectionName: 连接名 （有默认值）若不指定则创建的是默认连接
（6）DbType: 数据库驱动类型 默认为MYSQL   可选项：QSQLITE、MYSQL
* 输出参数：无
* 返回值  ：连接成功返回true  失败时返回false
********************************************************************************************/

bool SqlOp::CreateConnection(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password, QString ConnectionName, QString DbType)
{ 	QSqlDatabase db;
	if (ConnectionName == QString() && DbType == "QMYSQL")
		db = QSqlDatabase::addDatabase("QMYSQL");
	else if (ConnectionName == QString() && DbType != "QMYSQL")
		db = QSqlDatabase::addDatabase(DbType);
	else if (ConnectionName != QString() && DbType == "QMYSQL")
		db = QSqlDatabase::addDatabase("QMYSQL", ConnectionName);
	else if (ConnectionName != QString() && DbType != "QMYSQL")
		db = QSqlDatabase::addDatabase(DbType, ConnectionName);

	db.setDatabaseName(DatabaseName);
	db.setHostName(HostName);
	db.setUserName(UserName);
	db.setPassword(Password); // 没有密码的时候可以注释掉

	if (db.open())
	{
		qDebug() << "connect " + DatabaseName + " success";
		return true;
	}
	else
	{
		qDebug() << db.lastError().text();
		return false;
	}
}

/*******************************************************************************************
* 函数名称：GetDatabaseName
* 函数介绍：获取数据库中 与项目相关所有数据库的名字
* 输入参数：HostName ：主机IP地址 UserName ：用户名 Password：密码
* 输出参数：无
* 返回值  ：QVector<QString> result 存有数据库的名字
********************************************************************************************/

QVector<QString>  SqlOp::GetDatabaseName(const QString& HostName, const QString& UserName, const QString& Password)
{
	CreateConnection(HostName, "mysql", UserName, Password, "getDBName");
	QVector<QString> result;
	QSqlDatabase db = QSqlDatabase::database("getDBName");
	QSqlQuery query(db);
	query.exec("show databases");
	while (query.next())
	{
		QString name = query.value(0).toString();
		if (name.contains("cgpc", Qt::CaseInsensitive))//只获取  名字中包含“cgpc”字符串的 储罐pc数据库的名字
		{
			qDebug() << name;
			result.append(name);
		}

	}
	return result;
}

/*******************************************************************************************
* 函数名称：InitConnection
* 函数介绍：与服务器上项目的所有数据库建立连接，并把连接名保存到参数类中
* 输入参数：DbName：数据库名列表 HostName ：主机IP地址 UserName ：用户名 Password：密码
* 输出参数：无
* 返回值  ：QVector<QString> result 存有数据库的名字
********************************************************************************************/
bool SqlOp::InitConnection(QVector<QString> DbName, const QString& HostName, const QString& UserName, const QString& Password)
{
	//往参数类里map 添加连接 （此时并没有建立连接）
	//真正的连接等到使用时才会建立
	ThreadParameter::getInstance()->mapDbConnectionName.insert(importYear, "local2remoteDB");
	ThreadParameter::getInstance()->mapDbConnectionName.insert(exportYear, "remote2localDB");
	
	
	for (int i = 0; i < DbName.size(); i++)
	{

		QString temp = DbName[i].mid(4, 4);		//数据库名字为cgpc2018等  temp只取后面的年份
		QString ConnectionName = "remoteDB" + temp; //连接名 命名规范 remoteDB+年份
		if (CreateConnection(HostName, DbName[i], UserName, Password, ConnectionName))
		{
			ThreadParameter::getInstance()->mapDbConnectionName.insert(temp.toInt(), ConnectionName); //将<年份，连接名>存到参数类
			qDebug() << "create " << ConnectionName << " sucess";
		}
		else
		{
			qDebug() << "Failed to initialize connection";
			return false;
		}
	}
	return true;
}

/*******************************************************************************************
* 函数名称：CreateCurrentYearDB
* 函数介绍：检测当前年份 是否有当前年份的数据库，无则建库 建连接，并将信息添加到参数类的 mapDbConnectionName
* 输入参数：HostName ：主机IP地址 UserName ：用户名 Password：密码  map：系统参数类中的map
* 输出参数：无
* 返回值  ：QVector<QString> result 存有数据库的名字
********************************************************************************************/

bool SqlOp::CreateCurrentYearDB(QMap<int, QString>& map, const QString& HostName, const QString& UserName, const QString& Password)
{
	int year = ThreadParameter::getInstance()->currentYear; //获取当前年份
	QString dbName = "cgpc" + QString::number(year);  //数据库名字格式 cgpc+年份
	QString connectionName = "remoteDB" + QString::number(year); //连接名字格式 remoteDB +年份
	if (map.contains(year)) //如果包含当前年份的数据库  则返回 
	{
		qDebug() << "cgpc" << year << "exits";
		return true;
	}
	else
	{

		if (SqlOp::CreateDatabase(HostName, dbName, UserName, Password)) //创建当前年份数据库
		{
			if (SqlOp::CreateConnection(HostName, dbName, UserName, Password, connectionName)) //创建连接
			{
				map.insert(year, connectionName);   //添加到参数类里
				QString procedureImportCombine =
					"CREATE DEFINER = `tomato`@`%` PROCEDURE `ImportCombine`(IN `pID` CHAR(38))"
					" BEGIN"
					" START TRANSACTION;"
					" DELETE"
					" FROM"
					" MFL_TrackSSRecord"
					" WHERE"
					" RecordID IN("
					" SELECT"
					" MFL_TrackSSRecord.RecordID"
					" FROM"
					" (SELECT RecordID FROM MFL_TrackSSRecord WHERE"
					" TrackID IN(SELECT TrackID FROM MFL_TrackInfo WHERE PlateID IN(SELECT PlateID FROM MFL_PlateInfo WHERE ProjectID = pID))"
					" ) MFL_TrackSSRecord);"
					" DELETE"
					" FROM"
					" MFL_TrackCheckRecord"
					" WHERE"
					" RecordID IN(SELECT MFL_TrackCheckRecord.RecordID FROM(SELECT RecordID FROM MFL_TrackCheckRecord WHERE TrackID IN(SELECT TrackID FROM MFL_TrackInfo WHERE PlateID = pID)) MFL_TrackCheckRecord);"
					" DELETE"
					" FROM"
					" MFL_TrackInfo"
					" WHERE"
					" PlateID = pID;"
					" DELETE"
					" FROM"
					" MFL_PlateInfo"
					" WHERE"
					" PlateID = pID;"
					" COMMIT;"
					" END;";
				QString procedureImportCover =
					"CREATE DEFINER = `tomato`@`%` PROCEDURE `ImportCover`( IN `pID` CHAR(38)) "
					" BEGIN"
					" START TRANSACTION;"
					" DELETE"
					" FROM"
					" MFL_TrackSSRecord"
					" WHERE"
					" RecordID IN("
					" SELECT"
					" MFL_TrackSSRecord.RecordID"
					" FROM"
					" (SELECT RecordID FROM MFL_TrackSSRecord WHERE"
					" TrackID IN(SELECT TrackID FROM MFL_TrackInfo WHERE PlateID IN(SELECT PlateID FROM MFL_PlateInfo WHERE ProjectID = pID))"
					" ) MFL_TrackSSRecord);"
					" DELETE"
					" FROM"
					" MFL_TrackCheckRecord"
					" WHERE"
					" RecordID IN("
					" SELECT"
					" MFL_TrackCheckRecord.RecordID"
					" FROM"
					" (SELECT RecordID FROM MFL_TrackCheckRecord WHERE"
					" TrackID IN(SELECT TrackID FROM MFL_TrackInfo WHERE PlateID IN(SELECT PlateID FROM MFL_PlateInfo WHERE ProjectID = pID))"
					" ) MFL_TrackCheckRecord);"
					" DELETE FROM MFL_TrackInfo WHERE TrackID IN(SELECT MFL_TrackInfo.TrackID FROM(SELECT * FROM MFL_TrackInfo WHERE PlateID IN(SELECT PlateID FROM MFL_PlateInfo WHERE ProjectID = pID)) MFL_TrackInfo);"
					" DELETE FROM MFL_PlateInfo WHERE ProjectID = pID;"
					" DELETE FROM MFL_ProjectInfo WHERE ProjectID = pID;"
					" COMMIT;"
					" END;";
				QSqlQuery q(QSqlDatabase::database(connectionName));
				q.exec(procedureImportCombine);
				q.exec(procedureImportCover);
				return true;
			}
			else
			{
				qDebug() << "failed to create" << connectionName;
				return false;
			}
		}
		else
		{
			qDebug() << "failed to create" << dbName;
			return false;
		}


	}
}
/*******************************************************************************************
* 函数名称：CloseConnection
* 函数介绍：关闭数据库连接
* 输入参数：ConnectionName：连接名（有默认值）不指定则关闭默认连接
* 输出参数：无
* 返回值  ：无
********************************************************************************************/
bool SqlOp::CloseConnection(QString ConnectionName)
{
	QSqlDatabase db = QSqlDatabase::database(ConnectionName, false);//false 指定不打开未打开的连接，若为true，未打开的连接将会被打开
	if (db.isOpen())
	{
		db.close();
		QSqlDatabase::removeDatabase(ConnectionName);
		qDebug() << "close " + ConnectionName + " success";
		return true;
	}


	return false;
}



bool SqlOp::CloseAllConnection(QMap<int, QString>& map)
{
	QMap<int, QString>::iterator itr;
	for (itr = map.begin(); itr != map.end(); itr++)
	{
		int temp = itr.key();
		if (!CloseConnection(map[temp]))
		{
			return false;
		}
	}
	return true;
}
/*******************************************************************************************
* 函数名称：SelectQuery
* 函数介绍：查询语句，返回的数据为字符串形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 (3)year： 要查询数据库的年份
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QString> > SqlOp::SelectQuery(QString select, int num, const int year)
{
	QVector< QVector <QString> >  result;   //存放查询结果
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return result;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.exec(select);

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}
	else
	{
		qDebug() << "select success";
		while (query.next())
		{
			QVector <QString> resultTemp;
			QString temp;
			for (int i = 0; i < num; i++)
			{
				temp = query.value(i).toString();
				resultTemp.append(temp);
			}
			result.append(resultTemp);
		}
	}

	return result;
}

/*******************************************************************************************
* 函数名称：SelectQuery
* 函数介绍：带动态参数，查询语句，返回的数据为字符串形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 （3）bindValue：动态参数集合 （4）year： 要查询数据库的年份
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QString> > SqlOp::SelectQuery(QString select, int num, QVector<QString> bindValue, const int year)
{
	QVector< QVector <QString> >  result;   //存放查询结果
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return result;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.prepare(select);
	for (int i = 0; i < bindValue.size(); i++)
	{
		query.bindValue(i, bindValue[i]);
	}
	query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}
	else
	{
		qDebug() << "select success";
		while (query.next())
		{
			QVector <QString> resultTemp;
			QString temp;
			for (int i = 0; i < num; i++)
			{
				temp = query.value(i).toString();
				resultTemp.append(temp);
			}
			result.append(resultTemp);
		}
	}

	return result;
}

/*******************************************************************************************
* 函数名称：SelectQuery2
* 函数介绍：查询语句，返回的数据为可变类型形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 (3) year： 要查询数据库的年份
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QVariant> > SqlOp::SelectQuery2(QString select, int num, const int year)
{
	QVector< QVector <QVariant> >  result;   //存放查询结果
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return result;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.exec(select);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}
	else
	{
		qDebug() << "select success";
		while (query.next())
		{
			QVector <QVariant> resultTemp;
			QVariant temp;
			for (int i = 0; i < num; i++)
			{
				temp = query.value(i);
				resultTemp.append(temp);
			}
			result.append(resultTemp);
		}
	}

	return result;
}

/*******************************************************************************************
* 函数名称：SelectQuery2
* 函数介绍：带动态参数，查询语句，返回的数据为可变类型形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 （3）bindValue：动态参数集合 (4)  year： 要查询数据库的年份
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QVariant> > SqlOp::SelectQuery2(QString select, int num, QVector<QString> bindValue, const int year)
{
	QVector< QVector <QVariant> >  result;   //存放查询结果
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return result;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.prepare(select);
	for (int i = 0; i < bindValue.size(); i++)
	{
		query.bindValue(i, bindValue[i]);
	}
	query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}
	else
	{
		qDebug() << "select success";
		while (query.next())
		{
			QVector <QVariant> resultTemp;
			QVariant temp;
			for (int i = 0; i < num; i++)
			{
				temp = query.value(i);
				resultTemp.append(temp);
			}
			result.append(resultTemp);
		}
	}

	return result;
}


/*******************************************************************************************
* 函数名称：SelectQuery3
* 函数介绍：查询语句，返回的数据为可变类型形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 （3）指定数据库查询
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QVariant> > SqlOp::SelectQuery3(QString select, int num, QSqlDatabase qSqlDB)
{
	QVector< QVector <QVariant> >  result;   //存放查询结果
	QSqlQuery query(qSqlDB);
	query.exec(select);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else
	{
		qDebug() << "select success";
		while (query.next())
		{
			QVector <QVariant> resultTemp;
			QVariant temp;
			for (int i = 0; i < num; i++)
			{
				temp = query.value(i);
				resultTemp.append(temp);
			}
			result.append(resultTemp);
		}
	}

	return result;
}
/*******************************************************************************************
* 函数名称：Delete_UpdateQuery
* 函数介绍：删除或者更新语句
* 输入参数：（1）del：sql语句  （2）year： 要查询数据库的年份 (3)
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::Delete_UpdateQuery(QString del, const int year)
{
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return false;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.exec(del);

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
		return false;
	}
	qDebug() << "Delete_Update success";
	return true;
}

/*******************************************************************************************
* 函数名称：Delete_UpdateQuery
* 函数介绍：带动态参数，删除或者更新语句
* 输入参数：（1）del：sql语句；（2）bindValue：动态参数集合 （3）year： 要查询数据库的年份
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::Delete_UpdateQuery(QString del, QVector<QVariant> bindValue, const int year)
{
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return false;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.prepare(del);
	for (int i = 0; i < bindValue.size(); i++)
	{
		query.bindValue(i, bindValue[i]);
	}
	QList<QVariant> list = query.boundValues().values();
	for (int i = 0; i < list.size(); ++i)
		qDebug() << i << ": " << list.at(i).toString().toUtf8().data() << endl;
	query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
		return false;
	}
	qDebug() << "Delete_Update success";
	return true;
}

/*******************************************************************************************
* 函数名称：InsertQuery
* 函数介绍：插入语句
* 输入参数：（1）insert：sql语句；（2）year： 要查询数据库的年份
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::InsertQuery(QString insert, const int year)
{
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return false;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	QSqlQuery query(db);
	query.exec(insert);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}
	qDebug() << "insert success";
	return true;
}

/*******************************************************************************************
* 函数名称：InsertQuery
* 函数介绍：插入语句，带有参数
* 输入参数：（1）insert：sql语句； （2）bindValue：动态参数  （3）year： 要查询数据库的年份
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::InsertQuery(QString insert, QVector<QVariant> bindvalue, const int year)
{
	if (!ThreadParameter::getInstance()->mapDbConnectionName.contains(year))
	{
		qDebug() << "No connections are available";
		return false;
	}
	QSqlDatabase db = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year], false);
	int a = 0;
	QSqlQuery query(db);
	query.prepare(insert);
	for (int i = 0; i < bindvalue.size(); i++)
	{
		query.bindValue(i, bindvalue[i]);
	}
	qDebug() << "insert success" << query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	else if (!db.isOpen())
	{
		qDebug() << "database not open";
	}

	return true;
}
/*******************************************************************************************
* 函数名称：InsertTable
* 函数介绍：插入记录很多的表 如（trackcheckRecord SSRecord等）
* 1.构造sql语句如 insert into ‘table’ values (?,?)···(?,?) 一次插入十条记录，加快速度 
* 2.对结果集进行处理 使其分成10组 整列整列的匹配
* 3.执行sql批处理语句
* 4.对最后的单独剩出来 的记录进行处理  (如 104条记录 最后剩下4条 就不能拼凑成一次插入十条)
* 5.注 ：为什么选择一次10条 是因为 测了一些数据 发现10条最快
* 输入参数：（1）insert：插入语句； （2）append：拼接语句  (3) result: 要插入的数据集 （4） num ：表的列数 （5）qSqlDB：要插入的数据库
* 返回值  ：false 失败 true 成功
********************************************************************************************/
bool SqlOp::InsertTable(QString insert,QString append, QVector<QVector<QVariant> > result, int num, QSqlDatabase qSqlDB)
{
	QSqlQuery query(qSqlDB);
	QString tmpStr = insert;
	if (result.size() > 100)
	{
		//构造sql语句
		for (int i = 0; i < 9; i++)
		{
			insert.append(',');
			insert += append;
		}
		query.prepare(insert);

		//将结果集由一行一行变成一列一列
		QVariantList *colResult = new QVariantList[num];
		for (int i = 0; i < result.size(); i++)
		{
			for (int j = 0; j < num; j++)
				colResult[j] << result[i][j];
		}

		int rowCount = result.size() / 10;  //一共能分的组数
		//进行分组 colRes10 的意思是将colRes分成10组
		QVariantList *colRes10 = new QVariantList[num];
		for (int i = 0; i < 10 * rowCount; i++)
		{
			for (int j = 0; j < num; j++)
			{
				colRes10[j] << colResult[j][i];
			}
			if ((i + 1) % rowCount == 0)
			{
				for (int k = 0; k < num; k++)
				{
					query.addBindValue(colRes10[k]);
					colRes10[k].clear();
				}

			}
		}
		
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			return false;
		}


		//处理后面剩下的分组
		query.prepare(tmpStr);
		for (int i = 10 * rowCount; i < result.size(); i++)
		{
			for (int j = 0; j < num; j++)
			{
				colRes10[j] << colResult[j][i];
			}
		}

		for (int i = 0; i < num; i++)
		{
			query.addBindValue(colRes10[i]);
		}
		
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			return false;
		}
		
	}
	//如果数据量较小 直接使用批处理更快
	else
	{
		
		query.prepare(tmpStr);
		QVariantList *colResult = new QVariantList[num];
		for (int i = 0; i < result.size(); i++)
		{
			for (int j = 0; j < num; j++)
				colResult[j] << result[i][j];
		}
		for (int i = 0; i < num; i++)
		{
			query.addBindValue(colResult[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			return false;
		}
	}
	return true;
}
/*******************************************************************************************
* 函数名称：ExportProjectDb
* 函数介绍：导出sqlite数据库到本地
* 输入参数：（1）dbName：数据库名称路径； （2）ID：projectID
* 输出参数：int
* 返回值  ：0：成功  -1： 建立新连接失败 -2： sql语句执行失败 -3： 事务提交失败
********************************************************************************************/

int SqlOp::ExportProjectDb(const QString &dbName, const QString &ID, const int year)
{
	QTime time;
	time.start();
	// *************建立本地sqlite数据库 并建表
	if (!CreateConnection("localhost", dbName, "", "", "remote2localDB", "QSQLITE"))
	{
		qDebug() << "Create Connection failed";
		return -1;
	}

	QSqlDatabase exportdb = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[exportYear]);

	exportdb.transaction();
	QSqlQuery query(exportdb);




	//建表
	for (int i = 0; i < m_vSqlCreatable.size(); i++)
	{
		if (!query.exec(m_vSqlCreatable[i]))
			qDebug()<<i << "create table failed";
	}



	//查询  MFL_ProjectInfo 表
	const int projectTableCounts = 35; //项目信息表的列数
	QVector<QString> bindValue;
	bindValue.append(ID);
	QString query_projectinfo = "select * from MFL_ProjectInfo where ProjectID = ? ";
	QVector< QVector<QVariant> > projectinfo;
	projectinfo = SqlOp::SelectQuery2(query_projectinfo, projectTableCounts, bindValue, year);
	qDebug() << "select projectinfo time:" << time.elapsed() / 1000.0 << "s";

	//查询 MFL_DevInfo  表
	const int devinfoTableCounts = 20;  //设备信息表的列数
	QString query_devinfo = "select * from MFL_DevInfo where DevID in (select DevID from MFL_ProjectInfo where ProjectID = ?)";
	QVector< QVector<QVariant> > devinfo;
	devinfo = SqlOp::SelectQuery2(query_devinfo, devinfoTableCounts, bindValue, year);
	qDebug() << devinfo[0].size();

	//查询 MFL_DevFactors 表
	const int devFactorsTableCounts = 41;
	QString query_devFactors = QObject::tr("SELECT * FROM MFL_DevFactors WHERE DevID = '%1' ").arg(devinfo[0][0].toString());
	QVector< QVector<QVariant> > devFactorsInfo;
	devFactorsInfo = SqlOp::SelectQuery2(query_devFactors, devFactorsTableCounts,year);
	
	//查询 MFL_DeAdjust 表
	const int deAdjustTableCounts = 26;   //钢板表的列数
	QString query_Adjust = QObject::tr("SELECT * FROM MFL_DeAdjust WHERE DevID = '%1' ").arg(devinfo[0][0].toString());
	QVector< QVector<QVariant> > deAdjustInfo;
	deAdjustInfo = SqlOp::SelectQuery2(query_Adjust, deAdjustTableCounts, year);
	qDebug() << "deAdjustInfo time:" << time.elapsed() / 1000.0 << "s";

	//查询 MFL_PlateInfo  表
	const int plateTableCounts = 33;   //钢板表的列数
	QString query_plateinfo = "select * from MFL_PlateInfo where ProjectID = ? ";
	QVector< QVector<QVariant> > plateinfo;
	plateinfo = SqlOp::SelectQuery2(query_plateinfo, plateTableCounts, bindValue, year);
	qDebug() << "plateinfo time:" << time.elapsed() / 1000.0 << "s";

	//查询MFL_TrackInfo 表
	const int trackInfoTableCounts = 22;   //轨迹信息表的列数
	QString query_trackinfo = "select * from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = ?)";
	QVector< QVector<QVariant> > trackinfo;
	trackinfo = SqlOp::SelectQuery2(query_trackinfo, trackInfoTableCounts, bindValue, year);
	qDebug() << "trackinfo time:" << time.elapsed() / 1000.0 << "s";

	//查询MFL_TrackCheckRecord表
	const int trackRecordTableCounts = 10;
	QString query_trackrecord = "select * from MFL_TrackCheckRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = ?))";
	QVector< QVector<QVariant> > trackrecord;
	trackrecord = SqlOp::SelectQuery2(query_trackrecord, trackRecordTableCounts, bindValue, year);
	qDebug() << trackrecord.size();
	qDebug() << "trackrecord time:" << time.elapsed() / 1000.0 << "s";

	//查询MFL_TrackSSRecord表
	const int trackSSRecordTableCounts = 9;
	QString  query_trackSSRecordInfo = "select * from MFL_TrackSSRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = ?))";
	QVector< QVector<QVariant> > trackSSRecordInfo;
	trackSSRecordInfo = SqlOp::SelectQuery2(query_trackSSRecordInfo, trackSSRecordTableCounts, bindValue, year);
	
	//往数据库插入   MFL_DevInfo 表
	query.prepare("INSERT INTO MFL_DevInfo values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	for (int i = 0; i < devinfo[0].size(); i++)
	{
		query.bindValue(i, devinfo[0][i]);
	}
	if (!query.exec()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		exportdb.rollback();
		return -2;
	}

	//往数据库中插入 MFL_devAdjust表
	query.prepare("INSERT INTO MFL_DeAdjust values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	QVariantList *deAdjust = new QVariantList[deAdjustTableCounts];
	for (int i = 0; i < deAdjustInfo.size(); i++)
	{
		for (int j = 0; j < deAdjustTableCounts; j++)
			deAdjust[j] << deAdjustInfo[i][j];
	}
	for (int i = 0; i < deAdjustTableCounts; i++)
	{
		query.addBindValue(deAdjust[i]);
	}
	if (!query.exec()) //进行批处理，如果出错就输出错误
	{
		qDebug() <<"aaaa"<< query.lastError();
		exportdb.rollback();
		return -2;
	}


	//往数据库中插入 MFL_DevFactors表
	query.prepare("INSERT INTO MFL_DevFactors values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? )");
	QVariantList *devFactors = new QVariantList[devFactorsTableCounts];
	for (int i = 0; i < devFactorsInfo.size(); i++)
	{
		for (int j = 0; j < devFactorsTableCounts; j++)
			devFactors[j] << devFactorsInfo[i][j];
	}
	for (int i = 0; i < devFactorsTableCounts; i++)
	{
		query.addBindValue(devFactors[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() <<"bbbbbbb"<< query.lastError();
		exportdb.rollback();
		return -2;
	}

	//往新数据库插入  MFL_ProjectInfo 表
	query.prepare("INSERT INTO MFL_ProjectInfo values(?,?,? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?, ?,?  )");
	QVariantList *project = new QVariantList[projectTableCounts];
	for (int i = 0; i < projectinfo.size(); i++)
	{
		for (int j = 0; j < projectTableCounts; j++)
			project[j] << projectinfo[i][j];
	}
	for (int i = 0; i < projectTableCounts; i++)
	{
		query.addBindValue(project[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		exportdb.rollback();
		return -2;
	}

	//往新数据库插入  MFL_PlateInfo 表
	query.prepare("INSERT INTO MFL_PlateInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?, ?,?  )");
	QVariantList *plate = new QVariantList[plateTableCounts];
	for (int i = 0; i < plateinfo.size(); i++)
	{
		for (int j = 0; j < plateTableCounts; j++)
			plate[j] << plateinfo[i][j];
	}
	for (int i = 0; i < plateTableCounts; i++)
	{
		query.addBindValue(plate[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		exportdb.rollback();
		return -2;
	}

	//往新数据库插入  MFL_TrackInfo 表
	query.prepare("INSERT INTO MFL_TrackInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?)");
	QVariantList *track = new QVariantList[trackInfoTableCounts];
	for (int i = 0; i < trackinfo.size(); i++)
	{
		for (int j = 0; j < trackInfoTableCounts; j++)
			track[j] << trackinfo[i][j];
	}
	for (int i = 0; i < trackInfoTableCounts; i++)
	{
		query.addBindValue(track[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		exportdb.rollback();
		return -2;
	}

	//往新数据库插入  MFL_TrackCheckRecord 表
	query.prepare("INSERT INTO MFL_TrackCheckRecord values(? ,?, ? , ? , ? , ? , ? , ? , ? , ? )");
	QVariantList *trackcheck = new QVariantList[trackRecordTableCounts];
	for (int i = 0; i < trackrecord.size(); i++)
	{
		for (int j = 0; j < trackRecordTableCounts; j++)
			trackcheck[j] << trackrecord[i][j];
	}
	for (int i = 0; i < trackRecordTableCounts; i++)
	{
		query.addBindValue(trackcheck[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		exportdb.rollback();
		return -2;
	}
	qDebug() << "export time:" << time.elapsed() / 1000.0 << "s";

	//往数据库中插入MFL_TrackSSRecord表
	query.prepare("INSERT INTO MFL_TrackSSRecord values(? ,?, ? , ? , ? , ? , ? , ? , ?  )");
	QVariantList *trackSSRecord = new QVariantList[trackSSRecordTableCounts];
	for (int i = 0; i < trackSSRecordInfo.size(); i++)
	{
		for (int j = 0; j < trackSSRecordTableCounts; j++)
			trackSSRecord[j] << trackSSRecordInfo[i][j];
	}
	for (int i = 0; i < trackSSRecordTableCounts; i++)
	{
		query.addBindValue(trackSSRecord[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() <<"bbbbbbvccc"<< query.lastError();
		exportdb.rollback();
		return -2;
	}
	bool result = exportdb.commit();
	if (result)
	{
		return 0;
	}
	else
	{
		return -3;
	}
}

/*******************************************************************************************
* 函数名称：ImportProjectDbInfo
* 函数介绍：从外部数据文件.db中导入项目信息
* 输入参数：（1）外部数据文件的路径；
* 输出参数：QVector< QVector <QVariant>> 返回projectInfo,
* 返回值  ：操作结果
********************************************************************************************/
QVector< QVector <QVariant>> SqlOp::ImportProjectDbInfo(const QString &dbPath)
{
	QSqlDatabase importDB = QSqlDatabase::addDatabase("QSQLITE", "importDB");
	importDB.setDatabaseName(dbPath);
	if (!importDB.open()) {
		qDebug() << "Database Can't open!";
	}
	QVector< QVector<QVariant> > projectinfo;
	QString queryProject = "SELECT ProjectID FROM MFL_ProjectInfo";
	QVector< QVector <QVariant> > queryResult = SelectQuery3(queryProject, 1, importDB);
	if (queryResult.size() != 0)
	{
		QString projectID = queryResult[0][0].toString();

		//查询  MFL_ProjectInfo 表
		QString query_projectinfo = QObject::tr("select ProjectName,CompanyName,CheckMan,MFLAlert,EnvLength,EdgePlateNumber,EdgePlateWidth,UsedYear,CoatStatus,PlateMaterial,PlateThickness,StoreMaterial,CoatThickness,ProjectID from MFL_ProjectInfo where ProjectID ='%1' ").arg(projectID);

		projectinfo = SelectQuery3(query_projectinfo, 14, importDB);
	}

	return projectinfo;
}
/*******************************************************************************************
* 函数名称：ImportProjectDb
* 函数介绍：将外部数据文件.db中导入本地数据库
* 输入参数：（1）外部数据文件的路径；
* 输出参数：void
* 返回值  ：int 0：成功   -1 ：建立新连接失败   -2：sql语句执行失败  -3：事物提交失败
********************************************************************************************/
int SqlOp::ImportProjectDb(int year, int type)
{


	QTime time;
	time.start();

	
	const int devTableCounts = 20;
	const int deAdjustTableCounts = 26;
	const int deFactorsTableCounts = 41;
	const int projectInfoTableCounts = 35;
	const int plateInfoTableCounts = 33;
	const int trackInfoTableCounts = 22;
	const int checkRecordTableCounts = 10;
	const int checkSSRecordTableCounts = 9;

	QSqlDatabase importDB = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[importYear]);

	//查询  MFL_DevInfo表
	QString query_devinfo = "select * from MFL_DevInfo";
	QVector< QVector <QVariant> > devinfo = SelectQuery3(query_devinfo, devTableCounts, importDB);
	QString devID = devinfo[0][0].toString();

	QString queryProject = "SELECT ProjectID FROM MFL_ProjectInfo";
	QVector< QVector <QVariant> > queryResult = SelectQuery3(queryProject, 1, importDB);
	QString projectID = queryResult[0][0].toString();



	//查询  MFL_deAdjust 表
	QString query_devAdjustInfo = QObject::tr("SELECT * FROM MFL_DeAdjust WHERE DevID = '%1' ").arg(devID);
	QVector< QVector<QVariant> > deAdjustInfo;
	deAdjustInfo = SelectQuery3(query_devAdjustInfo, deAdjustTableCounts, importDB);
	
	//查询  MFL_DevFactors表
	QString query_devFactorsInfo = QObject::tr("SELECT * FROM MFL_DevFactors WHERE DevID = '%1' ").arg(devID);
	QVector< QVector<QVariant> > deFactorsInfo;
	deFactorsInfo = SelectQuery3(query_devFactorsInfo, deFactorsTableCounts, importDB);

	//查询  MFL_ProjectInfo 表
	QString query_projectinfo = QObject::tr("select * from MFL_ProjectInfo where ProjectID = '%1' ").arg(projectID);
	QVector< QVector<QVariant> > projectinfo;
	projectinfo = SelectQuery3(query_projectinfo, projectInfoTableCounts, importDB);


	//查询 MFL_PlateInfo  表
	QString query_plateinfo = QObject::tr("select * from MFL_PlateInfo where ProjectID = '%1' ").arg(projectID);
	QVector< QVector<QVariant> > plateinfo;
	plateinfo = SelectQuery3(query_plateinfo, plateInfoTableCounts, importDB);

	//查询MFL_TrackInfo 表
	QString query_trackinfo = QObject::tr("select * from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = '%1')").arg(projectID);
	QVector< QVector<QVariant> > trackinfo;
	trackinfo = SelectQuery3(query_trackinfo, trackInfoTableCounts, importDB);



	qDebug() << "selected time:" << time.elapsed() / 1000.0 << "s";

	/*-----------------向服务器数据库插入数据-----------------------------*/
	bool flag = true; //判断sql语句是否执行成功

	QSqlDatabase dbSqlOp = QSqlDatabase::database(ThreadParameter::getInstance()->mapDbConnectionName[year]);
	QSqlQuery query(dbSqlOp);

	if (!query.exec("START TRANSACTION"))//开启事物 
	{
		flag = false;
	};

	//查询服务器上是否有该设备 
	int devCount = -1;
	QString devQuery = "select count(*) from MFL_DevInfo where DevID =%1 ";
	query.exec(devQuery.arg(devID));
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else
	{
		qDebug() << "select success";
		query.next();
		devCount = query.value(0).toInt();
	}

	//如果有该设备就更新信息 ，没有就插入
	if (devCount == 0)
	{
		//往数据库插入   MFL_DevInfo 表
		query.prepare("INSERT INTO MFL_DevInfo values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		for (int i = 0; i < devinfo[0].size(); i++)
		{
			query.bindValue(i, devinfo[0][i]);
		}
		if (!query.exec()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			flag = false;
		}

		//插入 MFL_devAdjust表
		query.prepare("INSERT INTO MFL_DeAdjust values(?,?,date(?),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		QVariantList *deAdjust = new QVariantList[deAdjustTableCounts];
		for (int i = 0; i < deAdjustInfo.size(); i++)
		{
			for (int j = 0; j < deAdjustTableCounts; j++)
				deAdjust[j].push_back(deAdjustInfo[i][j]);
		}
		for (int i = 0; i < deAdjustTableCounts; i++)
		{
			query.addBindValue(deAdjust[i]);

		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << "aaaa" << query.lastError();
			flag = false;
		}

		query.prepare("INSERT INTO MFL_DevFactors values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? )");
		QVariantList *devFactors = new QVariantList[deFactorsTableCounts];
		for (int i = 0; i < deFactorsInfo.size(); i++)
		{
			for (int j = 0; j < deFactorsTableCounts; j++)
				devFactors[j] << deFactorsInfo[i][j];
		}
		for (int i = 0; i < deFactorsTableCounts; i++)
		{
			query.addBindValue(devFactors[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << "bbbbbbb" << query.lastError();
			flag = false;
			
		}
	}
	else
	{
		//QString strUpdate = "UPDATE MFL_DevInfo SET DevID = :DevID ,DevName = :DevName ,SysName = :SysName,CheckCompany= :CheckCompany,CheckMan= :CheckMan ,ProductionDate= :ProductionDate ,SellDate= :SellDate ,HardwareVersion= :HardwareVersion,SoftwareVersion= :SoftwareVersion,ChannelNumber= :ChannelNumber,SpaceLength=:SpaceLength,StepLength=:StepLength,DevWidth=:DevWidth,DevLength=:DevLength,DevBackEndLength=:DevBackEndLength,DevFrontEndLength=:DevFrontEndLength,DevWeight=:DevWeight,ServicePhone=:ServicePhone,Remark=:Remark,AdjustGroupID=:AdjustGroupID WHERE DevID = '%1' ";
		QString strUpdate = "UPDATE MFL_DevInfo SET DevID = ?,DevName = ? ,SysName = ?,CheckCompany= ?,CheckMan= ? ,ProductionDate= ? ,SellDate= ? ,HardwareVersion= ?,SoftwareVersion= ?,ChannelNumber= ?,SpaceLength=?,StepLength=?,DevWidth=?,DevLength=?,DevBackEndLength=?,DevFrontEndLength=?,DevWeight=?,ServicePhone=?,Remark=?,AdjustGroupID=? WHERE DevID = '%1' ";
		//Delete_UpdateQuery(strUpdate.arg(devID), devinfo[0], year);

		Delete_UpdateQuery(strUpdate.arg(devID), devinfo[0], year);

		//删除重新插可以起到更新的作用
		QString str2 = "DELETE FROM MFL_DeAdjust WHERE DevID =   '%1' ;";
		QString str3 = "DELETE FROM MFL_DevFactors WHERE DevID = '%1' ;";
		qDebug() << query.exec(str2.arg(devID));
		qDebug() << query.exec(str3.arg(devID));
		
		//插入 MFL_devAdjust表
		query.prepare("INSERT INTO MFL_DeAdjust values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		QVariantList *deAdjust = new QVariantList[deAdjustTableCounts];
		for (int i = 0; i < deAdjustInfo.size(); i++)
		{
			for (int j = 0; j < deAdjustTableCounts; j++)
				deAdjust[j].push_back(deAdjustInfo[i][j]);
		}
		for (int i = 0; i < deAdjustTableCounts; i++)
		{
			query.addBindValue(deAdjust[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << "aaaa" << query.lastError();
			flag = false;
		}

		query.prepare("INSERT INTO MFL_DevFactors values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? )");
		QVariantList *devFactors = new QVariantList[deFactorsTableCounts];
		for (int i = 0; i < deFactorsInfo.size(); i++)
		{
			for (int j = 0; j < deFactorsTableCounts; j++)
				devFactors[j] << deFactorsInfo[i][j];
		}
		for (int i = 0; i < deFactorsTableCounts; i++)
		{
			query.addBindValue(devFactors[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << "bbbbbbb" << query.lastError();
			flag = false;

		}
		
	}



	//插入  MFL_ProjectInfo 表
	query.prepare("INSERT INTO MFL_ProjectInfo values(?,?,?,?,?,?,?,?,?, ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ? , ? ,? )");
	QVariantList *project = new QVariantList[35];
	for (int i = 0; i < projectinfo.size(); i++)
	{
		for (int j = 0; j < 35; j++)
			project[j] << projectinfo[i][j];
	}
	for (int i = 0; i < 35; i++)
	{
		if (i == 32)
			project[i][0] = 0;		//导入的项目设置为未完成
		query.addBindValue(project[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
	{
		qDebug() << query.lastError();
		flag = false;
	}


	if (type == 2)
	{
		//插入  MFL_PlateInfo 表
		query.prepare("INSERT INTO MFL_PlateInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , 0 , ? , 0 , 0 , null , null, null, null , null , null, null, null , ? ,? )");
		QVariantList *plate = new QVariantList[33];
		for (int i = 0; i < plateinfo.size(); i++)
		{
			for (int j = 0; j < 33; j++)
				plate[j] << plateinfo[i][j];
		}
		for (int i = 0; i < 33; i++)
		{
			//最大缺陷，检测百分比，是否有应力，钢板图片  全部置为初始
			if (i == 19)
				continue;
			if (i >= 21 && i <= 30)
			{
				continue;
			}
			else
				query.addBindValue(plate[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			flag = false;
		}

		//插入 MFL_trackInfo 表
		query.prepare("INSERT INTO MFL_TrackInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?)");
		QVariantList *track = new QVariantList[22];
		for (int i = 0; i < trackinfo.size(); i++)
		{
			if (trackinfo[i][3] == 0)//只保存0列
			{
				for (int j = 0; j < 22; j++)
				{
					track[j] << trackinfo[i][j];
				}
			}
		}
		for (int i = 0; i < 22; i++)
		{
			query.addBindValue(track[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
			qDebug() << query.lastError();

	}
	else if (type == 1)
	{


		//查询MFL_TrackCheckRecord表
		QString query_trackrecord = QObject::tr("select * from MFL_TrackCheckRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = '%1'))").arg(projectID);
		QVector< QVector<QVariant> > trackrecord;
		trackrecord = SelectQuery3(query_trackrecord, 10, importDB);

		//查询 MFL_TrackSSRecord 表
		QString query_trackSSRecord = QObject::tr("select * from MFL_TrackSSRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = '%1'))").arg(projectID);
		QVector< QVector<QVariant> > trackSSRecordInfo;
		trackSSRecordInfo = SelectQuery3(query_trackSSRecord, checkSSRecordTableCounts, importDB);

		qDebug() << "selected time:" << time.elapsed() / 1000.0 << "s";

		//插入  MFL_PlateInfo 表
		query.prepare("INSERT INTO MFL_PlateInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ? , ? ,? )");
		QVariantList *plate = new QVariantList[33];
		for (int i = 0; i < plateinfo.size(); i++)
		{
			for (int j = 0; j < 33; j++)
				plate[j] << plateinfo[i][j];
		}
		for (int i = 0; i < 33; i++)
		{
			query.addBindValue(plate[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
		{
			qDebug() << query.lastError();
			flag = false;
		}
		qDebug() << "plateinfo time:" << time.elapsed() / 1000.0 << "s";

		QString sqlString = "INSERT INTO MFL_TrackInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?)";
		QString appendStr = "(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )";
		if (!InsertTable(sqlString, appendStr, trackinfo, trackInfoTableCounts, dbSqlOp))
			flag = false;
		qDebug() << "trackinfo time:" << time.elapsed() / 1000.0 << "s";
		
		sqlString = "INSERT INTO MFL_TrackCheckRecord values(?,?,?,?,?,?,?,?,?,?)";
		appendStr = "(?,?,?,?,?,?,?,?,?,?)";
		if (!InsertTable(sqlString, appendStr, trackrecord,checkRecordTableCounts,dbSqlOp))
		{
			flag = false;
		}
		qDebug() << "check time:" << time.elapsed() / 1000.0 << "s";
	
		sqlString = "INSERT INTO MFL_TrackSSRecord values(?,?,?,?,?,?,?,?,?)";
		appendStr = "(?,?,?,?,?,?,?,?,?)";
		if (!InsertTable(sqlString, appendStr, trackSSRecordInfo, checkSSRecordTableCounts, dbSqlOp))
			flag = false;
		qDebug() << "SS time:" << time.elapsed() / 1000.0 << "s";
	}


	if (!flag)//如果flag==false，回滚。
	{
		query.exec("ROLLBACK");
		return -2; //-2 sql语句执行错误
	}
	else
	{
		bool result = query.exec("COMMIT");
		if (result)
		{
			return 0;
		}
		else
		{
			return -3; //-3 为提交事务失败
		}
	}


}

