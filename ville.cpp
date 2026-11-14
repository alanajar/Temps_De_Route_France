#include "ville.h"

Ville::Ville(int id, const QString& nom, double lat, double lng,
             const QString& region, int population)
    : id(id)
    , nom(nom)
    , lat(lat)
    , lng(lng)
    , region(region)
    , population(population)
{}
