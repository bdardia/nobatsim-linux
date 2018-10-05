#include <list>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include "NoBatSim.h"

int main()
{
	using namespace std;
	srand(time(0));
	string line;
	string subline;
	string breakChar = string(",");
	ifstream myfile("savant_data.csv");
	string reviewableOutcomes[10] = { "swinging_strike,", "swinging_strike_blocked,", "foul,", "foul_tip,", "foul_bunt,", "missed_bunt,", "hit_into_play,", "hit_into_play_no_out,", "hit_into_play_score,", "hit_by_pitch," };
	string called = string("called_strike,");
	vector<string> currentPitch;
	bool inPA = false;
	bool newPitch = false;
	int zonePercent;
	system("clear");
	cout << "Enter zone% of season (can be found on fangraphs) without percent symbol (ex: 42.4%->42.4)";
	cin >> zonePercent;
	zonePercent *= 10;
	system("clear");
	if (myfile.is_open())
	{
		int y = 0;
		int i = 0;
		int x;
		int startValue = 0;
		int outerBallCount = 0, outerStrikeCount = 0;
		while (getline(myfile, subline))
		{
			updateStats();
			x = 180;
			newPitch = false;
			for (int j = 0; j < subline.length(); j++)
			{
				if (subline.substr(j, 1).compare(breakChar) == 0 || j + 1 == subline.length()) // loop through string and store each cell
				{
					if ((j - startValue + 1) < subline.length())
					{
						line = subline.substr(startValue, (j - startValue) + 1);
						//TextOut(hdc, 0, x += 20, currentField[i].c_str(), strlen(currentField[i].c_str()));
						//TextOut(hdc, 0, x += 20, line.c_str(), strlen(line.c_str()));
						//TextOut(hdc, 700, 700, " ", 1);
						currentPitch.push_back(line);
						startValue = j + 1;
						//std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					i++;
				}
			}
			// check if our count matches the data. If it doesn't, either skip pitches or generate new ones to realign.
			int ballCount = stoi(currentPitch.at(BALL_COUNT));
			int strikeCount = stoi(currentPitch.at(STRIKE_COUNT));
			if ((ballCount > outerBallCount || strikeCount > outerStrikeCount) && (outerBallCount == 0 && outerStrikeCount == 0))
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			else if ((ballCount < outerBallCount || strikeCount < outerStrikeCount) && (ballCount == 0 && strikeCount == 0))
			{
				while (outerBallCount < 4 && outerStrikeCount < 3)
				{
					int randomPitch = std::rand() % 1000 + 1;
					if (randomPitch <= zonePercent)
						outerStrikeCount++;
					else
						outerBallCount++;
				}
				if (outerBallCount > outerStrikeCount)
				{
					totalPlateAppearences++;
					totalWalks++;
				}
				else
				{
					totalPlateAppearences++;
					totalStrikeouts++;
				}
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
			}
			// check if this pitch is reviewable
			for (int pti = 0; pti < 10; pti++)
			{
				if (currentPitch.at(PITCH_TYPE).compare(reviewableOutcomes[pti]) == 0)
				{
					if (pti == 9)
					{
						outerBallCount = 0, outerStrikeCount = 0;
						inPA = false;
						newPitch = true;
						totalHitByPitches++;
						totalPlateAppearences++;
					}
					else if (pti > 5)
					{
						double ball_x = stod(currentPitch.at(PLATE_X));
						double ball_y = stod(currentPitch.at(PLATE_Y));
						double top_zone = stod(currentPitch.at(ZONE_TOP));
						double bot_zone = stod(currentPitch.at(ZONE_BOTTOM));
						if (ball_x > -0.839 && ball_x < 0.839)
							if (ball_y < top_zone && ball_y > bot_zone)
								outerStrikeCount++;
							else
								outerBallCount++;
						else
							outerBallCount++;

						if (outerBallCount == 4)
						{
							inPA = false;
							totalPlateAppearences++;
							totalWalks++;
						}
						else if (outerStrikeCount == 3)
						{
							inPA = false;
							totalPlateAppearences++;
							totalStrikeouts++;
						}
						newPitch = true;

						if (outerBallCount < 4 && outerStrikeCount < 3)
						{
							while (outerBallCount < 4 && outerStrikeCount < 3)
							{
								int randomPitch = std::rand() % 1000 + 1;
								if (randomPitch <= zonePercent)
									outerStrikeCount++;
								else
									outerBallCount++;
							}
							if (outerBallCount > outerStrikeCount)
							{
								totalPlateAppearences++;
								totalWalks++;
							}
							else
							{
								totalPlateAppearences++;
								totalStrikeouts++;
							}
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
						}
						outerBallCount = 0, outerStrikeCount = 0;
					}
					else
					{
						double ball_x = stod(currentPitch.at(PLATE_X));
						double ball_y = stod(currentPitch.at(PLATE_Y));
						double top_zone = stod(currentPitch.at(ZONE_TOP));
						double bot_zone = stod(currentPitch.at(ZONE_BOTTOM));
						if (ball_x > -0.839 && ball_x < 0.839)
							if (ball_y < top_zone && ball_y > bot_zone)
								outerStrikeCount++;
							else
								outerBallCount++;
						else
							outerBallCount++;
						if (outerBallCount == 4)
						{
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
							totalPlateAppearences++;
							totalWalks++;
						}
						else if (outerStrikeCount == 3)
						{
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
							totalPlateAppearences++;
							totalStrikeouts++;
						}
						else
						{
							inPA = true;
						}
						newPitch = true;
					}
				}
			}
			// continue if we altered the pitch
			if (newPitch)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			// if we're still in a count but ran out of real pitches, generate new ones
			if (ballCount == 0 && strikeCount == 0)
			{
				if (inPA)
				{
					while (outerBallCount < 4 && outerStrikeCount < 3)
					{
						int randomPitch = std::rand() % 1000 + 1;
						if (randomPitch <= zonePercent)
							outerStrikeCount++;
						else
							outerBallCount++;
					}
					if (outerBallCount > outerStrikeCount)
					{
						totalPlateAppearences++;
						totalWalks++;
					}
					else
					{
						totalPlateAppearences++;
						totalStrikeouts++;
					}
					outerBallCount = 0, outerStrikeCount = 0;
				}
			}
			else if (!inPA)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			if (newPitch)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			inPA = true;
			if (currentPitch.at(PITCH_TYPE).compare(called) == 0)
				outerStrikeCount++;
			else
				outerBallCount++;
			if (outerBallCount == 4)
			{
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
				totalPlateAppearences++;
				totalWalks++;
			}
			else if (outerStrikeCount == 3)
			{
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
				totalPlateAppearences++;
				totalStrikeouts++;
			}
			currentPitch.clear();
			i = 0;
			startValue = 0;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
	system("clear");
	cout << "\033[1mFinal Stats:\033[0m\n" << flush;
	updateStats();
	cout << "\n";
	return -1;
}

void updateStats()
{
	using namespace std;
	string ab = "PA: ";
	ab += to_string(totalPlateAppearences);
	cout << ab << " | "  << flush;
	string bb = "BB: ";
	bb += to_string(totalWalks);
	cout << bb << " | "  << flush;
	string hbp = "HBP: ";
	hbp += to_string(totalHitByPitches);
	cout << hbp << " | "  << flush;
	string k = "Strikeouts: ";
	k += to_string(totalStrikeouts);
	cout << k << " | "  << flush;
	string obp = "OBP: ";
	double calcOBP = ((double)(totalWalks + totalHitByPitches)/((double)totalPlateAppearences));
	obp += to_string(calcOBP);
	cout << obp << flush << "\r";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}
