/* Trenton Yuntian Wang
 * ywang382@jhu.edu
 * CSF Spring 2019
 * Assignment 5
 * Apr. 26, 2019
 */

#include <unistd.h>
#include <stdio.h>
#include <vector>

using std::vector;
typedef unsigned long long ul;

// Entry of hash table, stores the address which is hashed to it
typedef struct entry{
    char bit;
    ul address;
} entry;

// Hash tables used for certain branch prediction methods
vector<entry*> bahHash(1024, nullptr);
vector<entry*> tahHash(1024, nullptr);
vector<entry*> btaHash(1024, nullptr);
vector<entry*> colHash(1024, nullptr);
vector<entry*> satHash(1024, nullptr);
vector<entry*> twoHash(1024, nullptr);
vector<char> predict(32, 0);

/* Static branch prediction */
void sta(ul ad, ul tar, char flag, ul& correct, ul& wrong){
    if((tar < ad) == flag){
        correct++;
    } else{
        wrong++;
    }
}

/* Branch address hash */
void bah(ul ad, char flag, ul& correct, ul& wrong, ul& collision){
    if(bahHash[ad & 0x3ff] == nullptr || bahHash[ad & 0x3ff]->address != ad){
        collision++;
        delete bahHash[ad & 0x3ff];
        bahHash[ad & 0x3ff] = new entry{flag, ad};
        return;
    }
    if(bahHash[ad & 0x3ff]->bit == flag){
        correct++;
        return;
    }
    wrong++;
    bahHash[ad & 0x3ff]->bit = flag;
}

/* Target address hash */
void tah(ul ad, ul tar, char flag, ul& correct, ul& wrong, ul& collision){
    if(tahHash[tar & 0x3ff] == nullptr || tahHash[tar & 0x3ff]->address != ad){
        collision++;
        delete tahHash[tar & 0x3ff];
        tahHash[tar & 0x3ff] = new entry{flag, ad};
        return;
    }
    if(tahHash[tar & 0x3ff]->bit == flag){
        correct++;
        return;
    }
    wrong++;
    tahHash[tar & 0x3ff]->bit = flag;
}

/* Branch and target address hash */
void bta(ul ad, ul tar, char flag, ul& correct, ul& wrong, ul& collision){
    int key = ((ad & 0x1f) << 5) + (tar & 0x1f);
    if(btaHash[key] == nullptr || btaHash[key]->address != ad){
        collision++;
        delete btaHash[key];
        btaHash[key] = new entry{flag, ad};
        return;
    }
    if(btaHash[key]->bit == flag){
        correct++;
        return;
    }
    wrong++;
    btaHash[key]->bit = flag;
}

/* Branch address hash with collision detection*/
void col(ul ad, ul tar, char flag, ul& correct, ul& wrong){
    if(colHash[ad & 0x3ff] == nullptr || colHash[ad & 0x3ff]->address != ad){
        delete colHash[ad & 0x3ff];
        colHash[ad & 0x3ff] = new entry{flag, ad};
        sta(ad, tar, flag, correct, wrong);
        return;
    }
    if(colHash[ad & 0x3ff]->bit == flag){
        correct++;
        return;
    }
    wrong++;
    colHash[ad & 0x3ff]->bit = flag;
}

/* Saturation counter */
void sat(ul ad, char flag, ul& correct, ul& wrong, ul& collision){
    if(satHash[ad & 0x3ff] == nullptr){
        collision++;
        satHash[ad & 0x3ff] = new entry{flag, ad};
        return;
    }
    char temp = satHash[ad & 0x3ff]->bit;
    // Update saturation counter
    if(flag == 1 && temp  < 3){
        satHash[ad & 0x3ff]->bit++;
    } else if (flag == 0 && temp  > 0){
        satHash[ad & 0x3ff]->bit--;
    }
    if(satHash[ad & 0x3ff]->address != ad){
        collision++;
        satHash[ad & 0x3ff]->address = ad;
        return;
    }
    if((temp <= 1 && flag == 0) || (temp > 1 && flag == 1)){
        correct++;
    } else{
        wrong++;
    }
}

/* Two level branch prediction */
void two(ul ad, char flag, ul& correct, ul& wrong, ul& collision){
    if(twoHash[ad & 0x3ff] == nullptr){
        collision++;
        twoHash[ad & 0x3ff] = new entry{flag, ad};
        predict[0] = flag;
        return;
    }
    char temp = ((twoHash[ad & 0x3ff]->bit << 1) & 0x1f) + flag;
    if(twoHash[ad & 0x3ff]->address != ad){
        collision++;
        predict[twoHash[ad & 0x3ff]->bit] = flag;
        twoHash[ad & 0x3ff]->bit = temp;
        twoHash[ad & 0x3ff]->address = ad;
        return;
    }
    if(predict[twoHash[ad & 0x3ff]->bit] == flag){
        correct++;
        twoHash[ad & 0x3ff]->bit = temp;
        return;
    }
    wrong++;
    predict[twoHash[ad & 0x3ff]->bit] = flag;
    twoHash[ad & 0x3ff]->bit = temp;
}

/* Deallocates and clean up used memory*/
void cleanUp(){
    for(int i = 0; i < 1024; i++){
        delete bahHash[i];
        delete tahHash[i];
        delete btaHash[i];
        delete colHash[i];
        delete satHash[i];
        delete twoHash[i];
    }
}

int main(){
    // Variables to store values for each branch prediction methods
    ul staCorrect = 0, staWrong = 0, staCol = 0;
    ul bahCorrect = 0, bahWrong = 0, bahCol = 0;
    ul tahCorrect = 0, tahWrong = 0, tahCol = 0;
    ul btaCorrect = 0, btaWrong = 0, btaCol = 0;
    ul colCorrect = 0, colWrong = 0, colCol = 0;
    ul satCorrect = 0, satWrong = 0, satCol = 0;
    ul twoCorrect = 0, twoWrong = 0, twoCol = 0;

    ul address, target;
    char flag;
    while(scanf("%llx %llx %c",&address, &target, &flag) != EOF){
        char isTaken = (flag == 'T')? 1 : 0;
        // Process different branch predictions
        sta(address, target, isTaken, staCorrect, staWrong);
        bah(address, isTaken, bahCorrect, bahWrong, bahCol);
        tah(address, target, isTaken, tahCorrect, tahWrong, tahCol);
        bta(address, target, isTaken, btaCorrect, btaWrong, btaCol);
        col(address, target, isTaken, colCorrect, colWrong);
        sat(address, isTaken, satCorrect, satWrong, satCol);
        two(address, isTaken, twoCorrect, twoWrong, twoCol);
    }
    // Output value
    printf("STA: %20llu %20llu %20llu\n", staCorrect, staWrong, staCol);
    printf("BAH: %20llu %20llu %20llu\n", bahCorrect, bahWrong, bahCol);
    printf("TAH: %20llu %20llu %20llu\n", tahCorrect, tahWrong, tahCol);
    printf("BTA: %20llu %20llu %20llu\n", btaCorrect, btaWrong, btaCol);
    printf("COL: %20llu %20llu %20llu\n", colCorrect, colWrong, colCol);
    printf("SAT: %20llu %20llu %20llu\n", satCorrect, satWrong, satCol);
    printf("TWO: %20llu %20llu %20llu\n", twoCorrect, twoWrong, twoCol);

    // Memory cleanup
    cleanUp();
    return 0;
}
