#include <stdio.h>
#include <stdlib.h>

typedef struct treenode * tree;

tree maketree(int n);
tree insert(int v, tree t);
tree newtree(int v);
int check(tree t, int v);
int judge(tree t, int n);
void reset(tree t);
void freetree(tree t);

struct treenode
{
    int v;
    tree left;
    tree right;
    int flag;
};

int main()
{
    int n, i, l;
    tree t;
    scanf("%d", &n);
    while(n)
    {
        scanf("%d", &l);
	t = maketree(n);
	for(i = 0; i < l; i++)
	{
	    if(judge(t, n))
	    {
	        printf("yes\n");
	    }
	    else
	    {
	        printf("no\n");
	    }
	    reset(t);
	}
	freetree(t);
	scanf("%d", &n);
    }
    return 0;
}

tree maketree(int n)
{
    int v, i;
    scanf("%d", &v);
    
    tree t = newtree(v);

    for(i = 1; i < n; i++)
    {
	scanf("%d", &v);
        t = insert(v, t);
    }
    return t;
}

tree insert(int v, tree t)
{
    if( !t )
    {
        t = newtree(v);
    }
    else
    {
        if(v > t->v)
        {
            t->right = insert(v, t->right);
        }
        else
        {
            t->left = insert(v, t->left);
        }
    }
    return t;
}

tree newtree(int v)
{
    tree t;
    t = (tree)malloc(sizeof(struct treenode));
    t->v = v;
    t->left = t->right = NULL;
    t->flag = 0;
    return t;
}

int check(tree t, int v)
{
    if(t->flag)
    {
        if(v > t->v)
	{
	    return check(t->right, v);
	}
	else if(v < t->v)
	{
	    return check(t->left, v);
	}
	else
	{
	    return 0;
	}
    }
    else
    {
        if(v == t->v)
	{
	    t->flag = 1;
	    return 1;
	}
	else
	{
	    return 0;
	}
    }
}

int judge(tree t, int n)
{
    int i, v, flag = 0;  //flag = 0代表目前还一致，flag = 1代表已经不一致
    
    scanf("%d", &v);
    if(v != t->v)
    {
        flag = 1;
    }
    else
    {
        t->flag = 1;
    }

    for(i = 1; i < n; i++)
    {
        scanf("%d", &v);
	if((flag == 0) && (check(t, v) == 0))
        {
	    flag = 1;
	}
    }
    if(flag = 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void reset(tree t)
{
    if(t->left)
    {
	reset(t->left);
    }
    if(t->right)
    {
        reset(t->right);
    }
    t->flag = 0;
}

void freetree(tree t)
{
    if(t->left)
    {
        freetree(t->left);
    }
    if(t->right)
    {
        freetree(t->right);
    }
    free(t);
}
