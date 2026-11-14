#ifndef VILLE_H
#define VILLE_H

#include <QString>

class Ville {
public:
    Ville(int id, const QString& nom, double lat, double lng,
          const QString& region, int population);

    int     getId()         const { return id; }
    QString getNom()        const { return nom; }
    double  getLat()        const { return lat; }
    double  getLng()        const { return lng; }
    QString getRegion()     const { return region; }
    int     getPopulation() const { return population; }

private:
    int     id;
    QString nom;
    double  lat;
    double  lng;
    QString region;
    int     population;
};

#endif // VILLE_H
