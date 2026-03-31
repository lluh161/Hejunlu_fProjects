//map与unoedered_map
/*
    底层结构:map红与黑
            unordered_map哈希表
    顺序:map:key自动排序（升序）
            unordered_map无序
    查找速度:map O(logn)
            unordered 平均O(1) 最坏O(n)
    使用场景:需要有序、范围查找、性能稳定、内存紧张map
            只快查/插入/删除、追求速度、不关心顺序、接受偶尔慢一点unorder_map
    迭代器失效:map删哪个，哪个迭代器失效，其他没事
             unordered_map 扩容rehash时，全部失效
    
*/