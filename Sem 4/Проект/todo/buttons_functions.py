from data_functions import *
from globals import *
from datetime import datetime, timedelta
from todoist_api_functions import *
from additional_functions import *

def button_edit_token(call):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, call.message.chat.id)
        )
    user_states[call.message.chat.id] = "state_wait_token"

    keyboard_back = types.InlineKeyboardMarkup().add(
        types.InlineKeyboardButton("Назад", callback_data="show_profile")
    )

    bot.edit_message_text(
        chat_id=call.message.chat.id,
        message_id=call.message.message_id,
        text="Отправьте свой токен:",
        reply_markup=keyboard_back
    )


def button_edit_group(call):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, call.message.chat.id)
        )
    user_states[call.message.chat.id] = "state_wait_group"

    keyboard_back = types.InlineKeyboardMarkup().add(
        types.InlineKeyboardButton("Назад", callback_data="show_profile")
    )

    bot.edit_message_text(
        chat_id=call.message.chat.id,
        message_id=call.message.message_id,
        text="Отправьте номер своей группы:",
        reply_markup=keyboard_back
    )


def button_show_profile(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if user_id in user_states:
        user_states.pop(user_id)

    if call.data == "add_table":
        today = datetime.today()
        for i in range(0, 7):
            day_table = add_day_from_table(user_id, (today + timedelta(days=i)), get_name_project(user_id))
            if day_table:
                bot.edit_message_text(chat_id=call.message.chat.id,
                                        message_id=call.message.message_id,
                                        text="Загрузка расписания.\n{}".format(
                                            day_table)
                                        )

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="{}, Ваш профиль:\n"
                                "Токен: <tg-spoiler>{}</tg-spoiler>\n"
                                "Группа: {}\n".format(
                                call.message.chat.first_name,
                                get_token(user_id) if get_token(user_id) else "отсутствует",
                                get_group(user_id) if get_group(user_id) else "отсутствует"
                            ),
                            reply_markup=keyboard_profile_with_delete if get_token(user_id)
                            else keyboard_profile,
                            parse_mode="HTML"
                            )


def button_delete_profile(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if user_id in user_states:
        user_states.pop(user_id)

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Вы уверены что хотите удалить профиль?",
                            reply_markup=keyboard_delete_profile,
                            )


def button_delete_profile_yes(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    bot.edit_message_text(chat_id=call.message.chat.id,
                          message_id=call.message.message_id,
                          text="Ваш профиль удалён" if delete_user(user_id) else "Ошибка удаления профиля"
                          )


def button_show_settings(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if user_id in user_auto_switches.keys():
        user_auto_switches.pop(user_id)

    if user_id in user_auto_times.keys():
        user_auto_times.pop(user_id)

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите, что хотите настроить:",
                            reply_markup=keyboard_settings
                            )


def button_settings_notifications(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    if user_id in user_all_labels.keys():
        user_all_labels.pop(user_id)

    keyboard_settings_notifications = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton("Уведомления {}".format("✅" if get_notifications(user_id) else "❌"),
                                    callback_data="settings_notifications_switch")],
        [types.InlineKeyboardButton("Напоминать за {} минут".format(
            times_for_notifications[get_notifications_time(user_id)]
            if get_notifications_time(user_id) else times_for_notifications[0]),
                                    callback_data="settings_notifications_time")],
        [types.InlineKeyboardButton("Настроить метки",
                                    callback_data="settings_notifications_labels;0")],
        [types.InlineKeyboardButton("Назад",
                                    callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text=("Уведомления буду приходить для задач со следующими метками:"
                                "\n\t\t\t{}".format("\n\t\t\t".join(get_user_labels(user_id)))
                                if get_user_labels(user_id) else "Метки для уведомлений не выбраны.") +
                                "\nВыберите действие:",
                            reply_markup=keyboard_settings_notifications
                            )


def button_settings_notifications_switch(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    save_user(user_id,
              get_token(user_id),
              get_group(user_id),
              not get_notifications(user_id),
              get_user_labels(user_id),
              get_notifications_time(user_id),
              get_auto_switch(user_id),
              get_name_project(user_id),
              get_auto_time(user_id),
              get_date_of_last(user_id))

    keyboard_settings_notifications = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton("Уведомления {}".format("✅" if get_notifications(user_id) else "❌"),
                                    callback_data="settings_notifications_switch")],
        [types.InlineKeyboardButton("Напоминать за {} минут".format(
            times_for_notifications[get_notifications_time(user_id)]
            if get_notifications_time(user_id) else times_for_notifications[0]),
            callback_data="settings_notifications_time")],
        [types.InlineKeyboardButton("Настроить метки",
                                    callback_data="settings_notifications_labels;0")],
        [types.InlineKeyboardButton("Назад",
                                    callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text=("Уведомления буду приходить для задач со следующими метками:"
                                "\n\t\t\t{}".format("\n\t\t\t".join(get_user_labels(user_id)))
                                if get_user_labels(user_id) else "Метки для уведомлений не выбраны.") +
                                "\nВыберите действие:",
                            reply_markup=keyboard_settings_notifications
                            )


def button_settings_notifications_time(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    save_user(user_id,
              get_token(user_id),
              get_group(user_id),
              get_notifications(user_id),
              get_user_labels(user_id),
              (get_notifications_time(user_id) + 1) % len(times_for_notifications),
              get_auto_switch(user_id),
              get_name_project(user_id),
              get_auto_time(user_id),
              get_date_of_last(user_id))

    keyboard_settings_notifications = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton("Уведомления {}".format("✅" if get_notifications(user_id) else "❌"),
                                    callback_data="settings_notifications_switch")],
        [types.InlineKeyboardButton("Напоминать за {} минут".format(
            times_for_notifications[get_notifications_time(user_id)]
            if get_notifications_time(user_id) else times_for_notifications[0]),
            callback_data="settings_notifications_time")],
        [types.InlineKeyboardButton("Настроить метки",
                                    callback_data="settings_notifications_labels;0")],
        [types.InlineKeyboardButton("Назад",
                                    callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text=("Уведомления буду приходить для задач со следующими метками:"
                                "\n\t\t\t{}".format("\n\t\t\t".join(get_user_labels(user_id)))
                                if get_user_labels(user_id) else "Метки для уведомлений не выбраны.") +
                                "\nВыберите действие:",
                            reply_markup=keyboard_settings_notifications
                            )


def button_settings_notifications_labels(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    page = int(call.data.split(";")[1])

    if not user_all_labels.get(user_id):
        api = TodoistAPI(get_token(user_id))
        all_labels = sorted(set([label.name for label in api.get_labels()] + api.get_shared_labels()))
        user_labels = get_user_labels(user_id)
        user_all_labels[user_id] = [user_labels if user_labels else [], all_labels]

    buttons = [types.InlineKeyboardButton(
        "✅ {}".format(label if len(label) <= 14 else label[:14] + "...")
        if label in user_all_labels[user_id][0] else
        "❌ {}".format(label if len(label) <= 14 else label[:14] + "..."),
        callback_data="label_switch;{};{}".format(label if len(label) <= 14 else label[:14] + "...", page))
        for label in user_all_labels[user_id][1]]

    keyboard_labels = create_keyboard_with_page(buttons, page, "settings_notifications", "settings_notifications_labels", "settings_notifications_labels", 3)

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите метки, для которых будут включены уведомления:",
                            reply_markup=keyboard_labels
                            )


def button_label_switch(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    label_name = call.data.split(";")[1]
    page = call.data.split(";")[2]
    if label_name.endswith("..."):
        label_name = label_name.replace("...", "")
        for i in user_all_labels[user_id][1]:
            if i.startswith(label_name):
                label_name = i
                break
    if label_name in user_all_labels[user_id][0]:
        user_all_labels[user_id][0].remove(label_name)
    else:
        user_all_labels[user_id][0].append(label_name)

    save_user(user_id,
                get_token(user_id),
                get_group(user_id),
                get_notifications(user_id),
                user_all_labels[user_id][0],
                get_notifications_time(user_id),
                get_auto_switch(user_id),
                get_name_project(user_id),
                get_auto_time(user_id),
                get_date_of_last(user_id))

    buttons = [types.InlineKeyboardButton(
        "✅ {}".format(label if len(label) <= 14 else label[:14] + "...") if label in user_all_labels[user_id][0] else "❌ {}".format(label if len(label) <= 14 else label[:14] + "..."),
        callback_data="label_switch;{};{}".format(label if len(label) <= 14 else label[:14] + "...", page))
        for label in user_all_labels[user_id][1]]
    
    keyboard_labels = create_keyboard_with_page(buttons, int(page), "settings_notifications", "settings_notifications_labels", "settings_notifications_labels")

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите метки, для которых будут включены уведомления:",
                            reply_markup=keyboard_labels
                            )


def button_settings_auto(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    if not (user_id in user_auto_times):
        user_auto_times[user_id] = get_auto_time(user_id) if get_auto_time(user_id) else 0

    if not (user_id in user_auto_switches):
        user_auto_switches[user_id] = get_auto_switch(user_id)

    keyboard_settings_auto = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton(
            "Автозаполнение {}".format(
                "✅" if get_auto_switch(user_id) else "❌"
            ),
            callback_data="settings_auto_switch")],
        [types.InlineKeyboardButton(
            "Дней: {}".format(times_for_auto[get_auto_time(user_id)]),
            callback_data="settings_auto_time")],
        [types.InlineKeyboardButton(
            "Назад",
            callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите действие:",
                            reply_markup=keyboard_settings_auto
                            )


def button_settings_auto_time(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    user_auto_times[user_id] = int(user_auto_times[user_id]) + 1 if int(user_auto_times[user_id]) < len(times_for_auto) - 1 else 0
    
    keyboard_settings_auto = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton(
            "Автозаполнение{}".format(
                "✅" if user_auto_switches[user_id] else "❌"
            ),
            callback_data="settings_auto_switch")],
        [types.InlineKeyboardButton(
            "Дней: {}".format(
                times_for_auto[user_auto_times.get(user_id)]
            ),
            callback_data="settings_auto_time")],
        [types.InlineKeyboardButton(
            "Применить",
            callback_data="settings_auto_apply")]
        if user_auto_times.get(user_id) != get_auto_time(user_id) or
            (user_auto_switches[user_id] != get_auto_switch(user_id))
        else [],
        [types.InlineKeyboardButton(
            "Назад",
            callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите действие:",
                            reply_markup=keyboard_settings_auto
                            )


def button_settings_auto_apply(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    save_user(user_id,
                get_token(user_id),
                get_group(user_id),
                get_notifications(user_id),
                get_user_labels(user_id),
                get_notifications_time(user_id),
                user_auto_switches[user_id],
                get_name_project(user_id),
                user_auto_times[user_id],
                (datetime.today() - timedelta(days=1)).date().isoformat())
    
    if get_auto_switch(user_id):
        count_days = times_for_auto[get_auto_time(user_id)]
        error_steps = []
        for i in range(1, count_days + 1):
            if not add_day_from_table(user_id,
                                        datetime.strptime(get_date_of_last(user_id), "%Y-%m-%d") + timedelta(
                                            i),
                                        get_name_project(user_id)):
                error_steps.append(i)

            bot.edit_message_text(chat_id=call.message.chat.id,
                                    message_id=call.message.message_id,
                                    text="Загрузка расписания.\n"
                                        "Прогресс:\n" +
                                        create_progress_bar(count_days, i, error_steps)
                                    )

    keyboard_settings_auto = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton(
            "Автозаполнение {}".format(
                "✅" if get_auto_switch(user_id) else "❌"
            ),
            callback_data="settings_auto_switch")],
        [types.InlineKeyboardButton(
            "Дней: {}".format(
                times_for_auto[user_auto_times.get(user_id)]
            ),
            callback_data="settings_auto_time")],
        [types.InlineKeyboardButton(
            "Назад",
            callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите действие:",
                            reply_markup=keyboard_settings_auto
                            )
    
    user_auto_switches.pop(user_id)
    user_auto_times.pop(user_id)


def button_settings_auto_switch(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    user_auto_switches[user_id] = not user_auto_switches[user_id]

    if not (user_id in user_auto_times):
        user_auto_times[user_id] = get_auto_time(user_id) if get_auto_time(user_id) else 0

    keyboard_settings_auto = types.InlineKeyboardMarkup([
        [types.InlineKeyboardButton(
            "Автозаполнение {}".format(
                "✅" if user_auto_switches[user_id] else "❌"
            ),
            callback_data="settings_auto_switch")],
        [types.InlineKeyboardButton(
            "Дней: {}".format(
                times_for_auto[user_auto_times.get(user_id)]
            ),
            callback_data="settings_auto_time")],
        [types.InlineKeyboardButton(
            "Применить",
            callback_data="settings_auto_apply")]
        if user_auto_times.get(user_id) != get_auto_time(user_id) or
            (user_auto_switches[user_id] != get_auto_switch(user_id))
        else [],
        [types.InlineKeyboardButton(
            "Назад",
            callback_data="show_settings")]
    ])

    bot.edit_message_text(chat_id=call.message.chat.id,
                            message_id=call.message.message_id,
                            text="Выберите действие:",
                            reply_markup=keyboard_settings_auto
                            )


def button_prev_week(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    day = datetime.fromisoformat(call.data.split(";")[1])
    send_days(user_id, day, call.message.message_id)


def button_next_week(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    day = datetime.fromisoformat(call.data.split(";")[1])
    send_days(user_id, day, call.message.message_id)


def button_open_day(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    day = datetime.fromisoformat(call.data.split(";")[1])
    id = call.data.split(";")[2]
    if id:
        send_day(user_id, day, id)
    else:
        send_day(user_id, day)


def button_delete_task(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    day = call.data.split(";")[1]
    user_day[user_id] = day
    page = call.data.split(";")[2]
    api = TodoistAPI(get_token(user_id))
    tasks = api.get_tasks(filter="due: {}".format(datetime.fromisoformat(day).strftime("%Y-%m-%d")))
    user_tasks[user_id] = tasks
    buttons = [
        types.InlineKeyboardButton(
            text=task.content[:25] + "..." if len(task.content) > 25 else task.content, 
            callback_data="apply_del;{}".format(task.id)) for task in tasks
        ]
    
    keyboard_delete_task = create_keyboard_with_page(buttons, int(page), "open_day;{};{}".format(day, call.message.message_id), "delete_task;{}".format(day), "delete_task;{}".format(day), 2, 1)
    bot.edit_message_text(
        chat_id=call.message.chat.id,
        message_id=call.message.message_id,
        text="Выберите задачу для удаления:",
        reply_markup=keyboard_delete_task
    )


def button_apply_del(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    id = call.data.split(";")[1]
    keyboard_apply = types.InlineKeyboardMarkup([[
        types.InlineKeyboardButton(text="Да", callback_data="deletion_yes;{};{}".format(id, call.message.message_id)),
        types.InlineKeyboardButton(text="Нет", callback_data="deletion_no;{}".format(call.message.message_id))
    ]]
    )
    bot.send_message(
        chat_id=user_id,
        text="Вы уверены, что хотите удалить эту задачу?",
        reply_markup=keyboard_apply
    )


def button_deletion_yes(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    id = call.data.split(";")[1]
    msg_id = call.data.split(";")[2]
    api = TodoistAPI(get_token(user_id))
    api.delete_task(id)
    bot.delete_message(user_id, call.message.message_id)
    
    for task in user_tasks[user_id]:
        if task.id == id:
            user_tasks[user_id].remove(task)
    
    buttons = [
        types.InlineKeyboardButton(
            text=task.content[:25] + "..." if len(task.content) > 25 else task.content, 
            callback_data="apply_del;{}".format(task.id)) for task in user_tasks[user_id]
        ]
    keyboard_delete_task = create_keyboard_with_page(buttons, 0, "open_day;{};".format(user_day[user_id]), "delete_task;{}".format(user_day[user_id]), "delete_task;{}".format(user_day[user_id]), 2, 1)

    bot.edit_message_text(
        chat_id=call.message.chat.id,
        message_id=msg_id,
        text="Выберите задачу для удаления:",
        reply_markup=keyboard_delete_task
    )
    user_tasks.pop(user_id)
    user_day.pop(user_id)


def button_deletion_no(call, user_id):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    msg_id = call.data.split(";")[1]
    api = TodoistAPI(get_token(user_id))
    bot.delete_message(user_id, call.message.message_id)
    
    buttons = [
        types.InlineKeyboardButton(
            text=task.content[:25] + "..." if len(task.content) > 25 else task.content, 
            callback_data="apply_del;{}".format(task.id)) for task in user_tasks[user_id]
        ]
    keyboard_delete_task = create_keyboard_with_page(buttons, 0, "open_day;{};".format(user_day[user_id]), "delete_task;{}".format(user_day[user_id]), "delete_task;{}".format(user_day[user_id]), 2, 1)

    bot.edit_message_text(
        chat_id=call.message.chat.id,
        message_id=msg_id,
        text="Выберите задачу для удаления:",
        reply_markup=keyboard_delete_task
    )
    user_tasks.pop(user_id)
    user_day.pop(user_id)