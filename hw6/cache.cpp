/* Trenton Yuntian Wang
 * ywang382@jhu.edu
 * CSF Spring 2019
 * Assignment 6
 * May 1, 2019
 */

#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <algorithm>

using std::vector;
using std::list;
typedef unsigned long long ul;

/* Tables used for certain cache simulations */
vector<ul*> dirTable(8192, nullptr);
list<ul> assTable; // Stores most recent address in front

// The following store most recent address at the back
vector<vector<ul>> setTable(2048);
vector<vector<ul>> blkTable(256);
vector<vector<ul>> nwaTable(256);
vector<vector<ul>> prfTable(256);

/* Direct mapped */
void dir(ul ad, ul& hit, ul& miss){
    if(dirTable[ad & 0x1fff] == nullptr || *dirTable[ad & 0x1fff] != ad){
        miss++;
    } else{
        hit++;
    }
    delete dirTable[ad & 0x1fff];
    dirTable[ad & 0x1fff] = new ul;
    *dirTable[ad & 0x1fff] = ad;
}

/* Fully associative */
void ass(ul ad, ul& hit, ul& miss){
    list<ul>::iterator it = std::find(assTable.begin(), assTable.end(), ad);
    if (it == assTable.end()){
        miss++;
        // cache is full
        if (assTable.size() == 8192){
            //delete least recently used element
            assTable.pop_back();
        }
    } else {
        hit++;
        assTable.erase(it);
    }
    assTable.push_front(ad);
}

/* Set associative */
void Set(ul ad, ul& hit, ul& miss){
    int key = ad & 0x7ff;
    for(unsigned int i = 0; i < setTable[key].size(); i++){
        if(setTable[key][i] == ad){
            hit++;
            setTable[key].erase(setTable[key].begin()+ i);
            setTable[key].push_back(ad);
            return;
        }
    }
    miss++;
    if(setTable[key].size() == 4){
        setTable[key].erase(setTable[key].begin());
    }
    setTable[key].push_back(ad);
}

/* Blocked set associative */
void blk(ul ad, ul& hit, ul& miss){
    int key = (ad >> 3) & 0xff;
    for(unsigned int i = 0; i < blkTable[key].size(); i++){
        if(blkTable[key][i] == ad >> 3){
            hit++;
            blkTable[key].erase(blkTable[key].begin()+ i);
            blkTable[key].push_back(ad >> 3);
            return;
        }
    }
    miss++;
    if(blkTable[key].size() == 4){
        blkTable[key].erase(blkTable[key].begin());
    }
    blkTable[key].push_back(ad >> 3);
}

/* Write-through no write-allocate */
void nwa(ul ad, ul& hit, ul& miss, char store){
    int key = (ad >> 3) & 0xff;
    for(unsigned int i = 0; i < nwaTable[key].size(); i++){
        if(nwaTable[key][i] == ad >> 3){
            hit++;
            nwaTable[key].erase(nwaTable[key].begin()+ i);
            nwaTable[key].push_back(ad >> 3);
            return;
        }
    }
    miss++;
    if(store){return;} // Do not update cache for store commands
    if(nwaTable[key].size() == 4){
        nwaTable[key].erase(nwaTable[key].begin());
    }
    nwaTable[key].push_back(ad >> 3);
}

/* Prefetch */
void prf(ul ad, ul& hit, ul& miss, char store){
    int key = (ad >> 3) & 0xff;
    for(unsigned int i = 0; i < prfTable[key].size(); i++){
        if(prfTable[key][i] == ad >> 3){
            hit++;
            prfTable[key].erase(prfTable[key].begin()+ i);
            prfTable[key].push_back(ad >> 3);
            return;
        }
    }
    miss++;
    if(store){return;}

    // Fetch first block
    if(prfTable[key].size() == 4){
        prfTable[key].erase(prfTable[key].begin());
    }
    prfTable[key].push_back(ad >> 3);

    if(ad >= 0xfffffffffffffff8){return;} // Last block of memory, no additional block to fetch

    // Fetch additional block
    for(unsigned int i = 0; i < prfTable[(key+1)%256].size(); i++){
        if(prfTable[(key+1)%256][i] == (ad >> 3) + 1){
            prfTable[(key+1)%256].erase(prfTable[(key+1)%256].begin() + i);
        }
    }

      if(prfTable[(key+1)%256].size() == 4){
        prfTable[(key+1)%256].erase(prfTable[(key+1)%256].begin());
      }
      prfTable[(key+1)%256].push_back((ad >> 3) + 1);

}

/* Deallocates and cleans up used memory*/
void cleanUp(){
    for(int i = 0; i < 8192; i++){
        delete dirTable[i];
    }
}

int main(){
    // Variables to store values for each cache simulations
    ul dirHit = 0, dirMiss = 0;
    ul assHit = 0, assMiss = 0;
    ul setHit = 0, setMiss = 0;
    ul blkHit = 0, blkMiss = 0;
    ul nwaHit = 0, nwaMiss = 0;
    ul prfHit = 0, prfMiss = 0;

    ul address;
    char flag;
    while(scanf("%llx %c",&address, &flag) != EOF){
        char isStore = (flag == 'S')? 1 : 0;
        // Process different cache simulations
        dir(address, dirHit, dirMiss);
        ass(address, assHit, assMiss);
        Set(address, setHit, setMiss);
        blk(address, blkHit, blkMiss);
        nwa(address, nwaHit, nwaMiss, isStore);
        prf(address, prfHit, prfMiss, isStore);
    }
    // Output value
    printf("DIR: %20llu %20llu\n", dirHit, dirMiss);
    printf("ASS: %20llu %20llu\n", assHit, assMiss);
    printf("SET: %20llu %20llu\n", setHit, setMiss);
    printf("BLK: %20llu %20llu\n", blkHit, blkMiss);
    printf("NWA: %20llu %20llu\n", nwaHit, nwaMiss);
    printf("PRF: %20llu %20llu\n", prfHit, prfMiss);

    // Memory cleanup
    cleanUp();
    return 0;
}
