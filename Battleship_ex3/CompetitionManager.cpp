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
	int playerNum;
	double winsRatio;
	for (size_t i = 0; i<results.size(); i++) {
		playerNum = results[i].first;
		winsRatio = results[i].second.wins / double(results[i].second.gamesPlayed);

		cout << left << setw(8) << (to_string(i + 1) + '.') << setw(24) << competition.playerNames[playerNum]
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
	int minGames = INT_MAX;
	for (int i = 0; i < playersData.size(); i++)
		if (playersData[i].size() < minGames)
			minGames =int( playersData[i].size());
	return minGames;
}

void CompetitionManager::threadWorker(CompetitionManager& competition, PlayerComb& gamesQueue)
{
	while (!finished)
	{
		tuple<int, int, int> currentGame;
		string first = "getting game\n";
		{
			lock_guard<mutex> lock1(queueMutex);			
			cout << first;
			currentGame = gamesQueue.getGameParams();			
		}

		if (get<0>(currentGame) == -1)
		{
			break;
		}	
		
		auto boardIndex = get<0>(currentGame);
		auto playerIndexA = get<1>(currentGame);
		auto playerIndexB = get<2>(currentGame);
		string second = "<" + std::to_string(boardIndex) + "," + std::to_string(playerIndexA) + "," + std::to_string(playerIndexB) + ">\n";
		cout << second;
		auto currBoard = competition.boardVec[boardIndex];
		string filenameA = competition.playerNames[playerIndexA];
		string filenameB = competition.playerNames[playerIndexB];
		auto playerA= GameUtils::loadAlgo(competition.path, filenameA);
		auto playerB= GameUtils::loadAlgo(competition.path, filenameB);
		BattleshipGame game(currBoard,playerA.first,playerB.first);		
		tuple<int, int, int> gameResults = game.playGame();	
		{
			lock_guard<mutex> lock2(dataMutex);			
			updatePlayersData(playerIndexA, playerIndexB, get<0>(gameResults), get<1>(gameResults), get<2>(gameResults));			
		}

		++currentNumOfGames;
		result_printer.notify_one();
		FreeLibrary(playerA.second);
		FreeLibrary(playerB.second);
	}
	//print for debug purposes
	{
		lock_guard<mutex> lock1(debugMutex);		
		cout << std::this_thread::get_id() << " this thread is done mate" << endl;
	}

}

void CompetitionManager::launcher(CompetitionManager& competition)
{
	vector<thread> threads;
	PlayerComb playerComb(competition.numOfPlayers, competition.numOfBoards);
	playerComb.printComb();

	//print for debug purposes
	cout << "launching competition" << endl;
	for (int i = 0; i < competition.numOfThreads; i++)
	{		
		threads.push_back(thread(threadWorker,std::ref(competition),std::ref(playerComb)));
	}
	//data registration loop
	
	while (!finished) {
		unique_lock<mutex> lock1(printerMutex);
		int gap = max(1, competition.numOfGames / PRINT_FREQ);
		if (result_printer.wait_for(lock1, std::chrono::seconds(5), [gap] { return (currentNumOfGames >= ourLastPrintNumOfGames + gap); }))
		{
			ourLastPrintNumOfGames = currentNumOfGames;
			printResults(competition);
		}
		
	}
	//for debug
	if (currentNumOfGames != competition.numOfGames)
		cout << "Error: num of games played is: " << currentNumOfGames << " but should have played: " << competition.numOfGames << "games" << endl;

	for (thread& t : threads)
	{
		t.join();
	}
	//print final competition results (can be printed twice)
	printResults(competition);
	//print for debug purposes
	cout << "finished competition" << endl;
}

void CompetitionManager::updatePlayersData(int playerIndexA, int playerIndexB, int winnerNumber, int pointsForPlayerA, int pointsForPlayerB)
{
	playerData playerPrevDataA, playerPrevDataB;
	int numOfGamesA = int(playersData[playerIndexA].size());
	if (numOfGamesA != 0)
		playerPrevDataA = playersData[playerIndexA][numOfGamesA-1];
	playerPrevDataA.pointsFor += pointsForPlayerA;
	playerPrevDataA.pointsAgainst += pointsForPlayerB;
	++playerPrevDataA.gamesPlayed;
	int numOfGamesB= int(playersData[playerIndexB].size());
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