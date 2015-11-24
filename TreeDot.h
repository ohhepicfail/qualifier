#ifndef TREEDOT_H_INCLUDED
#define TREEDOT_H_INCLUDED

#include <ctype.h>


#define MAX_DATA    256
#define INIT_NUMBER 2

struct tdNode
{
    char data;
    bool number;
    int  left;
    int  right;
};


struct tdStack
{
    int * nodeNumber;
    int   stackSize;
    int   cur;
};


void        tdWrite         (char filename[], tdNode ** node, size_t sz);
int         tdNodeIsOk      (tdNode * node);
void        tdCountTreeSize (tdNode * tree, size_t * treeSize);
tdStack *   tdCreateStack   ();
void        tdDeleteStack   (tdStack ** stk);
size_t      tdStackResize   (tdStack * stk);
void        tdPush          (tdStack * stk, int number);
int         tdPop           (tdStack * stk);



void tdWrite (const char filenamein[], const char filenameout[])
{
    FILE * in  = fopen (filenamein , "r");
    FILE * out = fopen (filenameout, "w");
    assert (in);
    assert (out);

    fseek (in, 0, SEEK_END);
    size_t treeSize = ftell (in);
    assert (treeSize > 0);

    char * tree = (char *) calloc (treeSize, sizeof (char));
    assert (tree);

    fseek (in, 0, SEEK_SET);
    assert (treeSize + 1 != fread (tree, sizeof (char), treeSize, in));
    const char * treeBegin = tree;

    fclose (in);

    fprintf (out, "digraph tree {\n");
    fprintf (out, "\tnode [shape = record,height=.1];\n");

    tdStack * nodeNumbers = tdCreateStack ();
    for (int nodeCounter = 0; *tree;)
    {
        if (isdigit (*tree))
        {
            int left =  *(tree++) - '0';
            while (isdigit (*tree))
                left = left * 10 + (int) *(tree++) - '0';
            tree++;

            char * data = (char *) calloc (MAX_DATA, sizeof (char));
            assert (data);
            const char * bdata = data;
            while (*tree != ' ')
                *(data++) = *(tree++);
            ++tree;
            data = (char *) bdata;

            int right = (int) *(tree++) - '0';
            while (isdigit (*tree))
                right = right * 10 + (int) *(tree++) - '0';
            tree--;
            printf ("%d\t%s\t%d\n", left, data, right);
            fprintf (out, "\tnode%d[label = \"<f0> %d | <f1> %s | <f2> %d \"];\n", tdPop (nodeNumbers), left, data, right);
            free (data);
        }
        else if (*tree == '(')
        {
            tdPush (nodeNumbers, nodeCounter);
            nodeCounter++;
        }
        tree++;
    }

    tree = (char *) treeBegin;
    tdDeleteStack (&nodeNumbers);
    nodeNumbers = tdCreateStack ();

    for (int nodeCounter = 0; *tree;)
    {
        if (*tree == '(')
        {
            tdPush (nodeNumbers, nodeCounter);
            nodeCounter++;
        }
        else if (*tree == ')' && *(tree + 1))
        {
            const int bottom = tdPop (nodeNumbers);
            const int top    = tdPop (nodeNumbers);
            tdPush (nodeNumbers, top);

            fprintf (out, "\tnode%d -> node%d;\n", top, bottom);
        }
        tree++;
    }
    fprintf (out, "}");

    tdDeleteStack (&nodeNumbers);
    fclose (out);
    free (tree);
}



int tdNodeIsOk (tdNode * node)
{
    return  node                                                                                        \
            && node         != (tdNode *) node->left                                                    \
            && node         != (tdNode *) node->right                                                   \
            && ((tdNode *) node->left  != NULL ? (tdNode *) node->left != (tdNode *) node->right : 1)   \
            && ((tdNode *) node->left  != NULL ? tdNodeIsOk ((tdNode *) node->left)     : 1)            \
            && ((tdNode *) node->right != NULL ? tdNodeIsOk ((tdNode *) node->right)    : 1) ;
;
}



void tdCountTreeSize (tdNode * tree, size_t * treeSize)
{
    if (tree)
    {
        (*treeSize)++;
        if (tree->left)
            tdCountTreeSize ((tdNode *) tree->left, treeSize);
        if (tree->right)
            tdCountTreeSize ((tdNode *) tree->right, treeSize);
    }
}



tdStack * tdCreateStack ()
{
    tdStack * stk = (tdStack *) calloc (1, sizeof (tdStack));
    assert (stk);

    stk->nodeNumber = (int *) calloc (INIT_NUMBER, sizeof (int));
    assert (stk->nodeNumber);
    stk->stackSize  = INIT_NUMBER;
    stk->cur        = 0;

    assert (stk);
    return stk;
};



void tdDeleteStack (tdStack ** stk)
{
    assert (stk);
    assert (*stk);

    (*stk)->cur         = -1;
    (*stk)->stackSize   = -1;
    free ((*stk)->nodeNumber);
    free ( *stk);
}



size_t tdStackResize (tdStack * stk)
{
    assert (stk);

    stk->stackSize *= INIT_NUMBER;
    stk->nodeNumber = (int *) realloc (stk->nodeNumber, stk->stackSize * sizeof (stk->nodeNumber));

    return (size_t) stk->nodeNumber;
}



void tdPush (tdStack * stk, int number)
{
    assert (stk);

    if (stk->cur + 1 >= stk->stackSize)
        stk->stackSize = tdStackResize (stk);

    stk->nodeNumber[stk->cur] = number;
    stk->cur++;
}



int tdPop (tdStack * stk)
{
    assert (stk);

    stk->cur--;
    return stk->nodeNumber[stk->cur];
}


#endif // TREEDOT_H_INCLUDED
