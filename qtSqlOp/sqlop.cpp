#pragma execution_character_set("utf-8")
#include "sqlOp.h"
#include "qdebug.h"
//#include <sqlite3.h>

SqlOp::SqlOp()
{

}

SqlOp::~SqlOp()
{

}


/*******************************************************************************************
* 函数名称：CreateConnection
* 函数介绍：连接数据库
* 输入参数：（1）HostName：数据库网络地址； （2）DatabaseName：数据库名称； （3）UserName：用户名 ；（4）Password：密码
* 输出参数：无
* 返回值  ：连接成功返回true  失败时返回false
* 注     ：使用MYSQL时 一定要配置好环境 将autocommit关掉 使用拥有相应数据库权限的子用户，具体配置方法见文档（储罐mysql配置方法）
********************************************************************************************/
bool SqlOp::CreateConnection(const QString& HostName, const QString& DatabaseName, const QString& UserName, const QString& Password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); //数据库类型 使用SQLITE时可以改为QSQLITE
	db.setDatabaseName(DatabaseName);
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
* 函数名称：CreateConnection
* 函数介绍：关闭数据库
* 输入参数：无
* 输出参数：无
* 返回值  ：无
********************************************************************************************/
bool SqlOp::CloseConnection()
{
	QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
	if (db.isOpen())
	{
		db.close();
		qDebug() << "close " + db.databaseName() + " success";
		return true;
	}
	return false;
}

/*******************************************************************************************
* 函数名称：SelectQuery
* 函数介绍：查询语句，返回的数据为字符串形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QString> > SqlOp::SelectQuery(QString select, int num)
{
	QVector< QVector <QString> >  result;   //存放查询结果

	QSqlQuery query;
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
* 输入参数：（1）select：sql语句 （2）num：查询的列数 （3）bindValue：动态参数集合
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QString> > SqlOp::SelectQuery(QString select, int num, QVector<QString> bindValue)
{
	QVector< QVector <QString> >  result;   //存放查询结果
	QSqlQuery query;
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
* 输入参数：（1）select：sql语句 （2）num：查询的列数 
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QVariant> > SqlOp::SelectQuery2(QString select, int num)
{
	QVector< QVector <QVariant> >  result;   //存放查询结果
	QSqlQuery query;
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
* 函数名称：SelectQuery2
* 函数介绍：带动态参数，查询语句，返回的数据为可变类型形式
* 输入参数：（1）select：sql语句 （2）num：查询的列数 （3）bindValue：动态参数集合
* 输出参数：result
* 返回值  ：查询结果
********************************************************************************************/
QVector< QVector <QVariant> > SqlOp::SelectQuery2(QString select, int num, QVector<QString> bindValue)
{
	QVector< QVector <QVariant> >  result;   //存放查询结果
	QSqlQuery query;
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
* 输入参数：（1）del：sql语句 
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::Delete_UpdateQuery(QString del)
{
	QSqlQuery query;
	query.exec(del);

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	qDebug() << "Delete_Update success";
	return true;
}

/*******************************************************************************************
* 函数名称：Delete_UpdateQuery
* 函数介绍：带动态参数，删除或者更新语句
* 输入参数：（1）del：sql语句；（2）binfValue：动态参数集合
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::Delete_UpdateQuery(QString del, QVector<QVariant> bindValue)
{
	QSqlQuery query;
	query.prepare(del);
	for (int i = 0; i < bindValue.size(); i++)
	{
		query.bindValue(i, bindValue[i]);
	}
	query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	qDebug() << "Delete_Update success";
	return true;
}

/*******************************************************************************************
* 函数名称：InsertQuery
* 函数介绍：插入语句
* 输入参数：（1）insert：sql语句；
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::InsertQuery(QString insert)
{
	QSqlQuery query;
	query.exec(insert);
	//sqlite3_exec(db, "PRAGMA synchronous = OFF; ", 0, 0, 0);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	qDebug() << "insert success";
	return true;
}

/*******************************************************************************************
* 函数名称：InsertQuery
* 函数介绍：插入语句，带有参数
* 输入参数：（1）insert：sql语句； （2）bindValue：动态参数
* 输出参数：bool
* 返回值  ：操作结果
********************************************************************************************/
bool SqlOp::InsertQuery(QString insert, QVector<QVariant> bindvalue)
{
	QSqlQuery query;
	query.prepare(insert);
	for (int i = 0; i < bindvalue.size(); i++)
	{
		query.bindValue(i, bindvalue[i]);
	}
	query.exec();

	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return false;
	}
	qDebug() << "insert success";
	return true;
}


int SqlOp::ExportProjectDb(const QString &dbName, const QString &ID)
{
	QSqlDatabase exportdb = QSqlDatabase::addDatabase("QSQLITE", "export");
	exportdb.setDatabaseName(dbName);
	if (!exportdb.open()) {
		qDebug() << "Database Can't open!";
		return -1;
	}

	QSqlQuery query(exportdb);
	if (!query.exec("create table MFL_DevInfo ( DevID CHAR(8) not null, DevName VARCHAR(25), SysName VARCHAR(50), CheckCompany VARCHAR(60), CheckMan VARCHAR(30), ProductionDate  DATE, SellDate DATE, HardwareVersion VARCHAR(8), SoftwareVersion VARCHAR(8), ChannelNumber INTEGER, SpaceLength  FLOAT, StepLength FLOAT, DevWidth FLOAT, DevLength FLOAT, DevBackEndLength FLOAT, DevFrontEndLength FLOAT, DevWeight FLOAT, ServicePhone VARCHAR(20), Remark  VARCHAR(200), ThresholdValue1 FLOAT, ThresholdPer1 FLOAT, ThresholdPer1Value FLOAT, ThresholdPer2 FLOAT, ThresholdPer2Value FLOAT, ThresholdPer3 FLOAT, ThresholdPer3Value FLOAT, ThresholdPer4 FLOAT, ThresholdPer4Value FLOAT, Parameter1 FLOAT, Parameter2 FLOAT, Parameter3 FLOAT, Parameter4 FLOAT, Parameter5 FLOAT, Channel1Factor  float, Channel2Factor  float, Channel3Factor  float, Channel4Factor  float, Channel5Factor  float, Channel6Factor  float, Channel7Factor  float, Channel8Factor  float, Channel9Factor  float, Channel10Factor float, Channel11Factor float, Channel12Factor float, Channel13Factor float, Channel14Factor float, Channel15Factor float, Channel16Factor float, Channel17Factor float, Channel18Factor float, Channel19Factor float, Channel20Factor float, Channel21Factor float, Channel22Factor float, Channel23Factor float, Channel24Factor float, Channel25Factor float, Channel26Factor float, Channel27Factor float, Channel28Factor float, Channel29Factor float, Channel30Factor float, Channel31Factor float, Channel32Factor float, Channel33Factor float, Channel34Factor float, Channel35Factor float, Channel36Factor float, primary key (DevID) );"))
		qDebug() << "create  table MFL_DevInfo failed";
	if (!query.exec("create table MFL_DeAdjust ( AdjustID NUMERIC(6) not null, DevID CHAR(8), AdjustTime DATE, Remark VARCHAR(200), ThresholdValue1 FLOAT, ThresholdPer1  FLOAT, ThresholdPer1X FLOAT, ThresholdPer1Value FLOAT, ThresholdPer2  FLOAT, ThresholdPer2X FLOAT, ThresholdPer2Value FLOAT, ThresholdPer3  FLOAT, ThresholdPer3X FLOAT, ThresholdPer3Value FLOAT, ThresholdPer4  FLOAT, ThresholdPer4X FLOAT, ThresholdPer4Value FLOAT, Parameter1 FLOAT, Parameter2 FLOAT, Parameter3 FLOAT, Parameter4 FLOAT, Parameter5 FLOAT, SteelThickness INT, DisValue INT, primary key (AdjustID), foreign key (DevID) references MFL_DevInfo (DevID) );"))
		qDebug() << "create  table MFL_DeAdjust failed";
	if (!query.exec("create table MFL_ProjectInfo ( ProjectID CHAR(36) not null, DevID CHAR(8) not null, ProjectName VARCHAR(50), CompanyName VARCHAR(60), MFLAlert FLOAT, EnvShape VARCHAR(20), PointPosition INTEGER, CoordinatesAutoGen BOOL, EnvLength FLOAT, EnvWidth FLOAT, EnvArea FLOAT, PlateNumber INTEGER, EdgePlateNumber INTEGER, EdgePlateWidth FLOAT, DefectPlateNumber INTEGER default 0, CheckMan VARCHAR(20), CreateTime DATE, CheckTime DATE, UsedYear INTEGER, PlateThickness FLOAT, CoatThickness FLOAT, CoatStatus VARCHAR(50), PlateMaterial VARCHAR(50), StoreMaterial VARCHAR(50), DefectArea20 FLOAT default 0, DefectArea40 FLOAT default 0, DefectArea60 FLOAT default 0, DefectArea80 FLOAT default 0, MaxDefectPercent FLOAT default 0, ProjectPic LONG VARBINARY, IsCompleted BOOL, EdgeCircleDegree INTEGER, Remark VARCHAR(200), primary key (ProjectID), foreign key (DevID) references MFL_DevInfo (DevID) );"))
		qDebug() << "create  table MFL_ProjectInfo failed";
	if (!query.exec("create table MFL_PlateInfo ( PlateID CHAR(36) not null, ProjectID CHAR(36), RowNo int, ColumnNo int, PlateName VARCHAR(50), PointPosition INTEGER, PlateType INTEGER, PlateLength FLOAT, PlateWidth FLOAT, PlateLength2 FLOAT, PlateWidth2 FLOAT, XCoords FLOAT, YCoords FLOAT, CreateTime DATE, CheckTime DATE, DefectArea20 FLOAT default 0, DefectArea40 FLOAT default 0, DefectArea60 FLOAT default 0, DefectArea80 FLOAT default 0, MaxDefectPercent FLOAT default 0, PlateArea FLOAT, ScanPercent FLOAT default 0, IsDefect BOOL default '0', PlatePic LONG VARBINARY, PlatePic20 LONG VARBINARY, PlatePic30 LONG VARBINARY, PlatePic40 LONG VARBINARY, PlatePic50 LONG VARBINARY, PlatePic60 LONG VARBINARY, PlatePic70 LONG VARBINARY, PlatePic80 LONG VARBINARY, Remark VARCHAR(200), DetectDirection INT, primary key (PlateID), foreign key (ProjectID) references MFL_ProjectInfo (ProjectID) );"))
		qDebug() << "create  table MFL_PlateInfo failed";
	if (!query.exec("create table MFL_TrackInfo ( TrackID CHAR(36) not null, PlateID CHAR(36), TrackRowNo  INT, TrackColumnNo INT, XCoords FLOAT, YCoords FLOAT, CheckAngle  INTEGER default 0, CheckDirection INTEGER default 0, TrackLength FLOAT, TrackWidth  FLOAT, CreateTime  DATE, CheckTime DATE, DefectArea20  FLOAT default 0, DefectArea40  FLOAT default 0, DefectArea60  FLOAT default 0, DefectArea80  FLOAT default 0, MaxDefectPercent FLOAT default 0, TrackPic LONG VARBINARY, Remark  VARCHAR(200), ChannelNumber INTEGER, SpaceLength FLOAT, StepLength  FLOAT, primary key (TrackID), foreign key (PlateID) references MFL_PlateInfo (PlateID) );"))
		qDebug() << "create  table MFL_TrackInfo failed";
	if (!query.exec("create table MFL_TrackCheckRecord ( RecordID CHAR(36) not null, TrackID  CHAR(36), ChannelID CHAR(3), CheckTime DATE, XCoords  FLOAT, YCoords  FLOAT, CheckValue FLOAT, WaveletValue FLOAT, DefectPercent  FLOAT default 0, primary key (RecordID), foreign key (TrackID) references MFL_TrackInfo (TrackID) );"))
		qDebug() << "create  table MFL_TrackCheckRecord failed";



	//查询  MFL_ProjectInfo 表
	const int projectTableCounts = 33;
	QVector<QString> bindValue;
	bindValue.append(ID);
	QString query_projectinfo = "select * from MFL_ProjectInfo where ProjectID = ? ";
	QVector< QVector<QVariant> > projectinfo;
	projectinfo = SqlOp::SelectQuery2(query_projectinfo, projectTableCounts, bindValue);

	//查询 MFL_PlateInfo  表
	const int plateTableCounts = 33;   //钢板表的列数
	QString query_plateinfo = "select * from MFL_PlateInfo where ProjectID = ? ";
	QVector< QVector<QVariant> > plateinfo;
	plateinfo = SqlOp::SelectQuery2(query_plateinfo, plateTableCounts, bindValue);

	//查询MFL_TrackInfo 表
	const int trackInfoTableCounts = 22;   //轨迹信息表的列数
	QString query_trackinfo = "select * from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = ?)";
	QVector< QVector<QVariant> > trackinfo;
	trackinfo = SqlOp::SelectQuery2(query_trackinfo, trackInfoTableCounts, bindValue);

	//查询MFL_TrackCheckRecord表
	const int trackRecordTableCounts = 9;
	QString query_trackrecord = "select * from MFL_TrackCheckRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = ?))";
	QVector< QVector<QVariant> > trackrecord;
	trackrecord = SqlOp::SelectQuery2(query_trackrecord, trackRecordTableCounts, bindValue);
	exportdb.transaction();

	//往新数据库插入  MFL_ProjectInfo 表

	query.prepare("INSERT INTO MFL_ProjectInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?, ?,?  )");
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
		qDebug() << query.lastError();

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
		qDebug() << query.lastError();

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
		qDebug() << query.lastError();

	//往新数据库插入  MFL_TrackCheckRecord 表
	query.prepare("INSERT INTO MFL_TrackCheckRecord values(? , ? , ? , ? , ? , ? , ? , ? , ? )");
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
		qDebug() << query.lastError();

	bool result = exportdb.commit();
	if (result)
	{
		return 0;
	}
	else
	{
		//return -2;
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
* 返回值  ：
********************************************************************************************/
int SqlOp::ImportProjectDb(const QString &dbPath,int type)
{
	/*-----------------从外部数据文件db中获取数据-----------------------------*/
	QSqlDatabase importDB = QSqlDatabase::addDatabase("QSQLITE", "importDB");
	importDB.setDatabaseName(dbPath);
	if (!importDB.open()) {
		qDebug() << "Database Can't open!";
	}

	QString queryProject = "SELECT ProjectID FROM MFL_ProjectInfo";
	QVector< QVector <QVariant> > queryResult = SelectQuery3(queryProject, 1, importDB);
	QString projectID = queryResult[0][0].toString();

	//查询  MFL_ProjectInfo 表
	QString query_projectinfo = QObject::tr("select * from MFL_ProjectInfo where ProjectID = '%1' ").arg(projectID);
	QVector< QVector<QVariant> > projectinfo;
	projectinfo = SelectQuery3(query_projectinfo, 33, importDB);


	//查询 MFL_PlateInfo  表
	QString query_plateinfo = QObject::tr("select * from MFL_PlateInfo where ProjectID = '%1' ").arg(projectID);
	QVector< QVector<QVariant> > plateinfo;
	plateinfo = SelectQuery3(query_plateinfo, 33, importDB);

	//查询MFL_TrackInfo 表
	QString query_trackinfo = QObject::tr("select * from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = '%1')").arg(projectID);
	QVector< QVector<QVariant> > trackinfo;
	trackinfo = SelectQuery3(query_trackinfo, 22, importDB);

	//查询MFL_TrackCheckRecord表
	QString query_trackrecord = QObject::tr("select * from MFL_TrackCheckRecord where TrackID in (select TrackID from MFL_TrackInfo where PlateID in(select PlateID from MFL_PlateInfo where ProjectID = '%1'))").arg(projectID);
	QVector< QVector<QVariant> > trackrecord;
	trackrecord = SelectQuery3(query_trackrecord, 9, importDB);


	/*-----------------向本地数据插入数据-----------------------------*/
	QSqlDatabase dbSqlOp = QSqlDatabase::database();
	QSqlQuery query(dbSqlOp);
	dbSqlOp.transaction();

	//往新数据库插入  MFL_ProjectInfo 表
	query.prepare("INSERT INTO MFL_ProjectInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ? , ? ,? )");
	QVariantList *project = new QVariantList[33];
	for (int i = 0; i < projectinfo.size(); i++)
	{
		for (int j = 0; j < 33; j++)
			project[j] << projectinfo[i][j];
	}
	for (int i = 0; i < 33; i++)
	{
		if (i == 30)
			project[i][0]=0;		//导入的项目设置为未完成
		query.addBindValue(project[i]);
	}
	if (!query.execBatch()) //进行批处理，如果出错就输出错误
		qDebug() << query.lastError();
	if (type == 2)
	{
		//往新数据库插入  MFL_PlateInfo 表
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
			qDebug() << query.lastError();

	}
	if (type == 1)
	{
		//往新数据库插入  MFL_PlateInfo 表
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
			qDebug() << query.lastError();

		//往新数据库插入  MFL_TrackInfo 表
		query.prepare("INSERT INTO MFL_TrackInfo values(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ?, ?)");
		QVariantList *track = new QVariantList[22];
		for (int i = 0; i < trackinfo.size(); i++)
		{
			for (int j = 0; j < 22; j++)
				track[j] << trackinfo[i][j];
		}
		for (int i = 0; i < 22; i++)
		{
			query.addBindValue(track[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
			qDebug() << query.lastError();

		//往新数据库插入  MFL_TrackCheckRecord 表
		query.prepare("INSERT INTO MFL_TrackCheckRecord values(? , ? , ? , ? , ? , ? , ? , ? , ? )");
		QVariantList *trackcheck = new QVariantList[9];
		for (int i = 0; i < trackrecord.size(); i++)
		{
			for (int j = 0; j < 9; j++)
				trackcheck[j] << trackrecord[i][j];
		}
		for (int i = 0; i < 9; i++)
		{
			query.addBindValue(trackcheck[i]);
		}
		if (!query.execBatch()) //进行批处理，如果出错就输出错误
			qDebug() << query.lastError();
	}
	
	bool result = dbSqlOp.commit();
	if (result)
	{
		return 0;
	}
	else
	{
		return -2;
	}
	return 0;
}
