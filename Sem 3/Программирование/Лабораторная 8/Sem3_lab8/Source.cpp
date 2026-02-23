#include "Header.h"

QStringList Lheaders = {
	QStringLiteral("Фамилия"),
	QStringLiteral("Имя"),
	QStringLiteral("Отчество"),
	QStringLiteral("Адрес"),
	QStringLiteral("Дата рождения"),
	QStringLiteral("E-mail"),
	QStringLiteral("Номера телефонов")
};

QStringList Lvalues = {
	QStringLiteral("Не задано"),
	QStringLiteral("Домашний"), 
	QStringLiteral("Рабочий"), 
	QStringLiteral("Личный")
};

QStringList Lerrors = {
	QStringLiteral("Некорректные данные"),
	QStringLiteral("Не верный номер телефона. Номер телефона должен быть написан по одному из следующих образцов: +7 812 1234567, 8(800)123-1212, + 38 (032) 123 11 11. Строка автоматически заменена на \"Не задано\""),
	QStringLiteral("Фамилия, имя и отчество должны начинаться с заглавной буквы, могут содержать только буквы, цифры, дефис и пробел, а также не должны оканчиваться на дефис. Строка автоматически заменена на \"Не задано\""),
	QStringLiteral("Не верная дата. Строка автоматически заменена на \"Не задано\""),
	QStringLiteral("Не верный E-mail. Строка автоматически заменена на \"Не задано\""),
	QStringLiteral("Ошибка сохранения"),
	QStringLiteral("Таблица не была сохранена"),
	QStringLiteral("Не удалось открыть файл"),
	QStringLiteral("Неверный формат JSON"),
	QStringLiteral("Сохранить файл"),
	QStringLiteral("Открыть файл"),
	QStringLiteral("Подтвердите выход"),
	QStringLiteral("Сохранить файл перед выходом?"),
	QStringLiteral("Ошибка открытия файла")
};