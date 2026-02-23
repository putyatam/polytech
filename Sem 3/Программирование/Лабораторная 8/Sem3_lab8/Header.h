#pragma once
#include <QStringList>
enum Headers
{
	Surname,
	Name,
	Patronymic,
	Address,
	DOB,
	Email,
	Phone_Numbers
};

enum Values
{
	null,
	home,
	work,
	personal
};

enum Errors
{
	incorrect_data,
	incorrect_phone,
	incorrect_surname,
	incorrect_date,
	incorrect_email,
	error_save,
	table_not_saved,
	not_open,
	not_opened,
	bad_json,
	save_file,
	open_file,
	accept_exit,
	save_exit,
	error_open
};

extern QStringList Lheaders;
extern QStringList Lvalues;
extern QStringList Lerrors;