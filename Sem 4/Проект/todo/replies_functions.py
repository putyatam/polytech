from globals import *
import requests
from data_functions import *
import polyparsing

def wait_token(message, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    try:
        if not re.match(r"^[a-z0-9]+$", message.text):
            raise ValueError("Токен не соответствует маске")
        url = "https://api.todoist.com/rest/v2/projects"
        headers = {
            "Authorization": f"Bearer {message.text}"
        }
        response = requests.get(url, headers=headers)

        if response.status_code == 200:
            save_user(user_id,
                        message.text,
                        get_group(user_id),
                        get_notifications(user_id),
                        get_user_labels(user_id),
                        get_notifications_time(user_id),
                        get_auto_switch(user_id),
                        get_name_project(user_id),
                        get_auto_time(user_id),
                        get_date_of_last(user_id))
            bot.send_message(user_id, "Ваш токен успешно сохранён!")
            user_states.pop(user_id)
            return
        else:
            bot.send_message(user_id, "Токен недействителен. Попробуйте снова.")
            return

    except ValueError as error:
        print("Ошибка {}".format(error))
        bot.send_message(user_id, "Токен недействителен. Попробуйте снова.")
        return
    except requests.exceptions.RequestException as error:
        print("Ошибка {} при отправке запроса сайту todoist.".format(error))
        return


def wait_group(message, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    try:
        if not re.match(r"\d+[\\/]\d+", message.text):
            raise ValueError("Группа не соответствует маске: {}".format(message.text))
        text = message.text.replace("\\", "/")
        link = polyparsing.get_link_table(text)
        if link:
            save_user(user_id,
                        get_token(user_id),
                        text,
                        get_notifications(user_id),
                        get_user_labels(user_id),
                        get_notifications_time(user_id),
                        get_auto_switch(user_id),
                        get_name_project(user_id),
                        get_auto_time(user_id),
                        get_date_of_last(user_id))
            keyboard_adding_table = types.InlineKeyboardMarkup().add(
                types.InlineKeyboardButton("Да", callback_data="add_table"),
                types.InlineKeyboardButton("Нет", callback_data="not_add_table"),
            )
            bot.send_message(user_id,
                                "Расписание группы найдено. Хотите ли добавить расписание вашей группы в "
                                "свой список дел? \nВы всегда сможете сделать позже в настройках.",
                                reply_markup=keyboard_adding_table)
            user_states.pop(user_id)
            return
        else:
            raise ValueError("Неверный номер группы: {}".format(message.text))
    except Exception as error:
        print("Произошла ошибка: {}".format(error))
        bot.send_message(user_id, "Неверный номер группы. Попробуйте снова")
        return