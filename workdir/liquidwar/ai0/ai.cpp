/**************************************************************
Username: criyle
Language: C++
AIname:   RunAway_v0_68
***************************************************************/
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <map>
#include <cstring>
using namespace std;
 
#define DEBUG
#ifdef DEBUG
#define P(...) fprintf(stderr, __VA_ARGS__ )
#else
#define P(...)
#endif
 
double absfloor(double a)
{
    if (a > 0)
        return floor(a);
    else
        return ceil(a);
}
 
inline int sqr(const int &a)
{
    return a * a;
}
 
struct Vector
{
    int x, y;
 
    Vector(){}
    Vector(const int &_x, const int &_y)
        : x(_x), y(_y){}
 
    friend inline Vector operator+(const Vector &a, const Vector &b)
    {
        return Vector(a.x + b.x, a.y + b.y);
    }
    friend inline Vector operator-(const Vector &a, const Vector &b)
    {
        return Vector(a.x - b.x, a.y - b.y);
    }
 
    inline Vector &operator+=(const Vector &b)
    {
        x += b.x, y += b.y;
        return *this;
    }
    inline Vector &operator-=(const Vector &b)
    {
        x -= b.x, y -= b.y;
        return *this;
    }
 
    inline int len2() const
    {
        return sqr(x) + sqr(y);
    }
 
    friend inline istream& operator>>(istream& in, Vector &v)
    {
        return in >> v.x >> v.y;
    }
    friend inline ostream& operator<<(ostream &out, const Vector &v)
    {
        return out << v.x << " " << v.y;
    }
};
 
inline int dist2(const Vector &a, const Vector &b)
{
    return sqr(a.x - b.x) + sqr(a.y - b.y);
}
 
enum Owner
{
    O_RED, O_BLUE
};
 
const int TOT_GAME_TIME = 1800;
const int MAX_DROPLET_DISP = 30;
const int MAP_WIDTH = 10000;
const int MAP_HEIGHT = 10000;
const int MAKE_DROPLET_TIME = 5;
const int BRAIN_FULL_BLOOD = 5000;
const int DROPLET_FULL_BLOOD = 20;
const int BRAIN_RADIUS = 500;
 
struct Droplet
{
    int id;
    int owner;
    int blood;
    int level;
    Vector pos;
 
    Vector targetPos;
    bool shouldSpread;
    bool isChoosen;
 
    int type;
 
    inline int getShootingDist() const
    {
        return 300 + level * 3;
    }
 
    inline bool canShoot(const Droplet target) const
    {
        return dist2(pos, target.pos) <= sqr(getShootingDist());
    }
};
 
typedef int arr[2010];
arr b,siz;
inline int getf(int x){
    int i = x, j = x, k;
    while (b[i] != i) i = b[i];
    while (b[j] != j){k = b[j]; b[j] = i; j = k;}
    return i;
}
 
inline void merge(int x,int y){
    x = getf(x), y = getf(y);
    if(x == y) return;
    b[x] = y;
    siz[x] += siz[y];
}
 
inline bool cmpByLevel(const Droplet &lhs, const Droplet &rhs)
{
    return lhs.level != rhs.level ? lhs.level < rhs.level : siz[getf(lhs.id)] > siz[getf(rhs.id)];
}
inline bool cmpByBlood(const Droplet &lhs, const Droplet &rhs)
{
    return lhs.blood != rhs.blood ? lhs.blood < rhs.blood : lhs.level > rhs.level;
}
 
Vector redBrainPos(1000, 5000), blueBrainPos(9000, 5000), blueUp(9000, 4500), blueDown(9000,5000);
int redBrainBlood = BRAIN_FULL_BLOOD, blueBrainBlood = BRAIN_FULL_BLOOD;
vector<Droplet> redDroplets, blueDroplets;
int nRedWaitingDroplets = 0, nBlueWaitingDroplets = 0;
int makeDropletRestTime = 1;
int redScore = 0, blueScore = 0;
int restTime = TOT_GAME_TIME;
 
vector<int> choosenIds;
Vector choosenTargetPos;
vector<char> pressedKeys;
 
vector< pair<int, int> > move_shootings;
map<int, Vector> move_poss;
vector<int> move_newDropletsPosY;
 
inline Vector makeOk(const Vector &a)
{
    Vector b = a;
    b.x = min(max(b.x, 0), MAP_WIDTH);
    b.y = min(max(b.y, 0), MAP_HEIGHT);
    return b;
}
 
Vector getNearestCanGo(Vector pos, Vector target)
{
    Vector disp = target - pos;
    if (disp.len2() > sqr(MAX_DROPLET_DISP))
    {
        double z = MAX_DROPLET_DISP / sqrt(disp.len2());
        disp.x = (int)absfloor(disp.x * z);
        disp.y = (int)absfloor(disp.y * z);
    }
    return pos + disp;
}
 
Vector getBestCanGo(Vector pos, Vector target, int id)
{
    Vector disp = target - pos;
    int sd = redDroplets[id].getShootingDist();
    if (disp.len2() < sqr(sd))
        {
            double z = min((double)sd - sqrt(disp.len2()), (double)MAX_DROPLET_DISP) / sqrt(disp.len2());
            disp.x = (int)absfloor(-disp.x * z);
            disp.y = (int)absfloor(-disp.y * z);
        }
    else
        {
            double z = min(sqrt(disp.len2()) - sd + 3, (double)MAX_DROPLET_DISP) / sqrt(disp.len2());
            disp.x = (int)absfloor(disp.x * z);
            disp.y = (int)absfloor(disp.y * z);
        }
     
    return makeOk(pos + disp);
}
 
void loadSituation()
{
    for (vector<Droplet>::iterator it = redDroplets.begin(); it != redDroplets.end(); it++)
        if (it->blood > 0 && dist2(it->pos, blueBrainPos) <= sqr(BRAIN_RADIUS) && blueBrainBlood > 0)
            blueBrainBlood--;
    for (vector<Droplet>::iterator it = blueDroplets.begin(); it != blueDroplets.end(); it++)
        if (it->blood > 0 && dist2(it->pos, redBrainPos) <= sqr(BRAIN_RADIUS) && redBrainBlood > 0)
            redBrainBlood--;
    for (vector< pair<int, int> >::iterator it = move_shootings.begin(); it != move_shootings.end(); it++)
        if (blueDroplets[it->second].blood > 0)
            blueDroplets[it->second].blood--, redDroplets[it->first].level++;
    move_shootings.clear();
 
    int nBlueShootings;
    cin >> nBlueShootings;
    for (int i = 0; i < nBlueShootings; i++)
    {
        int shooter, target;
        cin >> shooter >> target;
        if (redDroplets[target].blood > 0)
            redDroplets[target].blood--, blueDroplets[shooter].level++;
        if (redDroplets[target].blood == 0)
            {
                siz[getf(target)]--;
                if(b[target] == target)
                    {
                        if(siz[target] == 0) continue;
                        int k = -1;
                        for (vector<Droplet>::iterator it = redDroplets.begin(); it != redDroplets.end(); it++)
                            if (it->blood > 0 && getf(it->id) == target)
                                    k = it->id;
                        b[k] = k;
                        siz[k] = siz[target];
                        for (vector<Droplet>::iterator it = redDroplets.begin(); it != redDroplets.end(); it++)
                            if (b[it->id] == target)
                                b[it->id] = k;
                    }
            }
    }
 
    for (map<int, Vector>::iterator it = move_poss.begin(); it != move_poss.end(); it++)
        if (redDroplets[it->first].blood > 0)
            redDroplets[it->first].pos = it->second;
    move_poss.clear();
 
    int nBlueNewPos;
    cin >> nBlueNewPos;
    for (int i = 0; i < nBlueNewPos; i++)
    {
        int id;
        Vector newPos;
        cin >> id >> newPos;
        if (blueDroplets[id].blood > 0)
            blueDroplets[id].pos = newPos;
    }
 
    nRedWaitingDroplets -= (int)move_newDropletsPosY.size();
    for (vector<int>::iterator it = move_newDropletsPosY.begin(); it != move_newDropletsPosY.end(); it++)
    {
        Droplet dr;
        dr.id = (int)redDroplets.size();
        dr.owner = O_RED;
        dr.blood = DROPLET_FULL_BLOOD;
        dr.level = 0;
        dr.pos = Vector(0, *it);
        dr.targetPos = Vector(-1, -1);
        redDroplets.push_back(dr);
    }
    move_newDropletsPosY.clear();
 
    int nBlueNewDropletsPosY;
    cin >> nBlueNewDropletsPosY;
    nBlueWaitingDroplets -= nBlueNewDropletsPosY;
    for (int i = 0; i < nBlueNewDropletsPosY; i++)
    {
        int y;
        cin >> y;
        Droplet dr;
        dr.id = (int)blueDroplets.size();
        dr.owner = O_BLUE;
        dr.blood = DROPLET_FULL_BLOOD;
        dr.level = 0;
        dr.pos = Vector(MAP_WIDTH, y);
        blueDroplets.push_back(dr);
    }
 
    for (vector<Droplet>::iterator it = redDroplets.begin(); it != redDroplets.end(); it++)
        if (it->blood > 0)
        {
            int d = (int)floor(sqrt(dist2(it->pos, blueBrainPos)) / 500);
            redScore += max(8 - d, 0);
        }
    for (vector<Droplet>::iterator it = blueDroplets.begin(); it != blueDroplets.end(); it++)
        if (it->blood > 0)
        {
            int d = (int)floor(sqrt(dist2(it->pos, redBrainPos)) / 500);
            blueScore += max(8 - d, 0);
        }
 
    makeDropletRestTime--;
    if (makeDropletRestTime == 0) {
        nRedWaitingDroplets++;
        nBlueWaitingDroplets++;
        makeDropletRestTime += MAKE_DROPLET_TIME;
    }
 
    restTime--;
 
    int nChoosenIds;
    cin >> nChoosenIds;
    choosenIds.clear();
    for (vector<Droplet>::iterator it = redDroplets.begin(); it != redDroplets.end(); it++)
        if (it->blood > 0)
        {
            it->shouldSpread = false;
            it->isChoosen = false;
        }
    for (int i = 0; i < nChoosenIds; i++)
    {
        int id;
        cin >> id;
        choosenIds.push_back(id);
        redDroplets[id].isChoosen = true;
    }
    cin >> choosenTargetPos;
 
    int nPressedKeys;
    cin >> nPressedKeys;
    pressedKeys.clear();
    for (int i = 0; i < nPressedKeys; i++)
    {
        char key;
        cin >> key;
        pressedKeys.push_back(key);
    }
}
 
void reportShooting(int shooter, int target)
{
    move_shootings.push_back(make_pair(shooter, target));
}
void reportPos(int id, Vector pos)
{
    move_poss[id] = pos;
}
void reportNewDropletsPosY(int y)
{
    move_newDropletsPosY.push_back(y);
}
 
void makeDecision()
{
    vector<Droplet> ourLi, enemyLi;
    for (vector<Droplet>::iterator ourIt = redDroplets.begin(); ourIt != redDroplets.end(); ourIt++)
        if (ourIt->blood > 0)
            ourLi.push_back(*ourIt);
    for (vector<Droplet>::iterator enemyIt = blueDroplets.begin(); enemyIt != blueDroplets.end(); enemyIt++)
        if (enemyIt->blood > 0)
            enemyLi.push_back(*enemyIt);
 
    sort(ourLi.begin(), ourLi.end(), cmpByLevel);
    sort(enemyLi.begin(), enemyLi.end(), cmpByBlood);
    //Recalc
    memset(siz, 0, sizeof(siz));
    for (vector<Droplet>::iterator ourIt = ourLi.begin(); ourIt != ourLi.end(); ourIt++)
        {
            siz[getf(ourIt->id)] += max(0, 1 + ourIt->level / 10 - (int)(sqrt(dist2(redDroplets[getf(ourIt->id)].pos, ourIt->pos)) / MAX_DROPLET_DISP) + ourIt->blood);
        }
     
    for (vector<Droplet>::iterator ourIt = ourLi.begin(); ourIt != ourLi.end(); ourIt++)
    {
        for (vector<Droplet>::iterator enemyIt = enemyLi.begin(); enemyIt != enemyLi.end(); enemyIt++)
            if (enemyIt->blood > 0 && ourIt->canShoot(*enemyIt))
            {
                enemyIt->blood--;
                reportShooting(ourIt->id, enemyIt->id);
                break;
            }
    }
 
    int All = 0, num = 0, tmp = 0, attachX = -1, attachY = -1;
    bool beAttach = false;
    for (vector<Droplet>::iterator enemyIt = enemyLi.begin(); enemyIt != enemyLi.end(); enemyIt++)
        if (enemyIt->pos.x < 3000)
            {
                tmp = 3000 - enemyIt->pos.x;
                All += enemyIt->pos.y * tmp;
                num += tmp;
                if (dist2(enemyIt->pos, redBrainPos) < sqr(800)) beAttach = true , attachX = enemyIt->pos.x, attachY = enemyIt->pos.y;
            }
     
    for (vector<Droplet>::iterator ourIt = ourLi.begin(); ourIt != ourLi.end(); ourIt++)
    {
        Vector All(0,0), tmp(0,0);
        bool canBeShoot = false;
        bool goAhead = false;
        int danger = 0, lanum = -1;
        redDroplets[ourIt->id].targetPos = Vector(-1, -1);
        if(redDroplets[getf(ourIt->id)].blood <= 0)
            {
                lanum = b[ourIt->id];
                b[ourIt->id] = ourIt->id; siz[ourIt->id] = 1 + ourIt->level / 10;
            }
        if (b[ourIt->id] != ourIt->id) continue;
        for (vector<Droplet>::iterator enemyIt = enemyLi.begin(); enemyIt != enemyLi.end(); enemyIt++)
            {
                if (enemyIt->blood > 0 && (sqr(enemyIt->getShootingDist() + MAX_DROPLET_DISP * 2) > dist2(ourIt->pos, enemyIt->pos)))
                    {
                        tmp = (ourIt->pos - enemyIt->pos);
                        tmp.x *= 10;
                        tmp.y *= 10;
                        All += tmp;
                        canBeShoot = true;
                        danger += 1 + enemyIt->level / 10 + enemyIt->blood;
                    }
            }
        if (canBeShoot)
            {
                double t = sqrt(All.len2());
                All.x = (double)(All.x * MAX_DROPLET_DISP / t);
                All.y = (double)(All.y * MAX_DROPLET_DISP / t);
            }
        //Disi
        if (danger > siz[ourIt->id] || (ourIt->pos.x < 3000 && beAttach))
            {
                int found = -1, len = sqr(MAX_DROPLET_DISP * 5);
                if (beAttach) len = sqr(MAX_DROPLET_DISP * 10);
                for (vector<Droplet>::iterator heIt = ourLi.begin(); heIt != ourLi.end(); heIt++)
                    if (b[heIt->id] == heIt->id && (heIt->pos - ourIt->pos).len2() < len && heIt->id != ourIt->id && (lanum == -1 ||lanum != heIt->id)){
                        found = heIt->id, len = (heIt->pos - ourIt->pos).len2();
                    }
                if (found != -1) merge(ourIt->id, found); 
            }
        if (danger <= siz[ourIt->id] - 1)
            {
                goAhead = true;
                int found = -1, len = sqr(MAX_DROPLET_DISP * 10);
                for (vector<Droplet>::iterator enemyIt = enemyLi.begin(); enemyIt != enemyLi.end(); enemyIt++)
                    if (enemyIt->blood > 0 && (enemyIt->pos - ourIt->pos).len2() < len)
                        found = enemyIt->id, len = dist2(enemyIt->pos, ourIt->pos);
                if(found != -1) redDroplets[ourIt->id].targetPos = blueDroplets[found].pos;
            }
        //Go Home
        if (beAttach && ourIt->pos.x < 3000 && danger <= siz[ourIt->id] + 1)
            {
                goAhead = true;
                int found = -1, len = sqr(MAX_DROPLET_DISP * 20);
                for (vector<Droplet>::iterator enemyIt = enemyLi.begin(); enemyIt != enemyLi.end(); enemyIt++)
                    if (enemyIt->blood > 0 && dist2(enemyIt->pos, ourIt->pos) < len && (ourIt->pos.y - 5000) * (ourIt->pos.y - enemyIt->pos.y) > 0)
                        found = enemyIt->id, len = dist2(enemyIt->pos, ourIt->pos);
                if(found != -1) redDroplets[ourIt->id].targetPos = blueDroplets[found].pos;
            }
 
        if (redDroplets[ourIt->id].targetPos.x != -1)
            reportPos(ourIt->id, getBestCanGo(ourIt->pos, redDroplets[ourIt->id].targetPos, ourIt->id));
        else
            {
                if(goAhead)
                    reportPos(ourIt->id, getNearestCanGo(ourIt->pos, blueBrainPos));
                else
                    reportPos(ourIt->id, getNearestCanGo(ourIt->pos, makeOk(ourIt->pos + All)));
            }
    }
    for (vector<Droplet>::iterator ourIt = ourLi.begin(); ourIt != ourLi.end(); ourIt++)
        if (b[ourIt->id] != ourIt->id)
            {
                if(redDroplets[getf(ourIt->id)].targetPos.x == -1)
                    reportPos(ourIt->id, getNearestCanGo(ourIt->pos, getNearestCanGo(ourIt->pos, move_poss[getf(ourIt->id)])));
                else
                    reportPos(ourIt->id, getBestCanGo(ourIt->pos, redDroplets[getf(ourIt->id)].targetPos, ourIt->id));
            }
    if(nRedWaitingDroplets>=1){
        if (beAttach) All = attachY; else if (num) All /= num; else All = 5000 + rand() % 200 - 100;
        int DropPos=5000;
        if (rand() % 3 == 0 || beAttach) DropPos = All;
        else DropPos = 5000 + rand() % 4999 - 2499;
        int sz = redDroplets.size();
        if (sz % 2 && redDroplets[sz - 1].blood > 0) merge(sz, sz - 1);
        for (int i = 0; i < nRedWaitingDroplets; i++)
            if(sz %2 ) reportNewDropletsPosY(move_poss[sz - 1].y);
            else reportNewDropletsPosY(DropPos);
    }
}
 
inline int notOk(const Vector&a)
{
    if(a.x < 0 || a.x > 10000 || a.y < 0 || a.y > 10000) return 1;
    return 0;
}
 
void printMovement()
{
    cout << fixed << setprecision(6);
 
    cout << move_shootings.size() << endl;
    for (vector< pair<int, int> >::iterator it = move_shootings.begin(); it != move_shootings.end(); it++)
        cout << it->first << " " << it->second << endl;
 
    cout << move_poss.size() << endl;
    for (map<int, Vector>::iterator it = move_poss.begin(); it != move_poss.end(); it++)
        {
            if (dist2(redDroplets[it->first].pos, it->second) > 900 || notOk(it->second))
                it->second = redDroplets[it->first].pos;
            cout << it->first << " " << it->second << endl;
        }
 
    cout << move_newDropletsPosY.size() << endl;
    for (vector<int>::iterator it = move_newDropletsPosY.begin(); it != move_newDropletsPosY.end(); it++)
        {
            if (*it < 0 || *it > 10000) *it = 5000;
            cout << *it << endl;
        }
}
 
int main()
{
    srand(173);
 
    for(int i=1; i<=500; i++) b[i] = i , siz[i] = 1;
     
    while (true)
    {
        loadSituation();
        makeDecision();
        printMovement();
    }
 
    return 0;
}