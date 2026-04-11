//银行家算法：银行家算法是避免死锁的经典算法，由艾兹格・迪杰斯特拉提出
/*
一、算法核心思想
    核心逻辑：
        把操作系统比作银行家，把进程比作贷款客户，每次分配资源前，先预判系统是否处于安全状态
        (存在一个进程执行序列，能让所有进程顺利完成)只有安全状态下，才分配资源；否则拒绝分配，避免死锁
二、算法核心数据结构
    以n个进程、m类资源为例：
        Available：系统剩余资源数（长度为m的数组）
        Max：每个进程的最大资源需求（n×m矩阵）
        Allocation：每个进程已分配的资源（n×m矩阵）
        Need：每个进程还需要的资源（Need[i][j]=Max[i][j]-Allocation[i][j]）
三、算法执行步骤
    计算剩余资源:Available=总资源-所有进程Allocation之和
    安全状态判定:初始化Work=Available，Finish=[false,false,...]
               遍历所有进程，找到Finish[i]=false且Need[i]≤Work的进程
               若找到，执行该进程，释放资源：Work+=Allocation[i]，Finish[i]=true，回到步骤2
               若所有Finish[i]=true，则系统安全；否则不安全
    资源分配：若系统安全，分配资源；否则拒绝分配
*/

//银行家算法伪代码
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

//银行家算法：判断系统是否处于【安全状态】
bool isSafeState(
    vector<int>& available,   //系统可用资源
    vector<vector<int>>& max, //每个进程最大需求
    vector<vector<int>>& alloc//每个进程已分配资源
) {
    int n=max.size();      //进程数
    int m=available.size();//资源种类数

    //1.计算Need矩阵：Need=Max-Allocation
    vector<vector<int>> need(n,vector<int>(m));
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            need[i][j]=max[i][j]-alloc[i][j];

    //2.初始化工作向量
    vector<int> work=available;
    vector<bool> finish(n,false);
    vector<int> safeSequence;//记录安全序列

    //3.寻找安全序列
    while(safeSequence.size()<n){
        bool found=false;

        for(int i=0;i<n;++i) {
            if(!finish[i]){
                //判断当前进程的需求是否≤work
                bool canAllocate=true;
                for(int j=0;j<m;++j){
                    if(need[i][j]>work[j]){
                        canAllocate=false;
                        break;
                    }
                }

                //可以分配：执行进程，释放资源
                if(canAllocate){
                    for(int j=0;j<m;++j)
                        work[j]+=alloc[i][j];

                    finish[i]=true;
                    safeSequence.push_back(i);
                    found=true;
                }
            }
        }

        //再也找不到可执行的进程->系统不安全
        if(!found) break;
    }

    //4.输出结果
    cout<<"\n=====银行家算法检测结果====="<<endl;
    cout<<"安全序列: ";
    for(int p:safeSequence)
        cout<<"P"<<p<<" ";
    cout<<endl;

    //判断所有进程是否都完成
    for(bool f:finish)
        if(!f) return false;

    return true;
}

// ==================== 主函数测试 ====================
int main(){
    //系统资源：3种资源ABC
    //可用资源
    vector<int> available={3, 3, 2};

    //每个进程最大需求
    vector<vector<int>> max={
        {7, 5, 3}, //P0
        {3, 2, 2}, //P1
        {9, 0, 2}, //P2
        {2, 2, 2}, //P3
        {4, 3, 3}  //P4
    };

    //每个进程已分配资源
    vector<vector<int>> alloc={
        {0, 1, 0}, //P0
        {2, 0, 0}, //P1
        {3, 0, 2}, //P2
        {2, 1, 1}, //P3
        {0, 0, 2}  //P4
    };

    //执行检测
    bool safe=isSafeState(available,max,alloc);
    if(safe)
        cout<<"系统处于【安全状态】，不会发生死锁！"<<endl;
    else
        cout<<"系统处于【不安全状态】，可能发生死锁！"<<endl;

    return 0;
}