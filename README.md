# Temps de route — SAE 2.02

Application Qt (C++) qui affiche un graphe de villes et les temps de trajet entre elles.
Projet réalisé par Ala NAJAR.

## Fonctionnalités

L'interface comporte quatre onglets :

- **Trajet** : calcul d'un trajet entre deux villes
- **Tableau** : génération d'un tableau des temps de trajet
- **Dépôt** : recherche du dépôt optimal
- **Filtres** : application de filtres sur les villes

Une carte affiche les villes et les liaisons.

## Données

- `villes.csv` : 100 villes (nom, coordonnées, pays, population, etc.)
- `temps.csv` : 542 liaisons sous la forme `ville1,ville2,temps`

## Prérequis

- Qt 6 (modules Core, Gui, Widgets)
- qmake6 et un compilateur C++17 (g++)

## Compilation et exécution

```bash
mkdir -p build/run && cd build/run
qmake6 ../../s202_Ala_Najar.pro
make -j4
./s202_Ala_Najar
```

Le programme doit être lancé depuis le dossier de compilation : le fichier `.pro`
y copie `villes.csv` et `temps.csv`, que l'application lit au démarrage.

Sinon, ouvrir `s202_Ala_Najar.pro` dans Qt Creator et lancer avec Ctrl+R.

## Structure

| Fichier | Rôle |
|---|---|
| `main.cpp` | Point d'entrée |
| `mainwindow.*` | Fenêtre principale et onglets |
| `graphe.*` | Graphe des villes et des liaisons |
| `ville.*` | Représentation d'une ville |
| `cartewidget.*` | Affichage de la carte |
