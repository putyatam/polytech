#pragma once
#include "dict_rb_tree.h"
#include "dict_hash.h"
#include "Tree_visual.h"

#include <QWidget>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QTableWidget>
#include <QFrame>
#include <QHeaderView>
#include <QTabWidget>
#include <QTextEdit>
#include <unordered_map>
#include <queue>


struct HuffmanNode {
    int value; // Код символа
    int freq;  // Частота
    HuffmanNode* left, * right;

    HuffmanNode(int val, int f)
        : value(val), freq(f), left(nullptr), right(nullptr) {}
};

// Компаратор для очереди с приоритетом
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};


class MainWindow : public QWidget {
    // Хэш
	QPushButton* button_hash_add;
    QPushButton* button_hash_remove;
    QPushButton* button_hash_find;
    QPushButton* button_hash_clear;
    QPushButton* button_hash_file;
    QPushButton* button_hash_generate_file;
    QPushButton* button_hash_update;

    QLineEdit* line_hash_add_key;
    QLineEdit* line_hash_add_value;
    QLineEdit* line_hash_remove_key;
    QLineEdit* line_hash_remove_result;
    QLineEdit* line_hash_find_key;
    QLineEdit* line_hash_find_result;
    QLineEdit* line_size_hash;

    QLabel* label_hash;
    QLabel* label_hash_add_key;
    QLabel* label_hash_add_value;
    QLabel* label_hash_remove_key;
    QLabel* label_hash_remove_result;
    QLabel* label_hash_find_key;
    QLabel* label_hash_find_result;
    QLabel* label_hash_size;

    QCheckBox* check_hash_clear;

    QTableWidget* table_hash;

    // Дерево
    QPushButton* button_tree_add;
    QPushButton* button_tree_remove;
    QPushButton* button_tree_find;
    QPushButton* button_tree_clear;
    QPushButton* button_tree_file;
    QPushButton* button_tree_generate_file;

    QLineEdit* line_tree_add_key;
    QLineEdit* line_tree_remove_key;
    QLineEdit* line_tree_remove_result;
    QLineEdit* line_tree_find_key;
    QLineEdit* line_tree_find_result;

    QLabel* label_tree;
    QLabel* label_tree_add_key;
    QLabel* label_tree_remove_key;
    QLabel* label_tree_remove_result;
    QLabel* label_tree_find_key;
    QLabel* label_tree_find_result;

    QCheckBox* check_tree_clear;

    QGraphicsScene* scene;
    Tree_view* view;

    //

    QPushButton* button_algo_generate_file;
    QPushButton* button_algo_file;
    QPushButton* button_compress;
    QPushButton* button_decompress;
    QPushButton* button_compress_double;
    QPushButton* button_decompress_double;
    QPushButton* button_is_correct;
    

    QTextEdit* text_algo_file;
    QTextEdit* text_algo_decompressed;
    QLineEdit* line_algo_compressed;
    QLabel* label_cost_compression;
    QLabel* label_koef_compression;
    QLabel* label_is_correct;
    QLabel* label_LZW;
    QLabel* label_text_algo_file;
    QLabel* label_text_algo_decompressed;
    QLabel* label_line_algo_LZW;



    //


    
    QTabWidget* tab_main;
    dict_rb_tree* dict_tree;
    dict_hash_table<QString, int>* dict_hash;
    QVector<int> v_LZW;
    HuffmanNode* root;
    QVector<bool> v_Huffman;

public:
    MainWindow(QWidget* parent = nullptr);
    //bool eventFilter(QObject* obj, QEvent* event);

    //void visualizeTree(Node* root, Node* nil, int x = 0, int y = 0, int level = 0, int x_offset = 40);

    void update_table();

};


void generate_text_to_file(const QString file_name, size_t count, bool more_spaces = false);
QString read_from_file(QString file_name);

QVector<int> compressLZW(const QString input);
QString decompressLZW(const QVector<int>& compressed);
QPair<QVector<bool>, HuffmanNode*> compressHuffman(const QVector<int>& lzwOutput);
HuffmanNode* buildHuffmanTree(const dict_hash_table<int, int>& freqMap);
void generateCodes(HuffmanNode* root, string code, dict_hash_table<int, QString>& huffmanCode);
QVector<int> decompressHuffman(const QVector<bool>& encodedData, HuffmanNode* root);
double rand_Laplace();

extern QString symbols;
extern bool double_compressed;
extern bool compressed;

