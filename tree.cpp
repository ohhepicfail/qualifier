#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <locale.h>
#include <windows.h>
#include "TreeDot.h"


#define MAX_DATA_SIZE   256
#define QUIZ( tree )    printf ("\t\t***QUIZ***\n"); quiz (tree);


struct Node_t
{
    char *   data;
    Node_t * left;
    Node_t * right;
};


Node_t *    newNode             (const char * newData, Node_t * leftNode, Node_t * rightNode);
void        TreeDel             (Node_t ** node);
int         TreeIsOk            (Node_t * node);
void        TreePrint           (Node_t * node, FILE * out);
void        TreePrintWithAdress (Node_t * node, FILE * out);
void        TreeDump            (Node_t * node, const char * filename);
Node_t *    TreeLoad            (const char * filename);
Node_t *    loadNextNode        (char ** tree);
Node_t *    quiz                (Node_t * tree);



int main ()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    const char dumpFilename[] = "dump";
    const char saveFilename[] = "save";

//    Node_t * nd = newNode ("aha",  newNode ("столовка", NULL, newNode ("ops", NULL, NULL)), NULL);
    Node_t * nd = TreeLoad (saveFilename);

    QUIZ (nd);

//    TreePrint  (nd, stdout);

    TreeDump (nd, dumpFilename);

    FILE * out = fopen (saveFilename, "w");
    assert (out);
    TreePrint (nd, out);
    fclose (out);

    TreeDel (&nd);

    return 0;

}



Node_t * newNode (const char * newData, Node_t * leftNode, Node_t * rightNode)
{
    assert (newData);
    // don't use assert () for leftNode and rightNode. It's correct

    Node_t * node = (Node_t *) calloc (1, sizeof (Node_t));
    assert (node);

    node->data = (char *) calloc (strlen (newData) + 1 , sizeof (char));
    assert (node->data);
    strcpy (node->data, newData);
    node->left   = leftNode;
    node->right  = rightNode;

    assert (TreeIsOk (node));
    return node;
}



void TreeDel (Node_t ** node)
{
    assert (node);
    assert (*node);

    if ((*node)->left)
        TreeDel (&(*node)->left);
    if ((*node)->right)
        TreeDel (&(*node)->right);

    free ((*node)->data);
    (*node)->data = NULL;
    free (*node);
    *node = NULL;
}



int TreeIsOk (Node_t * node)
{
    return  node                                                     \
            && node         != node->left                            \
            && node         != node->right                           \
            && (node->left  != NULL ? node->left != node->right : 1) \
            && (node->left  != NULL ? TreeIsOk (node->left)     : 1) \
            && (node->right != NULL ? TreeIsOk (node->right)    : 1) ;
}



void TreePrint (Node_t * node, FILE * out)
{
    assert (TreeIsOk (node));

    fprintf (out, "(");
    if (node->left)
        TreePrint (node->left, out);
    fprintf (out, "%s", node->data);
    if (node->right)
        TreePrint (node->right, out);
    fprintf (out, ")");
}



void TreePrintWithAdress (Node_t * node, FILE * out)
{
    assert (TreeIsOk (node));

    fprintf (out, "(");
    if (node->left)
        TreePrintWithAdress (node->left, out);
    else
        fprintf (out, "(0_-_0)");
    fprintf (out, "%d_ %s_%d", node->left, node->data, node->right);
    if (node->right)
        TreePrintWithAdress (node->right, out);
    else
        fprintf (out, "(0_-_0)");
    fprintf (out, ")");

}



void TreeDump (Node_t * node, const char * filename)
{
    assert (filename);
    if ( ! TreeIsOk (node))
    {
        printf ("ERROR: something gone bad with node. Hi from dump");
        abort ();
    }

    FILE * out = fopen (filename, "w");
    assert (out);
    TreePrintWithAdress (node, out);
    fclose (out);

    tdWrite (filename);
}



Node_t * TreeLoad (const char * filename)
{
    printf ("***loading tree...");
    assert (filename);

    FILE * in = fopen (filename, "r");
    assert (in);

    fseek (in, 0, SEEK_END);
    size_t treeSize = ftell (in);
    assert (treeSize > 0);
    treeSize++;
    char * tree = (char *) calloc (treeSize, sizeof (char));
    assert (tree);

    fseek (in, 0, SEEK_SET);
    assert (treeSize + 1 != fread (tree, sizeof (char), treeSize, in));
    Node_t * newTree = loadNextNode (&tree);

    fclose (in);
    free (tree);
    printf ("correctly completed\n");
    return newTree;
}



Node_t * loadNextNode (char ** tree)
{
    Node_t * left = NULL;
    if (**tree == '(')
    {
        (*tree)++;
        left = loadNextNode (tree);
    }
    if (strlen (*tree) == 0)
        return left;

    char * data = (char *) calloc (MAX_DATA_SIZE, sizeof (char));
    assert (data);
    const char * dataBegin = data;
    while (**tree != '(' && **tree != ')')
        *(data++) = *((*tree)++);
    data = (char *) dataBegin;

    Node_t * right = NULL;
    if (**tree == '(')
    {
        (*tree)++;
        right = loadNextNode (tree);
    }
    if (strlen (*tree) != 0)
        (*tree)++;

    Node_t * newND = newNode (data, left, right);
    free (data);

    return newND;
}



Node_t * quiz (Node_t * tree)
{
    const char yes[] = "да";
    const char  no[] = "нет";

    printf ("Это %s?\n", tree->data);
    char * answer = (char *) calloc (MAX_DATA_SIZE, sizeof (char));
    scanf ("%s", answer);
    while ( strcmp (answer, yes) && strcmp (answer, no))
    {
        printf ("это не ответ! Введите %s или %s\n", yes, no);
        scanf  ("%s", answer);
    }

    if ( ! strcmp (answer, yes))
    {
        if (tree->left)
            quiz (tree->left);
        else
        {
            printf ("Я так и знал!\n");
        }
    }
    else
    {
        if (tree->right)
            quiz (tree->right);
        else
        {
            printf ("Я не знаю, что это. Добавить ответ?\n");
            scanf ("%s", answer);
            while ( strcmp (answer, yes) && strcmp (answer, no))
            {
                printf ("это не ответ! Введите %s или %s\n", yes, no);
                scanf  ("%s", answer);
            }
            if ( ! strcmp (answer, yes))
            {
                printf ("Введите определение\n");
                scanf  ("%s", answer);
                tree->right = newNode (answer, NULL, NULL);
            }
        }
    }

    return tree;
}


