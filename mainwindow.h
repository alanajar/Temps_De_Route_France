#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QLabel>
#include "graphe.h"
#include "cartewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Graphe*      graphe;
    CarteWidget* carte;
    QTabWidget*  onglets;

    // Onglet 1 — Trajet
    QComboBox* comboDepart;
    QComboBox* comboArrivee;
    QLabel*    labelTrajet;

    // Onglet 2 — Tableau
    QListWidget*  listeTableau;
    QTableWidget* tableau;

    // Onglet 3 — Dépôt
    QListWidget* listeDepot;
    QLabel*      labelDepot;

    // Onglet 4 — Filtres
    QComboBox*   comboRegions;
    QComboBox*   comboTri;
    QListWidget* listeFiltrees;

    QWidget* ongletTrajet();
    QWidget* ongletTableau();
    QWidget* ongletDepot();
    QWidget* ongletFiltres();

    void remplirCombo(QComboBox* combo);
    void remplirListe(QListWidget* liste);
    QString formatTemps(int minutes) const;

private slots:
    void calculerTrajet();
    void calculerTableau();
    void calculerDepot();
    void filtrerVilles();
    void changerOnglet(int index);
    void filtrerListeTableau(const QString& texte);
    void filtrerListeDepot(const QString& texte);
};

#endif // MAINWINDOW_H
