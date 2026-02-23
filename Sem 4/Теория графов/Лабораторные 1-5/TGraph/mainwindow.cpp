#define _USE_MATH_DEFINES
#include <cmath>

#include <mainwindow.h>
#include <Graph_visual.h>
#include <QGridLayout>
#include <QMessageBox>
#include <globals.h>
#include <QHeaderView>
#include <algorithm>
#include <QRegExp>



MainWindow::MainWindow(QWidget* parent): QWidget(parent) {//
	
	// Визуальный граф
	visual = new Graph_visual(nullptr, this);
	/*visual->setFixedSize(600, 600);*/

	// Вкладки
	tab_main = new QTabWidget(this);
	tab_main->setFixedSize(600, 600);
	tab_main->setMinimumSize(600, 600);
	
	tab_tables = new QTabWidget(this);
	tab_algorithms = new QTabWidget(this);

	tab_main->addTab(tab_tables, QStringLiteral("Матрицы"));
	tab_main->addTab(tab_algorithms, QStringLiteral("Алгоритмы"));

	// Метод Шимбелла
	spin_shimbell = new QSpinBox();//
	spin_shimbell->setRange(1, max_nodes);
	QButtonGroup* shimbell_group = new QButtonGroup(this);
	radiobutton_shimbell_min = new QRadioButton(QStringLiteral("Минимум"));
	radiobutton_shimbell_max = new QRadioButton(QStringLiteral("Максимум"));
	shimbell_group->addButton(radiobutton_shimbell_min);
	shimbell_group->addButton(radiobutton_shimbell_max);
	button_shimbell = new QPushButton(QStringLiteral("Посчитать"));
	
	// Поиск пути
	spin_start_path = new QSpinBox(this);
	spin_start_path->setDisabled(true);
	spin_end_path = new QSpinBox(this);
	spin_end_path->setDisabled(true);
	button_find_paths = new QPushButton(QStringLiteral("Найти"), this);
	button_find_paths->setDisabled(true);
	label_count_paths = new QLabel(QStringLiteral("Количество маршрутов:"), this);
	QLabel* label_start_path = new QLabel(QStringLiteral("Начальная вершина"), this);
	QLabel* label_end_path = new QLabel(QStringLiteral("Конечная вершина"), this);

	QLabel* label_number_of_path = new QLabel(QStringLiteral("Номер маршрута"), this);
	button_show_path = new QPushButton(QStringLiteral("Показать путь"), this);
	button_show_path->setDisabled(true);
	spin_show_path = new QSpinBox();
	spin_show_path->setDisabled(true);
	label_count_edges_of_path = new QLabel(QStringLiteral("Длина:"), this);
	path_to_show = 0;

	// Поиск точек сочленения
	button_show_artic_points = new QPushButton(QStringLiteral("Показать точки сочленения"));
	button_show_artic_points->setDisabled(true);
	

	// Распределение и обновление графа
	spin_mu = new QDoubleSpinBox(this);
	spin_lambda = new QDoubleSpinBox(this);
	spin_nodes = new QSpinBox(this);

	spin_mu->setRange(-max_nodes, max_nodes);
	spin_mu->setDecimals(1);
	spin_mu->setSingleStep(0.5);

	spin_lambda->setRange(0.1, max_nodes / 2);
	spin_lambda->setDecimals(1);
	spin_lambda->setSingleStep(0.5);
	spin_nodes->setRange(2, 32);

	button_update = new QPushButton(QStringLiteral("Обновить"), this);
	QLabel* label_mu = new QLabel(QStringLiteral("Параметр положения"), this);
	QLabel* label_lambda = new QLabel(QStringLiteral("Параметр масштаба"), this);
	QLabel* label_nodes = new QLabel(QStringLiteral("Количество вершин"), this);

	// Характеристики графа
	check_oriented = new QCheckBox(QStringLiteral("Ориентированный"), this);
	check_acyclic = new QCheckBox(QStringLiteral("Ацикличный"), this);
	check_connected = new QCheckBox(QStringLiteral("Связный"), this);

	check_oriented->setChecked(true);
	check_acyclic->setChecked(true);
	check_connected->setChecked(true);

	// Генерация весов, пропускных способностей и стоимостей
	check_negative_weights = new QCheckBox(QStringLiteral("Отрицательные веса"), this);
	button_generate_weights = new QPushButton(QStringLiteral("Сгенерировать веса"), this);
	button_generate_weights->setDisabled(true);

	button_generate_bandwidths = new QPushButton(QStringLiteral("Сгенерировать пропуск. способ."), this);
	button_generate_bandwidths->setDisabled(true);
	button_generate_costs = new QPushButton(QStringLiteral("Сгенерировать стоимости"), this);
	button_generate_costs->setDisabled(true);


	// Настройка визуальной части графа
	button_autolaying = new QPushButton(QStringLiteral("Включить автоукладку"), this);
	timer_autolaying = new QTimer(this);
	button_show_weights = new QPushButton(QStringLiteral("Отобразить веса"), this);//
	button_show_bandwidths = new QPushButton(QStringLiteral("Отобразить пропуск. способ."), this);
	button_show_costs = new QPushButton(QStringLiteral("Отобразить стоимости"), this);

	button_find_max_sets = new QPushButton(QStringLiteral("Найти макс. независ. множ."), this);
	button_show_max_set = new QPushButton(QStringLiteral("Показать множество"), this);
	spin_show_max_set = new QSpinBox(this);
	button_show_max_set->setDisabled(true);
	spin_show_max_set->setDisabled(true);

	label_max_set_size = new QLabel(QStringLiteral("Мощность:"), this);
	

	// Лэйауты
	QGridLayout* layout_undergraph = new QGridLayout();

	QFrame* horizontalLine = new QFrame();
	horizontalLine->setFrameShape(QFrame::HLine);
	horizontalLine->setFrameShadow(QFrame::Raised);

	layout_undergraph->addWidget(button_autolaying, 0, 0);
	layout_undergraph->addWidget(button_show_artic_points, 0, 2);
	layout_undergraph->addWidget(button_show_weights, 1, 0);
	layout_undergraph->addWidget(button_show_bandwidths, 1, 1);
	layout_undergraph->addWidget(button_show_costs, 1, 2);
	layout_undergraph->addWidget(horizontalLine, 2, 0, 1, 3);
	layout_undergraph->addWidget(button_find_max_sets, 3, 0);
	layout_undergraph->addWidget(button_show_max_set, 3, 1);
	layout_undergraph->addWidget(spin_show_max_set, 3, 2);
	layout_undergraph->addWidget(label_max_set_size, 4, 2);


	QVBoxLayout* layout_visual = new QVBoxLayout();
	QGroupBox* group_visual = new QGroupBox(QStringLiteral("ВИЗУАЛИЗАЦИЯ"));
	group_visual->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 16px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);

	group_visual->setLayout(layout_visual);
	layout_visual->addWidget(visual, Qt::AlignCenter);
	layout_visual->addLayout(layout_undergraph);


	QVBoxLayout* layout_tables = new QVBoxLayout();
	QGroupBox* group_tables = new QGroupBox(QStringLiteral("МАТРИЦЫ И АЛГОРИТМЫ"));
	group_tables->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 16px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);

	group_tables->setLayout(layout_tables);//
	layout_tables->addWidget(tab_main);



	QGridLayout* layout_raspred = new QGridLayout();
	QGroupBox* group_raspred = new QGroupBox(QStringLiteral("РАСПРЕДЕЛЕНИЕ"));
	group_raspred->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 14px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);
	group_raspred->setLayout(layout_raspred);

	layout_raspred->addWidget(label_mu, 1, 0);
	layout_raspred->addWidget(label_lambda, 2, 0);
	layout_raspred->addWidget(spin_mu, 1, 1);
	layout_raspred->addWidget(spin_lambda, 2, 1);

	
	QGridLayout* layout_generation = new QGridLayout();
	QGroupBox* group_generation = new QGroupBox(QStringLiteral("ГЕНЕРАЦИЯ ГРАФА"));
	group_generation->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 14px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);
	group_generation->setLayout(layout_generation);

	layout_generation->addWidget(label_nodes, 2, 0);
	layout_generation->addWidget(spin_nodes, 2, 1);
	layout_generation->addWidget(button_update, 3, 1);

	layout_generation->addWidget(check_negative_weights, 4, 0);
	layout_generation->addWidget(button_generate_weights, 4, 1);
	layout_generation->addWidget(button_generate_bandwidths, 5, 1);
	layout_generation->addWidget(button_generate_costs, 6, 1);



	
	QGridLayout* layout_properties = new QGridLayout();
	QGroupBox* group_properties = new QGroupBox(QStringLiteral("СВОЙСТВА ГРАФА"));
	group_properties->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 14px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);

	group_properties->setLayout(layout_properties);
	layout_properties->addWidget(check_oriented, 1, 0);
	layout_properties->addWidget(check_acyclic, 2, 0);
	layout_properties->addWidget(check_connected, 3, 0);//


	QGridLayout* layout_find_paths = new QGridLayout();
	QGroupBox* group_find_paths = new QGroupBox(QStringLiteral("ПОИСК ПУТЕЙ"));
	group_find_paths->setStyleSheet(
		"QGroupBox {"
		"   border: 2px solid gray;"
		"   border-radius: 10px;"
		"   margin-top: 1ex;"
		"   background-color: #f0f0f0;"
		"   padding: 10px;"
		"   font-family: Arial;"
		"   font-size: 14px;"
		"   font-weight: bold;"
		"}"
		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   subcontrol-position: top center;"
		"   padding: 0 3px;"
		"   background-color: #f0f0f0;"
		"   color: black;"
		"}"
	);
	group_find_paths->setLayout(layout_find_paths);

	layout_find_paths->addWidget(label_start_path, 0, 0);
	layout_find_paths->addWidget(label_end_path, 1, 0);
	layout_find_paths->addWidget(spin_start_path, 0, 1);
	layout_find_paths->addWidget(spin_end_path, 1, 1);
	layout_find_paths->addWidget(button_find_paths, 2, 0, 1, 2);
	layout_find_paths->addWidget(label_count_paths, 3, 0, 1, 2);

	QFrame* verticalLine = new QFrame();
	verticalLine->setFrameShape(QFrame::VLine);
	verticalLine->setFrameShadow(QFrame::Raised);

	layout_find_paths->addWidget(verticalLine, 0, 3, 4, 1);


	layout_find_paths->addWidget(label_number_of_path, 0, 4);
	layout_find_paths->addWidget(spin_show_path, 0, 5);
	layout_find_paths->addWidget(label_count_edges_of_path, 1, 4);
	layout_find_paths->addWidget(button_show_path, 2, 4, 1, 2);




	QGridLayout* layout_main = new QGridLayout(this);

	layout_main->addWidget(group_raspred, 0, 0, 1, 3);
	layout_main->addWidget(group_generation, 0, 3, 1, 3);
	layout_main->addWidget(group_properties, 0, 6, 1, 2);
	layout_main->addWidget(group_find_paths, 0, 8, 1, 4);
	layout_main->addWidget(group_visual, 1, 0, 1, 6);
	layout_main->addWidget(group_tables, 1, 6, 1, 6);


	
	this->setLayout(layout_main);

	// Соединения
	QObject::connect(button_update, &QPushButton::clicked, this, &MainWindow::update_graph);

	QObject::connect(spin_mu, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&]() {
		spin_lambda->blockSignals(true);
		mu = spin_mu->value();
		if (spin_lambda->value() > abs(16.0 / spin_mu->value())) {
			spin_lambda->setValue(abs(16.0 / spin_mu->value()));
		}
		spin_lambda->blockSignals(false);
		});
	QObject::connect(spin_lambda, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&]() {
		spin_mu->blockSignals(true);
		lambda = spin_lambda->value();
		if (abs(spin_mu->value()) > abs(16.0 / spin_lambda->value())) {
			spin_mu->setValue(16.0 / spin_lambda->value());
		}
		spin_mu->blockSignals(false);
		});
	QObject::connect(button_shimbell, &QPushButton::clicked, this, &MainWindow::calculate_shimbell);
	QObject::connect(button_find_paths, &QPushButton::clicked, [&]() {
		
		all_paths.clear();
		QVector<int> path;
		graph->find_path(spin_start_path->value() - 1, spin_end_path->value() - 1, path, all_paths);
		std::sort(all_paths.begin(), all_paths.end(), [](const QVector<int>& a, const QVector<int>& b) {
			return a.size() < b.size();
			});

		label_count_paths->setText(QStringLiteral("Количество маршрутов: ") + QString::number(all_paths.size()));
		if (all_paths.size()) {
			button_show_path->setDisabled(false);
			spin_show_path->setDisabled(false);
			spin_show_path->setRange(1, all_paths.size());
			spin_show_path->setDisabled(false);
			label_count_edges_of_path->setText(QStringLiteral("Длина: ") + QString::number(all_paths[0].size() - 1));
		}
		else {
			spin_show_path->setDisabled(true);
			button_show_path->setDisabled(true);
			label_count_edges_of_path->setText(QStringLiteral("Длина: "));
		}
		
		});
	QObject::connect(spin_show_path, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		if (value == 0) {
			label_count_edges_of_path->setText(QStringLiteral("Длина:"));
		}
		else {
			label_count_edges_of_path->setText(QStringLiteral("Длина: ") + QString::number(all_paths[value - 1].size() - 1));
		}

		if (value == path_to_show) {
			button_show_path->setText(QStringLiteral("Скрыть путь"));
		}
		else {
			button_show_path->setText(QStringLiteral("Показать путь"));
		}
		});

	QObject::connect(button_show_path, &QPushButton::clicked, [&]() {
		button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
		show_saturation = false;
		visual->update_lines_chars(QVector<int>());
		if (spin_show_path->value() == path_to_show) {
			button_show_path->setText(QStringLiteral("Показать путь"));
			visual->update_lines_chars(QVector<int>());
			path_to_show = 0;//
		}
		else {
			path_to_show = spin_show_path->value();
			button_show_path->setText(QStringLiteral("Скрыть путь"));
			visual->update_lines_chars(all_paths[path_to_show - 1]);
		}
		
		});//
	QObject::connect(button_autolaying, &QPushButton::clicked, [&]() {
		if (timer_autolaying->isActive()) {
			timer_autolaying->stop();
			button_autolaying->setText(QStringLiteral("Включить автоукладку"));
			button_update->setDisabled(false);
		}
		else {
			//visual->set_random_poses_nodes();
			timer_autolaying->start(speed_laying);
			button_autolaying->setText(QStringLiteral("Выключить автоукладку"));
			button_update->setDisabled(true);
		}
		});
	QObject::connect(timer_autolaying, &QTimer::timeout, visual, &Graph_visual::force_move);//
	
	QObject::connect(button_show_weights, &QPushButton::clicked, [&]() {
		show_weights = !show_weights;
		
		if (show_weights) {
			show_bandwidths = false;
			show_costs = false;
			button_show_bandwidths->setText(QStringLiteral("Отобразить пропуск. способ."));
			button_show_costs->setText(QStringLiteral("Отобразить стоимости"));//
			button_show_weights->setText(QStringLiteral("Скрыть веса"));//
		}
		else {
			button_show_weights->setText(QStringLiteral("Отобразить веса"));
		}
		visual->update_all_edges();
		});
	QObject::connect(button_show_bandwidths, &QPushButton::clicked, [&]() {
		show_bandwidths = !show_bandwidths;
		
		if (show_bandwidths) {
			show_weights = false;
			show_costs = false;
			button_show_weights->setText(QStringLiteral("Отобразить веса"));
			button_show_costs->setText(QStringLiteral("Отобразить стоимости"));//
			button_show_bandwidths->setText(QStringLiteral("Скрыть пропуск. способ."));//
		}
		else {
			button_show_bandwidths->setText(QStringLiteral("Отобразить пропуск. способ."));
		}
		visual->update_all_edges();
		});
	QObject::connect(button_show_costs, &QPushButton::clicked, [&]() {
		show_costs = !show_costs;
		if (show_costs) {
			show_bandwidths = false;
			show_weights = false;
			button_show_weights->setText(QStringLiteral("Отобразить веса"));
			button_show_bandwidths->setText(QStringLiteral("Отобразить пропуск. способ."));
			button_show_costs->setText(QStringLiteral("Скрыть стоимости"));//
		}
		else {
			button_show_costs->setText(QStringLiteral("Отобразить стоимости"));//
		}
		visual->update_all_edges();
		});

	QObject::connect(button_show_artic_points, &QPushButton::clicked, [&]() {
		QVector<QColor> colors;
		if (button_show_artic_points->text() == QStringLiteral("Показать точки сочленения")) {
			button_show_max_set->setText(QStringLiteral("Показать множество"));
			button_show_artic_points->setText(QStringLiteral("Скрыть точки сочленения"));

			QVector<bool> points = graph->find_articulation_points();

			bool flag_points = true;
			for (bool point : points) {
				if (point) {
					flag_points = false;
					break;
				}
			}
			if (flag_points) {
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Information);
				msgBox.setWindowTitle(QStringLiteral("Информация"));
				msgBox.setText(QStringLiteral("Точки сочленения в графе не найдены."));
				msgBox.setStandardButtons(QMessageBox::Ok);
				msgBox.exec();
				button_show_artic_points->setText(QStringLiteral("Показать точки сочленения"));
			}
			for (int i = 0; i < graph->get_p(); i++) {
				colors.push_back(points[i] ? Qt::blue : QColor(173, 216, 230));
			}
		}
		else {
			button_show_artic_points->setText(QStringLiteral("Показать точки сочленения"));

			colors = QVector<QColor>(graph->get_p(), QColor(173, 216, 230));
		}
		
		visual->update_nodes_color(colors);
		});
	QObject::connect(button_generate_weights, &QPushButton::clicked, [&]() {
		graph->generate_matrix_weights(check_negative_weights->checkState());
		update_table_weights();
		update_Dijkstra();
		visual->update_digits();
		button_show_path->setText(QStringLiteral("Показать путь"));
		path_to_show = 0;
		show_saturation = false;
		visual->update_lines_chars(QVector<int>());
		});
	QObject::connect(button_generate_bandwidths, &QPushButton::clicked, [&]() {
		graph->generate_matrix_bandwidth();
		graph->clear_matrix_flows();
		update_table_bandwidths();
		visual->update_digits();
		button_show_path->setText(QStringLiteral("Показать путь"));
		path_to_show = 0;
		button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
		show_saturation = false;
		visual->update_lines_chars(QVector<int>());
		});
	QObject::connect(button_generate_costs, &QPushButton::clicked, [&]() {
		graph->generate_matrix_costs();
		update_table_costs();
		visual->update_digits();
		button_show_path->setText(QStringLiteral("Показать путь"));
		path_to_show = 0;
		show_saturation = false;
		visual->update_lines_chars(QVector<int>());
		});
	QObject::connect(button_find_max_sets, &QPushButton::clicked, [&]() {
		max_sets_nodes = graph->findAllMaxIndependentSets();
		button_show_max_set->setDisabled(false);
		spin_show_max_set->setRange(1, max_sets_nodes.size());
		spin_show_max_set->setValue(max_sets_nodes.size());
		spin_show_max_set->setDisabled(false);
		max_set_to_show = -1;
		});
	QObject::connect(button_show_max_set, &QPushButton::clicked, [&]() {
		QVector<QColor> colors;
		
		if (button_show_max_set->text() == QStringLiteral("Показать множество")) {
			max_set_to_show = spin_show_max_set->value() - 1;
			button_show_artic_points->setText(QStringLiteral("Показать точки сочленения"));
			
			button_show_max_set->setText(QStringLiteral("Скрыть множество"));

			QVector<bool> points = max_sets_nodes[max_set_to_show];

			for (int i = 0; i < graph->get_p(); i++) {
				colors.push_back(points[i] ? Qt::blue : QColor(173, 216, 230));
			}
			
		}
		else {
			max_set_to_show = -1;
			button_show_max_set->setText(QStringLiteral("Показать множество"));
			colors = QVector<QColor>(graph->get_p(), QColor(173, 216, 230));
		}
		visual->update_nodes_color(colors);
		});
	QObject::connect(spin_show_max_set, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		label_max_set_size->setText(QStringLiteral("Мощность множества: ") + QString::number(max_sets_nodes[spin_show_max_set->value() - 1].count(true)));
		if (value != max_set_to_show + 1) {
			button_show_max_set->setText(QStringLiteral("Показать множество"));
			
		}
		else {
			button_show_max_set->setText(QStringLiteral("Скрыть множество"));
		}
		});

}

void MainWindow::update_graph() {
	
	path_to_show = 0;
	max_set_to_show = -1;
	button_show_max_set->setText(QStringLiteral("Показать множество"));
	button_show_max_set->setDisabled(true);
	spin_show_max_set->setDisabled(true);
	spin_show_path->setValue(0);
	button_show_path->setText(QStringLiteral("Показать путь"));
	label_count_paths->setText(QStringLiteral("Количество маршрутов: "));
	mu = spin_mu->value();
	lambda = spin_lambda->value();

	
	button_generate_weights->setDisabled(false);
	button_generate_bandwidths->setDisabled(false);
	button_generate_costs->setDisabled(false);
	
	button_show_artic_points->setDisabled(false);
	button_show_artic_points->setText(QStringLiteral("Показать точки сочленения"));


	if (check_oriented->isChecked()) {
		graph = new Oriented_Graph(spin_nodes->value());
	}
	else{
		graph = new Unoriented_Graph(spin_nodes->value());
	}
	
	visual->update_graph(graph);
	update_tables();//
	update_Dijkstra();
	update_ford_fulkerson();
	update_spanning_trees();
	update_cycles();
	allow_find();
	button_show_path->setDisabled(true);
	spin_show_path->setDisabled(true);

	visual->update_digits();
	button_show_path->setText(QStringLiteral("Показать путь"));
	path_to_show = 0;
	button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
	show_saturation = false;
	visual->update_lines_chars(QVector<int>());
}

void MainWindow::update_tables() {//
	tab_tables->clear();
	tab_algorithms->clear();
	update_table_contiguity();
	update_table_kirchhoff();
	update_table_weights();
	update_table_bandwidths();
	update_table_costs();
	update_table_flows();
}

void MainWindow::update_Dijkstra() {
	tab_algorithms->removeTab(1);
	button_Dijkstra = new QPushButton(QStringLiteral("Найти кратчайший путь"), this);
	button_Dijkstra->setDisabled(check_negative_weights->isChecked());
	button_Dijkstra_all = new QPushButton(QStringLiteral("Найти все кратчайшие пути"), this);
	button_Dijkstra_all-> setDisabled(check_negative_weights->isChecked());
	spin_Dijkstra_first = new QSpinBox(this);
	spin_Dijkstra_second = new QSpinBox(this);
	spin_Dijkstra_first->setRange(1, graph->get_p());
	spin_Dijkstra_second->setRange(1, graph->get_p());

	combo_Dijkstra_matrix = new QComboBox(this);
	combo_Dijkstra_matrix->addItem(QStringLiteral("Матрица весов"));
	combo_Dijkstra_matrix->addItem(QStringLiteral("Матрица пропуск. способ."));
	combo_Dijkstra_matrix->addItem(QStringLiteral("Матрица стоимостей"));
	


	QLabel* label_Dijkstra_first = new QLabel(QStringLiteral("Первая вершина"));
	QLabel* label_Dijkstra_second = new QLabel(QStringLiteral("Последняя вершина"));


	QWidget* widget_Dijkstra = new QWidget(this);
	widget_Dijkstra_paths = new QWidget(this);
	QGridLayout* layout_Dijkstra = new QGridLayout(this);
	layout_Dijkstra->addWidget(label_Dijkstra_first, 0, 0);
	layout_Dijkstra->addWidget(label_Dijkstra_second, 1, 0);
	layout_Dijkstra->addWidget(spin_Dijkstra_first, 0, 1);
	layout_Dijkstra->addWidget(spin_Dijkstra_second, 1, 1);
	layout_Dijkstra->addWidget(combo_Dijkstra_matrix, 2, 0);
	layout_Dijkstra->addWidget(button_Dijkstra, 3, 0);
	layout_Dijkstra->addWidget(button_Dijkstra_all, 3, 1);
	layout_Dijkstra->addWidget(widget_Dijkstra_paths, 4, 0, Qt::AlignTop);
	widget_Dijkstra->setLayout(layout_Dijkstra);

	layout_Dijkstra_paths = new QGridLayout(this);
	widget_Dijkstra_paths->setLayout(layout_Dijkstra_paths);
	
	tab_algorithms->insertTab(1, widget_Dijkstra, QStringLiteral("Алгоритм Дейкстры"));//

	nodes_for_Dijkstra = new QVector<int>();
	nodes_for_Dijkstra_all = QVector<QVector<int>*>();

	QObject::connect(button_Dijkstra, &QPushButton::clicked, this, &MainWindow::calculate_Dijkstra);
	QObject::connect(button_Dijkstra_all, &QPushButton::clicked, this, &MainWindow::calculate_Dijkstra_all);
}

void MainWindow::update_spanning_trees() {
	tab_algorithms->removeTab(3);
	QVBoxLayout* layout_trees = new QVBoxLayout(this);

	button_count_spanning_trees = new QPushButton(QStringLiteral("Найти кол-во остовов"), this);
	button_show_spanning_tree = new QPushButton(QStringLiteral("Показать остов"), this);
	spin_show_spanning_tree = new QSpinBox(this);
	button_show_spanning_tree->setDisabled(true);
	spin_show_spanning_tree->setDisabled(true);

	QLabel* label_finding_spanning_trees = new QLabel(QStringLiteral("Нахождение остовных деревьев"), this);
	label_finding_spanning_trees->setFont(QFont("Arial", 12, QFont::Bold));
	label_count_spanning_trees = new QLabel(QStringLiteral("Количество остовов:"), this);
	QLabel* label_spanning_tree_to_show = new QLabel(QStringLiteral("Номер остова"), this);

	QGridLayout* layout_spanning_trees = new QGridLayout(this);
	layout_spanning_trees->addWidget(label_finding_spanning_trees, 0, 0, 1, 2, Qt::AlignCenter | Qt::AlignTop);
	layout_spanning_trees->addWidget(button_count_spanning_trees, 1, 0);
	layout_spanning_trees->addWidget(label_count_spanning_trees, 1, 1);
	layout_spanning_trees->addWidget(button_show_spanning_tree, 2, 0);
	layout_spanning_trees->addWidget(spin_show_spanning_tree, 2, 1);
	layout_spanning_trees->setSizeConstraint(QLayout::SetFixedSize);
	layout_trees->addLayout(layout_spanning_trees);
	layout_trees->addSpacing(50);

	QLabel* label_Boruvka = new QLabel(QStringLiteral("Алгоритм Борувки"), this);
	label_Boruvka->setFont(QFont("Arial", 12, QFont::Bold));
	button_min_weight_tree_calculate = new QPushButton(QStringLiteral("Найти остов"), this);
	button_show_boruvka_tree = new QPushButton(QStringLiteral("Показать остов"), this);
	label_min_weight_tree = new QLabel(QStringLiteral("Вес остова:"), this);

	button_show_boruvka_tree->setDisabled(true);
	if (graph->get_flag_oriented()) {
		button_min_weight_tree_calculate->setDisabled(true);
	}

	QButtonGroup* group_Boruvka = new QButtonGroup(this);
	radiobutton_boruvka_min = new QRadioButton(QStringLiteral("Мин"));
	radiobutton_boruvka_max = new QRadioButton(QStringLiteral("Макс"));
	radiobutton_boruvka_min->setChecked(true);
	group_Boruvka->addButton(radiobutton_boruvka_min);
	group_Boruvka->addButton(radiobutton_boruvka_max);

	QGridLayout* layout_Boruvka = new QGridLayout(this);
	layout_Boruvka->addWidget(label_Boruvka, 0, 0, 1, 2, Qt::AlignCenter | Qt::AlignTop);
	layout_Boruvka->addWidget(button_min_weight_tree_calculate, 1, 0, 2, 1);
	layout_Boruvka->addWidget(radiobutton_boruvka_min, 1, 1);
	layout_Boruvka->addWidget(radiobutton_boruvka_max, 2, 1);
	layout_Boruvka->addWidget(label_min_weight_tree, 3, 1);
	layout_Boruvka->addWidget(button_show_boruvka_tree, 3, 0);
	
	layout_Boruvka->setSizeConstraint(QLayout::SetFixedSize);
	layout_trees->addLayout(layout_Boruvka);
	layout_trees->addSpacing(50);


	QLabel* label_Prufer = new QLabel(QStringLiteral("Код Прюфера"), this);
	label_Prufer->setFont(QFont("Arial", 12, QFont::Bold));
	button_Prufer_code = new QPushButton(QStringLiteral("Закодировать"), this);
	line_Prufer = new QLineEdit(this);
	line_Prufer_weights = new QLineEdit(this);
	QLabel* label_code = new QLabel(QStringLiteral("Код:"), this);
	QLabel* label_weights = new QLabel(QStringLiteral("Веса:"), this);
	button_show_prufer_tree = new QPushButton(QStringLiteral("Показать дерево"), this);

	button_Prufer_code->setDisabled(true);
	button_show_prufer_tree->setDisabled(true);

	QGridLayout* layout_Prufer = new QGridLayout(this);
	layout_Prufer->addWidget(label_Prufer, 0, 0, 1, 2, Qt::AlignCenter | Qt::AlignTop);
	layout_Prufer->addWidget(button_Prufer_code, 1, 0);
	layout_Prufer->addWidget(button_show_prufer_tree, 1, 1);
	layout_Prufer->addWidget(label_code, 2, 0);
	layout_Prufer->addWidget(line_Prufer, 2, 1);
	layout_Prufer->addWidget(label_weights, 3, 0);
	layout_Prufer->addWidget(line_Prufer_weights, 3, 1);

	layout_Prufer->setSizeConstraint(QLayout::SetFixedSize);
	layout_trees->addLayout(layout_Prufer);
	
	

	



	QWidget* widget_trees = new QWidget(this);
	layout_trees->setSizeConstraint(QLayout::SetFixedSize);
	widget_trees->setLayout(layout_trees);

	tab_algorithms->insertTab(3, widget_trees, QStringLiteral("Алгоритмы остовных деревьев"));

	QObject::connect(button_count_spanning_trees, &QPushButton::clicked, [&]() {
		int count_spanning_trees = graph->find_count_spanning_trees();
		qDebug() << count_spanning_trees;
		label_count_spanning_trees->setText(QStringLiteral("Количество остовов: ") + QString::number(count_spanning_trees));
		spin_show_spanning_tree->setRange(1, count_spanning_trees);
		button_show_spanning_tree->setDisabled(false);
		spin_show_spanning_tree->setDisabled(false);

		QVector<QVector<int>> currentTree(graph->get_p(), QVector<int>(graph->get_p(), 0));//
		QVector<bool> visited(graph->get_p(), false);

		trees = graph->find_spanning_binary_trees();
		spin_show_spanning_tree->setRange(1, trees.size());
		qDebug() << trees.size();

		});
	QObject::connect(button_show_spanning_tree, &QPushButton::clicked, [&]() {
		if (button_show_boruvka_tree->text() == QStringLiteral("Скрыть остов")) {
			button_show_boruvka_tree->click();
		}
		if (button_show_prufer_tree->text() == QStringLiteral("Скрыть дерево")) {
			button_show_prufer_tree->click();
		}
		if (button_show_spanning_tree->text() == QStringLiteral("Показать остов")) {
			button_show_spanning_tree->setText(QStringLiteral("Скрыть остов"));
			graph_saved = graph;
			graph = new Tree_Graph(graph, trees[spin_show_spanning_tree->value() - 1]);//
			visual->update_graph(graph);
			visual->positionTreeNodes();
			button_Prufer_code->setDisabled(false or graph->get_flag_oriented());
		}
		else {
			button_show_spanning_tree->setText(QStringLiteral("Показать остов"));
			graph = graph_saved;
			visual->update_graph(graph);
			button_Prufer_code->setDisabled(true);
		}

		update_table_contiguity();
		update_table_weights();
		});
	QObject::connect(button_min_weight_tree_calculate, &QPushButton::clicked, [&]() {
		bool flag = false;
		if (button_show_boruvka_tree->text() == QStringLiteral("Скрыть остов")) {
			button_show_boruvka_tree->click();
			flag = true;
		}
		min_tree = graph->Boruvka(radiobutton_boruvka_min->isChecked());
		button_show_boruvka_tree->setDisabled(false);
		int weight = 0;
		for (int i = 0; i < min_tree.size(); i++) {
			for (int j = i; j < min_tree.size(); j++) {
				if (min_tree[i][j] == 1) {
					weight += graph->get_matrix_weights()[i][j];
				}
			}
		}
		label_min_weight_tree->setText(QStringLiteral("Вес остова:") + QString::number(weight));
		if (flag) {
			button_show_boruvka_tree->click();
		}
		});
	QObject::connect(button_show_boruvka_tree, &QPushButton::clicked, [&]() {
		if (button_show_prufer_tree->text() == QStringLiteral("Скрыть дерево")) {
			button_show_prufer_tree->click();
		}
		if (button_show_spanning_tree->text() == QStringLiteral("Скрыть остов")) {
			button_show_spanning_tree->click();
		}
		if (button_show_boruvka_tree->text() == QStringLiteral("Показать остов")) {
			button_show_boruvka_tree->setText(QStringLiteral("Скрыть остов"));
			graph_saved = graph;
			graph = new Tree_Graph(graph, min_tree);
			visual->update_graph(graph);
			visual->positionTreeNodes();
			button_Prufer_code->setDisabled(false or graph->get_flag_oriented());
		}
		else {
			button_show_boruvka_tree->setText(QStringLiteral("Показать остов"));
			graph = graph_saved;
			visual->update_graph(graph);
			button_Prufer_code->setDisabled(true);
		}

		update_table_contiguity();
		update_table_weights();
		});
	QObject::connect(button_Prufer_code, &QPushButton::clicked, [&]() {
		QVector<QPair<int, int>> prufer_code = graph->Prufer();
		QString code, weights;
		for (int i = 0; i < prufer_code.size(); i++) {
			code += (i != 0 ? ", " : "") + QString::number(prufer_code[i].first + 1);
			weights += (i != 0 ? ", " : "") + QString::number(prufer_code[i].second);
		}
		line_Prufer->setText(code);
		line_Prufer_weights->setText(weights);
		button_show_prufer_tree->setDisabled(false);
		});
	QObject::connect(line_Prufer, &QLineEdit::textEdited, [&](QString str) {
		QRegularExpression regex("^\\d+(?:, \\d+)*$");
		bool flag = line_Prufer->text().count(",") == line_Prufer_weights->text().count(",");
		if (flag and regex.match(str).hasMatch() and regex.match(line_Prufer_weights->text()).hasMatch()) {
			button_show_prufer_tree->setDisabled(false);
		}
		else {
			button_show_prufer_tree->setDisabled(true);
		}
		});
	QObject::connect(line_Prufer_weights, &QLineEdit::textEdited, [&](QString str) {
		QRegExp regex("^\\s*\\d+\\s*(,\\s*\\d+\\s*)*$");
		
		bool flag = line_Prufer->text().count(",") == line_Prufer_weights->text().count(",");
		if (flag and regex.exactMatch(str) and regex.exactMatch(line_Prufer->text())) {
			button_show_prufer_tree->setDisabled(false);
		}
		else {
			button_show_prufer_tree->setDisabled(true);
		}
		});
	QObject::connect(button_show_prufer_tree, &QPushButton::clicked, [&]() {
		if (button_show_boruvka_tree->text() == QStringLiteral("Скрыть остов")) {
			button_show_boruvka_tree->click();
		}
		if (button_show_spanning_tree->text() == QStringLiteral("Скрыть остов")) {
			button_show_spanning_tree->click();
		}

		if (button_show_prufer_tree->text() == QStringLiteral("Показать дерево")) {//
			button_show_prufer_tree->setText(QStringLiteral("Скрыть дерево"));

			QVector<int> code;
			QVector<int> weights;
			for (QString s : line_Prufer->text().split(",")) {
				code.push_back(s.remove(" ").toInt() - 1);
			}
			for (QString s : line_Prufer_weights->text().split(",")) {
				weights.push_back(s.remove(" ").toInt());
			}
			QVector<QPair<int, int>> result;
			for (int i = 0; i < code.size(); i++) {
				result.push_back(qMakePair<int, int>(code[i], weights[i]));
			}
			QPair<QVector<QVector<int>>, QVector<QVector<int>>> pair = graph->decodePrufer(result);
			prufer_contiguity = pair.first;
			prufer_weights = pair.second;
			
			

			graph_saved = graph;
			graph = new Tree_Graph(graph, prufer_contiguity, prufer_weights);
			visual->update_graph(graph);
			visual->positionTreeNodes();
			button_Prufer_code->setDisabled(false);
		}
		else {
			button_show_prufer_tree->setText(QStringLiteral("Показать дерево"));
			graph = graph_saved;
			visual->update_graph(graph);
			button_Prufer_code->setDisabled(true);
		}

		update_table_contiguity();
		update_table_weights();
		});
}


void MainWindow::calculate_Dijkstra() {
	QVector<QVector<int>> matrix;
	switch (combo_Dijkstra_matrix->currentIndex())
	{
	case 0:
		matrix = graph->get_matrix_weights();
		break;
	case 1:
		matrix = graph->get_matrix_bandwidths();
		break;
	case 2:
		matrix = graph->get_matrix_costs();
		break;
	}

	QPair<QVector<int>, QVector<QVector<int>>> result = graph->Dijkstra(spin_Dijkstra_first->value()-1, matrix);//

	int result_weight = result.first[spin_Dijkstra_second->value() - 1];
	QVector<int> result_path = result.second[spin_Dijkstra_second->value() - 1];
	result_path.push_back(spin_Dijkstra_second->value() - 1);

	QVector<int> weights_of_path;
	QVector<int> nodes_of_path;

	QString str;
	if (!result.second[spin_Dijkstra_second->value() - 1].isEmpty()) {
		for (int i = 0; i < result_path.size() - 1; i++) {
			weights_of_path.push_back(matrix[result_path[i]][result_path[i + 1]]);
			nodes_of_path.push_back(result_path[i] + 1);
		}
		nodes_of_path.push_back(result_path.last() + 1);

		str = QString::number(spin_Dijkstra_first->value()) + " ==(" + QString::number(result_weight) + ")=> " + QString::number(spin_Dijkstra_second->value()) + " : ";
		for (int i = 0; i < weights_of_path.size(); i++) {
			str += QString::number(nodes_of_path[i]) + " ==(" + QString::number(weights_of_path[i]) + ")=> ";
		}
		str += QString::number(nodes_of_path.last());
	}
	else {
		str = QString::number(spin_Dijkstra_first->value()) + " ==(" + QString::number(result_weight) + ")=> " + QString::number(spin_Dijkstra_second->value() + 1) + " : " + QStringLiteral("Путь не найден");
	}

	while (QLayoutItem* item = layout_Dijkstra_paths->takeAt(0)) {//
		if (QWidget* widget = item->widget()) {
			widget->deleteLater(); // Удаляем виджет
		}
		delete item; // Удаляем элемент макета
	}

	delete layout_Dijkstra_paths;
	QTextEdit* line = new QTextEdit(str, this);
	line->setReadOnly(true);
	line->setFixedHeight(25);
	layout_Dijkstra_paths = new QGridLayout(this);
	layout_Dijkstra_paths->addWidget(line, 0, 1);
	widget_Dijkstra_paths->setLayout(layout_Dijkstra_paths);
	
	button_show_Dijkstra = new QPushButton(QStringLiteral("Показать"), this);
	layout_Dijkstra_paths->addWidget(button_show_Dijkstra, 0, 0);

	nodes_for_Dijkstra->clear();
	for (int i : nodes_of_path) {
		nodes_for_Dijkstra->push_back(i - 1);
	}

	QObject::connect(button_show_Dijkstra, &QPushButton::clicked, [&]() {
		if (button_show_Dijkstra->text() == QStringLiteral("Показать")) {
			button_show_Dijkstra->setText(QStringLiteral("Скрыть"));
			visual->update_lines_chars(*nodes_for_Dijkstra);
			show_saturation = false;
			button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
		}
		else {
			button_show_Dijkstra->setText(QStringLiteral("Показать"));
			visual->update_lines_chars(QVector<int>());
		}
		});
}


void MainWindow::calculate_Dijkstra_all() {
	
	buttons_show_Dijkstra_all.clear();
	lines_Dijkstra_all = new QVector<QTextEdit*>(graph->get_p(), new QTextEdit());


	QVector<QVector<int>> matrix;
	switch (combo_Dijkstra_matrix->currentIndex())
	{
	case 0:
		matrix = graph->get_matrix_weights();
		break;
	case 1:
		matrix = graph->get_matrix_bandwidths();
		break;
	case 2:
		matrix = graph->get_matrix_costs();
		break;
	}
	

	QPair<QVector<int>, QVector<QVector<int>>> result = graph->Dijkstra(spin_Dijkstra_first->value() - 1, matrix);//

	while (QLayoutItem* item = layout_Dijkstra_paths->takeAt(0)) {
		if (QWidget* widget = item->widget()) {
			widget->deleteLater(); // Удаляем виджет
		}
		delete item; // Удаляем элемент макета
	}

	delete layout_Dijkstra_paths;

	layout_Dijkstra_paths = new QGridLayout(this);

	for (int value = 0; value < result.first.size(); value++) {

		int result_weight = result.first[value];
		QVector<int> result_path = result.second[value];
		result_path.push_back(value);

		QVector<int> weights_of_path;
		QVector<int> nodes_of_path;

		QString str;
		if (!result.second[value].isEmpty()) {
			for (int i = 0; i < result_path.size() - 1; i++) {
				weights_of_path.push_back(matrix[result_path[i]][result_path[i + 1]]);
				nodes_of_path.push_back(result_path[i] + 1);
			}
			nodes_of_path.push_back(result_path.last() + 1);

			str = QString::number(spin_Dijkstra_first->value()) + " ==(" + QString::number(result_weight) + ")=> " + QString::number(value+1) + " : ";
			for (int i = 0; i < weights_of_path.size(); i++) {
				str += QString::number(nodes_of_path[i]) + " ==(" + QString::number(weights_of_path[i]) + ")=> ";
			}
			str += QString::number(nodes_of_path.last());
		}
		else {
			str = QString::number(spin_Dijkstra_first->value()) + " ==(" + QString::number(result_weight) + ")=> " + QString::number(value + 1) + " : " + QStringLiteral("Путь не найден");
		}

		QTextEdit* line = new QTextEdit(str, this);
		line->setReadOnly(true);
		line->setFixedHeight(25);
		layout_Dijkstra_paths->addWidget(line, value, 1);


		button_show_Dijkstra = new QPushButton(QStringLiteral("Показать"), this);
		buttons_show_Dijkstra_all.push_back(button_show_Dijkstra);
		layout_Dijkstra_paths->addWidget(button_show_Dijkstra, value, 0);
		qDebug() << buttons_show_Dijkstra_all.size();


		QVector<int>* temp = new QVector<int>();
		for (int i : nodes_of_path) {
			temp->push_back(i - 1);
		}
		nodes_for_Dijkstra_all.push_back(temp);
		qDebug() << *nodes_for_Dijkstra_all.last();

		
	}
	
	QPushButton* button;
	for (int i = 0; i < nodes_for_Dijkstra_all.size(); i++) {
		button = buttons_show_Dijkstra_all[i];
		qDebug() << i << "---" << button->text();
		QObject::connect(button, &QPushButton::clicked, [i, this, button]() {//
			
			if (button) {

				if ((button->text() == QStringLiteral("Показать"))) {
					for (QPushButton* b : buttons_show_Dijkstra_all) {
						b->setText(QStringLiteral("Показать"));
					}
					button->setText(QStringLiteral("Скрыть"));
					visual->update_lines_chars(*nodes_for_Dijkstra_all[i]);
					show_saturation = false;
					button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
				}
				else {
					button->setText(QStringLiteral("Показать"));
					visual->update_lines_chars(QVector<int>());
				}
			}
			
			
			});
	}




	widget_Dijkstra_paths->setLayout(layout_Dijkstra_paths);


}



void MainWindow::update_table_contiguity() {

	QVector<QVector<int>> matrix_contiguity = graph->get_matrix_contiguity();
	table_contiguity = new QTableWidget();

	table_contiguity->setRowCount(graph->get_p());
	table_contiguity->setColumnCount(graph->get_p());
	table_contiguity->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_contiguity->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < graph->get_p(); i++) {
		table_contiguity->setColumnWidth(i, column_width);
		table_contiguity->setRowHeight(i, row_height);
	}

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_contiguity[i][j]));
			table_contiguity->setItem(i, j, item);
		}
	}
	
	QWidget* table_contiguity_container = new QWidget();
	QVBoxLayout* contiguity_layout = new QVBoxLayout(table_contiguity_container);
	contiguity_layout->addWidget(table_contiguity);
	table_contiguity_container->setLayout(contiguity_layout);

	tab_tables->removeTab(0);
	tab_tables->insertTab(0, table_contiguity_container, QStringLiteral("Смежности"));
	
}

void MainWindow::update_table_kirchhoff() {

	QVector<QVector<int>> matrix_kirchhoff = graph->get_matrix_kirchhoff();
	table_kirchhoff = new QTableWidget();

	table_kirchhoff->setRowCount(graph->get_p());
	table_kirchhoff->setColumnCount(graph->get_p());
	table_kirchhoff->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_kirchhoff->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < graph->get_p(); i++) {
		table_kirchhoff->setColumnWidth(i, column_width);
		table_kirchhoff->setRowHeight(i, row_height);
	}

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_kirchhoff[i][j]));
			table_kirchhoff->setItem(i, j, item);
		}
	}

	QWidget* table_kirchhoff_container = new QWidget();
	QVBoxLayout* kirchhoff_layout = new QVBoxLayout(table_kirchhoff_container);
	kirchhoff_layout->addWidget(table_kirchhoff);
	table_kirchhoff_container->setLayout(kirchhoff_layout);

	tab_tables->removeTab(1);
	tab_tables->insertTab(1, table_kirchhoff_container, QStringLiteral("Кирхгофа"));

}

void MainWindow::update_table_weights() {
	spin_shimbell->setValue(1);
	QVector<QVector<int>> matrix_weights = graph->get_matrix_weights();

	table_shimbell = new QTableWidget();
	table_weights = new QTableWidget();
	

	table_shimbell->setRowCount(graph->get_p());
	table_shimbell->setColumnCount(graph->get_p());
	table_shimbell->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_shimbell->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_weights->setRowCount(graph->get_p());
	table_weights->setColumnCount(graph->get_p());
	table_weights->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_weights->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);//
	for (int i = 0; i < graph->get_p(); i++) {
		table_shimbell->setColumnWidth(i, column_width);
		table_shimbell->setRowHeight(i, row_height);

		table_weights->setColumnWidth(i, column_width);
		table_weights->setRowHeight(i, row_height);
	}


	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item_shimbell  = new QTableWidgetItem(QString::number(matrix_weights[i][j]));
			QTableWidgetItem* item_weights = new QTableWidgetItem(QString::number(matrix_weights[i][j]));
			table_shimbell->setItem(i, j, item_shimbell);
			table_weights->setItem(i, j, item_weights);
		}
	}

	QWidget* table_shimbell_container = new QWidget(tab_algorithms);
	QVBoxLayout* shimbell_layout = new QVBoxLayout(table_shimbell_container);
	QHBoxLayout* shimbell_calculate_layout = new QHBoxLayout();
	shimbell_calculate_layout->addWidget(spin_shimbell);
	shimbell_calculate_layout->addWidget(button_shimbell);
	shimbell_calculate_layout->addWidget(radiobutton_shimbell_min);//
	shimbell_calculate_layout->addWidget(radiobutton_shimbell_max);
	radiobutton_shimbell_min->setChecked(true);
	radiobutton_shimbell_min->setChecked(false);

	shimbell_layout->addLayout(shimbell_calculate_layout);

	shimbell_layout->addWidget(table_shimbell);
	table_shimbell_container->setLayout(shimbell_layout);
	tab_algorithms->removeTab(0);
	tab_algorithms->insertTab(0, table_shimbell_container, QStringLiteral("Метод Шимбелла"));

	QWidget* table_weights_container = new QWidget();
	QVBoxLayout* weights_layout = new QVBoxLayout(table_weights_container);//
	weights_layout->addWidget(table_weights);
	table_weights_container->setLayout(weights_layout);
	tab_tables->removeTab(2);
	tab_tables->insertTab(2, table_weights_container, QStringLiteral("Матрица весов"));
	
}

void MainWindow::update_table_bandwidths() {

	QVector<QVector<int>> matrix_bandwidths = graph->get_matrix_bandwidths();
	table_bandwidths = new QTableWidget();

	table_bandwidths->setRowCount(graph->get_p());
	table_bandwidths->setColumnCount(graph->get_p());
	table_bandwidths->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_bandwidths->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < graph->get_p(); i++) {
		table_bandwidths->setColumnWidth(i, column_width);
		table_bandwidths->setRowHeight(i, row_height);
	}

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_bandwidths[i][j]));
			table_bandwidths->setItem(i, j, item);
		}
	}

	QWidget* table_bandwidths_container = new QWidget();
	QVBoxLayout* bandwidths_layout = new QVBoxLayout(table_bandwidths_container);
	bandwidths_layout->addWidget(table_bandwidths);
	table_bandwidths_container->setLayout(bandwidths_layout);

	tab_tables->removeTab(3);
	tab_tables->insertTab(3, table_bandwidths_container, QStringLiteral("Пропуск. способ."));

}


void MainWindow::update_table_costs() {

	QVector<QVector<int>> matrix_costs = graph->get_matrix_costs();
	table_costs = new QTableWidget();

	table_costs->setRowCount(graph->get_p());
	table_costs->setColumnCount(graph->get_p());
	table_costs->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_costs->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < graph->get_p(); i++) {
		table_costs->setColumnWidth(i, column_width);
		table_costs->setRowHeight(i, row_height);
	}

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_costs[i][j]));
			table_costs->setItem(i, j, item);
		}
	}

	QWidget* table_costs_container = new QWidget();
	QVBoxLayout* costs_layout = new QVBoxLayout(table_costs_container);
	costs_layout->addWidget(table_costs);
	table_costs_container->setLayout(costs_layout);

	tab_tables->removeTab(4);
	tab_tables->insertTab(4, table_costs_container, QStringLiteral("Стоимости"));

}

void MainWindow::update_table_flows() {

	QVector<QVector<int>> matrix_flows = graph->get_matrix_flows();
	table_flows = new QTableWidget();

	table_flows->setRowCount(graph->get_p());
	table_flows->setColumnCount(graph->get_p());
	table_flows->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table_flows->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < graph->get_p(); i++) {
		table_flows->setColumnWidth(i, column_width);
		table_flows->setRowHeight(i, row_height);
	}

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_flows[i][j]));
			table_flows->setItem(i, j, item);
		}
	}

	QWidget* table_flows_container = new QWidget();
	QVBoxLayout* flows_layout = new QVBoxLayout(table_flows_container);
	flows_layout->addWidget(table_flows);
	table_flows_container->setLayout(flows_layout);

	tab_tables->removeTab(5);
	tab_tables->insertTab(5, table_flows_container, QStringLiteral("Потоков"));

}



void MainWindow::calculate_shimbell() {
	QVector<QVector<int>> matrix_weights = graph->get_matrix_weights();
	
	QVector<QVector<int>> matrix_weights_power = matrix_weights;

	for (int i = 1; i < spin_shimbell->value(); i++) {
		matrix_weights_power = multiplyMatrices(matrix_weights_power, matrix_weights);
	}

	table_shimbell->setRowCount(graph->get_p());
	table_shimbell->setColumnCount(graph->get_p());

	for (int i = 0; i < graph->get_p(); i++) {
		for (int j = 0; j < graph->get_p(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(matrix_weights_power[i][j]));
			table_shimbell->setItem(i, j, item);
		}
	}
}


QVector<QVector<int>> MainWindow::multiplyMatrices(const QVector<QVector<int>>& A, const QVector<QVector<int>>& B) {
	int n = A.size();
	QVector<QVector<int>> result(n, QVector<int>(n, 0));

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			QVector<int> vector_k;
			for (int k = 0; k < n; ++k) {
				int c = (A[i][k] != 0 and B[k][j] != 0) ? A[i][k] + B[k][j] : 0;
				vector_k.push_back(c);
				
				
			}
			vector_k.removeAll(0);
			if (radiobutton_shimbell_max->isChecked()) {
				result[i][j] = *std::max_element(vector_k.begin(), vector_k.end());
			}
			else {
				result[i][j] = *std::min_element(vector_k.begin(), vector_k.end());
			}
		}
	}

	return result;
}

void MainWindow::allow_find() {
	spin_start_path->setRange(1, graph->get_p());
	spin_start_path->setDisabled(false);

	spin_end_path->setRange(1, graph->get_p());
	spin_end_path->setDisabled(false);

	button_find_paths->setDisabled(false);
	//
}


bool lines_is_cross(QGraphicsLineItem* line1, QGraphicsLineItem* line2) {
	QLineF line1F = line1->line();
	QLineF line2F = line2->line();

	QPointF point_intersection;
	QLineF::IntersectType intersection = line1F.intersect(line2F, &point_intersection);

	return intersection == QLineF::BoundedIntersection or line1->contains(line2F.p1()) or line2->contains(line1F.p1());
}


void MainWindow::update_ford_fulkerson() {
	tab_algorithms->removeTab(2);
	
	spin_ford_fulkerson_first = new QSpinBox(this);
	spin_ford_fulkerson_second = new QSpinBox(this);
	spin_ford_fulkerson_first->setRange(1, graph->get_p());
	spin_ford_fulkerson_second->setRange(1, graph->get_p());

	button_calculate_ford_fulkerson = new QPushButton(QStringLiteral("Найти максимальный поток"), this);
	button_show_saturation = new QPushButton(QStringLiteral("Показать насыщенность рёбер"), this);
	QLabel* label_spin_first = new QLabel(QStringLiteral("Начальная вершина"), this);
	QLabel* label_spin_second = new QLabel(QStringLiteral("Конечная вершина"), this);

	label_max_flow = new QLabel(QStringLiteral("Максимальный поток: "), this);
	label_cost_flow = new QLabel(QStringLiteral("Стоимость потока: "), this);

	QLabel* label_ford_fulkerson = new QLabel(QStringLiteral("Алгоритм Форда-Фалкерсона"), this);
	label_ford_fulkerson->setFont(QFont("Arial", 12, QFont::Bold));

	QGridLayout* ford_fulkerson_layout = new QGridLayout(this);
	ford_fulkerson_layout->addWidget(label_ford_fulkerson, 0, 0, 1, 2, Qt::AlignTop|Qt::AlignCenter);
	ford_fulkerson_layout->addWidget(label_spin_first, 1, 0, 1, 1);
	ford_fulkerson_layout->addWidget(label_spin_second, 2, 0, 1, 1);
	ford_fulkerson_layout->addWidget(spin_ford_fulkerson_first, 1, 1, 1, 1);
	ford_fulkerson_layout->addWidget(spin_ford_fulkerson_second, 2, 1, 1, 1);
	ford_fulkerson_layout->addWidget(button_calculate_ford_fulkerson, 3, 0, 1, 1);
	ford_fulkerson_layout->addWidget(button_show_saturation, 3, 1, 1, 1);
	ford_fulkerson_layout->addWidget(label_max_flow, 4, 0, 1, 1);
	ford_fulkerson_layout->addWidget(label_cost_flow, 4, 1, 1, 1);//


	spin_flow_fraction_first = new QSpinBox(this);
	spin_flow_fraction_second = new QSpinBox(this);

	spin_flow_fraction_first->setRange(1, 100);
	spin_flow_fraction_second->setRange(1, 100);


	spin_least_first = new QSpinBox(this);
	spin_least_second = new QSpinBox(this);
	spin_least_first->setRange(1, graph->get_p());
	spin_least_second->setRange(1, graph->get_p());

	QLabel* label_least_first = new QLabel(QStringLiteral("Начальная вершина"), this);
	QLabel* label_least_second = new QLabel(QStringLiteral("Конечная вершина"), this);

	button_find_least_cost = new QPushButton(QStringLiteral("Найти поток минимальной стоимости"), this);

	label_calculated_cost = new QLabel(QStringLiteral("Вычисленная стоимость потока: "), this);

	QLabel* label_times_max_flow = new QLabel(QStringLiteral("* макс. поток"));
	QLabel* label_required_flow = new QLabel(QStringLiteral("Требуемый поток: "));
	QFrame* horizontalLine = new QFrame();
	horizontalLine->setFrameShape(QFrame::HLine);
	horizontalLine->setFrameShadow(QFrame::Plain);

	QVBoxLayout* layout_fraction = new QVBoxLayout(this);
	layout_fraction->addWidget(spin_flow_fraction_first);
	layout_fraction->addWidget(horizontalLine);
	layout_fraction->addWidget(spin_flow_fraction_second);

	QGridLayout* layout_nodes_least = new QGridLayout(this);
	layout_nodes_least->addWidget(label_least_first, 1, 0);
	layout_nodes_least->addWidget(label_least_second, 2, 0);
	layout_nodes_least->addWidget(spin_least_first, 1, 1);
	layout_nodes_least->addWidget(spin_least_second, 2, 1);


	QGridLayout* layout_values_of_flow = new QGridLayout(this);
	QLabel* label_least_cost = new QLabel(QStringLiteral("Поток минимальной стоимости"), this);
	label_least_cost->setFont(QFont("Arial", 12, QFont::Bold));
	layout_values_of_flow->addWidget(label_least_cost, 0, 0, 1, 4, Qt::AlignTop | Qt::AlignCenter);

	layout_values_of_flow->addLayout(layout_nodes_least, 1, 0, 1, 4);

	layout_values_of_flow->addWidget(label_required_flow, 3, 0, 1, 1);
	layout_values_of_flow->addLayout(layout_fraction, 3, 1, 1, 1);
	layout_values_of_flow->addWidget(label_times_max_flow, 3, 2, 1, 1);
	layout_values_of_flow->addWidget(button_find_least_cost, 3, 3, 1, 1);
	layout_values_of_flow->addWidget(label_calculated_cost, 4, 3, 1, 1);

	QVBoxLayout* layout_flows_main = new QVBoxLayout(this);
	layout_flows_main->addLayout(ford_fulkerson_layout);
	layout_flows_main->addSpacing(50);
	layout_flows_main->addLayout(layout_values_of_flow);
	ford_fulkerson_layout->setSizeConstraint(QLayout::SetFixedSize);
	layout_flows_main->setSizeConstraint(QLayout::SetFixedSize);


	
	QObject::connect(button_calculate_ford_fulkerson, &QPushButton::clicked, [&]() {
		QPair<int, QVector<QVector<int>>> result = graph->fordFulkerson(spin_ford_fulkerson_first->value() - 1, spin_ford_fulkerson_second->value() - 1);
		
		label_max_flow->setText(QStringLiteral("Максимальный поток: ") + QString::number(result.first));

		int cost = 0;
		for (int i = 0; i < graph->get_p(); i++) {
			for (int j = 0; j < graph->get_p(); j++) {
				cost += graph->get_matrix_costs()[i][j] * result.second[i][j];
			}
		}
		label_cost_flow->setText(QStringLiteral("Стоимость потока: ") + QString::number(cost));
		visual->update_digits();
		update_table_flows();
		});

	QObject::connect(button_show_saturation, &QPushButton::clicked, [&]() {
		show_saturation = !show_saturation;
		if (show_saturation) {
			button_show_saturation->setText(QStringLiteral("Скрыть насыщенность рёбер"));
			button_show_path->setText(QStringLiteral("Показать путь"));
			path_to_show = 0;
			if (button_show_Dijkstra) {
				button_show_Dijkstra->setText(QStringLiteral("Показать"));
			}
			for (QPushButton* button : buttons_show_Dijkstra_all) {
				button->setText(QStringLiteral("Показать"));
			}
		}
		else {
			button_show_saturation->setText(QStringLiteral("Показать насыщенность рёбер"));
			
		}
		visual->update_lines_chars(QVector<int>());
		visual->update_all_edges();
		});

	QObject::connect(spin_flow_fraction_second, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		spin_flow_fraction_first->setMaximum(value);
		});
	QObject::connect(button_find_least_cost, &QPushButton::clicked, [&]() {
		QPair<int, QVector<QVector<int>>> result = graph->fordFulkerson(spin_least_first->value() - 1, spin_least_second->value() - 1);
		int max_flow = result.first;
		int required_flow = max_flow * (double(spin_flow_fraction_first->value()) / spin_flow_fraction_second->value());
		qDebug() << max_flow << " and " << required_flow << "v1" << spin_least_first->value() << "v2" << spin_least_second->value();
		int cost = graph->find_least_cost(spin_least_first->value() - 1, spin_least_second->value() - 1, required_flow);
		if (required_flow == 0) {
			cost = 0;
		}
		label_calculated_cost->setText(QStringLiteral("Минимальная стоимость потока (") + QString::number(required_flow) + "): " + QString::number(cost));
		visual->update_digits();
		});


	QWidget* widget_ford_fulkerson = new QWidget(this);
	widget_ford_fulkerson->setLayout(layout_flows_main);
	if (!graph->get_flag_oriented()) {
		widget_ford_fulkerson->setDisabled(true);
	}
	tab_algorithms->insertTab(2, widget_ford_fulkerson, QStringLiteral("Алгоритмы сети"));
	
}


void MainWindow::update_cycles() {
	tab_algorithms->removeTab(4);
	QVBoxLayout* layout_cycles = new QVBoxLayout(this);
	QGridLayout* layout_euler = new QGridLayout(this);

	QLabel* label_euler = new QLabel(QStringLiteral("Эйлеров цикл"), this);
	label_euler->setFont(QFont("Arial", 12, QFont::Bold));
	button_check_euler = new QPushButton(QStringLiteral("Проверить"), this);
	label_is_euler = new QLabel(this);
	button_modify_euler = new QPushButton(QStringLiteral("Модифицировать"), this);
	button_modify_euler->setDisabled(true);
	button_show_modify_edges_euler = new QPushButton(QStringLiteral("Показать изменения"), this);
	button_show_modify_edges_euler->setDisabled(true);
	button_show_euler_cycle = new QPushButton(QStringLiteral("Показать цикл"), this);
	button_show_euler_cycle->setDisabled(true);
	text_cycle_euler = new QLineEdit(this);
	text_cycle_euler->setReadOnly(true);
	
	layout_euler->addWidget(button_check_euler, 1, 0);
	layout_euler->addWidget(label_is_euler, 1, 1);
	layout_euler->addWidget(button_modify_euler, 2, 0);
	layout_euler->addWidget(button_show_modify_edges_euler, 2, 1);
	layout_euler->addWidget(button_show_euler_cycle, 3, 0);
	layout_euler->addWidget(text_cycle_euler, 3, 1);


	QGridLayout* layout_gam = new QGridLayout(this);

	QLabel* label_gam = new QLabel(QStringLiteral("Гамильтонов цикл"), this);
	label_gam->setFont(QFont("Arial", 12, QFont::Bold));
	button_check_gam = new QPushButton(QStringLiteral("Проверить"), this);
	label_is_gam = new QLabel(this);
	button_modify_gam = new QPushButton(QStringLiteral("Модифицировать"), this);
	button_modify_gam->setDisabled(true);
	button_show_modify_edges_gam = new QPushButton(QStringLiteral("Показать изменения"), this);
	button_show_modify_edges_gam->setDisabled(true);
	button_find_salesman = new QPushButton(QStringLiteral("Найти циклы"), this);
	button_find_salesman->setDisabled(true);
	button_show_salesman = new QPushButton(QStringLiteral("Показать цикл"), this);
	button_show_salesman->setDisabled(true);
	
	spin_salesman = new QSpinBox(this);
	spin_salesman->setDisabled(true);
	label_count_salesman = new QLabel(QStringLiteral("Кол-во циклов:"), this);
	label_weight_salesman = new QLabel(QStringLiteral("Вес цикла:"), this);
	text_cycles_salesman = new QTextEdit(this);
	text_cycles_salesman->setFixedSize(500, 70);
	text_cycles_salesman->setReadOnly(true);

	layout_gam->addWidget(button_check_gam, 1, 0);
	layout_gam->addWidget(label_is_gam, 1, 1);
	layout_gam->addWidget(button_modify_gam, 2, 0);
	layout_gam->addWidget(button_show_modify_edges_gam, 2, 1);

	layout_gam->addWidget(button_find_salesman, 3, 0);
	layout_gam->addWidget(label_count_salesman, 3, 1);
	layout_gam->addWidget(button_show_salesman, 4, 0);
	layout_gam->addWidget(spin_salesman, 4, 1);
	layout_gam->addWidget(label_weight_salesman, 5, 1);
	layout_gam->addWidget(text_cycles_salesman, 5, 0, 1, 4);


	QGridLayout* layout_salesman = new QGridLayout(this);

	QLabel* label_salesman = new QLabel(QStringLiteral("Задача коммивояжера"), this);
	label_salesman->setFont(QFont("Arial", 12, QFont::Bold));
	QLabel* label_heuristic = new QLabel(QStringLiteral("Выбор эвристики"), this);
	combo_salesman_heuristic = new QComboBox(this);
	label_salesman_min_weight = new QLabel(QStringLiteral("Вес: "));
	text_salesman = new QLineEdit(this);
	text_salesman->setReadOnly(true);
	combo_salesman_heuristic->addItem(QStringLiteral("Полный перебор"));
	combo_salesman_heuristic->addItem(QStringLiteral("Ближайший сосед"));
	combo_salesman_heuristic->addItem(QStringLiteral("Пропуск путей"));
	combo_salesman_heuristic->addItem(QStringLiteral("2-opt"));
	button_find_salesman_cycle = new QPushButton(QStringLiteral("Найти цикл"), this);
	button_find_salesman_cycle->setDisabled(true);
	button_show_salesman_cycle = new QPushButton(QStringLiteral("Показать цикл"), this);
	button_show_salesman_cycle->setDisabled(true);

	layout_salesman->addWidget(label_heuristic, 0, 0);
	layout_salesman->addWidget(combo_salesman_heuristic, 0, 1);
	layout_salesman->addWidget(button_find_salesman_cycle, 1, 0);
	layout_salesman->addWidget(text_salesman, 1, 1);
	layout_salesman->addWidget(button_show_salesman_cycle, 2, 0);
	layout_salesman->addWidget(label_salesman_min_weight, 2, 1);

	QObject::connect(button_find_salesman_cycle, &QPushButton::clicked, [&]() {
		switch (combo_salesman_heuristic->currentIndex())
		{
		case 0:
			cycle_salesman = graph->find_best_cycle_bf();
			break;
		case 1:
			cycle_salesman = graph->find_best_cycle_nb();
			break;
		case 2:
			cycle_salesman = graph->find_best_cycle_pass();
			break;
		case 3:
			cycle_salesman = graph->find_best_cycle_2opt();
			break;
		}
		qDebug() << cycle_salesman;
		if (cycle_salesman.isEmpty() or cycle_salesman.size() != graph->get_p() + 1) {
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setText(QStringLiteral("Цикл не найден"));
			msgBox.exec();
		}
		else {
			button_show_salesman_cycle->setDisabled(false);
		}
		

		int sum_weight = 0;
		QVector<QVector<int>> matrix = graph->get_matrix_weights();
		for (int i = 0; i < cycle_salesman.size() - 1; i++) {
			sum_weight += matrix[cycle_salesman[i]][cycle_salesman[i + 1]];
		}

		label_salesman_min_weight->setText(QStringLiteral("Вес:") + QString::number(sum_weight));
		QString str;
		for (int j = 0; j < cycle_salesman.size(); j++) {
			str += (j != 0 ? "->" : "") + QString::number(cycle_salesman[j] + 1);
		}
		text_salesman->setText(str);
		});
	QObject::connect(button_show_salesman_cycle, &QPushButton::clicked, [&]() {
		if (button_show_salesman_cycle->text() == QStringLiteral("Показать цикл")) {
			emit visual->stop_timer();
			button_show_salesman_cycle->setText(QStringLiteral("Скрыть цикл"));
			visual->update_lines_chars(cycle_salesman, true);
		}
		else {
			emit visual->stop_timer();
			button_show_salesman_cycle->setText(QStringLiteral("Показать цикл"));
			visual->update_lines_chars(QVector<int>());

		}
		});



	QObject::connect(button_check_euler, &QPushButton::clicked, [&]() {
		if (graph->graph_is_euler()) {
			label_is_euler->setText(QStringLiteral("Является эйлеровым"));
			button_modify_euler->setDisabled(true);
			button_show_modify_edges_euler->setDisabled(true);
			button_show_euler_cycle->setDisabled(false);
		}
		else {
			label_is_euler->setText(QStringLiteral("Не является эйлеровым"));
			button_modify_euler->setDisabled(false);
			button_show_euler_cycle->setDisabled(true);
		}
		if (graph->get_p() == 2) {
			button_modify_euler->setDisabled(true);
		}
		});
	QObject::connect(button_modify_euler, &QPushButton::clicked, [&]() {
		if (button_modify_euler->text() == QStringLiteral("Модифицировать")) {
			graph_saved_euler = graph;
			graph = new Unoriented_Graph(graph_saved_euler);
			button_modify_euler->setText(QStringLiteral("Отмена"));
			added_lines_euler = graph->modify_to_euler();
			button_show_modify_edges_euler->setDisabled(false);
			button_show_euler_cycle->setDisabled(false);
		}
		else {
			graph = graph_saved_euler;
			button_modify_euler->setText(QStringLiteral("Модифицировать"));
			button_show_modify_edges_euler->setDisabled(true);
			button_show_euler_cycle->setDisabled(true);
			text_cycle_euler->setText("");
		}
		

		
		update_table_contiguity();
		visual->update_graph(graph);

		
		});
	QObject::connect(button_show_modify_edges_euler, &QPushButton::clicked, [&]() {
		if (button_show_modify_edges_euler->text() == QStringLiteral("Показать изменения")) {
			visual->update_lines_chars(added_lines_euler);
			button_show_modify_edges_euler->setText(QStringLiteral("Скрыть изменения"));
		}
		else {
			visual->update_lines_chars(QVector<QPair<int, int>>());
			button_show_modify_edges_euler->setText(QStringLiteral("Показать изменения"));
		}
		
		});


	QObject::connect(button_check_gam, &QPushButton::clicked, [&]() {
		if (graph->graph_is_gam()) {
			label_is_gam->setText(QStringLiteral("Является гамильтоновым"));
			button_find_salesman->setDisabled(false);
			button_find_salesman_cycle->setDisabled(false);
		}
		else {
			label_is_gam->setText(QStringLiteral("Не является гамильтоновым"));
			
			button_find_salesman->setDisabled(true);
			button_show_salesman->setDisabled(true);
		}
		button_modify_gam->setDisabled(false);
		if (graph->get_p() == 2) {
			button_modify_gam->setDisabled(true);
		}
		});
	QObject::connect(button_modify_gam, &QPushButton::clicked, [&]() {
		if (button_modify_gam->text() == QStringLiteral("Модифицировать")) {
			graph_saved_gam = graph;
			graph = new Unoriented_Graph(graph_saved_gam);
			button_modify_gam->setText(QStringLiteral("Отмена"));
			added_lines_gam = graph->modify_to_gam();
			button_find_salesman->setDisabled(false);
			button_find_salesman_cycle->setDisabled(false);
			button_show_salesman->setDisabled(true);
			button_show_modify_edges_gam->setDisabled(false);
		}
		else {
			graph = graph_saved_gam;
			button_find_salesman->setDisabled(true);
			button_find_salesman_cycle->setDisabled(true);
			button_show_salesman->setDisabled(true);
			button_show_modify_edges_gam->setDisabled(true);
			button_modify_gam->setText(QStringLiteral("Модифицировать"));
			spin_salesman->setDisabled(true);
			text_cycles_salesman->setText("");
		}
		
		update_table_contiguity();
		visual->update_graph(graph);
		
		});
	QObject::connect(button_show_modify_edges_gam, &QPushButton::clicked, [&]() {
		if (button_show_modify_edges_gam->text() == QStringLiteral("Показать изменения")) {
			visual->update_lines_chars(added_lines_gam);
			button_show_modify_edges_gam->setText(QStringLiteral("Скрыть изменения"));
		}
		else {
			visual->update_lines_chars(QVector<QPair<int, int>>());
			button_show_modify_edges_gam->setText(QStringLiteral("Показать изменения"));
		}

		});


	QObject::connect(button_find_salesman, &QPushButton::clicked, [&]() {
		spin_salesman->setDisabled(false);
		cycles_salesman = graph->find_gam_full();
		spin_salesman->blockSignals(true);
		spin_salesman->setRange(1, cycles_salesman.size());
		spin_salesman->blockSignals(false);
		label_count_salesman->setText(QStringLiteral("Кол-во циклов: ") + QString::number(cycles_salesman.size()));
		cycle_to_show = -1;
		button_show_salesman->setDisabled(false);
		std::sort(cycles_salesman.begin(), cycles_salesman.end(), [&](QVector<int> a, QVector<int> b) {
			QVector<QVector<int>> matrix = graph->get_matrix_weights();
			int a_sum = 0;
			for (int i = 0; i < a.size() - 1; i++) {
				a_sum += matrix[a[i]][a[i + 1]];
			}

			int b_sum = 0;
			for (int i = 0; i < b.size() - 1; i++) {
				b_sum += matrix[b[i]][b[i + 1]];
			}
			return a_sum < b_sum;
			});
		int sum_weight = 0;
		QVector<QVector<int>> matrix = graph->get_matrix_weights();
		for (int i = 0; i < cycles_salesman[0].size() - 1; i++) {
			sum_weight += matrix[cycles_salesman[0][i]][cycles_salesman[0][i + 1]];
		}
		label_weight_salesman->setText(QStringLiteral("Вес цикла:") + QString::number(sum_weight));

		QString text;
		for (int i = 0; i < cycles_salesman.size(); i++) {
			int sum_weight = 0;
			QVector<QVector<int>> matrix = graph->get_matrix_weights();
			for (int j = 0; j < cycles_salesman[i].size() - 1; j++) {
				sum_weight += matrix[cycles_salesman[i][j]][cycles_salesman[i][j + 1]];
			}

			QString str = QString::number(i + 1) + QStringLiteral(") Вес = ") + QString::number(sum_weight) + QStringLiteral("; Путь = ");
			for (int j = 0; j < cycles_salesman[i].size(); j++) {
				str += (j != 0 ? "->" : "") + QString::number(cycles_salesman[i][j] + 1);
			}
			text += str + "\n";
		}
		text_cycles_salesman->setText(text);
		});
	QObject::connect(button_show_salesman, &QPushButton::clicked, [&]() {
		if (button_show_salesman->text() == QStringLiteral("Показать цикл")) {
			emit visual->stop_timer();
			visual->update_lines_chars(cycles_salesman[spin_salesman->value() - 1], Qt::blue);
			cycle_to_show = spin_salesman->value();
			button_show_salesman->setText(QStringLiteral("Скрыть цикл"));
		}
		else {
			emit visual->stop_timer();
			visual->update_lines_chars(QVector<int>());
			cycle_to_show = -1;
			button_show_salesman->setText(QStringLiteral("Показать цикл"));
		}
		
		});
	QObject::connect(spin_salesman, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		int sum_weight = 0;
		QVector<QVector<int>> matrix = graph->get_matrix_weights();
		for (int i = 0; i < cycles_salesman[value - 1].size() - 1; i++) {
			sum_weight += matrix[cycles_salesman[value - 1][i]][cycles_salesman[value - 1][i + 1]];
		}
		label_weight_salesman->setText(QStringLiteral("Вес цикла:") + QString::number(sum_weight));
		if (value == cycle_to_show) {
			button_show_salesman->setText(QStringLiteral("Скрыть цикл"));
		}
		else {
			button_show_salesman->setText(QStringLiteral("Показать цикл"));
		}
		});
	QObject::connect(button_show_euler_cycle, &QPushButton::clicked, [&]() {
		if (button_show_euler_cycle->text() == QStringLiteral("Показать цикл")) {
			emit visual->stop_timer();
			button_show_euler_cycle->setText(QStringLiteral("Скрыть цикл"));
			QVector<int> cycle = graph->find_cycle_euler();
			visual->update_lines_chars(cycle, true);
			QString str;
			for (int i = 0; i < cycle.size(); i++) {
				str += (i != 0 ? "->" : "") + QString::number(cycle[i]);
			}
			text_cycle_euler->setText(str);
		}
		else {
			emit visual->stop_timer();
			button_show_euler_cycle->setText(QStringLiteral("Показать цикл"));
			visual->update_lines_chars(QVector<int>());

		}
		});//



	layout_cycles->addWidget(label_euler, Qt::AlignTop | Qt::AlignCenter);
	layout_cycles->addLayout(layout_euler);
	layout_cycles->addWidget(label_gam, Qt::AlignTop | Qt::AlignCenter);
	layout_cycles->addLayout(layout_gam);
	layout_cycles->addWidget(label_salesman, Qt::AlignTop | Qt::AlignCenter);
	layout_cycles->addLayout(layout_salesman);
	QWidget* widget_cycles = new QWidget(this);
	layout_cycles->setSizeConstraint(QLayout::SetFixedSize);
	widget_cycles->setLayout(layout_cycles);
	if (graph->get_flag_oriented()) {
		widget_cycles->setDisabled(true);
	}
	
	tab_algorithms->insertTab(4, widget_cycles, QStringLiteral("Циклы"));
}


