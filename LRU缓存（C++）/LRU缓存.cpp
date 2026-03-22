/*LRU：高级数据结构组合设计能力
要求:存 key-value
         有固定容量,满了就删掉最久没使用的那个
         支持两个操作：get(key)、put(key, value)*/

//需求
/*1、存什么：key-value键值对
  2、做什么操作：get查询、put插入/更新
  3、限制：固定容量，所有操作O(1)
  4、规则：满了淘汰最久未使用，访问之后要变最新*/

//难点
/*1、如何快速查找？
  2、如何记录使用顺序？
  3、如何快速移动/删除节点？*/

//工具
/*1、哈希表：O(1)查找
  2、双向链表：O(1)移动、删除、维护顺序*/

//功能
/*1、移除节点
  2、添加到头部
  3、移动到头部
  4、删除尾部（淘汰）
  5、get/put*/

//边界
/*1、key不存在：返回 -1
  2、容量满：淘汰尾节点
  3、必须同时操作链表+哈希表
  4、只有淘汰时才delete节点*/

#include <iostream>
#include <unordered_map>//哈希表:查找快
#include <list>//双向链表：顺序清晰
using namespace std;
 
//双向链表节点
struct Node{
    //结构信息
    int key;
    int value;
    Node* prev;
    Node* next;

    //初始化：构造函数
    Node(int k,int v):key(k),value(v),prev(nullptr),next(nullptr){}
};

//LRUCache类
class LRUCache{
private:
    //双向链表的虚拟头节点，尾节点
    Node* head;
    Node* tail;
    //哈希表
    unordered_map<int,Node*> cache;
    //容量
    int cap;

    //删除任意节点
    void removeNode(Node* node);
    //最近使用插入头部
    void addToHead(Node* node);
    //访问过的的移动到头部
    void moveToHead(Node* node);
    //删除尾部最久未使用的节点
    void removeTail();

public:
    //构造函数
    LRUCache(int capacity){
        cap=capacity;
        head=new Node(-1,-1);
        tail=new Node(-1,-1);
        head->next=tail;
        tail->next=head;
    }
    //获取数据
    int get(int key);
    //更新数据
    void put(int kry,int value);
};

//删除指定节点
void LRUCache::removeNode(Node* node){
    Node* cur=node->next;
    cur->prev=node->prev;
    node->prev->next=cur;
}

//插到头节点后面
void LRUCache::addToHead(Node* node){
    node->next=head->next;
    node->prev=head;
    head->next=node;
    head->next->prev=node;
}

//先删除，再插入头部
void LRUCache::moveToHead(Node* node){
    removeNode(node);
    addToHead(node);
}

//删除尾部节点
void LRUCache::removeTail(){
    Node* delNode=tail->prev;//尾节点
    removeNode(delNode);//从链表移除
    cache.erase(delNode->key);//从哈希表删除
    delete delNode;//释放内存
}

//实现get函数
/*查哈希表，找不到返回 -1
  找到 → 把节点移到头部
  返回对应的 value*/
int LRUCache::get(int key){
    //在哈希表中没有找到key
    if(cache.find(key)==cache.end()){
        return -1;
    }
    //找到
    Node* node=cache[key];

    //放到头部
    moveToHead(node);

    //返回值
    return node->value;
}

//实现put函数
/*如果 key 已存在 → 更新值，并移到头部
  如果 key 不存在 → 创建新节点，加到头部
  加入后超出容量 → 删除尾节点（最久未使用），同时删除哈希表对应项
  记得维护哈希表*/
void LRUCache::put(int key,int value){
    //存在:值更新+到头部
    if (cache.find(key)!=cache.end()){
        Node* node=cache[key];
        node->value=value;
        moveToHead(node);
        return;
    }
    //不存在：建立新节点
    Node* newNode=new Node(key,value);
    cache[key]=newNode;//加入哈希表
    addToHead(newNode);//加入链表头部
    //判断是否超出容量
    if(cache.size()>cap){
        Node* delNode=tail->prev;
        removeNode(delNode);//删除久久未使用的节点
        cache.erase(delNode->key);//删除哈希表
        delete delNode;//释放内存
    }
}

//主程序入口
int main(){
    LRUCache lru(2);

    lru.put(1,10);
    lru.put(2,20);

    cout<<lru.get(1)<<endl;

    lru.put(3,30);
    cout<<lru.get(2)<<endl;

    lru.put(4,40);
    cout<<lru.get(1)<<endl;
    cout<<lru.get(3)<<endl;
    cout<<lru.get(4)<<endl;

    return 0;
}