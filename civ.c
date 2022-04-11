#include <stdio.h>
#include <string.h>

/****************************************
 * Carta Impera Victoria
 * =====================
 *
 * Un jeu de : Rémi Amy
 * Illustration : C. Matt
 *                I. Parovel
 * Édité par Ludonaute
 * Distribué par Asmodee
 ****************************************/

// Il y a exactement 104 cartes dans le jeu (CIV en chiffre romains),
// on ne risque donc pas de faire un paquet plus gros que ce nombre.
#define MAX_CARDS       104
#define MAX_PLAYERS     4

// Les domaines du jeu CIV,
// C'est l'analogue des couleurs dans un jeu de cartes classique.
enum domain {
    MILITARY, RELIGION, ECONOMY, SCIENCE, CULTURE, UTOPIA,
    DOMAIN_COUNT, FIRST_DOMAIN=MILITARY, NO_DOMAIN=-1
};

// Les âges en chiffres romains, ça fait plus classe.
enum age {
    I=1, II=2, III=3, // Attention les romains n'avaient pas de zéro
    AGE_COUNT=III, FIRST_AGE=I
};

// Les cartes de CIV sont caractérisées par
struct card {
    enum age age;       // Un âge
    enum domain domain; // Un domaine
};

// Un paquet de cartes
struct deck {
    int count; // Nombre d'éléments dans le tableau `cards`
    struct card cards[MAX_CARDS]; // Les cartes sont au début du tableau
};

struct player {
    const char *name;  // Le nom du joueur
    struct deck hand;  // La main du joueur
    int min_hand_size; // Le nombre de cartes que le joueur devra
                       // avoir en main à la fin de son tour
    int domains[DOMAIN_COUNT]; // Le nombre de domaines possédés par le
                               // joueur sur le plan de jeu (sans relation
                               // avec la main)
                               // Ce tableau est indicé par `enum domain`
};

// Durant le jeu, le joueur pourra jouer une action parmi celle définies
// par la première ligne de cette `enum`.
enum action {
    SACRIFICE, LEVEL_1, LEVEL_2,
    ACTION_COUNT, FIRST_ACTION=SACRIFICE, NO_ACTION=-1
};

// Suivant le nombre de joueurs, les valeurs seuils sont différentes.
struct rules {
    int hegemony_victory; // Nombre de cartes à posséder dans un domaine
                          // pour déclencher une victoire par hégémonie.
    int action_thresholds[ACTION_COUNT]; // La validité d'une action dépend
                    // du nombre de domaines possédés par le joueur (seuil).
                    // Ce tableau est indicé par `enum action`
};

// Un exemple de règle (3 joueurs)
static struct rules three_player_rules = {
     .hegemony_victory = 7, // Le premier joueur qui possède 7 cartes dans
                            // un domaine gagne par hégémonie.
     .action_thresholds = {1, 3, 5} // Pour utiliser SACRIFICE il faut au
                        // moins 1 carte dans un domaine, et pour utiliser
                        // l'effet de LEVEL_2, il en faut au moins 5.
};

// Chacune des `enum` possède son tableau de chaînes
static const char *domain_texts[DOMAIN_COUNT] = {
    "Military", "Religion", "Economy", "Science", "Culture", "Utopia",
};
static const char *age_texts[AGE_COUNT] = {
    "I", "II", "III"
};
static const char *action_texts[ACTION_COUNT] = {
    "Sacrifice", "Level 1", "Level 2",
};

/**
 * Retourne l'indice de la première occurrence de `str` dans le tableau
 * de chaînes de caractères `table` de longueur `n`, -1 s'il n'existe pas
 * de telle occurrence.
 */
int index_of(const char *str, int n, const char *table[]){
        for (int i=0; i<n; i++){
                if (strcmp(table[i], str) == 0)
                    return (i);
        }
        return -1;
}
    

/**
 * Retourne l'indice de la plus grande valeur présente dans `array` s'il y a
 * une unique occurrence de cette valeur, -1 sinon.
 *
 * ex: unique_max_index(5, {3,4,6,4,5}) renvoie 2
 *     unique_max_index(5, {3,4,6,4,6}) renvoie -1
 */
int unique_max_index(int len, int array[]){
    int index=0;
    if (len==0)
        return -1;
    for (int i=1; i<len; i++){
        if (array[i] > array[index])
            index=i;
    }
    int n=0;
    for (int i=0; i<len; i++){
        if (array[i] == array[index])
            n++;
        if (n == 2)
            return -1;
    }
    return index;
}
/**
 * Retourne le plus grand indice dans le tableau `values` (de longueur `len`)
 * telle que la valeur soit inférieure ou égale à `value`, -1 s'il n'existe pas
 * de telle valeur.
 *
 * ex: last_index_below(6, 5, {1,3,5,7,9}) renvoie 2
 *     last_index_below(6, 5, {1,3,5,6,7}) renvoie 3
 */
int last_index_below(int value, int len, int values[]){
    int index=-1;
    for (int i=0; i<len; i++){
            if (values[i] <= value)
                index=i;
    }
    return index;
    
}
/**
 * Cette fonction retourne un deck vide
 */
struct deck empty_deck() //fct empty_deck retourne struct deck vide
{
    struct deck deck = {.count = 0};
    return deck;
}

/**
 * Ajoute à la fin de `deck` les cartes contenues dans `other`.
 * L'ordre des cartes de `other` doit être respecté.
 */
void append_deck(struct deck* deck, struct deck other){
       int l_index_deck = deck->count;
       int l_index_other = other.count;
       (deck->count)+=other.count;
       for (int i=0; i<l_index_other; i++){
            (deck->cards[l_index_deck+i])=other.cards[i];
       }
}
       

/**
 * Retire les `n` dernières cartes de `deck`, et les retourne sous la
 * forme d'un nouveau paquet. On pioche dans l'ordre du paquet, donc
 * l'ordre dans le nouveau paquet est l'ordre inverse du `deck`. S'il
 * n'y a pas assez de cartes dans le paquet, on les prend toutes.
 *
 * Attention: La dernière carte de `deck` se retrouve donc la première carte du
 * paquet retourné.
 */
struct deck take_cards(int n, struct deck *deck){
    struct deck new_deck=empty_deck();
    if (n <= deck->count)
        new_deck.count = n;
    else{
        new_deck.count = deck->count;
    }
    int deck_size = deck->count;
    for (int i=0; i<new_deck.count; i++){
         new_deck.cards[i]=deck->cards[deck_size-1-i];
        (deck->cards[deck_size-1-i]).age=FIRST_AGE;
        (deck->cards[deck_size-1-i]).domain=NO_DOMAIN;
        (deck->count)--;
    }
    return new_deck;
}

    

/**
 * Complète la main du joueur `player` avec les cartes de la pioche
 * `pile`, piochées dans l'ordre du paquet.
 *
 * Le nombre de cartes en main à la fin est donné par `min_hand_size`
 * TODO A déplacer .. elle fait appel a un joueur
 */
void draw_cards(struct deck *pile, struct player *player){
    player->min_hand_size += pile->count;
    struct deck pile_inv = take_cards(pile->count, pile);
    append_deck(&(player->hand), pile_inv);
        
}

    

/**
 * Cette fonction retourne un nombre aléatoire compris entre 0 et `n` exclu.
 *
 * ex: random(3) peut retourner 0, 1 ou 2
 *
 * Elle vous est fournie (donc pas à faire), d'où le `extern` qui la précède
 */
extern int random(int n);

/**
 * Mélange le paquet de cartes `deck`.
 *
 * L'algorithme de mélange est IMPOSÉ:
 * - Échanger la dernière carte (n) avec un carte au hasard
 *   comprise entre la première et cette dernière incluse (n)
 * - Recommencer avec l'avant dernière (n-1) cette fois comprise
 *   entre la première et n-1
 * - etc
 */
void shuffle_deck(struct deck *deck){
        int n = deck->count;
        for (int i=0; i<n; i++){
            struct card temp_card   = deck->cards[n-1-i];
            struct card card_choice = deck->cards[random(n-1-i)];
            deck->cards[n-1-i]      = card_choice;
            card_choice             = temp_card;
        }
}

// Nous vous fournissons les deux afficheurs suivants, le troisième
// est pour vous.

/**
 * Affiche une carte sous la forme :
 * Military (II)
 */
extern void print_card(struct card card);

/**
 * Affiche un deck dans l'ordre des cartes :
 * [ Military (II) Religion (I) Utopia (III) ]:3
 */
extern void print_deck(struct deck deck);

/**
 * Affiche les informations relative à un joueur sous la forme donnée en
 * exemple ci après terminée par un saut de ligne.
 *
 * Floréal 3/4 [ Military:3 Utopia:2 ]
 *
 * Dans cet exemple : Floréal est le nom du joueur; 3, le nombre de
 * cartes en main; 4, le nombre de cartes que le joueur devra avoir en
 * main à la fin de son tour. Le joueur possède 3 domaines militaires
 * et 2 domaines d'utopie.
 *
 * Les domaines sont affichés dans l'ordre de l'énumération `enum
 * domain`. Si un joueur ne possède pas d'un type de domaine, ce
 * domaine sera omis.
 */
void print_player(struct player p){
    static const char *domain_texts[DOMAIN_COUNT] = {
    "Military", "Religion", "Economy", "Science", "Culture", "Utopia",};
    printf("%s %d/%d [",p.name, (p.hand).count, p.min_hand_size);
    for (int i=0; i<6; i++){
        if (p.domains[i] > 0){
            printf(" %s:%d",*domain_texts[i],p.domains[i]);
        }
    }
    printf(" ]");
}
    

/**
 * Génère un deck contenant `n` cartes d'âge `age` et de domaine `domain`
 */
struct deck generate_domain_deck(int n, enum age age, enum domain domain){
        struct deck new_deck = empty_deck();
        new_deck.count = n;
        struct card card = {.age = age, .domain = domain};
        for (int i=0; i<n; i++){
            new_deck.cards[i]=card;
        }
        return new_deck;
}

/**
 * Chaque âge correspond à un paquet de cartes de différents domaines. Cette
 * fonction a pour but de créer un paquet de cartes pour un âge donné:
 *
 * - le nombre de cartes pour chaque domaine est donné dans le tableau
 *   `cards_by_domain` (de longueur DOMAIN_COUNT) indicé par enum domain
 * - le paquet de cartes devra être mélangé
 * - `discarded` cartes doivent être retirées à la fin du paquet généré
 *   (après mélange, pour être différentes à chaque fois)
 */
struct deck generate_age_deck(enum age age, const int cards_by_domain[], int discarded);
    

/**
 * Retourne l'indice, dans le tableau players de player_count joeurs, du
 * joueur possédant le plus de domaines de type `domain`. Si ce joueur n'est
 * pas unique cette fonction retourne -1.
 */
int who_has_the_biggest(enum domain domain, int player_count, struct player players[]){
    int big_index = 0;
    for (int i=1; i<player_count; i++){
        if ((players[i]).domains[domain] == (players[big_index]).domains[domain] )
            return (-1);
        if ((players[i]).domains[domain] > (players[big_index]).domains[domain] )
            big_index = i;
    }
    return big_index;
}

/**
 * Défausse une carte de domaine `domain` de la main du joueur et augmente
 * de un le nombre de domaines possédés par le joueur. Cette fonction
 * retourne vrai si le joueur gagne par hégémonie, faux (0) sinon.
 *
 * Remarque:
 * - Si le joueur ne possède pas de telle carte, la fonction ne fait rien.
 * - Les cartes dans la main du joueur doivent rester contigües (i.e
 *   sans "trou"), peu importe leur ordre.
 */
int play_domain(enum domain domain, struct player *player, struct rules rules){
    int possede_carte = 0;
    for (int i=0; i < (player->min_hand_size); i++){
            if ( ((player->hand).cards[i]).domain == domain )
                possede_carte=1;
                (player->hand).count --;
                //procédure pour enlever la carte ...
                break;
    }
    if (possede_carte == 1){
        player->min_hand_size --;
        player->domains[domain]++;
        if (player->domains[domain] == rules.hegemony_victory)
            return 1;
        else
        {
            return 0;
        } 
    }
}

/**
 * Cette fonction retourne l'indice du joueur ayant gagné la partie par
 * majorité, et -1 s'il n'existe pas de tel gagnant. Cette victoire est
 * déterminée de la manière suivante :
 *
 * - pour chaque domaine, on attribue 1 point au joueur qui a le plus
 *   de cartes de ce domaine (en cas d’égalité, aucun joueur ne marque
 *   de point).
 * - le gagnant, est le joueur qui a le plus de point (il n’y a pas de
 *   gagnant en cas d’égalité).
 *
 * Remarque: Nous sommes au courant, ce ne sont pas les vrais règles.
 */
int majority_victory(int player_count, struct player players[]){
    int players_score[player_count];
    for (int i=0; i<DOMAIN_COUNT; i++){
        int domains_count[player_count];
        for (int j=0; j<player_count; j++){
            domains_count[j]=(players[j]).domains[i];
        }
        int w = unique_max_index(player_count, domains_count);
        if (w > 0)
            players_score[w]++;
    }
    return unique_max_index(player_count, players_score);
}

/**
 * Ce prédicat (fonction qui retourne vrai ou faux (0)) vérifie que l'action
 * proposée par un joueur est possible. L'action est définie par deux chaînes
 * de caractères `domain` et `action`.
 *
 * Nous rappelons qu'une action est possible si le joueur possède au moins le
 * nombre de domaines (champ `domains`) nécessaire pour atteindre la valeur
 * seuil (`action_thresholds`) associée.
 */
int can_do_action(struct rules rules, struct player player, const char *domain, const char *action){
    int nb_domain = player.domains[(enum domain) *domain];
    if ( rules.action_thresholds[(enum action) *action] <= nb_domain)
        return 1;
    else
    {
        return 0;
    }
}
