#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "TreeDot.h"



struct Node_t
{
    char     data;
    bool     number;
    Node_t * left;
    Node_t * right;
};


Node_t *    newNode             (const char newData, Node_t * leftNode, Node_t * rightNode);
void        delNode             (Node_t ** node);
int         NodeIsOk            (Node_t * node);
void        NodePrint           (Node_t * node, FILE * out);
void        NodePrintWithAdress (Node_t * node, FILE * out);
void        NodeDump            (Node_t * node);



int main ()
{
    Node_t * nd = newNode ('*',  newNode ('9', NULL, newNode ('!', NULL, NULL)), NULL);
    assert (errno == 0);
    FILE * out = fopen ("TreePrintWithAdress", "wb");
    assert (out);
    NodePrintWithAdress (nd, out);
    fclose (out);
//
//    NodeDump (nd);
//
//    delNode (&nd);

//    size_t treeSize = 0;
//    tdCountTreeSize ((tdNode *) nd, &treeSize);
    tdWrite ("TreePrintWithAdress", "out.dot");
//    printf ("%d\n", treeSize);

    return 0;

}



Node_t * newNode (const char newData, Node_t * leftNode, Node_t * rightNode)
{
    // don't use assert () for leftNode and rightNode. It's correct
    errno = 0;
    Node_t * node = (Node_t *) calloc (1, sizeof (Node_t));
    if (errno == ENOMEM)
    {
        printf ("ERROR: ENOMEM in newNode\n");
        abort  ();
    }

    node->data   = newData;
    node->left   = leftNode;
    node->right  = rightNode;
    node->number = ! ((int) leftNode + (int) rightNode);

    assert (node);
    return node;
}



void delNode (Node_t ** node)
{
    assert (node);
    assert (*node);

    if ((*node)->left)
        delNode (&(*node)->left);
    if ((*node)->right)
        delNode (&(*node)->right);

    (*node)->data   = 0;
    (*node)->number = 0;
    (*node)->left   = 0;
    (*node)->right  = 0;

    free (*node);

    *node = NULL;
}



int NodeIsOk (Node_t * node)
{
    return  node                                                     \
            && node         != node->left                            \
            && node         != node->right                           \
            && (node->left  != NULL ? node->left != node->right : 1) \
            && (node->left  != NULL ? NodeIsOk (node->left)     : 1) \
            && (node->right != NULL ? NodeIsOk (node->right)    : 1) ;
    ;
}



void NodePrint (Node_t * node, FILE * out)
{
    assert (NodeIsOk (node));

    fprintf (out, " (");
    if (node->left)
        NodePrint (node->left, out);
    fprintf (out, " %c )", node->data);
    if (node->right)
        NodePrint (node->right, out);
}



void NodePrintWithAdress (Node_t * node, FILE * out)
{
    assert (NodeIsOk (node));

    fprintf (out, "(");
    if (node->left)
        NodePrintWithAdress (node->left, out);
    else
        fprintf (out, "(0 - 0)");
    fprintf (out, "%d %c %d", node->left, node->data, node->right);
    if (node->right)
        NodePrintWithAdress (node->right, out);
    else
        fprintf (out, "(0 - 0)");
    fprintf (out, ")");

}



void NodeDump (Node_t * node)
{
    if ( ! NodeIsOk (node))
    {
        printf ("ERROR: something gone bad with node. Hi from NodeIsOk");
        abort ();
    }

}
