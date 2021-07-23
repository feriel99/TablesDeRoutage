#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define nb_sommets 100

#define TIER1 - 10
#define TIER2 - 20
#define TIER3 - 30
#define MATRICE 1000

struct reseau {

  int sommets; //nombre de sommets
  int depart; //depart du routage
  int destination; //destination du routage
  int ** temps; // matrice des temps
  int * col; //tableau pour parcours profondeur
  int ** matrice_adj; //matrice adjacence
  int ** Pred; //matrice des predecesseurs
  int ** FW; //matrice modifiée pour Floyd Warshall
};
typedef struct reseau Reseau;

int aleatoire(int min, int max) {
  if (max < min) {
    return 0;
  }

  int alea = (rand() % (max - min + 1)) + min;

  return alea;
}
int alea_temps(int type) {

  int temps;
  if (type == TIER1) {
    temps = aleatoire(5, 10);
    return temps;
  } else if (type == TIER2) {
    temps = aleatoire(10, 20);
    return temps;
  } else if (type == TIER3) {
    temps = aleatoire(15, 50);
    return temps;
  }

  return 0;
}

/*
 * LA CREATION DU RESEAU 
 *
 * */

Reseau initialiser_reseau(int n) {
  Reseau R;
  R.sommets = n;
  R.col = malloc(R.sommets * sizeof(int));
  int i;
  R.matrice_adj = malloc(R.sommets * sizeof(int * ));
  for (i = 0; i < R.sommets; i++) {
    R.matrice_adj[i] = calloc(R.sommets, sizeof(int));
  }

  R.temps = malloc(R.sommets * sizeof(int * ));
  for (i = 0; i < R.sommets; i++) {
    R.temps[i] = malloc(R.sommets * sizeof(int));
  }

  R.Pred = malloc(R.sommets * sizeof(int * ));
  for (i = 0; i < R.sommets; i++) {
    R.Pred[i] = malloc(R.sommets * sizeof(int));
  }

  R.FW = malloc(R.sommets * sizeof(int * ));
  for (i = 0; i < R.sommets; i++) {
    R.FW[i] = malloc(R.sommets * sizeof(int));
  }
  return R;
}

Reseau reseau_aleat(Reseau R) {
  R = initialiser_reseau(nb_sommets);
  int i, j;
  /*TIER1 */
  for (i = 0; i <= 9; i++) {
    for (j = 0; j <= 9; j++) {
      if ((rand() % 100) > 25 && i != j) {
        R.matrice_adj[i][j] = R.matrice_adj[j][i] = 1;
        R.temps[j][i] = R.temps[i][j] = alea_temps(TIER1);
        //   Sommet A      
      }
    }
  }
  /*TIER2 */
  int nb, nb2, x;
  for (i = 10; i <= 29; i++) { //pour chaque noeud du TIER2 
    nb = aleatoire(1, 2); // 1 ou 2 noeuds de TIER1 
    for (j = 0; j < nb; j++) {
      x = aleatoire(0, 9); //choix du noeud du TIER1 
      R.matrice_adj[i][x] = R.matrice_adj[x][i] = 1;
      R.temps[i][x] = R.temps[x][i] = alea_temps(TIER2);
    }
  }

  for (i = 10; i <= 29; i++) { //pour chaque noeud du TIER2
    nb2 = aleatoire(2, 3); // 2 ou 3 noeuds de TIER2
    for (j = 0; j < nb2; j++) {
      x = aleatoire(10, 29); //choix du noeud de TIER2   
      R.matrice_adj[i][x] = R.matrice_adj[x][i] = 1;
      R.temps[i][x] = R.temps[x][i] = alea_temps(TIER2);
    }
  }
  /*TIER3 */
  for (i = 29; i <= 99; i++) {
    for (j = 0; j < 2; j++) {
      x = aleatoire(10, 29); // 2 noeuds de TIER2
      R.matrice_adj[i][x] = R.matrice_adj[x][i] = 1;
      R.temps[i][x] = R.temps[x][i] = alea_temps(TIER3);
    }
  }
  for (i = 29; i <= 99; i++) {
    for (j = 0; j < 1; j++) {
      x = aleatoire(29, 99); //1 noeud du TIER3
      R.matrice_adj[i][x] = R.matrice_adj[x][i] = 1;
      R.temps[i][x] = R.temps[x][i] = alea_temps(TIER3);

    }
  }
  return R;
}

void afficher_reseau(Reseau R) {
  printf("La creation du reseau \n");
  int i, j;
  for (i = 1; i < R.sommets; i++) {
    for (j = 1; j < R.sommets; j++) {
      if (R.temps[i][j] != 0)

      {
        printf("A %d  <== temps : %d  ==> %d B\n ", i, R.temps[i][j], j);
      }
    }
  }
}
void desalloue_reseau(Reseau R) {
  int i;
  for (i = 0; i < R.sommets; i++) {
    free(R.matrice_adj[i]);
  }
  free(R.matrice_adj);

  for (i = 0; i < R.sommets; i++) {
    free(R.temps[i]);
  }
  free(R.temps);

  for (i = 0; i < R.sommets; i++) {
    free(R.Pred[i]);
  }
  free(R.Pred);

  for (i = 0; i < R.sommets; i++) {
    free(R.FW[i]);
  }
  free(R.FW);

  free(R.col);

}

/* La verification de la connexité du reseau
 * 
 */

Reseau colorier_sommet(Reseau R, int sommet, int couleur) {
  int i;
  R.col[sommet] = couleur;
  printf("le sommet %d  est  colorié  \n", sommet);
  for (i = 0; i < R.sommets; i++) {
    if (R.col[i] == 0 && R.col[i] != couleur) {
      R = colorier_sommet(R, i, couleur);
    }
  }
  return R;
}
Reseau parcours_profondeur(Reseau R) {
  int i, j, couleur, compteur = 0;
  for (i = 0; i < R.sommets; i++) {
    R.col[i] = 0;
  }
  couleur = 1;
  for (j = 0; j < R.sommets; j++) {
    if (R.col[j] == 0) {
      R = colorier_sommet(R, j, couleur);
      couleur++;
    }
  }
  printf("\n");
  printf("La vérification de la connexité du réseau\n");
  for (i = 0; i < R.sommets; i++) {
    printf("%d ", R.col[i]);
    if (R.col[i] == 1) {
      compteur++;
    }
  }
  printf("\n");

  if (compteur == R.sommets) {
    printf("Le réseau est bien connexe.\n");
  } else {
    printf("Le réseau n'est pas connexe \n");
    R = reseau_aleat(R);
  }
  return R;
}

/**
 * la détermination de la table de routage de chaque noeud
 * 
 **/

void Floyd_Warshall(Reseau R) {
  int i, j, k;
  for (i = 0; i < R.sommets; i++) {
    for (j = 0; j < R.sommets; j++) {
      R.FW[i][j] = MATRICE;
      R.Pred[i][j] = 0;
      if (R.matrice_adj[i][j] == 1) {
        R.Pred[i][j] = i;
      }
    }
  }
  // remplir la matrice des temps 

  for (i = 0; i < R.sommets; i++) {
    R.Pred[i][i] = i;
    R.FW[i][i] = 0;
    for (j = 0; j < R.sommets; j++) {
      if (R.matrice_adj[i][j] == 1) {
        R.FW[i][j] = R.temps[i][j];
      }
    }
  }
  // remplir la matrice des predecessseur 
  for (k = 0; k < R.sommets; k++) {
    for (i = 0; i < R.sommets; i++) {
      for (j = 0; j < R.sommets; j++) {
        if (R.FW[i][k] + R.FW[k][j] < R.FW[i][j]) {
          R.FW[i][j] = R.FW[i][k] + R.FW[k][j];
          R.Pred[i][j] = R.Pred[k][j];
        }
      }
    }
  }
  // afficher la matrice des temps 
  printf("Matrice des temps minimaux :\n");
  for (i = 0; i < R.sommets; i++) {
    for (j = 0; j < R.sommets; j++) {
      printf(" %3d ", R.FW[i][j]);
    }
    printf("\n");
  }

  printf("\n");

  // la matrices des predecceuseur 

  printf("Matrice des prédécesseur:\n");
  for (i = 0; i < R.sommets; i++) {
    for (j = 0; j < R.sommets; j++) {
      printf(" %3d ", R.Pred[i][j]);
    }
     printf("\n");
  }

}

/*
 * La reconstitution du chemin entre 2 noeuds

 * 
 * */
typedef struct Element Element;
struct Element {
  int nombre;
  Element * suivant;
};

typedef struct Pile Pile;
struct Pile {
  Element * premier;
};
Pile * initialiser() {
  Pile * pile = malloc(sizeof( * pile));
  pile -> premier = NULL;
  return pile;
}

void empiler(Pile * pile, int Nomb) {
  Element * nouveau = malloc(sizeof( * nouveau));
  if (pile == NULL || nouveau == NULL) {
    exit(EXIT_FAILURE);
  }

  nouveau -> nombre = Nomb;
  nouveau -> suivant = pile -> premier;
  pile -> premier = nouveau;
}

int depiler(Pile * pile) {
  if (pile == NULL) {
    exit(EXIT_FAILURE);
  }

  int nombreDepile = 0;
  Element * elementDepile = pile -> premier;

  if (pile != NULL && pile -> premier != NULL) {
    nombreDepile = elementDepile -> nombre;
    pile -> premier = elementDepile -> suivant;
    free(elementDepile);
  }

  return nombreDepile;
}

Pile * viderPile(Pile * pile) {
  while (pile -> premier != NULL) {
    depiler(pile);
  }
  return pile;
}

void afficherPile(Pile * pile, Reseau R) {
  if (pile == NULL) {
    exit(EXIT_FAILURE);
  }
  Element * noeud = pile -> premier;
  do {
    printf("Le message prend le sommet %d ==>", noeud -> nombre);
    noeud = noeud -> suivant;
  }
  while (noeud != NULL);
}

void reconstitution_chemin_sommets(Reseau R) {
  printf("Noeud émetteur:");
  scanf("%d", & (R.depart));
  printf("\n");
  printf("Noeud destinataire:");
  scanf("%d", & (R.destination));
  printf("\n");
  int recevoir = R.destination;
  Pile * p = initialiser();
  printf("Le message part de sommet %d ==>", R.depart);
  do {
    R.destination = R.Pred[R.depart][R.destination];
    empiler(p, R.destination);
  }
  while (R.Pred[R.depart][R.destination] != R.depart);
  afficherPile(p, R);

  printf("Le message arrive au sommet %d\n", recevoir);
  printf("\n");
  printf("Le temps pour recevoir le message de %d à %d est :%d\n",
    R.depart, recevoir, R.FW[R.depart][recevoir]);
  p = viderPile(p);
  free(p);
}

int main(int argc, char * argv[]) {
  srand(time(NULL));
  Reseau R;
  R = reseau_aleat(R);
  afficher_reseau(R);
  R = parcours_profondeur(R);
  Floyd_Warshall(R);
 reconstitution_chemin_sommets(R);
  desalloue_reseau(R);
  return 0;
}
