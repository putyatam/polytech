import json
from globals import *

# Получение всех данных
def get_data():
    try:
        with open(bd_file, "r") as bd:
            if bd:
                return json.load(bd)
            return
    except Exception as error:
        print("Ошибка {} при сборе данных из базы.".format(error))
        return {}


# Получаем токен
def get_token(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )

    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["info"][0]
    return False


# Получение номера группы
def get_group(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["info"][1]
    return False


# Получение статуса включения уведомлений
def get_notifications(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["notifications"][0]
    return False


# Получения выбранных меток для уведомлений
def get_user_labels(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["notifications"][1]
    return False


# Получения выбранного времени для уведомлений
def get_notifications_time(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["notifications"][2]
    return False


# Получение статуса автозаполнения 
def get_auto_switch(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["auto"][0]
    return False


# Получение названия проекта, в который будут сохраняться занятия из расписания
def get_name_project(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["auto"][1]
    return False


# Получение выбранного количества дней для автозаполнения
def get_auto_time(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["auto"][2]
    return False


# Получение даты последнего автоматически заполненного дня 
def get_date_of_last(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if str(user_id) in data.keys():
        return data.get(str(user_id))["settings"]["auto"][3]
    return False


# Сохранение все информации об юзере
def save_user(user_id, token, group, notifications_switch, labels, notifications_time, auto_switch, name_project,
              auto_time, date_of_last):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if data.get(str(user_id)):
        data.pop(str(user_id))
    data[user_id] = {"info": [token, group],
                     "settings": {
                         "notifications": [notifications_switch, labels, notifications_time],
                         "auto": [auto_switch, name_project, auto_time, date_of_last]}
                     }
    try:
        with open(bd_file, "w") as bd:
            json.dump(data, bd)
    except Exception as error:
        print("Ошибка {} при загрузке обновлённых данных в базу.".format(error))


# Удаление юзера из базы данных
def delete_user(user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    data = get_data()
    if data.get(str(user_id)):
        data.pop(str(user_id))
    try:
        with open(bd_file, "w") as bd:
            json.dump(data, bd)
    except Exception as error:
        print("Ошибка {} при удалении пользователя из базы.".format(error))
        return False
    return True