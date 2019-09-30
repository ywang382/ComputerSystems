#include <unistd.h>

/* Hex digits */
char hex2asc[17] = "0123456789ABCDEF";

/* Convert address fromt int to 4-digit hex representation*/
void convertAddress(char address[6], int n){
    int i = 3;
    while(n > 0 && i >= 0){
        address[i] = hex2asc[n%16];
        n /= 16;
        i--;
    }
}

/* Convert 8 chars each to 2-digit hex representations*/
void hexDump(char dump[25], char line[], int size){
    int i, j = 0;
    for(i = 0; i < 8; i++){
        if(i >= size){
            break;
        }
        unsigned int ascii = (unsigned int)line[i];
        dump[j+1] = hex2asc[ascii%16];
        ascii /= 16;
        dump[j] = hex2asc[ascii%16];
        j += 3;
    }
}

/* Display 8 bytes of data as characters*/
void convertData(char data[8], char line[], int size){
    int i;
    for(i = 0; i < 8; i++){
        if(i >= size){
            break;
        }
        if((int)line[i] < 32 || (int)line[i] > 126){
            data[i] = '.';
        } else{
            data[i] = line[i];
        }
    }
}


int main(){
    int count = 0; // used to count address

    while(1){
        char line[8]; // used to store each line that is read
        int nbytes = read(0, line, 8*sizeof(char));

	// Break out if error in reading, or no more lines left
	if(nbytes <= 0){ break; }

	// Place holders array to be converted
        char address[6] = "0000  ";
        char dump[25] = "                         ";
        char data[8] = "        ";

	convertAddress(address, count%65536);
        hexDump(dump, line, nbytes);
        convertData(data, line, nbytes);

	// Writing to stdout
	write(1, address, 6*sizeof(char));
        write(1, dump, 25*sizeof(char));
        write(1, data, nbytes*sizeof(char));
        write(1, "\n", sizeof(char));
	
	count += 8;
    }
    return (0);
}

