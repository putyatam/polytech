
import data_functions
import todoist_api_functions
import globals
import buttons_functions
import replies_functions
import datetime
import inspect



@globals.bot.message_handler(commands=["start"])
def send_start(message):
    user_id = message.chat.id
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if data_functions.get_token(user_id):
        globals.bot.send_message(user_id,
                         "Ваш токен уже сохранён!\nВоспользуйтесь меню."
                         )
    else:
        globals.bot.send_message(user_id,
                         "Ваш токен не сохранён!\nДля использования бота, зайдите в Профиль через меню и добавьте"
                         " ваш токен")


@globals.bot.message_handler(commands=["profile"])
def send_profile(message):
    user_id = message.chat.id
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    globals.bot.send_message(user_id, "{}, Ваш профиль:\n"
                              "Токен: {}\n"
                              "Группа: {}\n".format(message.chat.first_name,
                                                    "<tg-spoiler>{}</tg-spoiler>".format(data_functions.get_token(user_id))
                                                    if data_functions.get_token(user_id) else "отсутствует",
                                                    data_functions.get_group(user_id) if data_functions.get_group(user_id) else "отсутствует"
                                                    ),
                     reply_markup=globals.keyboard_profile_with_delete if data_functions.get_token(user_id)
                     else globals.keyboard_profile,
                     parse_mode="HTML"
                     )


@globals.bot.message_handler(commands=["settings"])
def send_settings(message):
    user_id = message.chat.id
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    globals.bot.send_message(user_id,
                     "Выберите, что хотите настроить:",
                     reply_markup=globals.keyboard_settings
                     )


@globals.bot.message_handler(commands=["calendar"])
def send_calendar(message):
    user_id = message.chat.id
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if not data_functions.get_token(user_id):
        globals.bot.send_message(user_id,
                         "Ваш токен не сохранён!\nДля использования бота, зайдите в Профиль через меню и добавьте"
                         " ваш токен")
        return
    todoist_api_functions.send_days(user_id, datetime.datetime.today())


@globals.bot.callback_query_handler(func=lambda call: True)
def callback(call):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, call.message.chat.id)
        )
    try:
        user_id = call.message.chat.id

        match call.data.split(";")[0]:
            case "edit_token":
                buttons_functions.button_edit_token(call)

            case "edit_group":
                buttons_functions.button_edit_group(call)

            case "show_profile" | "not_add_table" | "add_table" | "delete_profile_no":
                buttons_functions.button_show_profile(call, user_id)

            case "delete_profile":
                buttons_functions.button_delete_profile(call, user_id)

            case "delete_profile_yes":
                buttons_functions.button_delete_profile_yes(call, user_id)

            case "show_settings":
                buttons_functions.button_show_settings(call, user_id)

            case "settings_notifications":
                buttons_functions.button_settings_notifications(call, user_id)

            case "settings_notifications_switch":
                buttons_functions.button_settings_notifications_switch(call, user_id)

            case "settings_notifications_time":
                buttons_functions.button_settings_notifications_time(call, user_id)

            case "settings_notifications_labels":
                buttons_functions.button_settings_notifications_labels(call, user_id)

            case "label_switch":
                buttons_functions.button_label_switch(call, user_id)

            case "settings_auto":
                buttons_functions.button_settings_auto(call, user_id)


            case "settings_auto_time":
                buttons_functions.button_settings_auto_time(call, user_id)

            case "settings_auto_apply":
                buttons_functions.button_settings_auto_apply(call, user_id)

            case "settings_auto_switch":
                buttons_functions.button_settings_auto_switch(call, user_id)

            case "prev_week":
                buttons_functions.button_prev_week(call, user_id)

            case "next_week":
                buttons_functions.button_next_week(call, user_id)
            
            case "open_day":
                buttons_functions.button_open_day(call, user_id)
                
            case "delete_task":
                buttons_functions.button_delete_task(call, user_id)
            
            case "apply_del":
                buttons_functions.button_apply_del(call, user_id)
            
            case "deletion_yes":
                buttons_functions.button_deletion_yes(call, user_id)
                
            case "deletion_no":
                buttons_functions.button_deletion_no(call, user_id)
            case _:
                print("Не найден нужный callback")
                return
    except Exception as error:
        print("Ошибка callback: {}".format(error))


@globals.bot.message_handler(func=lambda message: message.text)
def response_message_text(message):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("/")[-1], 
        inspect.currentframe().f_code.co_name, message.chat.id)
        )
    try:
        user_id = message.chat.id
        match globals.user_states.get(user_id):
            case "state_wait_token":
                replies_functions.wait_token(message, user_id)

            case "state_wait_group":
                replies_functions.wait_group(message, user_id)

            case _:
                print("Сообщение не может быть обработано")
                return
            
    except Exception as error:
        print("Ошибка responce:{}".format(error))


def bot_polling():
    globals.bot.polling(long_polling_timeout=5)


while True:
    try:
        bot_polling()
        print("00a0a0aa")
    except Exception as error:
        print("Ошибка (bot_polling): {}".format(error))

