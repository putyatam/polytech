from openai import OpenAI
from time import time, strftime, localtime

client = OpenAI(base_url='http://localhost:11434/v1', api_key='ollama')

dialog = []
while True:
    user_input = input("Введите ваше сообщение ('stop' для завершения): ")
    time_start = time()
    
    if user_input.lower() == "stop":
        break

    dialog.append({"role": "user", "content": user_input})

    response = client.chat.completions.create(model="deepseek-r1:32b", messages=dialog)

    response_content = response.choices[0].message.content
    print("[{}] Ответ модели({} с):".format(strftime("%H:%M:%S", localtime(time())), round(time() - time_start, 1)),
          response_content)

    dialog.append({"role": "assistant", "content": response_content})
