// simulateur.c – Question 13 : convertir une machine de Turing en automate cellulaire
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLANK -1
#define MAX_TAILLE 100
#define MAX_TRANSITIONS 100

// ─── Structures de la machine de Turing ───────────────────────────────────────
typedef enum { LEFT, RIGHT } Direction;

typedef struct {
    int current_state;
    int read_symbol;
    int next_state;
    int write_symbol;
    Direction move;
} TransitionTuring;

typedef struct {
    int num_states;
    int start_state;
    int accept_state;
    int reject_state;
    TransitionTuring* transitions;
    int num_transitions;
} TuringMachine;

// ─── Ruban et configuration ───────────────────────────────────────────────────
typedef struct {
    int* tape;
    int size;
    int head_pos;
} Tape;

typedef struct {
    Tape tape;
    int current_state;
} ConfigurationTuring;

// ─── Cellule de l'automate cellulaire ─────────────────────────────────────────
typedef struct {
    int is_head;
    int state;
    int symbol;
} Cellule;

// ─── Fonctions de chargement ──────────────────────────────────────────────────
int lire_machine_turing(const char* nom_fichier, TuringMachine* machine, ConfigurationTuring* config) {
    FILE* f = fopen(nom_fichier, "r");
    if (!f) {
        printf("Erreur : impossible d'ouvrir %s\n", nom_fichier);
        return 0;
    }

    char ligne[100];
    machine->num_transitions = 0;
    machine->transitions = malloc(MAX_TRANSITIONS * sizeof(TransitionTuring));

    while (fgets(ligne, sizeof(ligne), f)) {
        if (strncmp(ligne, "start:", 6) == 0) {
            sscanf(ligne + 6, "%d", &machine->start_state);
        } else if (strncmp(ligne, "accept:", 7) == 0) {
            sscanf(ligne + 7, "%d", &machine->accept_state);
        } else if (strncmp(ligne, "reject:", 7) == 0) {
            sscanf(ligne + 7, "%d", &machine->reject_state);
        } else if (strncmp(ligne, "tape:", 5) == 0) {
            int valeurs[100], n = 0;
            char* token = strtok(ligne + 5, " \n");
            while (token) {
                valeurs[n++] = (strcmp(token, "_") == 0) ? BLANK : atoi(token);
                token = strtok(NULL, " \n");
            }
            config->tape.tape = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) config->tape.tape[i] = valeurs[i];
            config->tape.size = n;
            config->tape.head_pos = 0;
            config->current_state = machine->start_state;
        } else if (ligne[0] >= '0' && ligne[0] <= '9') {
            int q1, q2;
            char sr[10], sw[10], dir;
            if (sscanf(ligne, "%d %s %d %s %c", &q1, sr, &q2, sw, &dir) == 5) {
                TransitionTuring t;
                t.current_state = q1;
                t.read_symbol = (strcmp(sr, "_") == 0) ? BLANK : atoi(sr);
                t.write_symbol = (strcmp(sw, "_") == 0) ? BLANK : atoi(sw);
                t.next_state = q2;
                t.move = (dir == 'R') ? RIGHT : LEFT;
                machine->transitions[machine->num_transitions++] = t;
            }
        }
    }
    fclose(f);
    return 1;
}

// ─── Fonctions de simulation cellulaire ───────────────────────────────────────
void initialiser_configuration(Cellule* bande, int taille, const int* ruban, int pos_tete, int etat_courant) {
    for (int i = 0; i < taille; i++) {
        bande[i].symbol = ruban[i];
        bande[i].is_head = (i == pos_tete);
        bande[i].state = (i == pos_tete) ? etat_courant : -1;
    }
}

void afficher_cellules(Cellule* bande, int taille) {
    for (int i = 0; i < taille; i++) {
        if (bande[i].is_head)
            printf("(%d,%d) ", bande[i].state, bande[i].symbol);
        else
            printf("(*,%d) ", bande[i].symbol);
    }
    printf("\n");
}

void simuler_etape_cellulaire(Cellule* bande, int taille, const TuringMachine* tm) {
    Cellule copie[MAX_TAILLE];
    memcpy(copie, bande, taille * sizeof(Cellule));

    for (int i = 0; i < taille; i++) {
        if (copie[i].is_head) {
            int q = copie[i].state;
            int s = copie[i].symbol;

            for (int t = 0; t < tm->num_transitions; t++) {
                TransitionTuring tr = tm->transitions[t];
                if (tr.current_state == q && tr.read_symbol == s) {
                    bande[i].symbol = tr.write_symbol;
                    bande[i].is_head = 0;
                    bande[i].state = -1;
                    int cible = (tr.move == RIGHT) ? i + 1 : i - 1;
                    if (cible >= 0 && cible < taille) {
                        bande[cible].is_head = 1;
                        bande[cible].state = tr.next_state;
                    }
                    return;
                }
            }
        }
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    TuringMachine tm;
    ConfigurationTuring config;

    if (!lire_machine_turing("machine.txt", &tm, &config)) {
        printf("Erreur de lecture de machine.txt\n");
        return 1;
    }

    Cellule bande[MAX_TAILLE];
    initialiser_configuration(bande, config.tape.size, config.tape.tape, config.tape.head_pos, config.current_state);

    printf("Configuration initiale :\n");
    afficher_cellules(bande, config.tape.size);

    for (int i = 0; i < 10; i++) {
        simuler_etape_cellulaire(bande, config.tape.size, &tm);
        printf("Étape %d :\n", i + 1);
        afficher_cellules(bande, config.tape.size);
    }

    free(config.tape.tape);
    free(tm.transitions);
    return 0;
}
