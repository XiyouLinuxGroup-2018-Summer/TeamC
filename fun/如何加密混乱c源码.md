# 分享一个有趣的博客内容
## 开始程序
下面是一个找出素数的程序
```c
void primes(int cap)
{
    int i, j, composite;
 
    for(i = 2; i < cap; ++i) {
        composite = 0;
        for(j = 2; j * j < i; ++j) {
            composite += !(i % j);
        }
        if(!composite){
            printf("%dt", i);
        }
    }
}
 
int main()
{
    primes(100);
}
```
下面我们来看看如何把上面这段代码搞得复杂难懂。
## 第一步、把for变成while
通常来说，for循坏要以while循坏简单一些，上面的程序有二重for循环，我们不但要把其变成while循环，而且还要把二重循环的变成一重的循环，然后使用大量的if-else语句来判断。
```c
void primes(int cap)
{
    int i, j, composite, t = 0;
 
    while(t < cap * cap) {
        i = t / cap;
        j = t++ % cap;
        if(i <= 1);
        else if(!j)
            composite = j;
        else if(j == i && !composite)
            printf("%dt",i);
        else if(j > 1 && j < i)
            composite += !(i % j);
    }
}
 
int main()
{
    primes(100);
}
```
## 第二步，把循坏变成递归
递归在某些时候是可以把代码变得简单，但大多数的情况下是把代码变得复杂，而且很没有效率。下面是把上面的while循环变成了递归。变成了递归后，函数的参数都变成3个了。
```c
void primes(int cap, int t, int composite)
{
    int i,j;
    i = t / cap;
    j = t % cap;
    if(i <= 1)
        primes(cap,t+1,composite);
    else if(!j)
        primes(cap,t+1,j);
    else if(j == i && !composite)
        (printf("%dt",i), primes(cap,t+1,composite));
    else if(j > 1 && j < i)
        primes(cap,t+1, composite + !(i % j));
    else if(t < cap * cap)
        primes(cap,t+1,composite);
}
 
int main()
{
    primes(100,0,0);
}
```
## 第三步，弄乱代码结构/使用没有含义的变量名
关于如何弄乱代码结构，其中一个小技巧是，使用“？”表达式代替if-else语句。
```c
void primes(int m, int t, int c)
{
    int i,j;
    i = t / m;
    j = t % m;
    (i <= 1) ? primes(m,t+1,c) : (!j) ? primes(m,t+1,j) : (j == i && !c) ?
    (printf("%dt",i), primes(m,t+1,c)) : (j > 1 && j < i) ?
    primes(m,t+1,c + !(i % j)) : (t < m * m) ? primes(m,t+1,c) : 0;
}
 
int main()
{
    primes(100,0,0);
}
```
## 第四步，取消临时变量
临时变量一般用来保存反复使用的一个表达式的值。使用大量重复的表达式来取消这些临时变量的也可以让代码复杂起来。
```c
void primes(int m, int t, int c)
{
  ((t / m) <= 1) ? primes(m,t+1,c) : !(t % m) ? primes(m,t+1, t % m) :
  ((t % m)==(t / m) && !c) ? (printf("%dt",(t / m)), primes(m,t+1,c)) :
  ((t % m)> 1 && (t % m) < (t / m)) ? primes(m,t+1,c + !((t / m) % (t % m))) :
  (t < m * m) ? primes(m,t+1,c) : 0;
}
 
int main()
{
    primes(100,0,0);
}
```
## 第五步，继续弄乱变量名
我们知道，下划线是合法的变量名，所以，我们不妨用__，___，____来代替m，t，c。函数名也可以使用下划线来代替。让我们来看看求素数的函数能变成什么。
```c
void _(int __, int ___, int ____)
{
    ((___ / __) <= 1) ? _(__,___+1,____) : !(___ % __) ? _(__,___+1,___ % __) :
    ((___ % __)==(___ / __) && !____) ? (printf("%dt",(___ / __)),
    _(__,___+1,____)) : ((___ % __) > 1 && (___ % __) < (___ / __)) ?
    _(__,___+1,____ + !((___ / __) % (___ % __))) : (___ < __ * __) ?
    _(__,___+1,____) : 0;
}
 
int main()
{
    _(100,0,0);
}
```
## 第六步，移除常量
在上面的程序中，还有一些常量，你可以通过增加一个宏定义，或是增加一个函数的形参来取代这一常量。
```c
void _(int __, int ___, int ____, int _____)
{
    ((___ / __) <= _____) ? _(__,___+_____,____,_____) : !(___ % __) ? _(__,___+_____,___ % __, _____) :
    ((___ % __)==(___ / __) && !____) ? (printf("%dt",(___ / __)),
    _(__,___+_____,____,_____)) : ((___ % __) > _____ && (___ % __) < (___ / __)) ?
    _(__,___+_____,____,_____ + !((___ / __) % (___ % __))) : (___ < __ * __) ?
    _(__,___+_____,____,_____) : 0;
}
 
int main() {
    _(100,0,0,1);
}
```
**“吼哈哈～相信大家都有把源代码弄复杂的潜质”**
[原文出处](https://coolshell.cn/articles/933.html "如何加密/混乱C源代码")
