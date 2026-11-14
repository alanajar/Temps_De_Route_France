#ifndef CARTEWIDGET_H
#define CARTEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QMouseEvent>
#include <QToolTip>
#include <QWheelEvent>
#include "graphe.h"

class CarteWidget : public QWidget
{
    Q_OBJECT

public:
    // parent  : widget Qt parent — gère la mémoire automatiquement
    CarteWidget(Graphe* graphe, QWidget* parent = nullptr);

    void afficherChemin(const QVector<int>& idVilles);
    void afficherVillesSurlignees(const QVector<int>& idVilles);
    void afficherDepot(int idDepot, const QVector<int>& idVilles);
    void afficherFiltre(const QVector<int>& idVilles);
    void reinit();

protected:
    void paintEvent(QPaintEvent* event) override;    // appelé à chaque fois que la carte doit être redessinée
    void mouseMoveEvent(QMouseEvent* event) override; // appelé à chaque mouvement de la souris
    void wheelEvent(QWheelEvent* event) override;    // appelé à chaque molette

private:
    Graphe* graphe;

    enum Mode { Normal, Chemin, Surlignage, Depot, Filtre };
    Mode mode;

    QVector<int> selection; // ids des villes sélectionnées
    int          idDepot;

    // la cordonnée géographiques de la France
    static constexpr double LNG_MIN =  -5.5;
    static constexpr double LNG_MAX =   9.8;
    static constexpr double LAT_MIN =  41.0;
    static constexpr double LAT_MAX =  51.5;
    static constexpr int MARGE = 30;

    QPointF versPixel(double lng, double lat) const;
    void dessinerFond(QPainter& p);
    void dessinerVilles(QPainter& p);
    void dessinerLiaisons(QPainter& p);
    void dessinerChemin(QPainter& p);
    const Ville* villeProche(const QPointF& posSouris) const;

    double  zoom;   // 1.0 = normal, >1 = zoom avant
    QPointF offset; // décalage en pixels pour centrer le zoom sur la souris
};

#endif // CARTEWIDGET_H
