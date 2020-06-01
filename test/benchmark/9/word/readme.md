# benchmark

## 概述
    按正常使用的方式来做benchmark. 
    第一阶段: 10亿条
    第二阶段: 1000亿条
    
## word
    文档搜索使用方式
    
    bench环境:
    MacBook Pro (13-inch, 2017, Two Thunderbolt 3 ports)/2.3 GHz Intel Core i5/8 GB 2133 MHz LPDDR3/256GB disk.
 
    结果:
    /Users/zhangxinlong/数据之海/sea/cmake-build-debug/benchmark/9/bench_word_test
    word bench, count: 0, from 1587296850 to 1587296850, speed: 0
    word bench, count: 320000000, from 1587296850 to 1587296853, speed: 106666666
    word bench, count: 640000000, from 1587296850 to 1587296859, speed: 71111111
    word bench, count: 960000000, from 1587296850 to 1587296867, speed: 56470588
    word bench, count: 1000000000, from 1587296850 to 1587296868, speed: 55555555
    
    Process finished with exit code 0
    
    写入10亿条，速率 5000万条/秒, 单记录大小:8字节.
    
 
