#include "CompetitionManager.h"

void CompetitionManager::printResults(int fixture) {
	// creating vector of 
	vector<playerData> results;
	for (int i = 0; playersData.size(); i++) {
		results.push_back(playersData[i][fixture - 1]);
	}

	// sorting by wins percentage
	sort(results.begin(), results.end(), percentCompare);

	//------------title------------//
	cout << setw(8) << left << "#" << setw(24) << left << "Team Name" << setw(8) << left << "Wins";
	cout << setw(8) << left << "Losses" << setw(8) << left << "%" << setw(8) << left << "Pts For" << setw(12) << left << "Pts Against";
	cout << endl << endl;
	//-----------------------------//
	for (size_t i = 0; i<results.size(); i++) {
		string num = std::to_string(i + 1);
		num = num + '.';
		cout << setw(8) << left << num << setw(24) << left << names[i] << setw(8) << left << wins[i];
		cout << setw(8) << left << losses[i] << setw(8) << left << percent[i] << setw(8) << left << pts_for[i] << setw(12) << left << pts_against[i];
		cout << endl;
	}

}

bool percentCompare(playerData p1, playerData p2) { 
	return ((p1.wins / p1.gamesPlayed) > (p2.wins / p2.gamesPlayed));
}



int CompetitionManager::findMinGames() const
{
	int minGames = INFINITY;
	for (int i = 0; i < playerNames.size(); i++)
		if (playersData[i].size() > minGames)
			minGames = playersData[i].size();
	return minGames;
}