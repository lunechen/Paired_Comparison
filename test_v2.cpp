#include <bits/stdc++.h>
using namespace std;

// random engine
random_device rd;
default_random_engine generator = default_random_engine(rd());
uniform_int_distribution<int> unif(0,2147483647);

#define teamNum 100
long long int matchTimes = 0;

//#define input
#define match

vector <double> btModel(int **matchResult, vector <int> group){
    vector <double> prob[2];
    for (int i = 0; i < group.size(); i++){
        prob[0].push_back(1);
        prob[1].push_back(1);
    }
    
    for (int i = 0; i < 200; i++){
        double totalProb = 0;
        for (int j = 0; j < group.size(); j++){
            int teamA = group[j];
            int w = 0;
            double base = 0;
            for (int k = 0; k < group.size(); k++){
                int teamB = group[k];
                if (teamA == teamB) continue;
                w += matchResult[teamA][teamB];
                base += (matchResult[teamA][teamB] + matchResult[teamB][teamA]) / (prob[i % 2][j] + prob[i % 2][k]);
            }
            prob[(i + 1) % 2][j] = w / base;
            totalProb += w / base;
        }

        for (int j = 0; j < group.size(); j++){
            int teamA = group[j];
            prob[(i + 1) % 2][j] /= totalProb;
        }
    }
    return prob[100 % 2];
}

void makeMatch(double *teamStrength, int **matchResult, int teamA, int teamB){
    int randNum = unif(generator) % 1000 + 1;
    double teamAWin = teamStrength[teamA] / (teamStrength[teamA] + teamStrength[teamB]);
    matchResult[teamA][teamB] = randNum * teamAWin;
    matchResult[teamB][teamA] = randNum - matchResult[teamA][teamB];
    matchTimes += randNum;
}

void makeStrength(double *teamStrength, int teamCnt){
    long long int totalSum = 0;
    for (int i = 0; i < teamCnt; i++){
        int randNum = unif(generator);
        teamStrength[i] = randNum;
        totalSum += randNum;
    }

    for (int i = 0; i < teamCnt; i++){
        teamStrength[i] /= totalSum;
    }
}

void printStrength(double *teamStrength, int teamCnt){
    double sum = 0.0;
    for (int i = 0; i < teamNum; i++){
        printf("%d: %lf\n", i, teamStrength[i]);
        sum += teamStrength[i];
    }
    printf("total: %lf\n", sum);
}

vector <pair <double, int> > myWay4(double *teamStrength, int **matchResult, int teamCnt, int divides){
    vector <int> groups[divides];
    int id = 0;
    for (int i = 0; i < divides; i++){
        for (int j = 0; j < teamCnt / divides; j++){
            groups[i].push_back(id++);
            // push 0 for balance str
            if (i != 0) groups[i].push_back(0);
        }
    }

    for (int i = 0; i < divides; i++){
        for (int j = 0; j < groups[i].size(); j++){
            for (int k = j + 1; k < groups[i].size(); k++)
                makeMatch(teamStrength, matchResult, groups[i][j], groups[i][k]);
        }
    }

    // groupOne.push_back(0);
    

    double prob[2][teamCnt];
    for (int i = 0; i < teamCnt; i++){
        prob[0][i] = 1;
        prob[1][i] = 1;
    }

    int times = 1000;

    for (int i = 0; i < times; i++){
        double totalProb = 0;
        int w = 0;
        double base = 0;
        
        // bt model group by group
        for (int k = 0; k < divides; k++){
            for (int j = 0; j < groups[k].size(); j++){
                w = 0;
                base = 0;
                int teamA = groups[k][j];
                // 0 only for team 1
                if (k !=0 && teamA == 0) continue;
                for (int l = 0; l < groups[k].size(); l++){
                    int teamB = groups[k][l];
                    if (teamA == teamB) continue;
                    w += matchResult[teamA][teamB];
                    base += (matchResult[teamA][teamB] + matchResult[teamB][teamA]) / (prob[i % 2][teamA] + prob[i % 2][teamB]);
                }
                prob[(i + 1) % 2][teamA] = w / base;
                totalProb += w / base;
            }
        }

        // set all normalized
        for (int j = 1; j < teamCnt; j++){
            prob[(i + 1) % 2][j] /= totalProb;
        }
    }

    vector <pair <double, int> > real;
    vector <pair <double, int> > result;

    for (int i = 0; i < teamNum; i++){
        real.push_back({teamStrength[i], i});
        result.push_back({prob[times % 2][i], i});
    }

    sort(real.begin(), real.end());
    sort(result.begin(), result.end());

    double totalP = 0;

    // printf("%d\n", matchTimes);
    // for (int i = 0; i < teamCnt; i++){
    //     totalP += result[i].first;
    //     printf("%3d %3d: %lf %3d: %lf\n", i, result[i].second, result[i].first, real[i].second, real[i].first);
    // }
    // printf("%lf\n", totalP);

    return result;
}

double errCal(vector <pair <double, int> > result, double *teamStrength, int teamCnt){
    int total = 0;
    int err = 0;
    
    for (int i = 0; i < teamCnt; i++){
        for (int j = 0; j < teamCnt; j++){
            int teamA = result[i].second;
            int teamB = result[j].second;
            if (teamA == teamB) continue;
            else{
                total++;
                //printf("%d %d %lf %lf\n", teamA, teamB, result[i].first / (result[i].first + result[j].first), (teamStrength[teamA] / (teamStrength[teamA] + teamStrength[teamB])));
                if (abs(((result[i].first / (result[i].first + result[j].first)) - (teamStrength[teamA] / (teamStrength[teamA] + teamStrength[teamB])))) > 0.5)
                    err++;
            }
        }
    }
    return (double)err/total;
    
}

int main(){
    double teamStrength[teamNum];
    int *matchResult[teamNum];
    for (int i = 0; i < teamNum; i++){
        teamStrength[i] = 0;
        matchResult[i] = (int *) malloc(sizeof(int) * teamNum);
        for (int j = 0; j < teamNum; j++){
            matchResult[i][j] = -1;
        }
    }
    
    makeStrength(teamStrength, teamNum);
    #ifndef match
    for (int i = 0; i < teamNum; i++){
        for (int j = 0; j < teamNum; j++){
            #ifndef input
            makeMatch(teamStrength, matchResult, i, j);
            #endif
            #ifdef input
            scanf("%d", &matchResult[i][j]);
            #endif
        }
    }
    #endif

    #ifdef printresult
    for (int i = 0; i < teamNum; i++){
        for (int j = 0; j < teamNum; j++){
            printf("%4d ", matchResult[i][j]);
        }
        printf("\n");
    }
    #endif

    vector <pair<double, int> > results[3];
    results[0] = myWay4(teamStrength, matchResult, teamNum, 50);
    printf("%d\n", matchTimes);
    matchTimes = 0;
    results[1] = myWay4(teamStrength, matchResult, teamNum, 2);
    printf("%d\n", matchTimes);
    matchTimes = 0;
    results[2] = myWay4(teamStrength, matchResult, teamNum, 1);
    printf("%d\n", matchTimes);
    matchTimes = 0;

    for (int i = 0; i < teamNum; i++){
        printf("%d: ", i);
        for (int j = 0; j < 3; j++)
            printf("%6d: %lf ", results[j][i].second, results[j][i].first);
        printf("%6d: %lf\n", i, teamStrength[i]);
    }
    
    for (int i = 0; i < 3; i++){
        printf("%d: %lf\n", i, errCal(results[2], teamStrength, teamNum));
    }
    
    for (int i = 0; i < teamNum; i++) free(matchResult[i]);
    //seniorWay(teamStrength, matchResult, teamNum);
    //printStrength(teamStrength, teamNum);
    
    //myWay3(teamStrength, matchResult, teamNum);
    //printf("%lld\n", matchTimes);
    //for (int i = 0; i < teamNum; i++) free(matchResult[i]);
}

/*
  -1  251  259  253  392  305  428  226  227  244
 249   -1  260  291  416  327  427  229  240  282
 241  240   -1  304  401  321  445  246  239  266
 247  209  196   -1  405  305  418  205  211  239
 108   84   99   95   -1  148  302   81  102  112
 195  173  179  195  352   -1  384  179  173  199
  72   73   55   82  198  116   -1   73   67   82
 274  271  254  295  419  321  427   -1  254  283
 273  260  261  289  398  327  433  246   -1  300
 256  218  234  261  388  301  418  217  200   -1
*/

0 ~ 5
6 ~ 10