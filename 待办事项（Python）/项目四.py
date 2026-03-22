#待办事项管理工具
#功能：添加任务three、查看任务four、本地存储two、删除已完成的five

def tasks_one():
    try:
        with open("tasks.txt", "r", encoding="utf-8") as f:
            return [line.strip() for line in f.readlines()]
    except:
        return []

def tasks_two(tasks):
    with open("tasks.txt", "w", encoding="utf-8") as f:
        for task in tasks:
            f.write(task + "\n")

def task_three(tasks):
    content = input("请输入任务：")
    if content.strip():
        tasks.append(content)
        save_tasks(tasks)
        print("添加成功！")
    else:
        print("内容不能为空！")

def tasks_four(tasks):
    if not tasks:
        print("暂无任务")
        return
    print("\n==== 待办列表 ====")
    for i, t in enumerate(tasks, 1):
        print(f"{i}. {t}")

def task_five(tasks):
    """删除任务（新增功能）"""
    show_tasks(tasks)
    if not tasks:
        return
    try:
        idx = int(input("请输入要删除的任务序号：")) - 1
        if 0 <= idx < len(tasks):
            deleted = tasks.pop(idx)
            save_tasks(tasks)
            print(f"✅ 已删除：{deleted}")
        else:
            print("❌ 序号不存在！")
    except:
        print("❌ 输入无效！")
def main():
    tasks = tasks_one()
    while True:
        print("\n1.添加 2.查看 3.删除 4.退出")
        c = input("请选择：")
        if c == "1":
            task_three(tasks)
        elif c == "2":
            tasks_four(tasks)
        elif c == "3":
            task_five(tasks)
        elif c=="4":
            print("退出成功")
            break

if __name__ == "__main__":
    main()
