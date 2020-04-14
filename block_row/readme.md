# sea block

block 主要用来存储原始数据
1. block 主要分为两块，index和data, index存储索引，索引里包含时间和record id, data里是各种数据
2. 所有的数据在插入的时候都是append到文件后
3. 查询支持三种方式：1. 按时间查，2. 一次查多少条，3. 按record_id查

