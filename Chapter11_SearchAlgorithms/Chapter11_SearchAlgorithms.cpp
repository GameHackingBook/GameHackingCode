#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <queue>
#include <set>
#include <memory>

#include "WindowCode.h"


#define TILE_COST 1

class AStarNode;
typedef std::shared_ptr<AStarNode> AStarNodePtr;

class AStarNode
{
public:
	int x, y;
	int g, score;
	AStarNodePtr parent;

    AStarNode(int x, int y, int cost, AStarNodePtr p, int score = 0) 
		: x(x), y(y), g(cost), score(score), parent(p)
	{}
	static AStarNodePtr makePtr(int x, int y, int cost, AStarNodePtr p, int score = 0)
	{
		return AStarNodePtr(new AStarNode(x, y, cost, p, score));
	}
	int heuristic(const int destx, int desty) const
    {
		int xd = destx - x;
		int yd = desty - y;
		return abs(xd) + abs(yd);
    }
    void updateScore(int endx, int endy)
    {
		this->score = g + heuristic(endx, endy) * TILE_COST;
    }
	AStarNodePtr getCopy()
	{
		return AStarNode::makePtr(x, y, g, parent, score);
	}
	std::vector<AStarNodePtr> getChildren(int width, int height)
	{
		std::vector<AStarNodePtr> ret;
		auto copy = getCopy();
		if (x > 0)
			ret.push_back(AStarNode::makePtr(x - 1, y, g + TILE_COST, copy));
		if (y > 0)
			ret.push_back(AStarNode::makePtr(x, y - 1, g + TILE_COST, copy));
		if (x < width - 1)
			ret.push_back(AStarNode::makePtr(x + 1, y, g + TILE_COST, copy));
		if (y < height - 1)
			ret.push_back(AStarNode::makePtr(x, y + 1, g + TILE_COST, copy));
		return ret;
	}
};

bool operator<(const AStarNodePtr &a, const AStarNodePtr &b)
{
	return a->score > b->score;
}
bool operator==(const AStarNodePtr &a, const AStarNodePtr &b)
{
	return a->x == b->x && a->y == b->y;
}

template<int WIDTH, int HEIGHT>
void makeList(AStarNodePtr end, std::vector<AStarNodePtr> nodes, int path[WIDTH][HEIGHT])
{
	for (auto n = nodes.begin(); n != nodes.end(); n++)
		path[(*n)->x][(*n)->y] = 2;

	AStarNodePtr node = end;
	while (node.get() != nullptr)
	{
		path[node->x][node->y] = 1;
		node = node->parent;
	}
}

template<int WIDTH, int HEIGHT, int BLOCKING>
bool doAStarSearch(
		int map[WIDTH][HEIGHT],
		int startx, int starty,
		int endx, int endy,
		int path[WIDTH][HEIGHT])
{
	std::priority_queue<AStarNodePtr> frontier;
	std::vector<AStarNodePtr> allNodes;

	auto node = AStarNode::makePtr(startx, starty, 0, nullptr);
	node->updateScore(endx, endy);
	allNodes.push_back(node);

	while (true)
	{
		if (node->x == endx && node->y == endy)
		{
			makeList<WIDTH, HEIGHT>(node, allNodes, path);
			return true;
		}

		auto children = node->getChildren(WIDTH, HEIGHT);
		for (auto c = children.begin(); c != children.end(); c++)
		{
			if (map[(*c)->x][(*c)->y] == BLOCKING)
				continue;
			auto found = std::find(allNodes.rbegin(), allNodes.rend(), *c);
			if (found != allNodes.rend())
			{
				if (*found > *c)
				{
					(*found)->g = (*c)->g;
					(*found)->parent = (*c)->parent;
					(*found)->updateScore(endx, endy);
				}
			}
			else
			{
				(*c)->updateScore(endx, endy);
				frontier.push(*c);
				allNodes.push_back(*c);
			}
		}
		
		if (frontier.size() == 0)
			return false;

		node = frontier.top();
		frontier.pop();
	}
}



int main (int argc, char *argv[])
{
	searchFunction = doAStarSearch<XSIZE, YSIZE, BLOCKING_TILE>;
	showWindow();
	return 0;
}