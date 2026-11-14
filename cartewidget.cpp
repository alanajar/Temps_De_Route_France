#include "cartewidget.h"
#include <QPainter>
#include <QDebug>

CarteWidget::CarteWidget(Graphe* graphe, QWidget* parent)
    : QWidget(parent)
    , graphe(graphe)
    , mode(Normal)
    , idDepot(-1)
{
    setMinimumSize(400, 500);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(240, 248, 255));
    setPalette(pal);
    setMouseTracking(true);
}

void CarteWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // lisse les bords en colorisant les pixel intermédiaires

    dessinerFond(p);
    dessinerLiaisons(p);
    dessinerChemin(p);
    dessinerVilles(p);
}

// Convertit des coordonnées en pixels à l'écran
QPointF CarteWidget::versPixel(double lng, double lat) const
{
    double largeur = width()  - 2 * MARGE;
    double hauteur = height() - 2 * MARGE;

    //marge+(psoition du ville/ position du carte) *largeur en pixel)
    double x = MARGE + (lng - LNG_MIN) / (LNG_MAX - LNG_MIN) * largeur;
    double y = MARGE + (LAT_MAX - lat) / (LAT_MAX - LAT_MIN) * hauteur;

    double cx = width()  / 2.0;
    double cy = height() / 2.0;

    // Appliquer zoom centré sur le widget + décalage c'est elgoiner le postion du ville du centre
    x = cx + (x - cx) * zoom + offset.x();
    y = cy + (y - cy) * zoom + offset.y();

    return QPointF(x, y);
}

void CarteWidget::dessinerFond(QPainter& p)
{
    p.setBrush(QColor(240, 248, 255));
    p.setPen(QPen(Qt::gray, 1));
    // réduit de 1px de chaque côté
    p.drawRect(rect().adjusted(1, 1, -1, -1));
}

void CarteWidget::dessinerVilles(QPainter& p)
{
    p.setFont(QFont("Arial", 7));

    for (const Ville& v : graphe->getVilles()) {
        QPointF pos = versPixel(v.getLng(), v.getLat());
        bool    sel = selection.contains(v.getId());
        bool    dep = (v.getId() == idDepot);
        // Valeurs par défaut : gris pâle, pas de bordure, petit point
        QColor fill(200, 200, 200);
        QPen   pen  = Qt::NoPen;
        double r    = 3.0;

        if (mode == Normal) {
            fill = QColor(30, 100, 200); r = 4.0;
        } else if (dep) {
            fill = QColor(50, 180, 50);  pen = QPen(Qt::darkGreen,  2); r = 10.0; // vert :
        } else if (sel && mode == Chemin) {
            fill = QColor(220, 50,  50); pen = QPen(Qt::darkRed,    1); r = 6.0;  // rouge
        } else if (sel && (mode == Surlignage || mode == Depot)) {
            fill = QColor(255, 165,  0); pen = QPen(Qt::darkYellow, 1); r = 6.0;  // orange
        } else if (sel && mode == Filtre) {
            fill = QColor(30, 100, 200); r = 5.0;                                 // bleu
        } else if (!sel &&  mode == Filtre) {
            fill = QColor(220, 220, 220);                                          // gris
        }

        p.setBrush(fill);
        p.setPen(pen);
        p.drawEllipse(pos, r, r); // r = rayon horizontal, r = rayon vertical

        // Nom affiché si popu > 300000
        p.setPen(Qt::black);
        if (sel || dep || v.getPopulation() > 300000)
            p.drawText(pos + QPointF(6, 4), v.getNom());
    }
}

void CarteWidget::reinit()
{
    mode      = Normal;
    selection.clear();
    idDepot   = -1;
    zoom      = 1.0;
    offset    = QPointF(0, 0);
    update();
}

void CarteWidget::afficherChemin(const QVector<int>& idVilles)
{
    mode      = Chemin;
    selection = idVilles;
    update();
}

void CarteWidget::afficherVillesSurlignees(const QVector<int>& idVilles)
{
    mode      = Surlignage;
    selection = idVilles;
    update();
}

void CarteWidget::afficherDepot(int id, const QVector<int>& idVilles)
{
    mode      = Depot;
    idDepot   = id;
    selection = idVilles;
    update();
}

void CarteWidget::afficherFiltre(const QVector<int>& idVilles)
{
    mode      = Filtre;
    selection = idVilles;
    update();
}

// Dessine uniquement les liaisons directes
void CarteWidget::dessinerLiaisons(QPainter& p)
{
    p.setPen(QPen(QColor(150, 150, 150, 60), 1));

    for (int idA = 0; idA < NB_VILLES; idA++) {
        for (int idB = idA + 1; idB < NB_VILLES; idB++) {
            if (graphe->tempsLiaison(idA, idB) == INF) continue;

            const Ville* vA = graphe->getVille(idA);
            const Ville* vB = graphe->getVille(idB);
            if (!vA || !vB) continue;

            p.drawLine(versPixel(vA->getLng(), vA->getLat()),
                       versPixel(vB->getLng(), vB->getLat()));
        }
    }
}

void CarteWidget::dessinerChemin(QPainter& p)
{
    if (mode != Chemin && mode != Depot) return;
    if (selection.size() < 2) return;

    if (mode == Depot && idDepot != -1) {
        p.setPen(QPen(QColor(50, 180, 50), 2));
        const Ville* depot = graphe->getVille(idDepot);
        if (!depot) return;
        QPointF posDepot = versPixel(depot->getLng(), depot->getLat());

        for (int id : selection) {
            const Ville* v = graphe->getVille(id);
            if (!v) continue;
            p.drawLine(posDepot, versPixel(v->getLng(), v->getLat()));
        }
        return;
    }

    // Mode Chemin : relier les villes dans l'ordre du chemin optimal
    p.setPen(QPen(QColor(220, 50, 50), 3));
    for (int i = 0; i < selection.size() - 1; i++) {
        const Ville* vA = graphe->getVille(selection[i]);
        const Ville* vB = graphe->getVille(selection[i + 1]);
        if (!vA || !vB) continue;
        p.drawLine(versPixel(vA->getLng(), vA->getLat()),
                   versPixel(vB->getLng(), vB->getLat()));
    }
}

void CarteWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPointF posSouris = event->position();
    const Ville* v = villeProche(posSouris);

    if (v) {
        // %L1 ajoute des séparateurs de milliers
        QString pop = QString("%L1").arg(v->getPopulation());
        QString texte = QString("<b>%1</b><br>Région : %2<br>Population : %3")
                            .arg(v->getNom())
                            .arg(v->getRegion())
                            .arg(pop);
        // globalPosition() = position en coordonnées écran to point pour convertir en qpointf
        QToolTip::showText(event->globalPosition().toPoint(), texte, this);
    } else {
        QToolTip::hideText();
    }
}

// Retourne la ville la plus proche de la souris
const Ville* CarteWidget::villeProche(const QPointF& posSouris) const
{
    const double RAYON = 10.0;
    const Ville* proche   = nullptr;
    double       distMin  = RAYON;

    for (const Ville& v : graphe->getVilles()) {
        QPointF pos = versPixel(v.getLng(), v.getLat());
        double dx = posSouris.x() - pos.x();
        double dy = posSouris.y() - pos.y();
        double d  = std::sqrt(dx*dx + dy*dy); // théorème de Pythagore

        if (d < distMin) {
            distMin = d;
            proche  = &v;
        }
    }

    return proche;
}

void CarteWidget::wheelEvent(QWheelEvent* event)
{
    QPointF souris = event->position();
    // angleDelta().y() > 0 = molette vers le haut = zoom avant (×1.15)
    //                  < 0 = molette vers le bas  = zoom arrière (÷1.15)
    double facteur = (event->angleDelta().y() > 0) ? 1.15 : 1.0 / 1.15;

    // qBound(min, val, max) pour limiter le zoum
    double newZoom = qBound(0.5, zoom * facteur, 8.0);

    double cx = width()  / 2.0;
    double cy = height() / 2.0;
    // newOffset = distance reel de la souris au centre -((distance avec decalge ancien) * combien on a changé le zoom)
    offset.setX(souris.x() - cx - (souris.x() - cx - offset.x()) * (newZoom / zoom));
    offset.setY(souris.y() - cy - (souris.y() - cy - offset.y()) * (newZoom / zoom));

    zoom = newZoom;
    update();
}
