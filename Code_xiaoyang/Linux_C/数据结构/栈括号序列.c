#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stack
{
    char a;
    struct stack * next;
};

struct stack * createstack()
{
    struct stack * s;
    s = (struct stack *)malloc(sizeof(struct stack));
    s->next = NULL;
    return s;
}

int isempty(struct stack * s)
{
    if(s->next == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void push(char x, struct stack* s)
{
    struct stack * p;
    p = (struct stack *)malloc(sizeof(struct stack));
    p->a = x;
    p->next = s->next;
    s->next = p;
}

char front(struct stack * s)
{
    struct stack * p;
    p = s->next;
    return p->a;
}

char pop(struct stack * s)
{
    struct stack * p;
    char r;
    if(isempty(s))
    {
	printf("栈空\n");
        return NULL;
    }
    else
    {
        p = s->next;
	s->next = p->next;
	r = p->a;
	free(p);
	return r;
    }
}

int main()
{
    int n, i;
    char t[20];
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
        scanf("%s", t);
        int b = strlen(t);
	int j;
	struct stack * s = createstack();
	for(j = 0; j < b; j++)
	{
	    if(isempty(s))
	    {
	        push(t[j], s);
	    }
            if(t[j] != front(s))
	    {
	        push(t[j], s);
	    }
            else
	    {
	        pop(s);
	    }
	}
	if(isempty(s))
	{
	    printf("TRUE\n");
	}
	else
	{
	    printf("FALSE\n");
	}
    }
}
