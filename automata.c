/**
 * Le but de cet exercice est de faire des fonctions de création et
 * manipulation d'automates.  Bien que les automates aient étés vu en IF114,
 * les détails ne sont aucunement nécessaires pour réaliser l'exercice. Nous
 * rappelons tout de même qu'un automate est défini par :
 *
 * - Un alphabet. Ici nous prendrons les caractères ASCII `char`
 * - Un ensemble d'états (struct state)
 * - Un ensemble de transitions étiquetées par une lettre (struct transition)
 * - Un ensemble d'état initiaux, ici nous le limiterons à 1 par construction
 * - Un ensemble d'état finaux
 *
 * Un mot est dit reconnu (ou accepté) par un automate s'il existe un
 * chemin sous forme d'une suite de transitions partant de l'état
 * initial et menant à un état final telle que les étiquettes des
 * transitions correspondent exactement aux lettres du mot.
 *
 * REMARQUES GÉNÉRALES:
 *
 * - Si vous ne parvenez pas à réaliser une fonction, vous pouvez tout de même
 *   vous en servir (comme si vous aviez réussi à la faire).
 * - Les `const`s sont la pour vous aider, cependant s'ils vous gênent
 *   veuillez les supprimer des prototypes.
 * - Les fonctions sont proposées dans un ordre logique et non l'ordre de
 *   difficulté, vous pouvez les implémenter dans l'ordre de votre choix.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * BAREME:
 * - 1pts: index_of, is_valid_state, is_final_state, get_state, next_state,
 *         is_recognized, add_state, make_lexicon
 * - 2pts: read_word, make_number
 * - 3pts: add_transition, is_deterministic, add_word
 */

/**
 * Retourne l'indice de la première occurrence de `value` dans le
 * tableau d'entiers `values` (de taille `size`), ou -1 si l'élément
 * ne s'y trouve pas.
 **/
int index_of(int value, int size, int values[]){
    int index=-1;
    for (int i=0; i<size; i++){
        if (values[i]==value){
            index=i;
            return index;
        }
    }
    return index;
}

// Le nombre maximal d'états que pourront contenir les automates
#define MAX_STATES 19
// Le nombre maximal de transitions issues d'un état
#define MAX_TRANSITIONS 11

/**
 * Un automate contient un tableau `states` de MAX_STATES états. Les
 * indices des états correspondent aux indices dans ce tableau.
 */

/**
 * Les transitions sont stockées dans les états duquel elles
 * partent. Elles possèdent une étiquette `label` et un état d'arrivée
 * `target` représenté par son indice dans le tableau d'états de
 * l'automate.
 */
struct transition {
    char label; // Le caractère qui étiquette la transition
    int target; // L'id de destination de la transition
};

/**
 * Un état est représenté par un entier (indice dans le tableau
 * d'états de l'automate) et une instance de la structure `state`
 * stockée à cet indice dans le tableau `states` de la structure
 * `automaton`
 */
struct state {
    int is_final; // L'état est-il final (0 non, 1 oui)
    int transition_count; // Le nombre de transitions sortantes de cet état
    struct transition transitions[MAX_TRANSITIONS]; // Le tableau qui stocke les
    // transitions sortantes de cet état
};

struct automaton {
    int initial; // L'indice de l'état initial dans le tableau `states`
    int state_count; // Le nombre d'états définis pour cet automate
    struct state states[MAX_STATES]; // Le tableau stockant tous les états
};

/**
 * L'identifiant de l'état est-il valide pour un automate, i.e., s'il
 * correspond à un indice valide dans le tableaux des états.
 */
int is_valid_state(int state, struct automaton automaton){
    return ((state>=0) && (state<automaton.state_count));
}

/**
 * L'identifiant de l'état correspond il à un état final.
 */
int is_final_state(int state, struct automaton automaton){
    if (is_valid_state(state, automaton)){
        return (automaton.states[state].is_final == 1);
    }
    return 0;
}

/**
 * Retourne un `struct state` correspondant à un état sans transition.
 *
 * REMARQUE: Cette fonction vous est fournie, elle n'est donc pas à faire.
 */
struct state empty_state(){
    struct state empty = {}; //A-t-on droit ?? Ce n'est pas local (Donc détruit apres ?) ?***
    return empty;
}

/**
 * Retourne le `struct state` correspondant à l'état `state`, s'il existe
 * sinon retourne un état vide.
 */
struct state get_state(int state, struct automaton automaton){
    if (is_valid_state(state, automaton))
        return (automaton.states[state]);
    empty_state();
}
/**
 * Retourne l'indice de la destination de l'état `state` s'il existe une
 * transition etiqueté par la `letter`. Retourne -1 s'il n'existe pas de
 * transition valide.
 *
 * REMARQUE: On suppose que l'état appartient à un automate déterministe.
 */
int next_state(char letter, struct state state){
    for (int i=0; i<state.transition_count; i++){
        if (state.transitions[i].label == letter)
            return state.transitions[i].target;
    }
    return -1;
}

/**
 * Retourne l'état d'arrivée correspondant à la traversée de l'automate
 * `automaton` pour la chaine `str` (en partant de l'état initial).
 * Retourne -1 s'il n'existe pas de tel état.
 *
 * REMARQUE: L'automate est supposé déterministe.
 */
int read_word(const char *str, struct automaton automaton){
    int index_letter=0;
    char letter= str[index_letter];
    int index_state=automaton.initial;
    while ((letter != '\0')) {
        struct state state_actuel = automaton.states[index_state];
        index_state = next_state(letter,state_actuel);
        index_letter++;
        letter=str[index_letter];
    }
    return index_state;
}

/**
 * Retourne vrai si la chaîne `str` est reconnue valide pour l'automate
 * `automaton`
 *
 * REMARQUE: L'automate est supposé déterministe.
 */
int is_recognized(const char *str, struct automaton automaton){
    int index_state = read_word(str,automaton);
    if (index_state>=0)
        return (automaton.states[index_state].is_final);
    return 0;
}
/**
 * Retourne un automate vide mais valide.
 * Pour cela il doit au moins avoir un état sans transition, son état initial.
 *
 * REMARQUE: Cette fonction vous est fournie, elle n'est donc pas à faire.
 */
extern struct automaton make_automaton();

/**
 * Marque l'état `state` comme final
 *
 * REMARQUE: Cette fonction vous est fournie, elle n'est donc pas à faire.
 */
extern void make_state_final(int state, struct automaton *automaton);

/**
 * Ajoute et retourne l'indice du plus petit nouvel état valide pour
 * `automaton` si possible, sinon retourne -1.
 *
 * REMARQUE: Après l'appel de cette fonction, à moins que la fonction retourne
 * -1, l'indice retourné est valide pour l'automate
 */
int add_state(struct automaton *automaton){
    if (automaton->state_count<MAX_STATES){
        automaton->state_count++;
        automaton->states[automaton->state_count] = (struct state) {0,0,{}};
        return (automaton->state_count-1);
    }
    return -1;
}

/**
 * Ajoute une transition étiquetée par `letter` entre `source` et `dest` dans
 * l'automate `automaton`, si c'est possible (i.e., MAX_TRANSITIONS n'est pas
 * atteint, les états existent, etc.), et retourne l'indice de la destination.
 * En cas d'erreur la fonction retourne -1.
 *
 * REMARQUE:
 * - La transition ne doit pas être ajoutée (dupliquée) si elle existe déjà (ce
 *   n'est pas une erreur).
 */

int add_transition(char letter, int source, int dest, struct automaton *automaton){
    if ((source<automaton->state_count) && (dest<automaton->state_count)){
        if (automaton->states[source].transition_count < MAX_TRANSITIONS){
            int t_c = automaton->states[source].transition_count;
            for (int i=0;i<MAX_TRANSITIONS;i++){
                if (automaton->states[source].transitions[i].target == dest){
                    if(automaton->states[source].transitions[i].label == letter){
                        return dest;
                    }
                }
            }
            automaton->states[source].transitions[t_c].label  = letter;
            automaton->states[source].transitions[t_c].target = dest;
            automaton->states[source].transition_count++;
            return dest;
        }
    }
    return -1;
}

/*
 * Retourne un automate reconnaisant les entiers relatifs bien formés, i.e.,
 * l'expression régulière suivante "-?[0-9]+"
 *
 * Nous imposons l'automate suivant : (attention a la numérotation des états)
 * L'état 1 est final.
 *
 *                 +-----+
 *                 v     | [0-9]
 * ___            ___    |
 * |0|---[0-9]-->||1||---+
 * ---            ---
 *  |              ^
 *  |              |
 *  |            [0-9]
 *  |              |
 *  |             ___
 *  +----[-]----->|2|
 *                ---
 *
 */
struct automaton make_number(){
    struct state states[3] = {{0,2,(struct transition) {}}}
    struct automaton aut = {0, 3, }
}

/**
 * L'automate `automaton` est-il déterministe.
 *
 * Un automate est dit déterministe si :
 * - Il n'a qu'un seul état initial (trivialement vérifié par construction)
 * - Il n'a pas de transition epsilon (trivialement vérifié par leur non
 *   définition dans cet exercice)
 * - Pour tout état il n'existe qu'une seule destination possible pour une
 *   étiquette donnée. <--- La propriété à verifier
 */
int is_deterministic(struct automaton automaton){
    for (int i=0; i<automaton.state_count; i++){
        for (int j=0; j<automaton.states[i].transition_count; j++){
            char letter = automaton.states[i].transitions[j].label;
            int dest = automaton.states[i].transitions[j].target;
            for (int k=0; k<automaton.states[i].transition_count; k++){
                if ( (letter == automaton.states[i].transitions[k].label) && (dest !=automaton.states[i].transitions[k].target))
                    return 0;
                    
            }
        }
    }
    return 1;
}

/**
 * Ajoute l'ensemble des états et des transitions nécéssaires pour que
 * l'automate `automate` reconnaisse le mot `word` depuis l'état
 * `state` en préservant le determinisme, i.e., si un préfixe du mot
 * est reconnu par l'automate, alors ce préfixe est réutilisé.
 *
 * Un exemple ?? Eventuellement voir ci dessous
 */
//1- On determine le prefixe eventuel reconnu par l'automate.
//2- Ensuite on complete l'automate (par des transitions nécéssaires pour qu'il accepte le mot)
void
add_word(const char *word, int state, struct automaton *automaton)
    ;

/**
 * Retourne un lexique (automate) reconaissant les `length` mots `words`.
 * Un lexique est un automate fini deterministe reconaissant exactement un
 * ensemble de mot. Par exemple :
 *    make_lexicon(4, {"allo", "allonyme", "aligot", "castor"})
 *    Etats finaux : 4, 8, 12, 18

 ___      ___      ___      ___      _____      ___      ___      ___       ___
 |0|-[a]->|1|-[l]->|2|-[l]->|3|-[o]->||4||-[n]->|5|-[y]->|6|-[m]->|7|-[e]->||8||
 ---      ---      ---      ---      -----      ---      ---      ---       ---
 |                |        ___       ____      ____      ______
 |                 \--[i]->|9|->[g]->|10|-[o]->|11|-[t]->||12||
 |                         ---       ----      ----      ------
 |       ____      ____      ____      ____      ____      ______
 \--[c]->|13|-[a]->|14|-[s]->|15|-[t]->|16|-[o]->|17|-[r]->||18||
         ----      ----      ----      ----      ----      ------
 */
struct automaton make_lexicon(int length, const char *words[])
    ;