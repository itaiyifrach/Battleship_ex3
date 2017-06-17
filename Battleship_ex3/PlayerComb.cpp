#include "PlayerComb.h"
#include <iostream>
bool finished = false;

deque<pair<int, int>> PlayerComb::buildQueue(int playerNum, int numOfPlayers)
{
	deque<pair<int, int>> rivalPlayers;
	for (int i = 0; i < numOfPlayers; i++)
	{
		if (i != playerNum)
		{
			rivalPlayers.push_back(make_pair(i, 0));
		}
	}
	return rivalPlayers;
}

tuple<int, int, int> PlayerComb::getGameParams()
{	
	if (finished)
		return make_tuple(-1, -1, -1);
	int prevIndex = currentIndex;
	do
	{
		if(gamesQueue[currentIndex].size()>0)
		{
			pair<int,int> params = gamesQueue[currentIndex].front();
			gamesQueue[currentIndex].pop_front();
			auto game_tuple=make_tuple(params.second, currentIndex, params.first);
			if(params.second<numOfBoards-1)
			{
				++params.second;
				gamesQueue[currentIndex].push_back(params);
			}
							
			currentIndex = (currentIndex + 1)% numOfPlayers;
			return game_tuple;
		}
		currentIndex = (currentIndex + 1) % numOfPlayers;
		

	} while (currentIndex != prevIndex);
	//if we checked every queue and they are all empty- then there are no more games left
	finished = true;
	return make_tuple(-1, -1, -1);
	
	
}

void PlayerComb::printComb()
{
	for(int i=0;i<gamesQueue.size();i++)
	{
		cout << "queue number " << i << endl;
		cout << "{ ";
		for(int j=0;j<gamesQueue[i].size();j++)
		{
			cout << "<" << gamesQueue[i][j].first <<","<< gamesQueue[i][j].second << "> ";
		}
		cout << "}" << endl;
	}
}
