# controller
## 概述
控制器，用来控制多个block之间的关系.
当数据插入进来的时候，controller会决定数据写在本机还是其他节点，查询同样.
各个controller node之间通过dht来通信

## 节点发现
节点发现主要用来，发现邻居结点,并且会将它们记录在本地的内存Cache中。

## 数据复制
数据复制是指，一份数据，需要复制N份，以在集群里不会丢失数据.