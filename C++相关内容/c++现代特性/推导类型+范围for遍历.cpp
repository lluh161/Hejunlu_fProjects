//类型推导：auto/decltype不用手写的类型，让编译器自动识别
/*
    类型推导：
    auto作用：自动判断变量类型，int/double/const char*
        规则:auto会自动去掉引用 &
            想保留引用，写auto&
            想只读不修改，写const auto&
        使用:配合容器用:vector<int> v={1,2,3};
                      auto it=v.begin();//以前要写vector<int>::iterator it=v.begin();
            范围for用:for(auto x:v){
                        cout<<x<<endl;
                    }
    精确推导：
    decltype作用:精确推导表达式的类型，保留&和const，比auto更老实
            用法:int x = 10;
                decltype(x)   a=20;//int  
                decltype((x)) b=x;//int&带括号会变成引用）
            推导函数返回值:template <typename T, typename U>
                        auto add(T t, U u)->decltype(t + u) {
                            return t + u;
                        }

    区分:auto：看右边值，猜类型
        decltype：看表达式，原封不动返回类型
*/

//范围for：用来遍历数组、vector、字符串的简写版
/*
    写法:把v里的每个元素，依次拿出来放进x，循环一遍
        for (auto x:v){
            cout<<x<<" ";
        }
    三种常用形式:值拷贝:复制一份，不修改原数据
                    for (auto x : v){ ... }
               引用:直接用原数据，可修改
                    for (auto& x : v){
                        x = 0;//会修改vector里的值
                    }
               常量引用:只读，效率高
                    for (const auto& x : v){ ... }
*/