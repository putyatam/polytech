#include <QString>
#include <QVector>
#include <QList>
#include <QPair>
#include <qDebug>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>
#pragma once
using namespace std;

template <typename A, typename B>
class dict_hash_table {
    static_assert(
        (std::is_same_v<A, QString> || std::is_same_v<A, int>) &&
        (std::is_same_v<B, QString> || std::is_same_v<B, int>),
        "MyClass поддерживает только QString и int"
        );
    unsigned int size_table;
    QVector<QList<QPair<A, B>>> table;
public:
    dict_hash_table(int new_size = 16) {
        size_table = new_size;
        table = QVector<QList<QPair<A, B>>>(size_table);
    }
    int get_size_table() {
        return size_table;
    }
    QVector<QPair<A, B>> get_all_pairs() const {
        QVector<QPair<A, B>> result;
        for (int i = 0; i < size_table; i++) {
            for (int j = 0; j < table[i].size(); j++) {
                result.append(table[i][j]);
            }
        }
        return result;
    }
    QVector<QList<QPair<A, B>>>& get_table() {
        return table;
    }
    void add(const A key, const B value) {
        size_t hash = hashFunction(key);
        for (auto& pair : table[hash]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        table[hash].push_back(qMakePair<A, B>(key, value));
    }

    B find(const A key) const {
        size_t hash = hashFunction(key);
        for (const auto& pair : table[hash]) {
            if (pair.first == key) {
                return pair.second;
            }
        }
        if constexpr (std::is_same_v<B, int>) {
            return -1;
        }
        if constexpr (std::is_same_v<B, QString>) {
            return "-1";
        }
    }

    bool remove(const A key) {
        size_t hash = hashFunction(key);
        for (auto i = table[hash].begin(); i != table[hash].end(); i++) {
            if (i->first == key) {
                table[hash].erase(i);
                return true;
            }
            
        }
        return false;
    }

    bool contains(const A key) {
        size_t hash = hashFunction(key);
        for (const auto& pair : table[hash]) {
            if (pair.first == key) {
                return true;
            }
        }
        return false;
    }

    void clear() {
        for (int i = 0; i < size_table; i++) {
            table[i].clear();
        }
    }

    void print_table() {
        cout << endl;
        bool flag_empty = true;
        for (int i = 0; i < size_table; i++) {
            QList<QPair<A, B>> l = table[i];
            if (!l.empty()) {
                flag_empty = false;
                cout << i << ") ";
                bool flag_first = true;
                for (auto j = l.begin(); j != l.end(); j++) {
                    flag_first = false;
                }
            }
            
        }
        if (flag_empty) {
            cout << "EMPTY" << endl;
        }
    }

    void from_string(QString qtext) {
        string text = qtext.toStdString();
        vector<string> list_text;
        string str;
        int counter = 0;
        for (int i = 0; i < text.size(); i++) {
            char c = text[i];
            if (c != ' ') {
                str += c;
                if (i == text.size() - 1) {
                    add(QString::fromStdString(str), counter++);
                }
            }
            else {
                if (!str.empty()) {
                    add(QString::fromStdString(str), counter++);
                    str.clear();
                }

            }
        }
    }
    

    size_t hashFunction(const QString& qstr) const {
        string str = qstr.toStdString();
        unsigned long long hash = 0;
        unsigned long long power = 1;
        unsigned long long p = 257;
        for (char c : str) {
            hash += (c - 'a' + 1) * power;
            power *= p;
        }
        return hash % size_table;
    }

    size_t hashFunction(const int& qint) const {
        QString qstr = QString::number(qint);
        string str = qstr.toStdString();
        unsigned long long hash = 0;
        unsigned long long power = 1;
        unsigned long long p = 257;
        for (char c : str) {
            hash += (c - 'a' + 1) * power;
            power *= p;
        }
        return hash % size_table;
    }

    void print_counts() {
        for (int i = 0; i < size_table; i++) {
            cout << i << ") " << table[i].size() << endl;
        }
    }

    
};
