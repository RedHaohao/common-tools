# common-tools
  一些通用的工具，持续更新

### SQLOP
- 支持mysql sqlite
- 包括增删改查，导入和导出
- mysql导出的是sqlite.db文件，导入mysql时需要原先就有数据库并且建立了默认连接
- sqlite导出时需要 将默认连接的类型改为QSQLITE 在createConnection()中修改
