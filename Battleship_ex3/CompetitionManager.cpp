#include "CompetitionManager.h"

void CompetitionManager::printResults(CompetitionManager& competition) {
	// creating vector of results
	int fixture = findMinGames();
	if (fixture == 0)
		return;
	vector<pair<int, playerData>> results;
	for (int i = 0; playersData.size(); i++) {
		results.push_back(make_pair(i, playersData[i][fixture - 1]));
	}

	// sorting by wins percentage
	sort(results.begin(), results.end(), percentCompare);

	// saving cout state to restore after using iomanip
	ios oldState(nullptr);
	oldState.copyfmt(cout);
	//------------title------------//
	cout << "# CYCLE: " << fixture << endl << endl;
	cout << left << setw(8) << "#" << setw(24) << "Team Name" << setw(8) << "Wins"
		<< setw(8) << "Losses" << setw(8) << "%" << setw(8) << "Pts For" 
		<< setw(12) << left << "Pts Against";
	cout << endl << endl;
	//-----------------------------//
	int playerNum, winsRatio;
	for (size_t i = 0; i<results.size(); i++) {
		string num = std::to_string(i + 1);
		num = num + '.';
		playerNum = results[i].first;
		winsRatio = results[i].second.wins / double(results[i].second.gamesPlayed);

		cout << left << setw(8) << num << setw(24) << competition.playerNames[playerNum]
			<< setw(8) << results[i].second.wins << setw(8) << results[i].second.losses;
		cout.copyfmt(oldState);
		cout << left << setw(8) << setprecision(4) << winsRatio;
		cout.copyfmt(oldState);
		cout << left << setw(8) << results[i].second.pointsFor << setw(12) << results[i].second.pointsAgainst;
		cout << endl;
	}
	cout.copyfmt(oldState);

}

bool CompetitionManager::percentCompare(pair<int, playerData> p1, pair<int, playerData> p2) {
	return ((p1.second.wins / double (p1.second.gamesPlayed)) > (p2.second.wins / double(p2.second.gamesPlayed)));
}

int CompetitionManager::findMinGames()
{
	int minGames = INFINITY;
	for (int i = 0; i < playersData.size(); i++)
		if (playersData[i].size() > minGames)
			minGames = playersData[i].size();
	return minGames;
}

void CompetitionManager::threadWorker(CompetitionManager& competition, PlayerComb& gamesQueue)
{
	while (!finished)
	{
		unique_lock<mutex> lock1(queueMutex);
		lock1.lock();
		tuple<int, int, int> currentGame = gamesQueue.getGameParams();
		lock1.unlock;
		if (get<0>(currentGame) == -1)
		{
			break;
		}
		++currentNumOfGames;
		auto boardIndex = get<0>(currentGame);
		auto playerIndexA = get<1>(currentGame);
		auto playerIndexB = get<2>(currentGame);
		auto currBoard = competition.boardVec[boardIndex];
		string filenameA = competition.playerNames[playerIndexA];
		string filenameB = competition.playerNames[playerIndexB];
		auto playerA= GameUtils::loadAlgo(competition.path, filenameA);
		auto playerB= GameUtils::loadAlgo(competition.path, filenameB);
		BattleshipGame game(currBoard,playerA.first,playerB.first);
		tuple<int, int, int> gameResults = game.playGame();		
		unique_lock<mutex> lock2(dataMutex);
		lock2.lock();
		updatePlayersData(playerIndexA, playerIndexB, get<0>(gameResults), get<1>(gameResults), get<2>(gameResults));
		lock2.unlock;
		result_printer.notify_one();
		FreeLibrary(playerA.second);
		FreeLibrary(playerB.second);
	}
	//print for debug purposes
	unique_lock<mutex> lock1(debugMutex);
	lock1.lock();
	cout << std::this_thread::get_id() << " this thread is done mate" << endl;
	lock1.unlock();	
}

void CompetitionManager::launcher(CompetitionManager& competition)
{
	vector<thread> threads;
	PlayerComb playerComb(competition.numOfPlayers, competition.numOfBoards);
	//print for debug purposes
	cout << "launching competition" << endl;
	for (int i = 0; i < competition.numOfThreads; i++)
	{		
		threads.push_back(thread(threadWorker,std::ref(competition),std::ref(playerComb)));
	}
	//data registration loop
	std::chrono::seconds duration;
	while (!finished) {
		unique_lock<mutex> lock1(queueMutex);
		int gap = competition.numOfGames / PRINT_FREQ;
		result_printer.wait_for(lock1, std::chrono::seconds(2), [gap] { return (currentNumOfGames >= ourLastPrintNumOfGames + gap); });
		printResults(competition);
	}
	//for debug
	if (currentNumOfGames != competition.numOfGames)
		cout << "Error: num of games played is: " << currentNumOfGames << " but should have played: " << competition.numOfGames << "games" << endl;

	//print final competition results (can be printed twice)
	printResults(competition);
	for (thread& t : threads)
	{
		t.join();
	}
	//print for debug purposes
	cout << "finished competition" << endl;
}

void CompetitionManager::updatePlayersData(int playerIndexA, int playerIndexB, int winnerNumber, int pointsForPlayerA, int pointsForPlayerB)
{
	playerData playerPrevDataA, playerPrevDataB;
	int numOfGamesA = playersData[playerIndexA].size();
	if (numOfGamesA != 0)
		playerPrevDataA = playersData[playerIndexA][numOfGamesA-1];
	playerPrevDataA.pointsFor += pointsForPlayerA;
	playerPrevDataA.pointsAgainst += pointsForPlayerB;
	++playerPrevDataA.gamesPlayed;
	int numOfGamesB= playersData[playerIndexB].size();
	if (numOfGamesB != 0)
		playerPrevDataB = playersData[playerIndexB][numOfGamesB - 1];
	playerPrevDataB.pointsFor += pointsForPlayerB;
	playerPrevDataB.pointsAgainst += pointsForPlayerA;
	++playerPrevDataB.gamesPlayed;
	//check who won
	switch (winnerNumber)
	{
	case 0:
		++playerPrevDataA.wins;
		++playerPrevDataB.losses;
		break;
	case 1:
		++playerPrevDataB.wins;
		++playerPrevDataA.losses;
		break;
	default:
		break;
	}
	playersData[playerIndexA].push_back(playerPrevDataA);
	playersData[playerIndexB].push_back(playerPrevDataB);
	

}