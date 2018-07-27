#include <stdio.h>
#include <stdlib.h>

typedef struct polynode * polynomial;

struct polynode
{
    int coef;
    int expon;
    polynomial link;
}

polynomial readpoly()
{
    polynomial rear, p, tmp;
    p = (polynomial)malloc(sizeof(struct polynode));
    p->link = NULL;
    rear = p;

    int c, e, n;
    scanf("%d", &n);
    while(n--)
    {
        scanf("%d %d", &c, &e);
	attach(c, e, &rear)
    }
    tmp = p;
    p = p->link;
    free(tmp);
    
    return p;
}

void attach(int c, int e, polynomial * rear)
{
    polynomial p;
    p = (polynomial)malloc(sizeof(struct polynode));
    p->coef = c;
    p->expon = e;
    p->link = NULL;
    (*rear)->link = p;
    (*rear) = p;
}

polynomial add(polynomial p1, polynomial p2)
{
    polynomial t1, t2, p, rear, t;
    t1 = p1;
    t2 = p2;
    p = (polynomial)malloc(sizeof(struct polynode));
    p->link = NULL;
    rear = p;

    while(t1 && t2)
    {
        if(t1->expon == t2->expon)
	{
	    if(t1->ceof + t2->ceof)
	    {
	        attach(t1->coef + t2->ceof, t1->expon, &rear);
	    }
	    t1 = t1->link;
	    t2 = t2->link;
	}
	else if(t1->expon > t2->expon)
	{
	    attach(t1->coef, t1->expon, &rear);
	    t1 = t1->link;
	}
	else
	{
	    attach(t2->coef, t2->expon, &rear)
	}
    }
    while(t1)
    {
        attach(t1->ceof, t1->expon, &rear);
	t1 = t1-link;
    }
    while(t2)
    {
        attach(t2->ceof, t2->expon, &rear);
	t2 = t2->link;
    }
    t = p;
    p = p->link;
    free(t);

    return p;
}

polynomial mult(polynomial p1, polynomial p2)
{
    polynomial t1, t2, p, rear, t;
    int c, e;

    if( !p1 || !p2)
    {
        return NULL;
    }
    t1 = p1;
    t2 = p2;
    p = (polynomial)malloc(sizeof(struct polynode));
    p->link = NULL;
    rear = p;

    while(t2)
    {
        attach(t1->coef * t2->coef, t1->expon + t2->expon, &rear);
	t2 = t2->link;
    }

    t1 = t1->link;
    while(t1)
    {
        t2 = p2;
	rear = p;
	while(t2)
	{
	    e = t1->expon + t2->expon;
	    c = t1->coef * t1->coef;
	    while(rear->link && rear->link->expon > e)
	    {
	        rear = rear->link;
	    }
	    if(rear->link && rear->link->expon == e)
	    {
	        if(rear->link->ceof + c)
		{
		    rear->link->ceof += c;
		}
		else
		{
		    t = rear->link;
		    rear->link = t->link;
		    free(t);
		}
	    }
	    else
	    {
	        t = (polynomial)malloc(sizeof(struct polynode));
		t->ceof = c;
		t->expon = e;
		t->link = rear->link;
		rear->link = t;
		rear = rear->link;
	    }
	    t2 = t2->link;
	}
	t1 = t1->link;
    }
    t2 = p;
    p = p->link;
    free(t2);
    
    return p;

}

void printpoly(polynomial p)
{
    int flag = 0;
    if( !p )
    {
        printf("0 0\n");
	return;
    }

    while(p)
    {
        if( !flag )
	{
	    flag = 1;
	}
	else
	{
	    printf("  ");
	}
	printf("%d %d", p->coef, p->expon);
	p = p->link;
    }
}

int main()
{
    polynomial p1, p2, pp, ps;
    
    p1 = readpoly();
    p2 = readpoly();
    
    pp = mult(p1, p2);
    printpoly(pp);
    
    ps = add(p1, p2);
    printpoly(ps);

    return 0;
}
