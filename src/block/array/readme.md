# 列存储
## 概述
一张表可以抽象成一行一行的表头，每个表头指向一个列数据，列数据包含这张表里所有的数据。类似下面的这个图

+-----------------+
+  block id|hdr   +
+-----------------+
+record           +-------------> data block
+-----------------+
+record           +-------------> data block
+-----------------+
+record           +-------------> data block
+-----------------+
+record           +-------------> data block
+-----------------+

## record hdr
record 头表示一个db的基础信息

## record
每一个record代表DB表的一个列描述, 每个record, 可以是数字型，或者是字符型, 
目前只支持数字型的block存储定长的数据

## data block
data block 里包含所有的block数据, record指向第一个data block.
