int a;
int b;
a = 10;
b = 20;

{
    int c;
    c = a + b;
    print c;
    {
        int d;
        d = c * 2;
        print d;
    }
}

print a;
