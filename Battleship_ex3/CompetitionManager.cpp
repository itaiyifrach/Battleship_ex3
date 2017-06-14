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
		winsRatio = results[i].second.wins / (double)results[i].second.gamesPlayed;
		cout << setw(8) << left << num << setw(24) << left << playerNames[playerNum] 
			<< setw(8) << left << results[i].second.wins << setw(8) << left << results[i].second.losses 
			<< setw(8) << left << winsRatio 
			<< setw(8) << left << results[i].second.pointsFor << setw(12) << left << results[i].second.pointsAgainst;
		cout << endl;
	}

}

bool percentCompare(playerData p1, playerData p2) { 
	return ((p1.wins / (double) p1.gamesPlayed) > (p2.wins / (double) p2.gamesPlayed));
}