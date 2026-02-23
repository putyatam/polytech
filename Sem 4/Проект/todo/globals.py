from telebot import *

bot = telebot.TeleBot("7684560724:AAE2TkVH8rYaWQRAM8ysIDEVxzsyQhL4I7Y") # Токен бота
bd_file = "database.json" # Файл базы данных
days_of_week = ["Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"]
times_for_notifications = [0, 5, 10, 30, 60] # Значения для выбора времени напоминания
times_for_auto = [1, 3, 7, 14, 28] # Значения для выбора количества дней для заполнения из расписания
stickers_digits = ["0️⃣", "1️⃣", "2️⃣", "3️⃣", "4️⃣", "5️⃣", "6️⃣", "7️⃣", "8️⃣", "9️⃣"] # Стикеры для выделения дат
user_states = {} # Хранит (id пользователя, его состояние). Используется для сообщений от пользователя.
user_auto_times = {} # Для хранения времени авто уведом. выбранного пользователем перед сохранением в базу
user_auto_switches = {} # Для хранения состояния включения авто уведом. выбранного пользователем перед сохранением в базу
user_all_labels = {} # Для хранения всех меток пользователя
user_all_tasks = {} # Для хранения всех задач: апи туду вылетает при определённое частоте запросов, поэтому правильнее записывать
user_tasks = {}
user_day = {}


# Набор кнопок для меню профиль незарегистрированного пользователя
keyboard_profile = types.InlineKeyboardMarkup([
    [types.InlineKeyboardButton("Изменить токен", callback_data="edit_token")],
    [types.InlineKeyboardButton("Изменить группу", callback_data="edit_group")]
]
)
# Набор кнопок для меню профиль зарегистрированного пользователя
keyboard_profile_with_delete = types.InlineKeyboardMarkup([
    [types.InlineKeyboardButton("Изменить токен", callback_data="edit_token")],
    [types.InlineKeyboardButton("Изменить группу", callback_data="edit_group")],
    [types.InlineKeyboardButton("Удалить профиль", callback_data="delete_profile")]
]
)
# Набор кнопок для удаления профиля
keyboard_delete_profile = types.InlineKeyboardMarkup([
    [types.InlineKeyboardButton("Да", callback_data="delete_profile_yes"),
     types.InlineKeyboardButton("Нет", callback_data="delete_profile_no")]
]
)
# Набор кнопок для меню настройки
keyboard_settings = types.InlineKeyboardMarkup([
    [types.InlineKeyboardButton("Уведомления", callback_data="settings_notifications")],
    [types.InlineKeyboardButton("Автозаполнение", callback_data="settings_auto")],
    [types.InlineKeyboardButton("Заглушка", callback_data="dolphin")]
])