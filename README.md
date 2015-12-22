# Universal Battle Judger
## General Information
This is a project based on HUSTOJ sandbox. It is used to judge battle programs. It provides automatic compile, judge and error catch.

## communication protocol
string      meaning      

a x         change prensent ai to number x                       

b x        ai step judge finished                          

c n data   put n words to ai                     

d           read from ai, return formate n + data 

q           judge finished                   

r n string  ai_n's result string                    

s n x       ai_n's result score                        

k x         kill ai_n                           

## api
void stepend(int);

void judgeend();

putresult

void putresult(int, char *);

void putresult(int, string);

void putresult(int, const char *, ...);

void putscore(int, int);

char readchar();

int readint();

void linescan(const char *, ...);

writestr

void writestr(char *);

void writestr(string);

void writestr(const char *, ...);

void puterr(const char *, ...);

void kill(int x)


## How to use

    ai::judger *ju = new ai::judger(oj_home, ai_num);
    ju->work();
  
result save to ju


