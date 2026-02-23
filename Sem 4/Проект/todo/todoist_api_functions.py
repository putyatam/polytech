from todoist_api_python.api import TodoistAPI
from datetime import datetime, timedelta
from data_functions import *
import polyparsing
from textwrap import shorten
from telebot import *


# Получение списка всех дел из todoist в определённую дату
def get_tasks_for_date(user_id: int, target_date: datetime):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    try:
        api = TodoistAPI(get_token(user_id))
        tasks = api.get_tasks()

        tasks_for_date = []
        for task in tasks:
            if task.due:
                task_due_date = datetime.fromisoformat(task.due.date)
                if task_due_date.date() == target_date.date():
                    tasks_for_date.append(task)

        return tasks_for_date
    except Exception as error:
        print("Ошибка: {}".format(error))
        return []


# Получение проекта из todoist по его названию. Если нет - создаст
def get_project_by_name(user_id: int, project_name: str):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    try:
        api = TodoistAPI(get_token(user_id))
        projects = api.get_projects()

        for project in projects:
            if project.name == project_name:
                return project

        new_project = api.add_project(name=project_name, color="green", shared=False)
        return new_project
    except Exception as error:
        print(f"Ошибка: {error}")
        return None

# Добавление в todoist занятия из расписания. Они записываются в проект с заданным названием
def add_day_from_table(user_id: int, date: datetime, project_name: str):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    table_day = polyparsing.get_schedule(get_group(user_id), date)
    api = TodoistAPI(get_token(user_id))
    if table_day:
        try:
            current_tasks = get_tasks_for_date(user_id, date)
            for lesson in table_day:

                start_str, end_str = lesson[0].split("-")
                start_time = datetime.strptime(start_str, "%H:%M")
                end_time = datetime.strptime(end_str, "%H:%M")
                time_difference = end_time - start_time
                duration_in_minutes = time_difference.total_seconds() // 60

                due_date = datetime(
                    date.year,
                    date.month,
                    date.day,
                    int(start_str.split(":")[0]),
                    int(start_str.split(":")[1])
                )
                flag = False
                for task_from_todoist in current_tasks:
                    if task_from_todoist.content == "{}. {}".format(lesson[1], lesson[2]) and \
                            datetime.fromisoformat(task_from_todoist.due.datetime).time() == due_date.time():
                        flag = True
                        break
                if flag:
                    continue

                task = api.add_task(
                    content="{}. {}".format(lesson[1], lesson[2]),
                    description="{}\n{}".format(lesson[3], lesson[4]),
                    due_datetime=due_date.isoformat(),
                    duration=duration_in_minutes,
                    duration_unit="minute",
                    labels=["занятия"],
                    project_id=get_project_by_name(user_id, project_name if project_name else "Политех. Занятия").id
                )
            return True

        except Exception as error:
            print("Произошла ошибка во время формирования задачи: {}".format(error))
            return False
    else:
        print("Не получены данные из get_schedule")
        return False




# Отправка сообщений с расписанием по дням на count_days дней, начиная с даты date_start
def send_days(user_id: int, date_start: datetime, msg_id = None):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    try:
        api = TodoistAPI(get_token(user_id))
        tasks = api.get_tasks()
        tasks_by_day = {}
        
        for task in tasks:
            if task.due:
                due_date = datetime.fromisoformat(task.due.date)
                date_str = due_date.strftime('%Y-%m-%d')

                if date_str not in tasks_by_day:
                    tasks_by_day[date_str] = []

                tasks_by_day[date_str].append(task)
        

        week_start_flag = True
        week_end_flag = False
        n = 0
        for i in range(-7, 7):
            if week_end_flag:
                break
            current_date = date_start + timedelta(i)
            if current_date.weekday() != 0 and week_start_flag:
                continue
            if current_date.weekday() == 6:
                week_end_flag = True
            week_start_flag = False
            text_of_day = ""
            text_of_day += "{}<b>.</b>{}<b>.</b>{}  <b>{}</b>{}\n".format(
                stickers_digits[current_date.day // 10] + stickers_digits[current_date.day % 10],
                stickers_digits[current_date.month // 10] + stickers_digits[current_date.month % 10],
                stickers_digits[(current_date.year % 1000) // 10] + stickers_digits[(current_date.year % 1000) % 10],
                days_of_week[current_date.weekday()],
                "\0"*100
            )
            flag_with_time = True
            flag_without_time = False
            if current_date.strftime("%Y-%m-%d") in tasks_by_day:
                for task in tasks_by_day[current_date.strftime("%Y-%m-%d")]:
                    if task.due.datetime:
                        dt = task.due.datetime[:19]
                        if flag_with_time:
                            text_of_day += "        <b>Задачи с заданным временем:</b>\n"
                            flag_with_time = False
                        dt_end = (datetime.fromisoformat(dt) + timedelta(minutes=int(task.duration.amount if task.duration else 0))).strftime("%H:%M")
                        dt_start = datetime.fromisoformat(dt).strftime("%H:%M")
                        text_of_day += "    <u>{}{}</u>\n".format(
                            datetime.fromisoformat(dt).strftime("%H:%M"), 
                            (" - " + dt_end) if dt_start != dt_end else ""
                            )
                        text_of_day += "{} <i>{}</i>\n".format("🟢" if datetime.fromisoformat(dt) >= datetime.today()
                                                     else "🟠" if datetime.fromisoformat(dt) <= datetime.today() <= 
                                                     (datetime.fromisoformat(dt) + timedelta(minutes=int(task.duration.amount if task.duration else 0))) else "🔴", 
                                                     task.content[:35] + "..." if len(task.content) > 35 else task.content)
                        text_of_day += "    {}{}".format(shorten(task.description.split("\n")[0], 35, placeholder="..."), "\n\n" if task.description else "\n")
                    else:
                        flag_without_time = True

                if flag_without_time:
                    text_of_day += "        <b>Задачи без заданного времени:</b>\n"
                for task in tasks_by_day[current_date.strftime("%Y-%m-%d")]:
                    if not task.due.datetime:
                        text_of_day += "{} <i>{}</i>\n".format("🔵", (task.content[:35] + "..." if len(task.content) > 35 else task.content))
                        text_of_day += "    {}{}".format(shorten(task.description.split("\n")[0], 35, placeholder="..."), "\n\n" if task.description else "\n")
            else:
                text_of_day += "        На этот день у Вас нет задач."

            keyboard_common_day = types.InlineKeyboardMarkup(
                [
                    [
                        types.InlineKeyboardButton(
                            "Открыть день",
                            callback_data="open_day;{};".format(current_date.isoformat())),
                        types.InlineKeyboardButton(
                            "Добавить задачу",
                            callback_data="add_task;{}".format(current_date.isoformat())),
                    ]
                ]
            )

            keyboard_sunday_day = types.InlineKeyboardMarkup(
                [
                    [
                        types.InlineKeyboardButton(
                            "Открыть день",
                            callback_data="open_day;{}".format(current_date.isoformat())),
                        types.InlineKeyboardButton(
                            "Добавить задачу",
                            callback_data="add_task;{}".format(current_date.isoformat()))
                    ],
                    [
                        types.InlineKeyboardButton(
                            "◀️",
                            callback_data="prev_week;{}".format((current_date - timedelta(7)).isoformat())),
                        types.InlineKeyboardButton(
                            "▶️",
                            callback_data="next_week;{}".format((current_date + timedelta(7)).isoformat()))
                    ]
                ]
            )
            if msg_id:
                bot.edit_message_text(
                    chat_id=user_id,
                    text=text_of_day,
                    parse_mode="HTML",
                    reply_markup= keyboard_sunday_day if week_end_flag else keyboard_common_day,
                    message_id=msg_id-6+n
                )
            else:
                bot.send_message(user_id,
                                text=text_of_day,
                                parse_mode="HTML",
                                reply_markup= keyboard_sunday_day if week_end_flag else keyboard_common_day )
            n += 1
    except Exception as error:
        print("Ошибка {}".format(error))


def send_day(user_id: int, date: datetime, msg_id = None):
    print("Выполнение функции {} -> {} для пользователя {}".format(
        inspect.getframeinfo(inspect.currentframe()).filename.split("\\")[-1], 
        inspect.currentframe().f_code.co_name, user_id)
        )
    
    api = TodoistAPI(get_token(user_id))
    tasks = api.get_tasks(filter="due: {}".format(date.strftime("%Y-%m-%d")))
    
    text_of_day = ""
    text_of_day += "{}<b>.</b>{}<b>.</b>{}  <b>{}</b>{}\n".format(
        stickers_digits[date.day // 10] + stickers_digits[date.day % 10],
        stickers_digits[date.month // 10] + stickers_digits[date.month % 10],
        stickers_digits[(date.year % 1000) // 10] + stickers_digits[(date.year % 1000) % 10],
        days_of_week[date.weekday()],
        "\0"*100
    )

    flag_with_time = True
    flag_without_time = False

    for task in tasks:
        if task.due.datetime:
            dt = task.due.datetime[:19]
            if flag_with_time:
                text_of_day += "        <b>Задачи с заданным временем:</b>\n"
                flag_with_time = False
            dt_end = (datetime.fromisoformat(dt) + timedelta(minutes=int(task.duration.amount if task.duration else 0))).strftime("%H:%M")
            dt_start = datetime.fromisoformat(dt).strftime("%H:%M")
            text_of_day += "    <u>{}{}</u>\n".format(
                datetime.fromisoformat(dt).strftime("%H:%M"), 
                (" - " + dt_end) if dt_start != dt_end else ""
                )
            text_of_day += "{} <i>{}</i>\n".format("🟢" if datetime.fromisoformat(dt) >= datetime.today()
                                            else "🟠" if datetime.fromisoformat(dt) <= datetime.today() <= 
                                            (datetime.fromisoformat(dt) + timedelta(minutes=int(task.duration.amount if task.duration else 0))) else "🔴", 
                                            task.content)
            text_of_day += "{}\n".format(task.description)
            text_of_day += "{}{}".format(", ".join(["#"+i for i in task.labels]), "\n\n" if task.description else "\n")
        else:
            flag_without_time = True

    if flag_without_time:
        text_of_day += "        <b>Задачи без заданного времени:</b>\n"
    for task in tasks:
        if not task.due.datetime:
            text_of_day += "{} <i>{}</i>\n".format("🔵", (task.content))
            text_of_day += "{}{}".format(task.description, "\n" if task.description else "")
            text_of_day += "{}{}".format(", ".join(["#"+i for i in task.labels]), "\n\n" if task.labels else "\n")
    
    keyboard_full_day = types.InlineKeyboardMarkup(
                [
                    [
                        types.InlineKeyboardButton(
                            "Добавить задачу",
                            callback_data="add_task;{}".format(date.isoformat())
                            ),
                        types.InlineKeyboardButton(
                            "Удалить задачу",
                            callback_data="delete_task;{};0".format(date.isoformat())
                            )
                    ],
                    [
                        types.InlineKeyboardButton(
                            "Изменить задачу",
                            callback_data="change_task;{}".format(date.isoformat())
                            ),
                    ]
                ]
            )
    if msg_id:
        bot.edit_message_text(
            chat_id=user_id,
            text=text_of_day,
            parse_mode="HTML",
            reply_markup=keyboard_full_day,
            message_id=msg_id
        )
    else:
        bot.send_message(
            user_id,
            text=text_of_day,
            parse_mode="HTML",
            reply_markup=keyboard_full_day
            )