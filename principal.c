#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BLANK -1

#define MAX_TRANSITIONS 100 

// Structure pour une transition : (gauche, centre, droite) → nouvel état
typedef struct {
    int left;
    int center;
    int right;
    int new_state;
} Transition;

// Structure pour l'automate cellulaire
typedef struct {
    int num_states;                // nombre d'états possibles )
    int default_state;             // état par défaut (utilisé en bordure, souvent 0)
    Transition rules[MAX_TRANSITIONS];  // la table des règles
    int num_rules;                 // nombre de règles dans la table
} Automate;

// Structure pour une configuration (état de la ligne de cellules)
typedef struct {
    int* cells;   // tableau dynamique des états des cellules
    int size;     // nombre de cellules
} Configuration;


// structure de données de Machine de turing  ( q 8) 

// Alphabet = {0, 1, BLANK (on peut mettre -1 pour blanc)}
#define BLANK -1

// Mouvement
typedef enum { LEFT, RIGHT } Direction;

// Une transition dans la table
typedef struct {
    int current_state;
    int read_symbol;      // symbole lu sur le ruban
    int next_state;    
    int write_symbol;     // symbole à écrire
    Direction move;       // direction de déplacement
} TransitionTuring;

// La machine elle-même 
typedef struct {
    int num_states;
    int start_state;
    int accept_state;
    int reject_state;
    TransitionTuring* transitions;  
    int num_transitions;
} TuringMachine;

// Le ruban (tableau dynamique)
typedef struct {
    int* tape;      // tableau de symboles
    int size;       // taille allouée
    int head_pos;   // position actuelle de la tête
} Tape;

// Configuration actuelle de la machine de Turing
typedef struct {
    Tape tape;            // ruban avec tête
    int current_state;    // état courant
} ConfigurationTuring;



int lire_automate_depuis_fichier(const char* nom_fichier, Automate* a, Configuration* c) {
    FILE* file = fopen(nom_fichier, "r");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier.\n");
        return 0;
    }

    char ligne[100];
    a->num_rules = 0;
    a->num_states = 2;
    a->default_state = 0;

    // Lire les règles (ex: 110 1)
    while (fgets(ligne, sizeof(ligne), file)) {
        if (strncmp(ligne, "mot:", 4) == 0) {
            break; // On a atteint la ligne du mot
        }

        if (a->num_rules >= MAX_TRANSITIONS) {
            printf("Erreur : trop de règles.\n");
            fclose(file);
            return 0;
        }

        int l, c, r, res;
        if (sscanf(ligne, "%1d%1d%1d %1d", &l, &c, &r, &res) == 4) {
            a->rules[a->num_rules].left = l;
            a->rules[a->num_rules].center = c;
            a->rules[a->num_rules].right = r;
            a->rules[a->num_rules].new_state = res;
            a->num_rules++;
        }
    }

    // Lire la configuration initiale
    char* ptr = ligne + 4; // sauter "mot:"
    while (*ptr == ' ') ptr++; // ignorer les espaces

    int len = strlen(ptr);
    if (ptr[len - 1] == '\n') ptr[len - 1] = '\0'; // enlever \n

    c->size = strlen(ptr);
    c->cells = malloc(c->size * sizeof(int));
    if (!c->cells) {
        printf("Erreur d'allocation mémoire.\n");
        fclose(file);
        return 0;
    }

    for (int i = 0; i < c->size; i++) {
        c->cells[i] = ptr[i] - '0'; // convertir char '0' ou '1' en int 0 ou 1
    }

    fclose(file);
    return 1;
}
Configuration appliquer_une_etape(const Automate* a, const Configuration* c) {
    Configuration new_conf;
    new_conf.size = c->size;
    new_conf.cells = malloc(new_conf.size * sizeof(int));
    
    

    for (int i = 0; i < c->size; i++) {
        int gauche = (i == 0) ? a->default_state : c->cells[i - 1];
        int centre = c->cells[i];
        int droite = (i == c->size - 1) ? a->default_state : c->cells[i + 1];

        int nouvel_etat = a->default_state; // Par défaut, rien ne change

        // Chercher dans les règles une correspondance
        for (int j = 0; j < a->num_rules; j++) {
            Transition t = a->rules[j];
            if (t.left == gauche && t.center == centre && t.right == droite) {
                nouvel_etat = t.new_state;
                break;
            }
        }

        new_conf.cells[i] = nouvel_etat;
    }

    return new_conf;
}
void afficher_configuration(const Configuration* c) {
    for (int i = 0; i < c->size; i++) {
        printf("%d", c->cells[i]);
    }
    printf("\n");
}
int configurations_identiques(const Configuration* a, const Configuration* b) {
    if (a->size != b->size) return 0;
    for (int i = 0; i < a->size; i++) {
        if (a->cells[i] != b->cells[i]) return 0;
    }
    return 1;
}

void simuler_automate(const Automate* a, Configuration config_initiale, int nb_max,
                      int surveiller_transition, int l, int c, int r, int new_state) {
    Configuration current = config_initiale;

    printf("Étape 0 : ");
    afficher_configuration(&current);

    for (int t = 1; t <= nb_max; t++) {
        int transition_detectee = 0;

        Configuration suivant;
        suivant.size = current.size;
        suivant.cells = malloc(suivant.size * sizeof(int));

        for (int i = 0; i < current.size; i++) {
            int gauche = (i == 0) ? a->default_state : current.cells[i - 1];
            int centre = current.cells[i];
            int droite = (i == current.size - 1) ? a->default_state : current.cells[i + 1];

            int nouvel_etat = a->default_state;

            for (int j = 0; j < a->num_rules; j++) {
                Transition tr = a->rules[j];
                if (tr.left == gauche && tr.center == centre && tr.right == droite) {
                    nouvel_etat = tr.new_state;

                    // MODE 2 : détecter la transition surveillée
                    if (surveiller_transition &&
                        gauche == l && centre == c && droite == r &&
                        nouvel_etat == new_state) {
                        transition_detectee = 1;
                    }

                    break;
                }
            }

            suivant.cells[i] = nouvel_etat;
        }

        printf("Étape %d : ", t);
        afficher_configuration(&suivant);

        // MODE 3 : arrêter si la config ne change plus
        if (configurations_identiques(&current, &suivant)) {
            printf("➤ Arrêt automatique : la configuration est stable.\n");
            free(suivant.cells);
            break;
        }

        // MODE 2 : arrêter si la transition spéciale a été utilisée
        if (transition_detectee) {
            printf("➤ Arrêt automatique : transition (%d%d%d → %d) appliquée.\n", l, c, r, new_state);
            free(suivant.cells);
            break;
        }

        free(current.cells);
        current = suivant;
    }

    free(current.cells); // nettoyage final
}
// question 10 
#include <string.h>



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
        // Lire les états spéciaux
        if (strncmp(ligne, "start:", 6) == 0) {
            sscanf(ligne + 6, "%d", &machine->start_state);
        } else if (strncmp(ligne, "accept:", 7) == 0) {
            sscanf(ligne + 7, "%d", &machine->accept_state);
        } else if (strncmp(ligne, "reject:", 7) == 0) {
            sscanf(ligne + 7, "%d", &machine->reject_state);
        }

        // Lire le ruban initial
        else if (strncmp(ligne, "tape:", 5) == 0) {
            int valeurs[100], n = 0;
            char* token = strtok(ligne + 5, " \n");
            while (token) {
                if (strcmp(token, "_") == 0)
                    valeurs[n++] = BLANK;
                else
                    valeurs[n++] = atoi(token);
                token = strtok(NULL, " \n");
            }

            config->tape.tape = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                config->tape.tape[i] = valeurs[i];
            }
            config->tape.size = n;
            config->tape.head_pos = 0;
            config->current_state = machine->start_state;
        }

        // Lire les transitions
        else if (ligne[0] >= '0' && ligne[0] <= '9') {
            int q1, q2;
            char sr[10], sw[10], dir;
            if (sscanf(ligne, "%d %s %d %s %c", &q1, sr, &q2, sw, &dir) == 5) {
                TransitionTuring t;
                t.current_state = q1;

                // lire le symbole lu
                if (strcmp(sr, "_") == 0)
                    t.read_symbol = BLANK;
                else
                    t.read_symbol = atoi(sr);

                // lire le symbole à écrire
                if (strcmp(sw, "_") == 0)
                    t.write_symbol = BLANK;
                else
                    t.write_symbol = atoi(sw);

                t.next_state = q2;
                t.move = (dir == 'R') ? RIGHT : LEFT;

                machine->transitions[machine->num_transitions++] = t;
            }
        }
    }

    fclose(f);
    return 1;
}
int effectuer_un_pas(const TuringMachine* tm, ConfigurationTuring* config) {
    int symbole_lu = config->tape.tape[config->tape.head_pos];
    int etat_actuel = config->current_state;

    //  DEBUG : affichage de l’état courant et du symbole lu
    printf("DEBUG - État courant : %d, Symbole lu : %d\n", etat_actuel, symbole_lu);

    for (int i = 0; i < tm->num_transitions; i++) {
        TransitionTuring t = tm->transitions[i];

        // DEBUG : afficher chaque transition testée
        printf("  ➤ Test transition : état=%d, lu=%d → état'=%d, écrit=%d, dir=%s\n",
               t.current_state, t.read_symbol, t.next_state, t.write_symbol,
               (t.move == RIGHT) ? "R" : "L");

        if (t.current_state == etat_actuel && t.read_symbol == symbole_lu) {
            // Appliquer la transition
            config->tape.tape[config->tape.head_pos] = t.write_symbol;
            config->current_state = t.next_state;

            if (t.move == RIGHT) {
                config->tape.head_pos++;
                if (config->tape.head_pos >= config->tape.size) {
                    config->tape.size++;
                    config->tape.tape = realloc(config->tape.tape, config->tape.size * sizeof(int));
                    config->tape.tape[config->tape.size - 1] = BLANK;
                }
            } else { // LEFT
                if (config->tape.head_pos == 0) {
                    config->tape.size++;
                    int* nouveau_ruban = malloc(config->tape.size * sizeof(int));
                    nouveau_ruban[0] = BLANK;
                    memcpy(&nouveau_ruban[1], config->tape.tape, (config->tape.size - 1) * sizeof(int));
                    free(config->tape.tape);
                    config->tape.tape = nouveau_ruban;
                    config->tape.head_pos = 0;
                } else {
                    config->tape.head_pos--;
                }
            }

            return 1; // Transition trouvée
        }
    }

    return 0; // Aucune transition trouvée
}
void afficher_ruban(const ConfigurationTuring* config) {
    for (int i = 0; i < config->tape.size; i++) {
        if (i == config->tape.head_pos) printf("["); else printf(" ");
        if (config->tape.tape[i] == BLANK) printf("_");
        else printf("%d", config->tape.tape[i]);
        if (i == config->tape.head_pos) printf("]"); else printf(" ");
    }
    printf("   État: %d\n", config->current_state);
}
void simuler_machine_turing(const TuringMachine* tm, ConfigurationTuring* config) {
    int etape = 0;

    printf("Étape %d :\n", etape);
    afficher_ruban(config);

    while (config->current_state != tm->accept_state &&
           config->current_state != tm->reject_state) {

        if (!effectuer_un_pas(tm, config)) {
            printf(" Aucune transition applicable. Calcul interrompu.\n");
            return;
        }

        etape++;
        printf("Étape %d :\n", etape);
        afficher_ruban(config);
    }

    if (config->current_state == tm->accept_state) {
        printf(" Machine arrêtée en état d'ACCEPTATION.\n");
    } else {
        printf(" Machine arrêtée en état de REJET.\n");
    }
}
int main() {
    int choix;

    printf("Quel type de machine voulez-vous simuler ?\n");
    printf("1. Automate cellulaire\n");
    printf("2. Machine de Turing\n");
    printf("Choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        // ░░░░░░ AUTOMATE CELLULAIRE ░░░░░░
        Automate a;
        Configuration c;

        if (!lire_automate_depuis_fichier("symetrique.txt", &a, &c)) {
            printf("Erreur lors du chargement du fichier.\n");
            return 1;
        }

        printf("Automate et configuration chargés !\n");
        printf("Configuration initiale : ");
        for (int i = 0; i < c.size; i++) {
            printf("%d", c.cells[i]);
        }
        printf("\n");

        int nb_max;
        printf("Entrez le nombre maximum d'étapes à simuler : ");
        scanf("%d", &nb_max);

        int surveiller = 0;
        printf("Souhaitez-vous surveiller une transition particulière ? (1 = oui, 0 = non) : ");
        scanf("%d", &surveiller);

        int l = 0, c1 = 0, r = 0, res = 0;
        if (surveiller) {
            printf("Entrez la transition à surveiller (ex: 1 1 1 0 pour 111 -> 0) : ");
            scanf("%d %d %d %d", &l, &c1, &r, &res);
        }

        simuler_automate(&a, c, nb_max, surveiller, l, c1, r, res);
    }

    else if (choix == 2) {
        // ░░░░░░ MACHINE DE TURING ░░░░░░
        TuringMachine tm;
        ConfigurationTuring config;

        if (lire_machine_turing("machine.txt", &tm, &config)) {
            printf("Machine chargée avec succès.\n");

            int mode;
            printf("Choisir le mode :\n");
            printf("1. Un seul pas de calcul\n");
            printf("2. Simulation complète jusqu’à ACCEPT ou REJECT\n");
            printf("Mode : ");
            scanf("%d", &mode);

            if (mode == 1) {
                // Question 11 : un seul pas
                printf("Configuration initiale :\n");
                afficher_ruban(&config);

                if (effectuer_un_pas(&tm, &config)) {
                    printf("Après un pas de calcul :\n");
                    afficher_ruban(&config);
                } else {
                    printf("Aucune transition applicable. Arrêt.\n");
                }
            } else if (mode == 2) {
                // Question 12 : simulation complète
                simuler_machine_turing(&tm, &config);
            } else {
                printf("Mode invalide.\n");
            }

        } else {
            printf("Erreur lors du chargement de la machine de Turing.\n");
        }
    }

    else {
        printf("Choix invalide.\n");
    }

    return 0;
} 