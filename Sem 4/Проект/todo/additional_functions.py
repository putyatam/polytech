from telebot import types


# Создание прогресс бара
def create_progress_bar(total_steps: int, # Всего шагов
                        current_progress: int, # Сколько выполнено
                        error_steps: list # Номера шагов с ошибками
                        ):
    
    max_width = 7
    filled = '🟩'
    error = '🟥'
    empty = '⬜'
    progress_bar = ''

    for step in range(1, total_steps + 1):
        if step in error_steps:
            progress_bar += error
        elif step <= current_progress:
            progress_bar += filled
        else:
            progress_bar += empty

        if step % max_width == 0 and step != total_steps:
            progress_bar += '\n'
    return progress_bar


# Создание страницы кнопок с кнопками пагинации
def create_keyboard_with_page(buttons: list, # Список кнопок
                              page: int, # Номер страницы
                              callback_back: str,
                              callback_prev: str,
                              callback_next: str,
                              max_rows_page: int = 7, # Макс кол-во строк
                              max_buttons_row: int = 2,
                              ): # Макс кол-во столбцов
    rows = []
    for i in range(0, len(buttons), max_buttons_row):
        row = buttons[i:i + max_buttons_row]
        rows.append(row)
    total_pages = (len(rows) + max_rows_page - 1) // max_rows_page

    if page < 0 or page >= total_pages:
        raise ValueError("Ошибка: неверный номер страницы ({})".format(page))

    start_idx = page * max_rows_page
    end_idx = start_idx + max_rows_page
    page_rows = rows[start_idx:end_idx]
    keyboard = []
    for row in page_rows:
        keyboard_row = [button for button in row]
        keyboard.append(keyboard_row)

    if total_pages > 0:
        pagination_buttons = [types.InlineKeyboardButton(
            "Назад",
            callback_data="{}".format(callback_back))]
        if page > 0:
            pagination_buttons.append(types.InlineKeyboardButton(
                "◀️",
                callback_data="{};{}".format(callback_prev, page-1)))
        if page < total_pages - 1:
            pagination_buttons.append(types.InlineKeyboardButton(
                "▶️",
                callback_data="{};{}".format(callback_next, page+1)))
        keyboard.append(pagination_buttons)
    return types.InlineKeyboardMarkup(keyboard)