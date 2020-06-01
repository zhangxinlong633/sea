# arch

数据的海洋

## 里程碑
1. block 操作 -> ok
2. 单结点 -> ok
3. 一亿条数据 -> current
4. 无中心 -> current

## limit
1. phone_list 800万个结点可以访问1PB字节的数据, 一个结点128字节，800节点需要1GB内存空间

## query
查询是不可避免的，查询语言，准备第一阶段实现一套类似flink datastream api. 后续实现类似 flink sql.
还有一个简易接口就是rest，不用像写data stream 那么麻烦，通过浏览器也可以实现.

## todo list
2020-04-01
1. phone list -> ok
2. gossip list -> 废弃
3. http server -> ok

2020-04-05
1. dht -> current
2. http get, post 一亿条数据接口
3. gossip

2020-04-19
1. 1亿条数据用各个模块bechmark. -> ok
2. 1000亿条数据继续benchmark. -> current

2020-04-25
1. 分布式FS, 可以海量的get/put图片
2. 分布式MQ, 可以插入海量的message,再顺序读出来.
3. 分布式时序数据库,可以插入海量的指标，然后聚合分析出来.
4. 分布式文档搜索，插入海量的文档，可以根据关键词来快速查询.



