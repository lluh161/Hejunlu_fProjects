# 轻量级大模型问答助手
import openai

# 配置大模型接口
openai.api_key="sk-LLUp3cHMls8T17aB949738834aD64989A141599dAdB1EcD8"
openai.base_url="https://api.aihubmix.com/v1"
# 加个超时设置，避免长时间等待
openai.timeout = 10

def ask_ai(prompt):
    resp = openai.ChatCompletion.create(
        model="qwen-turbo",
        messages=[{"role": "user", "content": prompt}]
    )
    return resp.choices[0].message.content

def main():
    print("🤖 AI问答助手已启动 (输入 exit 退出) ")
    while True:
        q = input("你: ")
        if q.lower() == "exit":
            print("再见！")
            break
        ans = ask_ai(q)
        print(f"AI: {ans}\n")

if __name__ == "__main__":
    main()
