#include "mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QHeaderView>
#include <QDebug>
#include <QCompleter>

// static on les declare pas il sont juste utilisé en cpp
static bool parNom(const Ville* a, const Ville* b)        { return a->getNom() < b->getNom(); }
static bool parPopulation(const Ville* a, const Ville* b) { return a->getPopulation() > b->getPopulation(); }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Temps de route — SAE 2.02 - Ala NAJAR");
    resize(1100, 700);

    graphe = new Graphe();
    graphe->chargerVilles("villes.csv");
    graphe->chargerTemps("temps.csv");
    graphe->calculerChemins();

    carte = new CarteWidget(graphe);

    onglets = new QTabWidget();
    onglets->addTab(ongletTrajet(),  "Trajet");
    onglets->addTab(ongletTableau(), "Tableau");
    onglets->addTab(ongletDepot(),   "Dépôt");
    onglets->addTab(ongletFiltres(), "Filtres");
    connect(onglets, &QTabWidget::currentChanged, this, &MainWindow::changerOnglet);

    // Splitter : onglets à gauche, carte à droite
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(onglets);
    splitter->addWidget(carte);
    splitter->setSizes({350, 750});

    setCentralWidget(splitter);

    setStyleSheet(
        "QTabWidget::pane { border: 1px solid #ccc; }"
        "QPushButton { background: #4a90d9; color: white; padding: 8px; border-radius: 4px; }"
        "QPushButton:hover { background: #357abd; }"
        "QLabel { color: #333; }"
        );
}

MainWindow::~MainWindow()
{
    delete graphe;
}
// ONGLET 1 — TRAJET
QWidget* MainWindow::ongletTrajet()
{
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel* titre = new QLabel("<b>Calculer un trajet</b>");
    titre->setAlignment(Qt::AlignCenter);

    QLabel* labelDepart = new QLabel("Ville de départ :");
    comboDepart = new QComboBox();
    comboDepart->setEditable(true); // l'utilisateur peut taper dans le combo
    comboDepart->setInsertPolicy(QComboBox::NoInsert); // mais ce qu'il tape ne s'ajoute PAS à la liste
    remplirCombo(comboDepart);

    // QCompleter système d'autocomplétion
    // comboDepart->model() = utilise la même liste de villes que le combo
    QCompleter* compDepart = new QCompleter(comboDepart->model(), comboDepart);
    compDepart->setCompletionMode(QCompleter::PopupCompletion); // affiche les suggestions dans un menu déroulant
    compDepart->setFilterMode(Qt::MatchContains);               // cherche partout dans le mot
    compDepart->setCaseSensitivity(Qt::CaseInsensitive);        // minuscule et majuscule
    comboDepart->setCompleter(compDepart);                      // attache le completer au combo

    QLabel* labelArrivee = new QLabel("Ville d'arrivée :");
    comboArrivee = new QComboBox();
    comboArrivee->setEditable(true);
    comboArrivee->setInsertPolicy(QComboBox::NoInsert);
    remplirCombo(comboArrivee);

    QCompleter* compArrivee = new QCompleter(comboArrivee->model(), comboArrivee);
    compArrivee->setCompletionMode(QCompleter::PopupCompletion);
    compArrivee->setFilterMode(Qt::MatchContains);
    compArrivee->setCaseSensitivity(Qt::CaseInsensitive);
    comboArrivee->setCompleter(compArrivee);

    QPushButton* btn = new QPushButton("Calculer le trajet");
    connect(btn, &QPushButton::clicked, this, &MainWindow::calculerTrajet);

    labelTrajet = new QLabel("Choisissez deux villes\npuis cliquez sur Calculer.");
    labelTrajet->setAlignment(Qt::AlignCenter);
    labelTrajet->setWordWrap(true);
    labelTrajet->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    labelTrajet->setMinimumHeight(100);

    layout->addWidget(titre);
    layout->addWidget(labelDepart);
    layout->addWidget(comboDepart);
    layout->addWidget(labelArrivee);
    layout->addWidget(comboArrivee);
    layout->addWidget(btn);
    layout->addWidget(labelTrajet);
    layout->addStretch();

    return w;
}
// ONGLET 2 — TABLEAU
QWidget* MainWindow::ongletTableau()
{
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel* titre = new QLabel("<b>Tableau des temps</b>");
    titre->setAlignment(Qt::AlignCenter);

    QLineEdit* recherche = new QLineEdit();
    recherche->setPlaceholderText("Rechercher une ville");

    listeTableau = new QListWidget();
    listeTableau->setSelectionMode(QAbstractItemView::MultiSelection);
    remplirListe(listeTableau);

    connect(recherche, &QLineEdit::textChanged, this, &MainWindow::filtrerListeTableau);

    tableau = new QTableWidget();
    tableau->setMaximumHeight(200);

    QPushButton* btn = new QPushButton("Générer le tableau");
    connect(btn, &QPushButton::clicked, this, &MainWindow::calculerTableau);

    layout->addWidget(titre);
    layout->addWidget(recherche);
    layout->addWidget(listeTableau);
    layout->addWidget(btn);
    layout->addWidget(tableau);

    return w;
}
// ONGLET 3 — DÉPÔT
QWidget* MainWindow::ongletDepot()
{
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel* titre = new QLabel("<b>Meilleur emplacement dépôt</b>");
    titre->setAlignment(Qt::AlignCenter);

    QLabel* explication = new QLabel(
        "Sélectionnez plusieurs villes.\n"
        "On trouvera celle qui minimise\n"
        "la somme des temps vers toutes les autres."
        );
    explication->setWordWrap(true);

    QLineEdit* recherche = new QLineEdit();
    recherche->setPlaceholderText("Rechercher une ville");

    listeDepot = new QListWidget();
    listeDepot->setSelectionMode(QAbstractItemView::MultiSelection);
    remplirListe(listeDepot);

    connect(recherche, &QLineEdit::textChanged, this, &MainWindow::filtrerListeDepot);

    QPushButton* btn = new QPushButton("Trouver le dépôt optimal");
    connect(btn, &QPushButton::clicked, this, &MainWindow::calculerDepot);

    labelDepot = new QLabel("Sélectionnez des villes\npuis cliquez sur Trouver.");
    labelDepot->setAlignment(Qt::AlignCenter);
    labelDepot->setWordWrap(true);
    labelDepot->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    labelDepot->setMinimumHeight(80);

    layout->addWidget(titre);
    layout->addWidget(explication);
    layout->addWidget(recherche);
    layout->addWidget(listeDepot);
    layout->addWidget(btn);
    layout->addWidget(labelDepot);

    return w;
}
// ONGLET 4 — FILTRES
QWidget* MainWindow::ongletFiltres()
{
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel* titre = new QLabel("<b>Filtrer les villes</b>");
    titre->setAlignment(Qt::AlignCenter);

    QLabel* labelRegion = new QLabel("Région :");
    comboRegions = new QComboBox();
    comboRegions->addItem("Toutes les régions");

    // Remplir avec les régions uniques triées
    QStringList regions;
    for (const Ville& v : graphe->getVilles()) {
        if (!regions.contains(v.getRegion()))
            regions.append(v.getRegion());
    }
    regions.sort();
    comboRegions->addItems(regions);

    QLabel* labelTri = new QLabel("Trier par :");
    comboTri = new QComboBox();
    comboTri->addItem("Nom");
    comboTri->addItem("Population (décroissant)");

    QPushButton* btn = new QPushButton("Appliquer le filtre");
    listeFiltrees = new QListWidget();
    connect(btn, &QPushButton::clicked, this, &MainWindow::filtrerVilles);



    layout->addWidget(titre);
    layout->addWidget(labelRegion);
    layout->addWidget(comboRegions);
    layout->addWidget(labelTri);
    layout->addWidget(comboTri);
    layout->addWidget(btn);
    layout->addWidget(listeFiltrees);

    return w;
}

// SLOTS
void MainWindow::calculerTrajet()
{
    // currentData() récupère l'id de la ville : les donnée chaché
    int idDepart  = comboDepart->currentData().toInt();
    int idArrivee = comboArrivee->currentData().toInt();

    if (idDepart == idArrivee) {
        labelTrajet->setText("Choisissez deux villes différentes !");
        return;
    }

    int temps = graphe->meilleurTemps(idDepart, idArrivee);

    if (temps == INF) {
        labelTrajet->setText("Trajet impossible !\nCes deux villes ne sont pas connectées.");
        carte->reinit();
        return;
    }

    QVector<int> chemin = graphe->getChemin(idDepart, idArrivee);


    QString texteChemin;
    QStringList noms;
    for (int id : chemin) {
        const Ville* v = graphe->getVille(id);
        if (v) noms << v->getNom();
    }
    texteChemin = noms.join(" , ");

    labelTrajet->setText(
        QString("Temps : <b>%1</b><br> %2<br> %3 villes traversées")
            .arg(formatTemps(temps))
            .arg(texteChemin)
            .arg(chemin.size())
        );

    carte->afficherChemin(chemin);
}

void MainWindow::calculerTableau()
{
    QVector<int> ids;
    QStringList  noms;

    for (QListWidgetItem* item : listeTableau->selectedItems()) {
        ids  << item->data(Qt::UserRole).toInt();
        noms << item->text();
    }

    if (ids.size() < 2) {
        tableau->clear();
        return;
    }

    int n = ids.size();

    tableau->setRowCount(n);
    tableau->setColumnCount(n);
    tableau->setHorizontalHeaderLabels(noms);
    tableau->setVerticalHeaderLabels(noms);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int temps = graphe->meilleurTemps(ids[i], ids[j]);
            QString texte = (temps == INF) ? "0" : formatTemps(temps);
            QTableWidgetItem* cellule = new QTableWidgetItem(texte);
            cellule->setTextAlignment(Qt::AlignCenter);

            if (i == j)
                cellule->setBackground(QColor(220, 220, 220));

            tableau->setItem(i, j, cellule);
        }
    }

    tableau->resizeColumnsToContents();
    carte->afficherVillesSurlignees(ids);
}

void MainWindow::calculerDepot()
{
    QVector<int> ids;

    for (QListWidgetItem* item : listeDepot->selectedItems())
        ids << item->data(Qt::UserRole).toInt();

    if (ids.size() < 2) {
        labelDepot->setText(" Sélectionnez au moins 2 villes !");
        return;
    }

    // Trouver la ville qui minimise la somme des temps vers toutes les autres
    int idOptimal = -1;
    int sommeMini = INT_MAX; //INT_MAX = plus grand entier possible

    for (int idCandidat : ids) {
        int  somme    = 0;
        bool possible = true;

        for (int idAutre : ids) {
            if (idCandidat == idAutre) continue;
            int t = graphe->meilleurTemps(idCandidat, idAutre);
            if (t == INF) { possible = false; break; }
            somme += t;
        }

        if (possible && somme < sommeMini) {
            sommeMini  = somme;
            idOptimal  = idCandidat;
        }
    }

    if (idOptimal == -1) {
        labelDepot->setText("Certaines villes ne sont pas connectées !");
        return;
    }

    const Ville* depot = graphe->getVille(idOptimal);
    labelDepot->setText(
        QString("Dépôt optimal : <b>%1</b>\nSomme des temps : %2")
            .arg(depot->getNom())
            .arg(formatTemps(sommeMini))
        );

    carte->afficherDepot(idOptimal, ids);
}

void MainWindow::filtrerVilles()
{
    QString region = comboRegions->currentText();
    QString tri= comboTri->currentText();

    QVector<const Ville*> filtrees;
    for (const Ville& v : graphe->getVilles()) {
        if (region == "Toutes les régions" || v.getRegion() == region)
            filtrees.append(&v);
    }

    if (tri == "Nom")
        std::sort(filtrees.begin(), filtrees.end(), parNom);
    else
        std::sort(filtrees.begin(), filtrees.end(), parPopulation);

    listeFiltrees->clear();
    QVector<int> ids;

    for (const Ville* v : filtrees) {
        QString texte = QString("%1 (%2)").arg(v->getNom()).arg(v->getPopulation());
        listeFiltrees->addItem(texte);
        ids.append(v->getId());
    }

    carte->afficherFiltre(ids);
}

void MainWindow::changerOnglet(int index)
{
    Q_UNUSED(index);
    carte->reinit();
}
// UTILITAIRES
void MainWindow::remplirCombo(QComboBox* combo)
{
    QVector<const Ville*> triees;
    for (const Ville& v : graphe->getVilles())
        triees.append(&v);
    std::sort(triees.begin(), triees.end(), parNom);
    for (const Ville* v : triees)
        // 2 arg le nom et la valeur récupérée avec currentData()
        combo->addItem(v->getNom(), v->getId());
}

void MainWindow::remplirListe(QListWidget* liste)
{
    QVector<const Ville*> triees;
    for (const Ville& v : graphe->getVilles())
        triees.append(&v);
    std::sort(triees.begin(), triees.end(), parNom);
    for (const Ville* v : triees) {
        QListWidgetItem* item = new QListWidgetItem(v->getNom());
        // Qt::UserRole = slot de donnée cachée dans l'item récupérée avec data
        item->setData(Qt::UserRole, v->getId());
        liste->addItem(item);
    }
}

// Cache les villes qui contienne pas le texte ecrit
void MainWindow::filtrerListeTableau(const QString& texte)
{
    for (int i = 0; i < listeTableau->count(); i++) {
        QListWidgetItem* item = listeTableau->item(i);
        item->setHidden(!item->text().contains(texte, Qt::CaseInsensitive));
    }
}

void MainWindow::filtrerListeDepot(const QString& texte)
{
    for (int i = 0; i < listeDepot->count(); i++) {
        QListWidgetItem* item = listeDepot->item(i);
        item->setHidden(!item->text().contains(texte, Qt::CaseInsensitive));
    }
}

QString MainWindow::formatTemps(int minutes) const
{
    if (minutes == 0) return "0 min";
    int heures = minutes / 60;
    int mins   = minutes % 60;
    if (heures == 0)
        return QString("%1 min").arg(mins);
    // arg(mins, 2, 10, '0') → affiche sur 2 chiffres en base 10 avec zéro devant (ex: 5 → "05")
    return QString("%1h %2min").arg(heures).arg(mins, 2, 10, QChar('0'));
}
