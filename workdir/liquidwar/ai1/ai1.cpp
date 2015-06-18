#include <iostream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <map>
#include <list>
using namespace std;
const int inf = 1 << 28;

inline void maxer (int &a, int x)
{
    if(a < x)  a = x;
}
inline double rand_theta ()
{
    return (double)rand() / RAND_MAX * 2 * acos(-1);
}
inline double absfloor(double a)
{
    return a > 0? floor(a): ceil(a);
}
inline int sqr(int a)
{
    return a * a;
}

struct Vector
{
    int x, y;

    Vector(){}
    Vector(const int &_x, const int &_y)
        : x(_x), y(_y){}

    friend bool operator == (const Vector &a, const Vector &b)
    {
        return a.x == b.x && a.y == b.y;
    }

    friend Vector operator + (const Vector &a, const Vector &b)
    {
        return Vector(a.x + b.x, a.y + b.y);
    }
    friend Vector operator - (const Vector &a, const Vector &b)
    {
        return Vector(a.x - b.x, a.y - b.y);
    }

    Vector& operator += (const Vector &b)
    {
        x += b.x, y += b.y;
        return *this;
    }
    Vector& operator -= (const Vector &b)
    {
        x -= b.x, y -= b.y;
        return *this;
    }

    friend inline istream& operator >> (istream& in, Vector &v)
    {
        return in >> v.x >> v.y;
    }
    friend inline ostream& operator << (ostream &out, const Vector &v)
    {
        return out << v.x << " " << v.y;
    }
};

inline int len2 (const Vector &a)
{
    return sqr(a.x) + sqr(a.y);
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
vector<Vector> max_move;

void make_max_move ()
{
    const int &r = MAX_DROPLET_DISP;
    max_move.push_back(Vector(-r, 0));
    max_move.push_back(Vector(r, 0));
    max_move.push_back(Vector(0, r));
    max_move.push_back(Vector(0, -r));
    for(int i=1; i<r; ++i)
    {
        int j = (int)sqrt(sqr(r) - sqr(i));
        max_move.push_back(Vector(i, j));
        max_move.push_back(Vector(i, -j));
        max_move.push_back(Vector(-i, j));
        max_move.push_back(Vector(-i, -j));
    }
}

inline Vector rand_max_move ()
{
    return max_move[rand() % max_move.size()];
    //double theta = rand_theta();
    //return Vector((int)absfloor(MAX_DROPLET_DISP * sin(theta)), (int)absfloor(MAX_DROPLET_DISP * cos(theta)));
}

struct Droplet
{
    int id;
    int owner;
    int blood;
    int level;
    Vector pos;
    bool moved;

    Vector targetPos;
    bool shouldSpread;
    bool isChoosen;

    Droplet () {}
    Droplet (int _id, int _owner, Vector _pos):
        id(_id), owner(_owner), blood(DROPLET_FULL_BLOOD), level(0),
        pos(_pos), targetPos(Vector(-1,-1)),
        shouldSpread(false), isChoosen(false), moved(false) {}

    int getShootingDist() const
    {
        return 300 + level * 3;
    }

    bool canShoot(const Droplet& target) const
    {
        return len2(pos - target.pos) <= sqr(getShootingDist());
    }
};

inline bool moved (const Droplet& d)
{
    return d.moved;
}
inline bool dead (const Droplet& d)
{
    return d.blood <= 0;
}
inline bool cmpByLevel(const Droplet &lhs, const Droplet &rhs)
{
    return lhs.level < rhs.level;
}
inline bool cmp_by_blood_level(const Droplet &a, const Droplet &b)
{
    return a.blood == b.blood && a.level > b.level || a.blood < b.blood;
}

Vector redBrainPos(1000, 5000), blueBrainPos(9000, 5000);
Vector stay_pos(7000, 6000), endtar(8600, 5000), start_pos(0, 5000);
int redBrainBlood = BRAIN_FULL_BLOOD, blueBrainBlood = BRAIN_FULL_BLOOD;
vector<Droplet> redDroplets, blueDroplets;
list<Droplet> ourLi, enemyLi;
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

inline void force_in_range (Vector& p)
{
    p.x = min(max(p.x, 0), MAP_WIDTH);
    p.y = min(max(p.y, 0), MAP_HEIGHT);
}

Vector go_to (Vector pos, Vector target, const int d = MAX_DROPLET_DISP)
{
    Vector disp = target - pos;
    if (len2(disp) > sqr(d))
    {
        double z = d / sqrt(len2(disp));
        disp.x = (int)absfloor(disp.x * z);
        disp.y = (int)absfloor(disp.y * z);
    }
    return pos + disp;
}

void loadSituation()
{
    for (auto it = redDroplets.begin(); it != redDroplets.end(); it++)
        if (it->blood > 0 && len2(it->pos - blueBrainPos) <= sqr(BRAIN_RADIUS) && blueBrainBlood > 0)
            blueBrainBlood--;
    for (auto it = blueDroplets.begin(); it != blueDroplets.end(); it++)
        if (it->blood > 0 && len2(it->pos - redBrainPos) <= sqr(BRAIN_RADIUS) && redBrainBlood > 0)
            redBrainBlood--;
    for (auto it = move_shootings.begin(); it != move_shootings.end(); it++)
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
    for (const int& y: move_newDropletsPosY)
        redDroplets.push_back(Droplet(redDroplets.size(), O_RED, Vector(0, y)));
    move_newDropletsPosY.clear();

    int nBlueNewDropletsPosY;
    cin >> nBlueNewDropletsPosY;
    nBlueWaitingDroplets -= nBlueNewDropletsPosY;
    for (int i = 0; i < nBlueNewDropletsPosY; i++)
    {
        int y;
        cin >> y;
        blueDroplets.push_back(Droplet(blueDroplets.size(), O_BLUE, Vector(MAP_WIDTH, y)));
    }

    for (Droplet& red: redDroplets)
        if (red.blood > 0)
        {
            int d = (int)floor(sqrt(len2(red.pos - blueBrainPos)) / 500);
            redScore += max(8 - d, 0);
        }
    for (Droplet& blue: blueDroplets)
        if (blue.blood > 0)
        {
            int d = (int)floor(sqrt(len2(blue.pos - redBrainPos)) / 500);
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

    if (choosenTargetPos.x != -1)
    {
        for (int i = 0; i < nChoosenIds; i++)
        {
            int id = choosenIds[i];
            redDroplets[id].targetPos = choosenTargetPos;
        }
    }

    for (int k = 0; k < nPressedKeys; k++)
    {
        switch (pressedKeys[k])
        {
            case 's':
            case 'S':
                for (int i = 0; i < nChoosenIds; i++)
                {
                    int id = choosenIds[i];
                    redDroplets[id].targetPos = Vector(-1, -1);
                }
                break;
            case 'x':
            case 'X':
                for (int i = 0; i < nChoosenIds; i++)
                {
                    int id = choosenIds[i];
                    redDroplets[id].shouldSpread = true;
                }
                break;
        }
    }

    ourLi.assign(redDroplets.begin(), redDroplets.end());
    enemyLi.assign(blueDroplets.begin(), blueDroplets.end());
    ourLi.remove_if(dead);
    enemyLi.remove_if(dead);
}

void reportShooting(int shooter, int target)
{
    move_shootings.push_back(make_pair(shooter, target));
}
void report_pos (Droplet& d, Vector pos)
{
    move_poss[d.id] = pos;
    d.moved = true;
}
void reportNewDropletsPosY(int y)
{
    move_newDropletsPosY.push_back(y);
}
void report_target (Droplet& d, Vector target)
{
    report_pos(d, go_to(d.pos, target));
}

void shoot ()
{
    ourLi.sort(cmpByLevel);
    enemyLi.sort(cmp_by_blood_level);

    for (Droplet& our: ourLi)
        for (Droplet& enemy: enemyLi)
            if (!dead(enemy) && our.canShoot(enemy))
            {
                enemy.blood--;
                reportShooting(our.id, enemy.id);
                break;
            }
}

void user_move ()
{
    for(Droplet& our: ourLi)
        if (our.targetPos.x != -1)
        {
            if (our.shouldSpread)
            {
                our.targetPos += rand_max_move();
                force_in_range(our.targetPos);
                redDroplets[our.id].targetPos = our.targetPos;
            }
            report_target(our, our.targetPos);
        }
        else if (our.shouldSpread)
        {
            Vector pos = our.pos + rand_max_move();
            force_in_range(pos);
            report_pos(our, pos);
        }
    ourLi.remove_if(moved);
}

int cnt_be_attacked (Vector p)
{
    int cnt = 0;
    for(Droplet& enemy: enemyLi)
        if(len2(enemy.pos - p) <= sqr(enemy.getShootingDist() + 30))
            ++cnt;
    return cnt;
}

Vector escape_move (Vector p, Vector t)
{
    struct Cmper
    {
        Vector tar;
        bool operator () (Vector a, Vector b)
        {
            int cnta = cnt_be_attacked(a),
                cntb = cnt_be_attacked(b);
            if(cnta != cntb)
                return cnta < cntb;
            int da = len2(a - tar),
                db = len2(b - tar);
            return da < db;
        }
    };
    vector<Vector> tar;
    for(Vector r: max_move)
    {
        r += p, force_in_range(r);
        tar.push_back(r);
    }
    return *min_element(tar.begin(), tar.end(), Cmper{t});
}

struct In_circle
{
    Vector o;
    int r;
    In_circle (Vector _o, int _r):
        o(_o), r(_r) {}
    bool operator () (const Droplet& p)
    {
        return p.blood > 0 && len2(p.pos - o) <= sqr(r);
    }
};
struct Dist_cmper
{
    Vector o;
    Dist_cmper (Vector _o):
        o(_o) {}
    bool operator () (const Droplet& a, const Droplet& b)
    {
        return len2(a.pos - o) < len2(b.pos - o);
    }
    bool operator () (const Vector& a, const Vector& b)
    {
        return len2(a - o) < len2(b - o);
    }
};

int enemy_power_around (Vector p, int r = 600)
{
    return count_if(blueDroplets.begin(), blueDroplets.end(), In_circle(p, r));
}
int our_power_around (Vector p, int r = 600)
{
    int cnt = 0;
    for(Droplet& our: redDroplets)
        if(our.blood > 0 && In_circle(p, r)(our) && Dist_cmper(start_pos)(our.pos, p))
            ++cnt;
    return cnt;
    //return count_if(redDroplets.begin(), redDroplets.end(), In_circle(p, r));
}

void reinforce (Vector p)
{
    ourLi.sort(Dist_cmper(p));
    int i=0;
    for(Droplet& our: ourLi)
    {
        if(i >= 5)  break;
        if(Dist_cmper(start_pos)(p, our.pos))
            continue;
        report_pos(our, escape_move(our.pos, go_to(p, start_pos, 300)));
        ++i;
    }
    ourLi.remove_if(moved);
}

void check_attack (const Droplet& enemy)
{
    int cnt1 = our_power_around(enemy.pos),
        cnt2 = enemy_power_around(enemy.pos);
    if(cnt1 < cnt2)    return;
    for(Droplet& our: ourLi)
        if(len2(our.pos - enemy.pos) <= sqr(600))
            report_target(our, enemy.pos);
    ourLi.remove_if(moved);
}

void makeDecision()
{
    //cerr << count_if(redDroplets.begin(), redDroplets.end(), dead) << ' ';
    //cerr << count_if(blueDroplets.begin(), blueDroplets.end(), dead) << endl;
    shoot();
    user_move();

    if(restTime < 1500 && redScore == 0 && blueScore == 0)
        for(Droplet& our: ourLi)
            report_target(our, blueBrainPos);
    ourLi.remove_if(moved);

    ourLi.sort(Dist_cmper(blueBrainPos));
    int t = 0;
    for(Droplet& our: ourLi)
    {
        report_pos(our, escape_move(our.pos, endtar));
        if(++t == 10)   break;
    }
    ourLi.remove_if(moved);

    for(Droplet& our: ourLi)
        if(In_circle(blueBrainPos, 500)(our))
            report_pos(our, escape_move(our.pos, endtar));
    ourLi.remove_if(moved);


    enemyLi.sort(Dist_cmper(redBrainPos));
    for(const Droplet& enemy: enemyLi)
        check_attack(enemy);
    for(const Droplet& enemy: enemyLi)
        reinforce(enemy.pos);
    for(Droplet& our: ourLi)
        report_pos(our, escape_move(our.pos, endtar));

    for (int i = 0; i < nRedWaitingDroplets; i++)
        reportNewDropletsPosY(5000);
}

void printMovement()
{
    cout << fixed << setprecision(6);

    cout << move_shootings.size() << endl;
    for(const auto& it: move_shootings)
        cout << it.first << " " << it.second << endl;

    cout << move_poss.size() << endl;
    for(const auto& it: move_poss)
        cout << it.first << " " << it.second << endl;

    cout << move_newDropletsPosY.size() << endl;
    for(const auto& it: move_newDropletsPosY)
        cout << it << endl;
}

int main()
{
    srand(time(0));
    make_max_move();
    while (true)
    {
        loadSituation();
        makeDecision();
        printMovement();
    }

    return 0;
}
