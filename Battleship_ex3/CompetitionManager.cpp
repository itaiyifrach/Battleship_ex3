#include "CompetitionManager.h"
//global structure to hold players data results
vector<vector<playerData>> playersData;
//global mutexs
mutex printerMutex;
mutex queueMutex;
mutex dataMutex;
mutex debugMutex;
//condition variable for printer
condition_variable result_printer;
//atomic vars for the printing
int ourLastPrintNumOfGames = 0;
atomic<int> currentNumOfGames = 0;



void CompetitionManager::printResults(CompetitionManager& competition, int maxLengthName) {
	// creating vector of results
	int fixture = findMinGames();
	if (fixture == 0||fixture==INT_MAX)
		return;
	vector<pair<int, playerData>> results;
	for (int i = 0; i<playersData.size(); i++) {
		results.push_back(make_pair(i, playersData[i][fixture - 1]));
	}

	// sorting by wins percentage
	sort(results.begin(), results.end(), percentCompare);

	// saving cout state to restore after using iomanip
	ios oldState(nullptr);
	oldState.copyfmt(cout);
	//------------title------------//	
	cout << "# CYCLE: " << fixture << endl;
	cout << left << setw(8) << "#" << setw(maxLengthName + 4) << "Team Name" << setw(8) << "Wins"
		<< setw(8) << "Losses" << setw(8) << "%" << setw(8) << "Pts For" 
		<< setw(12) << left << "Pts Against";
	cout << endl << endl;
	//-----------------------------//
	int playerNum;
	double winsRatio;
	for (size_t i = 0; i<results.size(); i++) {
		playerNum = results[i].first;
		winsRatio = results[i].second.wins / double(results[i].second.gamesPlayed);

		cout << left << setw(8) << (to_string(i + 1) + '.') << setw(maxLengthName + 4) << get<2>(competition.playersVec[playerNum])
			<< setw(8) << results[i].second.wins << setw(8) << results[i].second.losses;
		cout.copyfmt(oldState);
		cout << left << setw(8) << setprecision(2) << winsRatio;
		cout.copyfmt(oldState);
		cout << left << setw(8) << results[i].second.pointsFor << setw(12) << results[i].second.pointsAgainst;
		cout << endl;
	}
	cout << endl;
	cout.copyfmt(oldState);

}

bool CompetitionManager::percentCompare(pair<int, playerData> p1, pair<int, playerData> p2) {
	return ((p1.second.wins / double (p1.second.gamesPlayed)) > (p2.second.wins / double(p2.second.gamesPlayed)));
}

int CompetitionManager::findMinGames()
{
	int minGames = INT_MAX;
	for (int i = 0; i < playersData.size(); i++)
		if (playersData[i].size() < minGames)
			minGames =int( playersData[i].size());
	return minGames;
}

int CompetitionManager::getMaxLengthName(CompetitionManager& competition)
{
	int maxLength = 0;
	auto n = competition.playersVec.size();

	for (auto i = 0; i < n; i++) {
		auto tempLength = (get<2>(competition.playersVec[i])).length();
		if (maxLength <= tempLength)
		{
			maxLength = int(tempLength);
		}
	}

	return maxLength;
}

void CompetitionManager::threadWorker(CompetitionManager& competition, PlayerComb& gamesQueue)
{

		while (!finished)
		{
			tuple<int, int, int> currentGame;
			{
				lock_guard<mutex> lock1(queueMutex);
				currentGame = gamesQueue.getGameParams();
			}

			if (get<0>(currentGame) == -1)
			{
				break;
			}

			auto boardIndex = get<0>(currentGame);
			auto playerIndexA = get<1>(currentGame);
			auto playerIndexB = get<2>(currentGame);
			auto currBoard = competition.boardVec[boardIndex];
			unique_ptr<IBattleshipGameAlgo> playerA(get<0>(competition.playersVec[playerIndexA])());
			unique_ptr<IBattleshipGameAlgo> playerB(get<0>(competition.playersVec[playerIndexB])());
			BattleshipGame game(currBoard, playerA.get(), playerB.get());
			tuple<int, int, int> gameResults = game.playGame();
			updatePlayersData(playerIndexA, playerIndexB, get<0>(gameResults), get<1>(gameResults), get<2>(gameResults));
			++currentNumOfGames;
			result_printer.notify_one();
		}


}

void CompetitionManager::launcher(CompetitionManager& competition)
{
	vector<thread> threads;
	PlayerComb playerComb(competition.numOfPlayers, competition.numOfBoards);
	BSLogger::loggerPrintInfo(LAUNCHING_COMP + to_string(competition.numOfPlayers) + " players and "
								+ to_string(competition.numOfBoards) + " boards");

	int maxLengthName = getMaxLengthName(competition);
	for (int i = 0; i < competition.numOfThreads; i++)
	{		
		threads.push_back(thread(threadWorker,std::ref(competition),std::ref(playerComb)));
	}
	//data registration loop
	unique_lock<mutex> lock1(printerMutex);		
	int gap = max(1, competition.numOfGames / 10);
	while (!finished ) 
	{
		if (result_printer.wait_for(lock1, std::chrono::seconds(3), [gap] { return (currentNumOfGames >= ourLastPrintNumOfGames + gap); }))
		{
			ourLastPrintNumOfGames = currentNumOfGames;
			printResults(competition, maxLengthName);
		}
		
	}

	for (thread& t : threads)
	{
		t.join();
	}
	//print final competition results (can be printed twice)
	printResults(competition, maxLengthName);
	BSLogger::loggerPrintInfo(EXITING_COMP + to_string(currentNumOfGames) + " games");
}

void CompetitionManager::updatePlayersData(int playerIndexA, int playerIndexB, int winnerNumber, int pointsForPlayerA, int pointsForPlayerB)
{
	playerData playerPrevDataA, playerPrevDataB;
	{
		lock_guard<mutex> lock1(dataMutex);
		int numOfGamesA = int(playersData[playerIndexA].size());
		if (numOfGamesA != 0)
			playerPrevDataA = playersData[playerIndexA][numOfGamesA - 1];
		int numOfGamesB = int(playersData[playerIndexB].size());
		if (numOfGamesB != 0)
			playerPrevDataB = playersData[playerIndexB][numOfGamesB - 1];
	}

	playerPrevDataA.pointsFor += pointsForPlayerA;
	playerPrevDataA.pointsAgainst += pointsForPlayerB;
	++playerPrevDataA.gamesPlayed;

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
	{
		lock_guard<mutex> lock1(dataMutex);
		playersData[playerIndexA].push_back(playerPrevDataA);
		playersData[playerIndexB].push_back(playerPrevDataB);
	}

}