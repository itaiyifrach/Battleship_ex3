#include "CompetitionManager.h"

void CompetitionManager::printResults(int fixture) {
	// creating vector of 
	vector<pair<int, playerData>> results;
	for (int i = 0; playersData.size(); i++) {
		results.push_back(make_pair(i, playersData[i][fixture - 1]));
	}

	// sorting by wins percentage
	sort(results.begin(), results.end(), percentCompare);

	//------------title------------//
	cout << setw(8) << left << "#" << setw(24) << left << "Team Name" << setw(8) << left << "Wins"
		<< setw(8) << left << "Losses" << setw(8) << left << "%" << setw(8) << left << "Pts For" 
		<< setw(12) << left << "Pts Against";
	cout << endl << endl;
	//-----------------------------//
	int playerNum, winsRatio;
	for (size_t i = 0; i<results.size(); i++) {
		string num = std::to_string(i + 1);
		num = num + '.';
		playerNum = results[i].first;
		winsRatio = results[i].second.wins / double(results[i].second.gamesPlayed);
		cout << setw(8) << left << num << setw(24) << left << playerNames[playerNum] 
			<< setw(8) << left << results[i].second.wins << setw(8) << left << results[i].second.losses 
			<< setw(8) << left << winsRatio 
			<< setw(8) << left << results[i].second.pointsFor << setw(12) << left << results[i].second.pointsAgainst;
		cout << endl;
	}

}

bool CompetitionManager::percentCompare(pair<int, playerData> p1, pair<int, playerData> p2) {
	return ((p1.second.wins / double (p1.second.gamesPlayed)) > (p2.second.wins / double(p2.second.gamesPlayed)));
}

int CompetitionManager::findMinGames() const
{
	int minGames = INFINITY;
	for (int i = 0; i < playerNames.size(); i++)
		if (playersData[i].size() > minGames)
			minGames = playersData[i].size();
	return minGames;
}

void CompetitionManager::threadWorker()
{
	while (true)
	{
		unique_lock<mutex> lock1(queueMutex);
		lock1.lock();
		tuple<int, int, int> currentGame = getGameTuple();
		lock1.unlock;
		if (get<0>(currentGame) == -1)
		{
			break;
		}
			
		auto currBoard = boardVec[get<0>(currentGame)];
		string filenameA = playerNames[get<1>(currentGame)];
		string filenameB = playerNames[get<2>(currentGame)];
		auto playerA= GameUtils::loadAlgo(path, filenameA);
		auto playerB= GameUtils::loadAlgo(path, filenameB);
		BattleshipGame game(currBoard,playerA.first,playerB.first);
		tuple<int, int, int> gameResults = game.playGame();		
		unique_lock<mutex> lock2(dataMutex);
		lock2.lock();
		updatePlayerData(gameResults);		
		lock2.unlock;
		result_printer.notify_one();
		FreeLibrary(playerA.second);
		FreeLibrary(playerB.second);
	}
	unique_lock<mutex> lock1(queueMutex);
	lock1.lock();
	cout << std::this_thread::get_id() << " this thread is done mate" << endl;
	lock1.unlock();	
}

void CompetitionManager::dataRegistration()
{
	while (!finished) {
		unique_lock<mutex> lock1(queueMutex);
		int gap = numOfGames / PRINT_FREQ;
		result_printer.wait(lock1, [gap] { return (currentNumOfGames >= ourLastPrintNumOfGames + gap); });
		printResults;
	}
}

void CompetitionManager::launcher()
{
	vector<thread> threads;
	initPairMaker playerComb(numOfPlayers,numOfBoards);
	for (int i = 0; i < numOfThreads; i++)
	{
		threads.emplace_back(threadWorker,...);
	}
	threads.emplace_back(dataRegistration, ...);
	for (thread& t : threads)
	{
		t.join();
	}
}