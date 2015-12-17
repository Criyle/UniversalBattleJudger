# Universal Battle Judger
## General Information
This is a project based on HUSTOJ sandbox. It is used to judge battle programs. It provides automatic compile, judge and error catch.

## Files
### api
api's header

### doc
documents

### judge_client
application

### liquidwar
sample
### ref
refrences

## api
### void stepend(int);
step end
### void judgeend();
judge end
### putresult
#### void putresult(int, char *);
#### void putresult(int, string);
#### void putresult(int, const char *, ...);
output result
### void putscore(int, int);
output scores
### char readchar();
input a single character
### int readint();
input a single number
### void linescan(const char *, ...);
input a single line with format
### writestr
#### void writestr(char *);
#### void writestr(string);
#### void writestr(const char *, ...);
output a string
### void puterr(const char *, ...);
put string to stderr
### void kill(int x)
kill ai_x


## communication protocol
| string     | meaning                             |
| a x        | change prensent ai to number x                       |
| b x        | ai step judge finished                          |
| c n data   | put n words to ai                     |
| d          | read from ai, return formate n + data |
| q          | judge finished                   |
| r n string | ai_n's result string                    |
| s n x      | ai_n's result score                        |
| k x        | kill ai_n                           |

## files definition
### compiler.h&c
class compiler
### judger.h&c
class judger
### okcalls.h
sandbox from hustoj
### runner.h&c
class runner,ai_runner,manager_runner
### judger_client
application

## How to use

'''
  ai::judger *ju = new ai::judger(oj_home, ai_num);
  ju->work();
  
'''
result save to ju


