import datetime
import pprint

import requests
from bs4 import BeautifulSoup


def get_link_table(group_number):
    try:
        response = requests.get("https://ruz.spbstu.ru/search/groups?q={}".format(group_number))
        soup = BeautifulSoup(response.text, 'html.parser')
        target_h3 = None
        for h3 in soup.find_all("h3"):
            if h3.text.strip() == "Группы не найдены":
                target_h3 = h3
                break

        if target_h3:
            print("Ошибка: группа '{}' не найдена".format(group_number))
            return False
        else:
            links = soup.find_all('a')
            filtered_links = [link.get('href') for link in links if group_number == link.text]
            if len(filtered_links) != 1:
                return False
            return "https://ruz.spbstu.ru{}".format(filtered_links[0])

    except requests.exceptions.RequestException as error:
        print("Ошибка {} при отправке запроса сайту ruz.spbstu.ru.".format(error))
        return


def get_schedule(group_number, date):
    url = get_link_table(group_number)
    if url:
        try:
            response = requests.get("{}?date={}".format(url, date.strftime("%Y-%m-%d")))
            response.raise_for_status()
            soup = BeautifulSoup(response.text, 'html.parser')
            schedule_days = soup.find_all(class_="schedule__day")
            list_lessons = []
            for day in schedule_days:
                date_table = day.find(class_="schedule__date").text.split(" ")[0]
                if int(date_table) == date.day:
                    lessons = day.find_all(class_="lesson")
                    for lesson in lessons:
                        time = lesson.find(class_="lesson__time")
                        subject = lesson.find(class_="lesson__subject")
                        lesson_type = lesson.find(class_="lesson__type")
                        teacher = lesson.find(class_="lesson__teachers")
                        place = lesson.find(class_="lesson__places")

                        time = time.text if time else "Не указано"
                        flag = True
                        for i in list_lessons:
                            if i[0] == time and time != "Не указано":
                                flag = False
                                break
                        if flag:
                            subject = subject.text.replace(time + " ", "") if subject else "Не указано"
                            lesson_type = lesson_type.text if lesson_type else "Не указано"
                            teacher = teacher.text[1:] if teacher else "Не указано"
                            place = place.text if place else "Не указано"
                            list_lessons.append([time, subject, lesson_type, teacher, place])
            return list_lessons

        except requests.exceptions.RequestException as error:
            print("Ошибка при запросе к сайту: {}".format(error))
        except Exception as error:
            print("Произошла ошибка в работе get_schedule: {}".format(error))
    return False

