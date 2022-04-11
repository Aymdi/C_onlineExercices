/**
 * Important : ne changez PAS ni les structures, ni les defines, ni les
 * prototypes, ni les variables globales  au risque d'avoir tous les exercices
 * faux.
 *
 * Bien que les `const` aient été mis pour vous aider, s'ils vous gènent vous
 * pouvez les retirer des prototypes.
 *
 * EXPERIMENTAL:
 *  Cet exercice est un test de la nouvelle infrasctucture de test unitaire.
 * Les messages d'erreurs ont étés améliorés même en cas de segfault.
 * Cependant comme pour tout projet exerimental son comportement peut être
 * erratique. Merci de me faire remonter tous les bogues (comportements
 * étranges, message pas téribles).
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Nous souhaitons réaliser une application de gestion de cocktail.
 * Pour cela nous définissons les structures suivantes :
 * - struct product;
 * - struct recipe;
 * - struct ingredient;
 *
 * Remarque :
 *
 * - Si une fonction ne compile pas, commentez là (et revenez-y plus tard, s'il
 *   vous reste du temps) ! Tout fichier qui ne compile pas ne sera pas
 *   regardé.
 *
 * - Si vous ne parvenez pas a faire une fonction, vous pouvez tout de même
 *   l'utiliser (comme si vous l'aviez faîte), nous vous fournissons une
 *   version fonctionelle.
 *
 * Barème indicatif :
 * - 1 pt/fonction :
 *   product_by_name, cost, most_profitable, name_recipe, print_recipe, 
 *   recipe_contains, can_mix_cocktail, recipes_available
 * - 2 pt/fonction :
 *   merge_ingredient, mix_cocktail, cocktails_with, concat, sneaky_bartender,
 *   program
 **/

#define MAX_INGREDIENTS 10

struct product {
  int id;           // `id` correspond à son indice dans le stock.
  const char *name; // Le nom d'un produit
  int quantity;     // La quantité disponible dans le stock
  int unit_price;   // Le prix unitaire d'un produit
};

struct ingredient {
  int product_id; // `product_id` correspond à son indice dans le stock.
  int quantity;   // La quantité nécessaire d'un produit pour réaliser
                  // le cocktail
};

struct recipe {
  const char *name; // Le nom d'un cocktail
  int price;        // Le prix de vente d'un cocktail
  int ingredient_count; // Le nombre d'ingredients
  struct ingredient ingredients[MAX_INGREDIENTS]; // La liste des ingredients
};

// Le stock de produits et le nombre d'élements du stock
#define STOCK_LENGTH 12
extern struct product stock[STOCK_LENGTH];

// L'ensemble des cocktails disponibles et le nombre de cocktails
#define MENU_LENGTH 5
extern struct recipe menu[MENU_LENGTH];

/**
 * Cette fonction retourne un pointeur sur un produit du stock nommé
 * `name`.  S'il n'existe pas de tel produit, elle retourne NULL.
 */
struct product *product_by_name(const char *name){
    for (int i=0; i<STOCK_LENGTH; i++){
        if ((stock[i]).name == name)
            return(&stock[i]);
    }
    return NULL;
}

/**
 * Cette fonction retourne le coût d'un `cocktail`. 
 * Le coût est la somme des coûts unitaires de chaque produit
 * multiplié par le quantité d'un produit dans la recette
 */
int cost(struct recipe cocktail){
    int cost=0;
    for (int i=0; i<cocktail.ingredient_count; i++){
        for (int j=0; j<STOCK_LENGTH; j++){
            if ( (cocktail.ingredients[i]).product_id == stock[j].id ){
                 cost += ((cocktail.ingredients[i]).quantity)*(stock[j].unit_price);
            }
        }  
    }
    return cost;
}

/**
 * Retourne la recette la plus rentable parmi le tableau `list` de taille
 * `size`, i.e. celle où la différence entre le prix et le coût est la plus
 * élevée.
 * Cette fonction retourne NULL si la liste est vide;
 *
 * Attention: on est autorisé à vendre à perte.
 */ 
const struct recipe *most_profitable(int size, const struct recipe list[]){
    if (size == 0)
        return NULL;
    signed int t_diff[size];
    for (int i=0; i<size; i++){
        t_diff[i] = (list[i].price)-(cost(list[i]));
    }
    int i_max=0;
    for (int i=1; i<size; i++){
        if (t_diff[i] > t_diff[i_max]){
            i_max = i;
        }
    }
    return (&list[i_max]);
}

/**
 * Affiche une recette sous la forme :
 * `<Nom> (<Prix>e): <Ing1> (<Qty1>), <Ing2> (<Qty2>), ...`
 * Remarque: Il n'y a pas de virgule inutile en fin de liste d'ingrédients
 * Exemple:
 * Sex on the beach (3e): Vodka (3), Peach (2), Chambord (2), Pineapple (5), Cramberry (6)
 */
void print_recipe(struct recipe cocktail){
    printf("%s (%de):",cocktail.name,cocktail.price);
    for (int i=0; i<cocktail.ingredient_count; i++){
        for (int j=0; j<STOCK_LENGTH; j++){
            if ((product_by_name(stock[j].name))->id == (cocktail.ingredients[i]).product_id ){
                printf(" %s (%d)",stock[j].name, (cocktail.ingredients[i]).quantity);
            }
        }
        if (i != cocktail.ingredient_count-1)
            printf("%s", ",");
    }
}

/**
 * Si la recette `cocktail` contient un ingrédient de même `id` que
 * `product_id`, retourne l'adresse de cet ingrédient, sinon NULL.
 */
struct ingredient *recipe_contains(struct recipe *cocktail, int product_id){
    for (int i=0; i<cocktail->ingredient_count; i++){
        if ((cocktail->ingredients[i]).product_id == product_id){
            return(&(cocktail->ingredients[i]));
        }
    }
    return NULL;
}

/**
 * Rajoute l'`ingredient` à la recette `cocktail`. Si la recette contenait
 * déjà un tel produit, la quantité est rajoutée à la précédente.
 * 
 * Remarque: Attention à MAX_INGREDIENTS
 */
void merge_ingredient(struct recipe *cocktail, const struct ingredient ingredient){
    if (recipe_contains(cocktail,ingredient.product_id) != NULL){
        (recipe_contains(cocktail,ingredient.product_id)->quantity) += ingredient.quantity;
    }
    else
    {
        if (cocktail->ingredient_count < MAX_INGREDIENTS){
            cocktail->ingredients[cocktail->ingredient_count] = ingredient;
            cocktail->ingredient_count++;
        }
    }
}

/**
 * Retourne 1 si le `cocktail` est réalisable compte tenu des stocks donnés.
 * i.e. il y a tous les produits nécessaires en quantités suffisantes. Sinon
 * cette fonction retourne 0.
 */
int can_mix_cocktail(struct recipe cocktail){
    for (int i=0; i<cocktail.ingredient_count; i++){
        for (int j=0; j<STOCK_LENGTH; j++){
            if ( (cocktail.ingredients[i]).product_id == stock[j].id){
                if ( (cocktail.ingredients[i]).quantity > stock[j].quantity )
                    return 0;
            }
        }
    }
    return 1;
}

/**
 * Cette fonction réalise un `cocktail`, i.e. soustrait du stock les
 * quantités nécessaires pour le réaliser.
 *
 * Cette fonction ne prépare le cocktail que s'il est réalisable.
 *
 * Remarque: Cette fonction n'a pas encore de test
 */
void mix_cocktail(struct recipe cocktail){
    if (can_mix_cocktail(cocktail)){
        for (int i=0; i<cocktail.ingredient_count; i++){
            for (int j=0; j<STOCK_LENGTH; j++){
                if ( (cocktail.ingredients[i]).product_id == stock[j].id ){
                    (stock[j].quantity) -= (cocktail.ingredients[i]).quantity;
                }
            }
        }
    }
}

/**
 * Remplit dans `cocktails_left`, la liste des cocktails encore réalisables
 * avec le stock actuel.
 * Cette fonction retourne le nombre d'élements présents dans `cocktail_left`
 */
int recipes_available(int size, const struct recipe cocktails[],struct recipe cocktails_left[]){
    int size_left=0;
    for (int i=0; i<size; i++){
        if (can_mix_cocktail(cocktails[i])){
            cocktails_left[size_left] = cocktails[i];
            size_left++;
        }
    }
    return size_left;
}

/**
 * Remplit dans `proposals`, la liste des `cocktails` (de taille
 * `cocktails_count`) contenant au moins tous les produits spécifiés par
 * `products_ids` (de taille `products_count`). La fonction retourne le
 * nombre d'éléments présents dans `proposal`.
 */
int cocktails_with(int products_count, const int products_ids[],
    int cocktails_count, struct recipe cocktails[],
    struct recipe proposals[]){
    int size_proposals=0;
    for (int i=0; i<cocktails_count; i++){
        int c=0;
        for (int j=0; j<products_count; j++){
            if (recipe_contains(&cocktails[i],products_ids[j]) == NULL){
                break;
            }
            c++;
        }
        if (c == products_count){
            proposals[size_proposals] = cocktails[i];
            size_proposals++;
        }
    }
    return size_proposals;
}

/**
 * Retourne le nom recette la plus rentable contenant au moins tous les produits
 * `product_ids` (de taille `products_count`) parmis les recettes proposées
 * `cocktails` de taille cocktails_count, encores réaliasables.
 * S'il n'y a pas de solution cette fonction retoune NULL.
 */
const char *sneaky_bartender(
    int products_count, const int products_ids[],
    int cocktails_count, struct recipe cocktails[]){
    struct recipe proposals[cocktails_count];
    int size_proposals = cocktails_with(products_count, products_ids,cocktails_count, cocktails,proposals);
    if ( size_proposals > 0){
        struct recipe proposals_left[size_proposals];
        int size_proposals_left = recipes_available(size_proposals,proposals, proposals_left);
        return ( most_profitable(size_proposals_left,proposals_left)->name );
    }
    return NULL;
}

/**
 * Concatène les `n` chaînes de caractères contenues dans
 * `strs` dans `result` et retourne la longueur de la chaîne
 * ainsi créée.
 * Remarque:
 *  - `result` est supposé assez grand pour contenir le résultat.
 *  - L'utilisation de strcat est interdite (car inutile)
 */
int concat(int n, const char *strs[], char *result){
    int size= strlen(result);
    for (int i=0; i<n; i++){
        result[size+i] = strs[i];
    }
    return ("%d",strlen(result));
}

/**
 * Cette fonction fabrique un nom pour la recette `cocktail`, le nom sera
 * retourné via `name`. Le nom proposé est la concaténation séparé par des
 * `-` de tous les noms de produits contenu dans la recette (et dans
 * l'ordre de la recette).
 * Eg. { { Vodka, 3}, { Orange, 6 } } => Vodka-Orange
 */
void name_recipe(struct recipe *cocktail, char name[]);

/**
 * On se propose de faire un programme qui affiche la liste des cocktails
 * contenant tous les noms de produits passés en arguments encores
 * réalisables.
 * Le cocktail le plus rentable sera préfixé d'une étoile `*`.
 * Chaque cocktail sera affiché seul sur sa ligne.
 * La valeur retourné sera le nombre de cocktails affichés.
 *
 * Remarque: Dans cette fonction, si vous voulez debugger à l'aide de `printf`
 * utilisez fprintf(stderr, ....) à la place.
 */
int program(int argc, const char *argv[]);