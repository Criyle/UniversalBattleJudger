#include "manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <climits>
using namespace std;
using namespace judge;

enum Owner
	{
		O_NONE = 0, O_RED = 1, O_BLUE = 2
	};
struct ErrorMessage
{
	string errMsg,errCode;
};
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

struct Droplet
{
	int id;
	Owner owner;
	int blood;
	int level;
	Vector pos;

	Vector targetPos;
	bool shouldSpread;
	bool isChoosen;

	inline int getShootingDist() const
	{
		return 300 + level * 3;
	}

	inline bool canShoot(const Droplet target) const
	{
		return dist2(pos, target.pos) <= sqr(getShootingDist());
	}
};

struct Movement
{
	ErrorMessage errMsg;
	vector< pair<int, int> > shootings;
	map<int, Vector> poss;
	vector<int> newDropletsPosY;
};

struct GameMessage
{
	int result;
	ostringstream redInfo,blueInfo;
};

class Judger
{
private:
	Vector redBrainPos, blueBrainPos;
	int redBrainBlood, blueBrainBlood;
	vector<Droplet> redDroplets, blueDroplets;
	int nRedWaitingDroplets, nBlueWaitingDroplets;
	int redMakeDropletRestTime, blueMakeDropletRestTime;
	int redScore, blueScore;
	int restTime;

	bool isOver;;

	Movement redMovement;
	Movement blueMovement;

	Movement nextRedMovement;
	Movement nextBlueMovement;

	Droplet *getDroplet(Owner owner, int id);
	void printSitu(Owner owner);
	char readChar(FILE *in);
	int readInt(FILE *in, char ed);
	void readMovement(Owner owner);
	vector<Vector> dispList;
	void move(Owner owner);
	bool checkMovementValid(Owner owner);
	void calcWinnerAndGameOver(bool isRedLost, bool isBlueLost);
	void gameOver();
public:
	Owner winner;
	GameMessage msgout;
	bool getIsOver();
	void run(Owner owner);
	void simulate();
	Judger();
};

class myException
{
public:
	explicit myException() throw(){}
	virtual ~myException() throw(){}
	virtual string code() const throw() = 0;
	virtual string what() const throw() = 0;
};
typedef long long s64;

//#define for_stl(it, v) for (typeof(v.begin()) it = v.begin(); it != v.end(); it++)
#define for_shoot(it, v) for (vector< pair<int, int> >::iterator it = v.begin(); it != v.end(); it++)
#define for_pos(it, v) for (map<int, Vector>::iterator it = v.begin(); it != v.end(); it++)
#define for_drop(it, v) for (vector<int>::iterator it = v.begin(); it != v.end(); it++)
#define for_droplet(it, v) for (vector<Droplet>::iterator it = v.begin(); it != v.end(); it++)

double absfloor(double a)
{
	if (a > 0)
		return floor(a);
	else
		return ceil(a);
}

Owner getEnemy(Owner owner)
{
	if (owner == O_RED)
		return O_BLUE;
	else
		return O_RED;
}

string toStr(Owner owner)
{
	switch (owner)
		{
		case O_RED:
			return "Red";
		case O_BLUE:
			return "Blue";
		default:
			return "None";
		}
}
string getMsgHeader(Owner owner)
{
	switch (owner)
		{
		case O_RED:
			return "Red > ";
		case O_BLUE:
			return "Blue> ";
		default:
			return "None> ";
		}
}

const int TOT_GAME_TIME = 1800;
const int MAX_DROPLET_DISP = 30;
const int MAP_WIDTH = 10000;
const int MAP_HEIGHT = 10000;
const int MAKE_DROPLET_TIME = 5;
const int BRAIN_FULL_BLOOD = 5000;
const int DROPLET_FULL_BLOOD = 20;
const int BRAIN_RADIUS = 500;

class invalid_movement_error : public myException
{
private:
	string what_arg;
public:
	explicit invalid_movement_error(const string& _what_arg) throw()
		: what_arg(_what_arg){}
	virtual ~invalid_movement_error() throw(){}
	virtual string what() const throw()
	{
		return what_arg;
	}
	virtual string code() const throw()
	{
		return "<font color=red>Invalid Output</font>";
	}
};
class invalid_input_error : public myException
{
public:
	virtual string what() const throw()
	{
		return "invalid output.";
	}
	virtual string code() const throw()
	{
		return "<font color=red>Invalid Output</font>";
	}
};
class failed_to_print_error : public myException
{
public:
	virtual string what() const throw()
	{
		//return "Input Exceeded"
		return "failed to print the situation information.";
	}
	virtual string code() const throw()
	{
		return "<font color=red>Input Exceeded</font>";
	}
};

inline bool lessPri(const Vector &a, const Vector &b)
{
	if (a.len2() != b.len2())
		return a.len2() < b.len2();
	if (a.x != b.x)
		return a.x < b.x;
	return a.y < b.y;
}

Droplet* Judger::getDroplet(Owner owner, int id)
{
	switch (owner)
		{
		case O_RED:
			if (!(0 <= id && id < (int)redDroplets.size()))
				return NULL;
			return &redDroplets[id];
		case O_BLUE:
			if (!(0 <= id && id < (int)blueDroplets.size()))
				return NULL;
			return &blueDroplets[id];
		default:
			return NULL;
		}
}

void Judger::printSitu(Owner owner)
{
	Movement *em = owner == O_RED ? &blueMovement : &redMovement;

	writestr("%d\n", (int)em->shootings.size());
	for_shoot(it, em->shootings)
		writestr("%d %d\n", it->first, it->second);

	writestr("%d\n", (int)em->poss.size());
	for_pos(it, em->poss)
		{
			int x = it->second.x, y = it->second.y;
			if (owner == O_BLUE)
				x = MAP_WIDTH - x;
			writestr("%d %d %d\n", it->first, x, y);
		}

	writestr("%d\n", (int)em->newDropletsPosY.size());
	for_drop(it, em->newDropletsPosY)
		writestr("%d\n", *it);

	writestr("0\n");
	writestr("-1 -1\n");
	writestr("0\n");
}

char Judger::readChar(FILE *in)
{
	return readchar();
}

void Judger::readMovement(Owner owner)
{
	Movement *m = owner == O_RED ? &nextRedMovement : &nextBlueMovement;

	*m = Movement();
		
	int nShootings = readint();
	if (nShootings < 0)
		throw invalid_movement_error("the number of shootings is negative.");
	for (int i = 0; i < nShootings; i++)
		{
			int shooter = readint(), target = readint();
			m->shootings.push_back(make_pair(shooter, target));
		}

	int nNewPos = readint();
	if (nNewPos < 0)
		throw invalid_movement_error("the number of new positions is negative.");
	for (int i = 0; i < nNewPos; i++)
		{
			int id = readint(), x = readint(), y = readint();
			if (owner == O_BLUE)
				x = MAP_WIDTH - x;
			if (m->poss.count(id))
				throw invalid_movement_error("try moving multiple times.");
			m->poss[id] = Vector(x, y);
		}

	int nNewDropletsPosY = readint();
	if (nNewDropletsPosY < 0)
		throw invalid_movement_error("the number of new droplets is negative.");
	for (int i = 0; i < nNewDropletsPosY; i++)
		{
			int y = readint();
			m->newDropletsPosY.push_back(y);
		}
}

void Judger::move(Owner owner)
{
	try
		{
			printSitu(owner);
			readMovement(owner);
		}
	catch (myException &e)
		{
			Movement *m = owner == O_RED ? &nextRedMovement : &nextBlueMovement;
			m->errMsg.errMsg =  getMsgHeader(owner) + e.what();
			m->errMsg.errCode = e.code();
		}
}

bool Judger::checkMovementValid(Owner owner)
{
	Movement *m = owner == O_RED ? &redMovement : &blueMovement;

	if (!m->errMsg.errMsg.empty())
		return false;

	try
		{
			set<int> shooterIdSet;
			for_shoot(it, m->shootings)
				{
					Droplet *shooter = getDroplet(owner, it->first);
					Droplet *target = getDroplet(getEnemy(owner), it->second);
					if (shooter == NULL)
						throw invalid_movement_error("invalid shooter.");
					if (target == NULL)
						throw invalid_movement_error("invalid target.");
					if (shooterIdSet.count(it->first))
						throw invalid_movement_error("try shooting multiple targets.");
					if (!shooter->canShoot(*target))
						throw invalid_movement_error("out of firing range.");
					shooterIdSet.insert(it->first);
				}

			for_pos(it, m->poss)
				{
					Droplet *droplet = getDroplet(owner, it->first);
					if (droplet == NULL)
						throw invalid_movement_error("invalid move.");
					if (dist2(droplet->pos, it->second) > sqr(MAX_DROPLET_DISP))
						throw invalid_movement_error("move too fast.");
					if (0 > it->second.x || it->second.x > MAP_WIDTH)
						throw invalid_movement_error("move out of range.");
					if (0 > it->second.y || it->second.y > MAP_HEIGHT)
						throw invalid_movement_error("move out of range.");
				}

			int nWaitingDroplets = owner == O_RED ? nRedWaitingDroplets : nBlueWaitingDroplets;
			if ((int)m->newDropletsPosY.size() > nWaitingDroplets)
				throw invalid_movement_error("too much new droplets.");
			for_drop(it, m->newDropletsPosY)
				{
					if (0 > *it || *it > MAP_HEIGHT)
						throw invalid_movement_error("the new droplet is out of range.");
				}
		}
	catch (myException &e)
		{
			m->errMsg.errMsg = getMsgHeader(owner) + e.what();
			m->errMsg.errCode = e.code();
			return false;
		}

	return true;
}

void Judger::calcWinnerAndGameOver(bool isRedLost, bool isBlueLost)
{
	if(!msgout.redInfo.str().empty() && !msgout.blueInfo.str().empty()){
		winner = O_NONE;
	}
	else if (isRedLost && isBlueLost)
		{
			if (redBrainBlood != blueBrainBlood)
				{
					if (redBrainBlood > blueBrainBlood)
						winner = O_RED;
					else
						winner = O_BLUE;
				}
			else if (redScore != blueScore)
				{
					if (redScore > blueScore)
						winner = O_RED;
					else
						winner = O_BLUE;
				}
			else
				winner = O_NONE;
		}
	else if (isRedLost)
		winner = O_BLUE;
	else if (isBlueLost)
		winner = O_RED;
	gameOver();
}

void Judger::gameOver()
{
	isOver = true;
	if(!msgout.redInfo.str().empty() || !msgout.blueInfo.str().empty()){
		msgout.result=0;
		if(msgout.blueInfo.str().empty()){
			msgout.blueInfo<<"<font color=green>Normal Operation</font>";
			msgout.result=2;
		}else if(msgout.redInfo.str().empty()){
			msgout.redInfo<<"<font color=green>Normal Operation</font>";
			msgout.result=1;
		}
	}else{
		if(winner == O_RED){
			msgout.redInfo<<"<font color=green>";
			msgout.blueInfo<<"<font color=red>";
			msgout.result=1;
		}else if(winner == O_BLUE){
			msgout.blueInfo<<"<font color=green>";
			msgout.redInfo<<"<font color=red>";
			msgout.result=2;
		}else{
			msgout.redInfo<<"<font color=orange>";
			msgout.blueInfo<<"<font color=orange>";
			msgout.result=0;
		}
		msgout.redInfo << "HP:" << redBrainBlood <<" PT:" << redScore << "</font>";
		msgout.blueInfo << "HP:" << blueBrainBlood <<" PT:" << blueScore << "</font>";
	}
}

Judger::Judger()
	: redBrainPos(1000, 5000), blueBrainPos(9000, 5000),
	  redBrainBlood(BRAIN_FULL_BLOOD), blueBrainBlood(BRAIN_FULL_BLOOD),
	  nRedWaitingDroplets(0), nBlueWaitingDroplets(0),
	  redMakeDropletRestTime(1), blueMakeDropletRestTime(1),
	  redScore(0), blueScore(0),
	  restTime(TOT_GAME_TIME)
{
	isOver = false;

	for (int x = -MAX_DROPLET_DISP; x <= MAX_DROPLET_DISP; x++)
		for (int y = -MAX_DROPLET_DISP; y <= MAX_DROPLET_DISP; y++)
			if (x * x + y * y <= sqr(MAX_DROPLET_DISP))
				dispList.push_back(Vector(x, y));
	sort(dispList.begin(), dispList.end(), lessPri), reverse(dispList.begin(), dispList.end());

	nextRedMovement = Movement();
	nextBlueMovement = Movement();
	simulate();
}

bool Judger::getIsOver()
{
	return isOver;
}

void Judger::run(Owner owner)
{
	stepbegin(owner == O_RED? 0: 1);
	move(owner);
	stepend(owner == O_RED? 0: 1);
}

void Judger::simulate()
{
	//cerr << "restTime: " << restTime << endl;

	redMovement = nextRedMovement;
	blueMovement = nextBlueMovement;

	bool isRedLost = !checkMovementValid(O_RED);
	bool isBlueLost = !checkMovementValid(O_BLUE);

	if (isRedLost || isBlueLost)
		{
			if (isRedLost)
				msgout.redInfo << redMovement.errMsg.errCode;
			if (isBlueLost)
				msgout.blueInfo << blueMovement.errMsg.errCode;
			calcWinnerAndGameOver(isRedLost, isBlueLost);
			return;
		}

	for_droplet(it, redDroplets)
		if (it->blood > 0 && dist2(it->pos, blueBrainPos) <= sqr(BRAIN_RADIUS) && blueBrainBlood > 0)
			blueBrainBlood--;
	for_droplet(it, blueDroplets)
		if (it->blood > 0 && dist2(it->pos, redBrainPos) <= sqr(BRAIN_RADIUS) && redBrainBlood > 0)
			redBrainBlood--;
	isRedLost = redBrainBlood == 0;
	isBlueLost = blueBrainBlood == 0;
	if (isRedLost || isBlueLost)
		{
			calcWinnerAndGameOver(isRedLost, isBlueLost);
			return;
		}

	for_shoot(it, redMovement.shootings)
		if (blueDroplets[it->second].blood > 0)
			blueDroplets[it->second].blood--, redDroplets[it->first].level++;
	for_shoot(it, blueMovement.shootings)
		if (redDroplets[it->second].blood > 0)
			redDroplets[it->second].blood--, blueDroplets[it->first].level++;

	for_pos(it, redMovement.poss)
		if (redDroplets[it->first].blood > 0)
			redDroplets[it->first].pos = it->second;
	for_pos(it, blueMovement.poss)
		if (blueDroplets[it->first].blood > 0)
			blueDroplets[it->first].pos = it->second;

	nRedWaitingDroplets -= (int)redMovement.newDropletsPosY.size();
	for_drop(it, redMovement.newDropletsPosY)
		{
			Droplet dr;
			dr.id = (int)redDroplets.size();
			dr.owner = O_RED;
			dr.blood = DROPLET_FULL_BLOOD;
			dr.level = 0;
			dr.pos = Vector(0, *it);
			redDroplets.push_back(dr);
		}
	nBlueWaitingDroplets -= (int)blueMovement.newDropletsPosY.size();
	for_drop(it, blueMovement.newDropletsPosY)
		{
			Droplet dr;
			dr.id = (int)blueDroplets.size();
			dr.owner = O_BLUE;
			dr.blood = DROPLET_FULL_BLOOD;
			dr.level = 0;
			dr.pos = Vector(MAP_WIDTH, *it);
			blueDroplets.push_back(dr);
		}

	for_droplet(it, blueDroplets)
		if (it->blood > 0)
			{
				int d = (int)floor(sqrt(dist2(it->pos, redBrainPos)) / 500);
				blueScore += max(8 - d, 0);
			}
	for_droplet(it, redDroplets)
		if (it->blood > 0)
			{
				int d = (int)floor(sqrt(dist2(it->pos, blueBrainPos)) / 500);
				redScore += max(8 - d, 0);
			}

	redMakeDropletRestTime--;
	if (redMakeDropletRestTime == 0) {
		nRedWaitingDroplets++;
		redMakeDropletRestTime += MAKE_DROPLET_TIME;
	}
	blueMakeDropletRestTime--;
	if (blueMakeDropletRestTime == 0) {
		nBlueWaitingDroplets++;
		blueMakeDropletRestTime += MAKE_DROPLET_TIME;
	}

	restTime--;
	if (restTime == 0)
		{
			calcWinnerAndGameOver(true, true);
			return;
		}
}

int main()
{
	init(2);
	Judger ju;
			
	while (!ju.getIsOver())
		{
			ju.run(O_RED);
			ju.run(O_BLUE);
			ju.simulate();
		}

	string b;
	b = ju.msgout.redInfo.str();
	putresult(0, b);
	b = ju.msgout.blueInfo.str();
	putresult(1, b);

	if(ju.winner == O_RED)
		putscore(0, 3);
	else if(ju.winner == O_BLUE)
		putscore(1, 3);
	
	judgeend();
	return 0;
}
