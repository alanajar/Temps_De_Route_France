#include "graphe.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Graphe::Graphe()
{
    indexParId.fill(-1, NB_VILLES);
    initMatrice();
}

void Graphe::initMatrice()
{
    for (int i = 0; i < NB_VILLES; i++) {
        for (int j = 0; j < NB_VILLES; j++) {
            dist[i][j] = (i == j) ? 0 : INF;
        }
    }
}

bool Graphe::chargerVilles(const QString& cheminFichier)
{
    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erreur : impossible d'ouvrir" << cheminFichier;
        return false;
    }

    QTextStream flux(&fichier);
    flux.setEncoding(QStringConverter::Utf8); //car les dans les nom des ville il ya des '

    flux.readLine(); // ignorer l'en-tête

    while (!flux.atEnd()) {
        QString ligne = flux.readLine();
        // trimmed pour supprimer les espace
        if (ligne.trimmed().isEmpty()) continue;

        QStringList champs = ligne.split(',');
        if (champs.size() < 15) continue;

        QString   nom    = champs[0].trimmed();
        int       cityId = champs[1].trimmed().toInt();
        double    lat    = champs[3].trimmed().toDouble();
        double    lng    = champs[4].trimmed().toDouble();
        QString   region = champs[8].trimmed();
        int       pop    = champs[14].trimmed().toInt();

        int index = villes.size();
        villes.append(Ville(cityId, nom, lat, lng, region, pop));

        if (cityId >= 0 && cityId < NB_VILLES)
            indexParId[cityId] = index;
    }

    fichier.close();
    qDebug() << "Villes chargées :" << villes.size();
    return true;
}

bool Graphe::chargerTemps(const QString& cheminFichier)
{
    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erreur : impossible d'ouvrir" << cheminFichier;
        return false;
    }

    QTextStream flux(&fichier);
    int nbLiaisons = 0;

    while (!flux.atEnd()) {
        QString ligne = flux.readLine();
        if (ligne.trimmed().isEmpty()) continue;

        QStringList champs = ligne.split(',');

        int idA   = champs[0].trimmed().toInt();
        int idB   = champs[1].trimmed().toInt();
        int temps = champs[2].trimmed().toInt();

        dist[idA][idB] = temps; // liaison pour les 2 ville au meme temps
        dist[idB][idA] = temps;

        nbLiaisons++;
    }

    fichier.close();
    qDebug() << "Liaisons chargées :" << nbLiaisons;
    return true;
}

const Ville* Graphe::getVille(int id) const
{
    if (id < 0 || id >= NB_VILLES) return nullptr;
    int index = indexParId[id];
    if (index == -1) return nullptr;
    return &villes[index];
}

int Graphe::tempsLiaison(int idA, int idB) const
{
    if (idA < 0 || idA >= NB_VILLES) return INF;
    if (idB < 0 || idB >= NB_VILLES) return INF;
    return dist[idA][idB];
}

void Graphe::calculerChemins()
{
    // -1 signifie pas de chemin directe ou j si il y a
    for (int i = 0; i < NB_VILLES; i++) {
        for (int j = 0; j < NB_VILLES; j++) {
            next[i][j] = (i == j || dist[i][j] == INF) ? -1 : j;
        }
    }

    // Floyd-Warshall : pour chaque intermédiaire k, tester si i→k→j est plus court que i→j
    // Si oui : mettre à jour le temps ET le prochain nœud (next[i][j] = next[i][k])
    for (int k = 0; k < NB_VILLES; k++) {
        for (int i = 0; i < NB_VILLES; i++) {
            for (int j = 0; j < NB_VILLES; j++) {
                if (dist[i][k] == INF || dist[k][j] == INF) continue;

                int nouveauTemps = dist[i][k] + dist[k][j];
                if (nouveauTemps < dist[i][j]) {
                    dist[i][j] = nouveauTemps;
                    //on passe d'abord par k
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

int Graphe::meilleurTemps(int idA, int idB) const
{
    if (idA < 0 || idA >= NB_VILLES) return INF;
    if (idB < 0 || idB >= NB_VILLES) return INF;
    return dist[idA][idB];
}

QVector<int> Graphe::getChemin(int idA, int idB) const
{
    QVector<int> chemin;

    if (dist[idA][idB] == INF) return chemin; // trajet impossible

    int courant = idA;
    chemin.append(courant);

    while (courant != idB) {
        courant = next[courant][idB];
        chemin.append(courant);
    }

    return chemin;
}
