#ifndef GRAPHE_H
#define GRAPHE_H

#include <QVector>
#include <QString>
#include "ville.h"

const int INF = 999999;
const int NB_VILLES = 99;

class Graphe {
public:
    Graphe();

    bool chargerVilles(const QString& cheminFichier);
    bool chargerTemps(const QString& cheminFichier);

    const QVector<Ville>& getVilles() const { return villes; }
    const Ville* getVille(int id) const;
    int tempsLiaison(int idA, int idB) const; // liaison directe seulement

    void calculerChemins(); // calcule tous les chemins optimaux
    int meilleurTemps(int idA, int idB) const;  // résultat après Floyd-Warshall
    QVector<int> getChemin(int idA, int idB) const;

private:
    QVector<Ville> villes;
    QVector<int>   indexParId;

    int dist[NB_VILLES][NB_VILLES]; // temps optimal entre chaque paire de villes
    int next[NB_VILLES][NB_VILLES]; // prochain nœud sur le chemin optimal

    void initMatrice();
};

#endif // GRAPHE_H
